# User-facing api of mini_coro

## thread_pool

### thread_pool creation

### threads management

### tasks management

### thread_pool faqs:

#### 1. Why is there a upper limit on the number of threads a pool can has?

1. There is some limits on the number of hardware cores a computer can have. If you need to make efficient use of Xeon phi or gpu, mini_coro can help you.
2. If you need more threads to make some blocking synchronious api calls, just create another kind of thread pool.

## epoll dispatcher
