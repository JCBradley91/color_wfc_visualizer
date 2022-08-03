
#include "./thread_pool.hpp"

ThreadPool::ThreadPool(uint8_t threadCount, uint64_t maxJobQueueCount)
    : _maxThreads(threadCount), _maxJobQueueCount(maxJobQueueCount) {
  assert(threadCount > 0);
  assert(maxJobQueueCount > 0);
  this->_threads.resize(this->_maxThreads);
  for (size_t i = 0; i < this->_maxThreads; i++) {
    this->_threads[i] = std::thread(std::bind(&ThreadPool::ThreadRun, this));
  }
}

ThreadPool::~ThreadPool() {
  this->Shutdown();
  this->_threads.clear();
}

const uint64_t ThreadPool::GetCompletedCount() { return this->_completedCount; }

void ThreadPool::JoinAll() {
  this->_condition.notify_all();
  for (auto &t : this->_threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}

bool ThreadPool::QueueJob(Job j) {
  this->_mutex.lock();
  if (this->_running == true) {
    if (this->_jobQueue.size() < this->_maxJobQueueCount) {
      this->_jobQueue.push(std::move(j));
      this->_condition.notify_one();
      this->_mutex.unlock();
      return true;
    }
  }
  this->_mutex.unlock();
  return false;
}

void ThreadPool::Pause() {
  this->_mutex.lock();
  if (this->_isPaused == true) {
    return; // called more than once
  }
  this->_isPaused = true;
  this->_mutex.unlock();
  this->JoinAll();
}

void ThreadPool::Resume() {
  this->_mutex.lock();
  this->_isPaused = false;
  for (size_t i = 0; i < this->_maxThreads; i++) {
    this->_threads[i] = std::thread(std::bind(&ThreadPool::ThreadRun, this));
  }
  this->_mutex.unlock();
}

void ThreadPool::Shutdown() {
  this->_mutex.lock();
  if (this->_running == false) {
    return;
  }
  this->_running = false;
  while (this->_jobQueue.empty() == false) {
    this->_jobQueue.pop();
  }
  this->_mutex.unlock();
  this->JoinAll();
}

void ThreadPool::ThreadRun() {
  Job j;
  while (this->_running && !this->_isPaused) {
    {
      std::unique_lock<std::mutex> lck(this->_mutex);
      this->_condition.wait(lck, [this] {
        return !this->_jobQueue.empty() || !this->_running || this->_isPaused;
      });
      if (!this->_running || this->_isPaused) {
        return;
      }
      if (this->_jobQueue.empty()) {
        continue;
      }
      j = std::move(this->_jobQueue.front());
      this->_jobQueue.pop();
    }
    j();
    this->_completedCount++;
  }
}
