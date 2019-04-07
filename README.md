# mini_coro: a simple (but not toy) C++ asynchronous multi-thread network library

mini_coro is based on the following techniques: C++20 corouinte/non-blocking io/multi-threading. It contains a thread poll which runs your coroutines, a epoll dispatcher getting events from linux kernel and dispatch to your coroutines, and asynchronous socket api for your coroutine to co_await.

Its concurrency pattern is as follows:

1. the same coroutine can only be submitted to one thread poll.
2. the same coroutine may be run on different threads at different time between suspend point.
3. the same coroutine may not be run on different threads at the same time.
4. no preemption, make sure your coroutine are nice to others.
5. of course, different coroutines can be run on different threads at the same time.

Some goals:

1. prodution ready
2. batteries-included: asynchronous-dns/asynchronous-file-operator/asynchronous-timer/coroutine-synchronization-primitives
3. simple and efficient
4. support linux epoll and bsd kqueue

Some non-goals:

1. support windows iocp 

See our [api docs](api.md).

See our [dev docs](dev.md)
