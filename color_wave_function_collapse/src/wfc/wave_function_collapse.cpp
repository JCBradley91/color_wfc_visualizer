
#include "./wave_function_collapse.hpp"

WaveFunctionCollapse::WaveFunctionCollapse(const Position &maxPosition,
  std::function<CellCandidate *(Position pos)> &&cellCandidateSuperpositionCTOR,
  NextObservationMethod nextObservationMethod, uint64_t seed,
  bool isDeferredExecution)
    : _nextObservationMethod(nextObservationMethod), _maxPosition(maxPosition),
      _cellCandidateCTOR(std::move(cellCandidateSuperpositionCTOR)) {

  assert(this->_nextObservationMethod != NextObservationMethod::None);

  for (size_t i = 0; i < DIMENSION_COUNT; i++) {
    assert(this->_maxPosition.pos[i] < this->_positionRules.maxDistance);
  }

  this->_rand.seed(seed);

  Position pos;
  this->RecursiveBuildBaseGrid(pos);
  for (auto &cg : this->_candidateGrid) {
    uint64_t adjacentKey;
    Position adjacentPosition;
    for (size_t i = 0; i < DIMENSION_COUNT; i++) {
      if (cg.second->GetPosition().pos[i] > 0) {
        adjacentPosition = Position(cg.second->GetPosition());
        adjacentPosition.pos[i]--;
        adjacentKey = Position::ToHash(adjacentPosition);
        cg.second->SetNeighborBefore(i, this->_candidateGrid.at(adjacentKey));
      }
      if (cg.second->GetPosition().pos[i] < this->_maxPosition.pos[i]) {
        adjacentPosition = Position(cg.second->GetPosition());
        adjacentPosition.pos[i]++;
        adjacentKey = Position::ToHash(adjacentPosition);
        cg.second->SetNeighborAfter(i, this->_candidateGrid.at(adjacentKey));
      }
    }
  }

  if (!isDeferredExecution) {
    this->Run();
  }
}

WaveFunctionCollapse::~WaveFunctionCollapse() {
  for (auto &cc : this->_candidateGrid) {
    delete cc.second;
  }
}

const std::map<uint64_t, CellCandidate *> &WaveFunctionCollapse::GetGrid() {
  return this->_candidateGrid;
}

bool WaveFunctionCollapse::GetIsComplete() {
  auto result =
    std::find_if(this->_candidateGrid.begin(), this->_candidateGrid.end(),
      [](const auto &x) { return x.second->IsObserved() == false; });
  if (result != this->_candidateGrid.end()) {
    return false;
  }
  return true;
}

const Position WaveFunctionCollapse::GetMaxPosition() {
  return this->_maxPosition;
}

bool WaveFunctionCollapse::RecursiveBuildBaseGrid(
  Position position, uint64_t positionKey, int16_t index) {

  // check if out of range
  if (index >= DIMENSION_COUNT ||
      position.pos[index] > this->_maxPosition.pos[index]) {
    return false;
  }

  // insert if not already inserted
  if (!this->_candidateGrid.contains(positionKey)) {
    uint64_t positionIndex = Position::ToHash(position);
    this->_candidateGrid.insert_or_assign(
      positionIndex, this->_cellCandidateCTOR(position));
  }

  // handle next for current dimension
  Position nextPosition(position);
  nextPosition.pos[index]++;
  RecursiveBuildBaseGrid(nextPosition, Position::ToHash(nextPosition), index);
  // defer to next dimension
  if (index <= DIMENSION_COUNT - 1ULL) {
    RecursiveBuildBaseGrid(position, positionKey, index + 1);
  }

  return true;
}

void WaveFunctionCollapse::RunToCompletion() {
  if (!this->GetIsComplete()) {
    this->Run();
  }
}

void WaveFunctionCollapse::Run(bool runOnce) {
  uint64_t nextEntropy = std::numeric_limits<uint64_t>().max();
  std::list<CellCandidate *> nextEntropyCandidates;
  CellCandidate *cellCandidate = nullptr;
  while (true) {

    //
    // find next cell to observe
    //
    auto gridIT = this->_candidateGrid.begin();
    while (gridIT != this->_candidateGrid.end()) {
      uint64_t entropy = gridIT->second->GetEntropy();
      if (entropy == 0U) {
        gridIT++;
        continue;
      }
      if (this->_nextObservationMethod == NextObservationMethod::Random) {
        nextEntropyCandidates.emplace_back(gridIT->second);
      } else if (this->_nextObservationMethod ==
                   NextObservationMethod::HighestEntropyRandom ||
                 this->_nextObservationMethod ==
                   NextObservationMethod::HighestEntropyReverseSequential ||
                 this->_nextObservationMethod ==
                   NextObservationMethod::HighestEntropySequential) {
        if (nextEntropy == std::numeric_limits<uint64_t>().max()) {
          nextEntropy = entropy;
        }
        if (entropy > nextEntropy) {
          nextEntropy = entropy;
          nextEntropyCandidates.clear();
        }
        if (entropy == nextEntropy) {
          nextEntropyCandidates.emplace_back(gridIT->second);
        }
      } else if (this->_nextObservationMethod ==
                   NextObservationMethod::LowestEntropyRandom ||
                 this->_nextObservationMethod ==
                   NextObservationMethod::LowestEntropyReverseSequential ||
                 this->_nextObservationMethod ==
                   NextObservationMethod::LowestEntropySequential) {
        if (nextEntropy == std::numeric_limits<uint64_t>().max()) {
          nextEntropy = entropy;
        }
        if (entropy < nextEntropy) {
          nextEntropy = entropy;
          nextEntropyCandidates.clear();
        }
        if (entropy == nextEntropy) {
          nextEntropyCandidates.emplace_back(gridIT->second);
        }
      }
      gridIT++;
    }

    // check if we found a cell to resolve, if not then we're done
    if (nextEntropyCandidates.size() == 0ULL) {
      break;
    } else if (nextEntropyCandidates.size() == 1ULL) {
      cellCandidate = nextEntropyCandidates.front();
    } else {
      auto begin = nextEntropyCandidates.begin();
      auto end = nextEntropyCandidates.end();
      std::uniform_int_distribution<uint64_t> listLengthRand(
        0, std::distance(begin, end) - 1U);
      switch (this->_nextObservationMethod) {
      case NextObservationMethod::None:
      case NextObservationMethod::Random:
      case NextObservationMethod::LowestEntropyRandom:
      case NextObservationMethod::HighestEntropyRandom:
        std::advance(begin, listLengthRand(this->_rand));
        cellCandidate = *begin;
        break;
      case NextObservationMethod::LowestEntropySequential:
      case NextObservationMethod::HighestEntropySequential:
        cellCandidate = nextEntropyCandidates.front();
        break;
      case NextObservationMethod::LowestEntropyReverseSequential:
      case NextObservationMethod::HighestEntropyReverseSequential:
        cellCandidate = nextEntropyCandidates.back();
        break;
      default:
        break;
      }
    }

    //
    // observe & collapse
    //
    cellCandidate->Observe(this->_rand);

    //
    // reset and keep running
    //
    if (runOnce) {
      break;
    }
    nextEntropy = std::numeric_limits<uint64_t>().max();
    nextEntropyCandidates.clear();
    cellCandidate = nullptr;
  }
}

void WaveFunctionCollapse::Step() { this->Run(true); }
