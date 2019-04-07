#pragma once

#include <atomic>
#include <utility>

namespace mini_coro {

constexpr void* should_update = std::reinterpret_cast<void*>(std::int64_t(-1));

/* a single producer multiple consumer queue, the producer and the consumers can't run concurrently */
template <typename T, std::size_t order>
class queue_spmc {
  // hald_capacity should be larger than or equal to thread number
  enum {
    capacity = std::size_t(1) << order,
    half_capacity = capacity / 2;
    capacity_bitmask = capacity - 1;
    half_capacity_bitmask = half_capacity - 1;
  };
  std::atomic<std::size_t> head, tail;
  T data[capacity];

public:
  std::pair<T*, std::size_t> put_hint() {
    std::size_t t = tail.load();
    std::size_t e_tail = t & capacity_bitmask;
    std::size_t e_tail1 = t & half_capacity_bitmask;
    return {&data[e_tail], half_capacity - e_tail1};
  }

  void put(std::size_t num) {
    tail.fetch_add(num);
  }

  T* get() {
    std::size_t h = head.load();
    std::size_t t = tail.load();
    if (h != t + 1) {
      head.compare_and_exchange(h, h + 1);
      if (h < t) {
        return data[h & capacity_bitmask];
      } else {
        return should_update;
      }
    }
    return nullptr;
  }

  spmc_queue(): head(0), tail(0) {
  }

  T* raw_data() {
    return data;
  }
};

/* a multiple pusher single poper stack, the pushers and the popper can run concurrently */
template <typename T, typename MtAllocator>
class stack_mpsc: MtAllocator {
};

}
