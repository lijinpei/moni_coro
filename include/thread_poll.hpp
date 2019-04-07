#pragma once

#include "common.hpp"

#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <deque>
#include <unique_ptr>

#include <semaphore.h>

namespace mini_coro {

struct WatcherInfo {
  Fd;
  Task read_task;
  Task write_task;
};

struct EpollInfo {
  Task read_task;
  Task write_task;
};

class Scheduler {
  std::vector<std::thread> threads;
  std::atomic<bool> should_stop;
  std::unordered_map<Fd, EpollInfo> this_round_events;
  queue_spmc<Task> this_round_tasks;
  stack_mpsc<WatcherInfo> next_round_fds;
  stack_mpsc<Task> next_round_tasks;
  sem_t sem;
public:
  scheduler(int n_threads) {
    thread.reserve(n);
    for (int i = 0; i < n; ++i) {
      threads.empalce_back(execute_thread, this, i);
    }
  }
  
  void working(int thread_id) {
    std::unique_lock<std::mutex> lock(tasks_mutex);
    while (!should_stop.load()) {
    }
  }

  Task poll_new_events() {
    // all the events for this round has been consumed
    this_round_events.clear();
    for (auto& watch_info: next_round_fds) {
    }

    // feed the newly watched events to epoll
    for (auto& epoll_info: this_round_events) {
    }

    // do epoll wait, a new round has start
  }

  void gracefully_shutdown() {
    should_stop.store(true);
  }

  void force_shutdown() {
    threads.resize(0);
  }

  void add_task(Task task) {
  }
};

class scheduler_promise_base {
  scheduler &sch;
public:
  void schedule_on(scheduler& sch_): sch(sch_) {}
  coro_ns::coroutine_handle<void> post_suspend_schedule() {
    sch.schedule();
  }
};

}
