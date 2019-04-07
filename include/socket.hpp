#pragma once

#include "common.hpp"
#include "actor.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <system_error>

class ipv4_endpoint {
  in_addr_t addr;
  in_port_t port;
public:
  static endpoint make(std::array<uint8_t, 4>& addr_, uint16_t port_) : port(port_), addr{addr_[3], addr_[2], addr[1], addr[0]} {}
};

class awaiter_base {
    template <typename T>
    coro_ns::coroutine_handle<void> await_suspend(coro_ns::coroutine_handle<T> handle) {
      executor& executor = static_cast<scheduler_promise_base>(handle.promise()).get_executor();
      this->re_register(executor);
      return executor.schedule();
    }
};

class tcp_socket {
  int fd;
public:
  static tcp_socket bind_listen(const ipv4_endpoint& ep, int backlog, bool reuse_port, std::error_code& ec) {
    auto make_ec = [&]() {
      ec = std::error_code(errno,  std::system_category());
    };
    tcp_socket ret;
    int& fd = ret.fd;
    fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
      make_ec();
      return ret;
    }
    if (reuse_port) {
      int optval = 1;
      if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
        make_ec();
        return ret;
      }
    }
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ep.addr;
    addr.sin_port = htons(ep.port);
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
      make_ec();
      return ret;
    }
    if (listen(fd, backlog) < 0) {
      make_ec();
      return ret;
    }
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
      make_ec();
      return ret;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) {
      make_ec();
      return ret;
    }
    ec = std::error_code();
    return ret;
  }

  void add_to_actor(std::error_code& ec) {
  }

  struct accept_awaiter {
    int fd;
    executor& exe;
    bool await_ready() {
      return false;
    }
    await_suspend() {
    }
  };

  accept_awaiter accept(executor& exe = executor::get_default()) {
    return accept_awaiter(fd, ec);
  }

  class write_awaiter: public awaiter_base {
    int fd;
    char* data;
    size_t size, size_written;
    std::error_code ec;
    write_awaiter(int fd_, const char* data_, size_t size_): fd(fd), data(data_), size(size_), size_written(0) {}
  public:
    bool await_ready() {
      while (size_writtern < size) {
      }
      return size_written == size;
    }
    std::pair<size_t, std::error_code> awaite_resume() {
      return {size_written, ec};
    }
  };

  template <typename T>
  write_awaiter write(const T& buf) {
    return write_awaiter(fd, buffer_traits<T>::data(buf), buffer_traits<T>::size(buf));
  }

  template <bool read_all>
  class read_awaiter: public awaiter_base {
    int fd;
    char* data;
    size_t size, size_readen;
    std::error_code ec;
  public:
    bool await_ready() {
      while (read_all && size_readen < size || !read_all && !size_readen) {
      }
      return read_all && size_readen == size || !read_all && size_readen;
    }

    std::conditional<read_all, std::error_code, std::pair<size_t, std::error_code>> awaite_resume() {
      if constexpr (read_all) {
        return ec;
      } else {
        return {size_readen, ec};
      }
    }
  };

  // read some *none-zero* bytes
  template <typename T>
  read_awaiter<false> read_some(T& buf) {
    return read_some_awaiter(fd, buffer_traits<T>::data(buf), buffer_traits<T>::size(buf));
  }

  template <typename T>
  read_awaiter<true> read_all(T& buf) {
    return read_some_awaiter(fd, buffer_traits<T>::data(buf), buffer_traits<T>::size(buf));
  }
};

