#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

using Job = std::packaged_task<void()>;

class ThreadPool {

public:
  explicit ThreadPool(uint8_t threadCount, uint64_t maxJobQueueCount = 255U);
  ~ThreadPool();
  bool QueueJob(Job j);
  void Pause();
  void Resume();
  void Shutdown();
  const uint64_t GetCompletedCount();

private:
  bool _running = true;
  bool _isPaused = false;
  uint8_t _maxThreads = 0;
  uint64_t _maxJobQueueCount = 0;
  std::atomic<uint64_t> _completedCount = 0;
  std::mutex _mutex;
  std::condition_variable _condition;
  std::queue<Job> _jobQueue;
  std::vector<std::thread> _threads;
  void JoinAll();
  void ThreadRun();
};

#endif /* THREAD_POOL_HPP */