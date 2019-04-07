#pragma once

#include "common.hpp"

#include <executor.hpp>
#include <unordered_map>

/*
struct notifier {
  void_func_t readable_notifier;
  void_func_t writable_notifier;
  void notify_readable() {
    readable_notifier();
  }
  void notify_writable() {
    writable_notifier();
  }
};
*/

class EpollNotifier {
  int epoll_fd;
  std::unordered_map<int, scheduler*> schedulers;
  std::mutex actions_mutex;
public:
  notifier() {
  }
  void run() {
  }
  int register_scheduler() {
  }
  scheduler* deregister_scheduler(int id) {
  }
}

struct EpollInfo {
  Task read_consumer;
  Task write_consumer;
};
