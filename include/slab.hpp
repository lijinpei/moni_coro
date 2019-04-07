#pragma once

#include <atomic>
#include <cstddef>
#include <iostream>
#include <cassert>

namespace mini_coro {

/* a concurrent slab, it is assumed BaseAllocator is mt-safe */
template <std::size_t order, uint16_t buffer_size, typename BaseAllocator>
class slab_mt : BaseAllocator {
  static_assert(sizeof(typename BaseAllocator::value_type) == 1);
  enum {
    page_size = std::size_t(1) << order,
  };
  void* buffer[buffer_size];
  std::atomic<uint16_t> head;
  std::atomic<int> active_count;
  void base_deallocate(void* buf) {
    this->BaseAllocator::deallocate(reinterpret_cast<typename BaseAllocator::pointer>(buf), page_size);
  }
public:
  slabed_pages(BaseAllocator& ba) : BaseAllocator(ba), buffer{}, head(0), active_count(0) {}

  void* allocate() {
    active_count.fetch_add(1);
    uint16_t h = head.load();
    while(h &&  h <= buffer_size) {
      if (head.compare_exchange_weak(h, h - 1)) {
        auto* ptr = reinterpret_cast<std::atomic<void*>*>(&buffer[h - 1]);
        void* ret;
        while (true) {
          ret = ptr->exchange(nullptr);
          if (ret) {
            active_count.fetch_sub(1);
            return ret;
          }
        }
      }
    }
    active_count.fetch_sub(1);
    return this->BaseAllocator::allocate(page_size);
  }

  void deallocate(void* buf) {
    active_count.fetch_add(1);
    uint16_t h = head.load();
    while (h <= buffer_size) {
      if (head.compare_exchange_weak(h, h + 1)) {
        if (h == buffer_size) {
          while (active_count.load() != 1) {
          }
          for (std::size_t i = buffer_size; i; --i) {
            void* buf = buffer[i - 1];
            buffer[i - 1] = nullptr;
            assert(buf);
            base_deallocate(buf);
          }
          head.store(0);
          base_deallocate(buf);
          active_count.fetch_sub(1);
          return;
        } else {
          auto* ptr = reinterpret_cast<std::atomic<void*>*>(&buffer[h]);
          void* nv = nullptr;
          while (!ptr->compare_exchange_weak(nv, buf)) {
            nv = nullptr;
          }
          active_count.fetch_sub(1);
          return;
        }
      }
    }
    active_count.fetch_sub(1);
    base_deallocate(buf);
  }

  void clear() {}
};

/* a single thread slab, BaseAllocator needs not to be mt-safe */
template <typename T>
class slab_st {
};

}
