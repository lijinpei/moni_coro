#pragma once

using coro_ns = std::experimental;

enum {
  pointer_available_bits = 8;
  fd_bits = 32;
  malloc_alignment_bits = 3;
};

enum {
  thread_pool_id_bits = 8;
  epoll_info_bits = 24;
};

using fd_t = int32_t;

std::pair<uint8_t, void*> decode_pointer(void* p) {
}

void* encode_pointer(uint8_t id, void*p) {
}

namespace mini_coro {
  /* a coroutine_handle<void> is basically a pointer to its' frame, which contains the resume, destroy pointer */
  class Task {
    using coro_handle = coro_ns::coroutine_handle<void>;
    coro_handle handle;
  public:
    task(coro_handle h): handle(h) {}
    ~task() {
      handle.destroy();
    }
  };
}

class packed_id_ev_fd {
  uint64_t val;
  packed_id_ev_fd(uint32_t v): val(v) {}
public:
  thread_pool_id_t get_thread_pool_id() {
  }
  uint32_t get_event() {
  }
  int32_t get_fd() {
  }
  packed_id_ev_fd from_raw(uint64_t v) {
    return packed_id_ev_fd(v);
  }
  uint64_t into_raw() {
    return val;
  }
};
