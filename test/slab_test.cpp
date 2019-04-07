#include "slab.hpp"
#include <memory>
#include <thread>
#include <vector>
#include <iostream>

const int order = 1;
const int buf_size = 8;
const int thread_num = 16;
const int outer_loop = 1000;
const int inner_loop = 20;

int main() {
  std::allocator<char> base_allocator;
  mini_coro::slabed_pages<order, buf_size, std::allocator<char>> slab(base_allocator);
  std::vector<std::thread> threads;
  for (int i = 0; i < thread_num; ++i) {
    threads.emplace_back([&]() {
      void* page[inner_loop];
      for (int i = 0; i < outer_loop; ++i) {
        for (int j = 0; j < inner_loop; ++j) {
          page[j] = slab.allocate();
        }
        for (int j = 0; j < inner_loop; ++j) {
          slab.deallocate(page[j]);
        }
      }
    });
  }
  for (int i = 0; i < thread_num; ++i) {
    threads[i].join();
  }
}
