#ifndef CELL_CANDIDATE_TYPE_HPP
#define CELL_CANDIDATE_TYPE_HPP

#include "./cell_candidate.hpp"
#include "./position.hpp"
#include <list>
#include <stack>
#include <vector>

// clang-format off
#define CNT_TYPE(T) std::list<T>
#define CNT_P_TYPE(T) std::list<T *>
#define TYPENAME_CNT_P_ITERATOR(T) typename CNT_P_TYPE(T)::iterator
#define TYPENAME_CNT_P_ITERATOR_PAIR(T) std::pair<TYPENAME_CNT_P_ITERATOR(T), TYPENAME_CNT_P_ITERATOR(T)>
// clang-format on

template <typename T> class CellCandidateType : public CellCandidate {
public:
  inline CellCandidateType(Position position) : CellCandidate(position) {
    this->_possibleValues.resize(this->_defaultPossibilities.size());
    auto beg = this->_possibleValues.begin();
    uint64_t index = 0;
    while (beg != this->_possibleValues.end()) {
      *beg = &this->_defaultPossibilities[index++];
      beg++;
    }
  }
  inline ~CellCandidateType<T>() {}
  virtual bool CanBeNeighborsWithValue(uint8_t adjacencyIndex, T *value) = 0;
  inline uint64_t GetEntropy() override {
    if (this->_isObserved) {
      return 0ULL;
    }
    return this->_possibleValues.size();
  }
  inline T *GetValue() { return this->_value; }
  inline bool HandleNeighborCollapsed() override {
    // this is called on neighbors of changed or updated cells, not the observed
    // cell itself
    if (this->_isObserved) {
      return false;
    }
    this->_isCollapsing = true;
    uint64_t beforeCount = this->_possibleValues.size();
    bool firstPopulated = false;
    TYPENAME_CNT_P_ITERATOR(T) tmpStartIT;
    TYPENAME_CNT_P_ITERATOR(T) tmpEndIT;
    auto valIT = this->_possibleValues.begin();
    while (valIT != this->_possibleValues.end()) {
      bool valid = true;
      for (size_t i = 0; i < DIMENSION_COUNT * 2; i++) {
        CellCandidateType<T> *mcc =
          static_cast<CellCandidateType<T> *>(this->GetAdjacency(i));
        if (mcc == nullptr) {
          continue;
        }
        if (!mcc->CanBeNeighborsWithValue(i, *valIT)) {
          valid = false;
          break;
        }
      }
      if (valid == false) {
        if (firstPopulated == false) {
          firstPopulated = true;
          tmpStartIT = valIT;
          tmpEndIT = valIT;
        } else {
          tmpEndIT = valIT;
        }
      } else {
        if (firstPopulated == true) {
          firstPopulated = false;
          _toRemoveStack.emplace(tmpStartIT, tmpEndIT);
        }
      }
      valIT++;
    }
    if (firstPopulated == true) {
      _toRemoveStack.emplace(tmpStartIT, tmpEndIT);
    }
    TYPENAME_CNT_P_ITERATOR_PAIR(T) itPair;
    while (!this->_toRemoveStack.empty()) {
      itPair = std::move(this->_toRemoveStack.top());
      this->_toRemoveStack.pop();
      std::advance(itPair.second, 1);
      this->_possibleValues.erase(itPair.first, itPair.second);
    }
    uint64_t afterCount = this->_possibleValues.size();
    this->_isCollapsing = beforeCount != afterCount;
    return beforeCount == afterCount;
  }
  inline void HandleObserved(uint64_t indexToObserve) override {
    auto it = this->_possibleValues.begin();
    std::advance(it, indexToObserve);
    this->_value = *it;
    this->_possibleValues.clear();
    this->_isObserved = true;
  }
  virtual inline void HandleOnSettling() override {
    // do nothing
  }
  inline static void SetDefaultPossibilities(
    const std::vector<T> &possibilities) {
    CellCandidateType<T>::_defaultPossibilities = possibilities;
  }

protected:
  inline static std::vector<T> _defaultPossibilities;
  CNT_P_TYPE(T) _possibleValues;
  T *_value = nullptr;

private:
  std::stack<TYPENAME_CNT_P_ITERATOR_PAIR(T)> _toRemoveStack;
};

#endif /* CELL_CANDIDATE_TYPE_HPP */