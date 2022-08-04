#ifndef CELL_CANDIDATE_HPP
#define CELL_CANDIDATE_HPP

#include "./position.hpp"
#include <array>
#include <atomic>
#include <map>
#include <random>

class CellCandidate {
public:
  explicit inline CellCandidate(Position position) : _position(position) {}
  inline virtual ~CellCandidate() {}
  virtual uint64_t GetEntropy() = 0;
  inline CellCandidate *GetNeighborAfter(uint8_t dimensionIndex) {
    return this->GetAdjacency(dimensionIndex * 2U + 1U);
  }
  inline CellCandidate *GetNeighborBefore(uint8_t dimensionIndex) {
    return this->GetAdjacency(dimensionIndex * 2U);
  }
  inline const Position GetPosition() { return this->_position; }
  inline bool IsObserved() { return this->_isObserved; }
  inline void Observe(std::mt19937 &rand) {
    std::uniform_int_distribution<uint64_t> dist(0, this->GetEntropy() - 1);
    this->HandleObserved(dist(rand));
    this->CollapseNeighbors();
  }
  inline void Settle() {
    this->_isCollapsing = false;
    this->HandleOnSettling();
  }
  inline void SetNeighborAfter(uint8_t dimensionIndex, CellCandidate *cell) {
    this->SetAdjacency(dimensionIndex * 2U + 1U, cell);
  }
  inline void SetNeighborBefore(uint8_t dimensionIndex, CellCandidate *cell) {
    this->SetAdjacency(dimensionIndex * 2U, cell);
  }

protected:
  bool _isObserved = false;
  bool _hasBeenTouched = false;
  bool _isCollapsing = false;
  Position _position;
  inline void CollapseNeighbors() {
    CellCandidate *neighbor;
    if (this->_isCollapsing == true) {
      return;
    }
    this->_isCollapsing = true;
    std::vector<CellCandidate *> neighbors;
    std::vector<CellCandidate *> lastCollapsedNeighbors;
    std::vector<CellCandidate *> collapsedNeighbors;
    std::map<uint64_t, CellCandidate *> neighborMap;
    neighbors.reserve(1000);
    lastCollapsedNeighbors.reserve(1000);
    collapsedNeighbors.reserve(1000);
    do {
      neighbors.clear();
      for (size_t i = 0; i < NEIGHBOR_COUNT; i++) {
        if (lastCollapsedNeighbors.size() == 0) {
          neighbor = this->GetAdjacency(i);
          if (neighbor != nullptr && neighbor->IsObserved() == false) {
            neighbors.emplace_back(neighbor);
            neighborMap.emplace(
              Position::ToHash(neighbor->GetPosition()), neighbor);
          }
        } else {
          for (auto n : lastCollapsedNeighbors) {
            n = n->GetAdjacency(i);
            if (n != nullptr && n->IsObserved() == false &&
                n->_isCollapsing == false &&
                !neighborMap.contains(Position::ToHash(n->GetPosition()))) {
              neighbors.emplace_back(n);
              neighborMap.emplace(Position::ToHash(n->GetPosition()), n);
            }
          }
        }
      }
      collapsedNeighbors.insert(std::end(collapsedNeighbors),
        std::begin(lastCollapsedNeighbors), std::end(lastCollapsedNeighbors));
      lastCollapsedNeighbors.clear();
      for (auto n : neighbors) {
        if (!n->IsObserved()) {
          if (!n->HandleNeighborCollapsed()) {
            lastCollapsedNeighbors.emplace_back(n);
          }
        }
      }
    } while (lastCollapsedNeighbors.size() != 0);
    for (auto &n : collapsedNeighbors) {
      n->Settle();
    }
    this->_isCollapsing = false;
  }
  inline CellCandidate *GetAdjacency(uint8_t adjacencyIndex) {
    return this->_adjacencies.at(adjacencyIndex);
  }
  /*!
   * Called when a neighbor of this cell was modified
   *
   * @returns False if the entropy of this cell was modified
   */
  virtual bool HandleNeighborCollapsed() = 0;
  virtual void HandleObserved(uint64_t indexToObserve) = 0;
  virtual void HandleOnSettling() = 0;
  inline void SetAdjacency(uint8_t adjacencyIndex, CellCandidate *cell) {
    this->_adjacencies[adjacencyIndex] = cell;
  }
  inline void SettleNeighbors() {
    if (this->_isCollapsing == false) {
      return;
    }
    this->_isCollapsing = false;
    for (size_t i = 0; i < NEIGHBOR_COUNT; i++) {
      CellCandidate *neighbor = this->GetAdjacency(i);
      if (neighbor != nullptr && !neighbor->IsObserved()) {
        neighbor->SettleNeighbors();
      }
    }
  }

private:
  /*!
   * position i is before, i+1 is after
   */
  std::array<CellCandidate *, NEIGHBOR_COUNT> _adjacencies = EMPTY_ADJACENCIES;
};

#endif /* CELL_CANDIDATE_HPP */