#ifndef CELL_CANDIDATE_TYPE_HPP
#define CELL_CANDIDATE_TYPE_HPP

#include "./cell_candidate.hpp"
#include "./position.hpp"
#include <list>
#include <stack>
#include <vector>

// clang-format off
#define CONTAINER_TYPE(T) std::list<T>
#define CONTAINER_P_TYPE(T) std::list<T *>
#define TYPENAME_CONTAINER_ITERATOR(T) typename CONTAINER_TYPE(T)::iterator
#define TYPENAME_CONTAINER_P_ITERATOR(T) typename CONTAINER_P_TYPE(T)::iterator
#define TYPENAME_CONTAINER_ITERATOR_PAIR(T) std::pair<TYPENAME_CONTAINER_ITERATOR(T), TYPENAME_CONTAINER_ITERATOR(T)>
#define TYPENAME_CONTAINER_P_ITERATOR_PAIR(T) std::pair<TYPENAME_CONTAINER_P_ITERATOR(T), TYPENAME_CONTAINER_P_ITERATOR(T)>
// clang-format on

template <typename T> class CellCandidateType : public CellCandidate {
public:
  inline CellCandidateType(Position position) : CellCandidate(position) {}
  inline ~CellCandidateType<T>() {}
  virtual bool CanBeNeighborsWithValue(uint8_t adjacencyIndex, T *value) = 0;
  inline uint64_t GetEntropy() override {
    if (this->_isObserved) {
      return 0ULL;
    } else if (!this->_hasBeenTouched) {
      return this->_defaultPossibilities.size();
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
    bool takeSubtractPath = true;
    if (!this->_hasBeenTouched) {
      auto lowestEntropy = this->_defaultPossibilities.size();
      for (size_t i = 0; i < DIMENSION_COUNT * 2; i++) {
        CellCandidateType<T> *mcc =
          static_cast<CellCandidateType<T> *>(this->GetAdjacency(i));
        if (mcc != nullptr) {
          auto entropy = mcc->GetEntropy();
          if (entropy < lowestEntropy) {
            lowestEntropy = entropy;
          }
        }
      }
      if (lowestEntropy / (float)this->_defaultPossibilities.size() < 0.5) {
        takeSubtractPath = false;
      } else {
        this->_possibleValues.resize(this->_defaultPossibilities.size());
        auto beg = this->_possibleValues.begin();
        uint64_t index = 0;
        while (beg != this->_possibleValues.end()) {
          *beg = &this->_defaultPossibilities[index++];
          beg++;
        }
      }
    }
    if (takeSubtractPath) {
      uint64_t beforeCount = this->_possibleValues.size();
      bool firstPopulated = false;
    TYPENAME_CONTAINER_P_ITERATOR(T) tmpStartIT;
    TYPENAME_CONTAINER_P_ITERATOR(T) tmpEndIT;
      auto valIT = this->_possibleValues.begin();
      while (valIT != this->_possibleValues.end()) {
        bool valid = true;
      for (size_t i = 0; i < NEIGHBOR_COUNT; i++) {
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
    TYPENAME_CONTAINER_P_ITERATOR_PAIR(T) itPair;
      while (!this->_toRemoveStack.empty()) {
        itPair = std::move(this->_toRemoveStack.top());
        this->_toRemoveStack.pop();
        std::advance(itPair.second, 1);
        this->_possibleValues.erase(itPair.first, itPair.second);
      }
      uint64_t afterCount = this->_possibleValues.size();
      this->_isCollapsing = beforeCount != afterCount;
      this->_hasBeenTouched = true;
      this->OnHandledCollapsedNeighbor();
      return beforeCount == afterCount;
    } else {
      auto possibilities = this->_defaultPossibilities.data();
      for (size_t x = 0; x < this->_defaultPossibilities.size(); x++) {
        T *possibility = &possibilities[x];
        if (possibility == nullptr) {
          continue;
        }
        bool allValid = true;
        for (size_t i = 0; i < DIMENSION_COUNT * 2; i++) {
          CellCandidateType<T> *mcc =
            static_cast<CellCandidateType<T> *>(this->GetAdjacency(i));
          if (mcc == nullptr) {
            continue;
          }
          if (!mcc->CanBeNeighborsWithValue(i, possibility)) {
            allValid = false;
            break;
          }
        }
        if (allValid) {
          this->_possibleValues.push_back(possibility);
        }
      }
      this->_hasBeenTouched = true;
      this->_isCollapsing =
        this->_possibleValues.size() != this->_defaultPossibilities.size();
      this->OnHandledCollapsedNeighbor();
      return !this->_isCollapsing;
    }
  }
  inline void HandleObserved(uint64_t indexToObserve) override {
    if (this->_hasBeenTouched) {
      auto it = this->_possibleValues.begin();
      std::advance(it, indexToObserve);
      this->_value = *it;
      this->_possibleValues.clear();
    } else {
      auto it = this->_defaultPossibilities.begin();
      std::advance(it, indexToObserve);
      this->_value = &*it;
      this->_hasBeenTouched = true;
    }
    this->_isObserved = true;
  }
  virtual inline void HandleOnSettling() override {
    // do nothing
  }
  virtual inline void OnHandledCollapsedNeighbor() {
    // do nothing;
  }
  inline static void SetDefaultPossibilities(
    const std::vector<T> &possibilities) {
    CellCandidateType<T>::_defaultPossibilities = possibilities;
  }

protected:
  inline static std::vector<T> _defaultPossibilities;
  CONTAINER_P_TYPE(T) _possibleValues;
  T *_value = nullptr;

private:
  std::stack<TYPENAME_CONTAINER_P_ITERATOR_PAIR(T)> _toRemoveStack;
};

#endif /* CELL_CANDIDATE_TYPE_HPP */