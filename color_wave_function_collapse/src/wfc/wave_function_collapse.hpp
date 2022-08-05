#ifndef WAVE_FUNCTION_COLLAPSE_HPP
#define WAVE_FUNCTION_COLLAPSE_HPP

#include "cell_candidate.hpp"
#include "position.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <random>

enum struct NextObservationMethod {
  None = 0,
  Random,
  LowestEntropySequential,
  LowestEntropyReverseSequential,
  LowestEntropyRandom,
  HighestEntropySequential,
  HighestEntropyReverseSequential,
  HighestEntropyRandom,
  Default = LowestEntropyRandom,
};

struct PositionRules {
  uint16_t maxDistance = 1U << DIMENSION_DISTANCE_BIT_SHIFT;
};

class WaveFunctionCollapse {

public:
  WaveFunctionCollapse(const Position &maxPosition,
    std::function<CellCandidate *(Position pos)>
      &&cellCandidateSuperpositionCTOR,
    NextObservationMethod nextObservationMethod =
      NextObservationMethod::Default,
    uint64_t seed = std::chrono::steady_clock::now().time_since_epoch().count(),
    bool isDeferredExecution = false);
  ~WaveFunctionCollapse();
  const std::map<uint64_t, CellCandidate *> &GetGrid();
  bool GetIsComplete();
  const Position GetMaxPosition();
  void RunToCompletion();
  void Step();
  void Stop();

protected:
  std::atomic<bool> _isStopping = false;
  std::atomic<bool> _hasBuiltGrid = false;
  const NextObservationMethod _nextObservationMethod;
  std::mt19937 _rand;
  PositionRules _positionRules{};
  Position _maxPosition;
  std::map<uint64_t, CellCandidate *> _candidateGrid;
  std::function<CellCandidate *(Position pos)> _cellCandidateCTOR;
  bool RecursiveBuildBaseGrid(Position position = Position(),
    uint64_t positionKey = 0ULL, int16_t index = 0);
  void RecursiveBuildBaseGridWrapper(Position position = Position(),
    uint64_t positionKey = 0ULL, int16_t index = 0);
  void Run(bool runOnce = false);

private:
};

#endif /* WAVE_FUNCTION_COLLAPSE_HPP */