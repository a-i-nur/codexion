*This project has been created as part of the 42 curriculum by aakhmeto.*

# Codexion

## Description

Codexion is a concurrent C simulation in which coder threads share a circular
set of USB dongles. Each coder needs two neighboring dongles to compile, then
debugs and refactors before requesting the dongles again.

The simulation supports FIFO and Earliest Deadline First (EDF) arbitration,
mandatory dongle cooldowns, precise burnout detection, and clean termination
when every coder completes the required number of compiles.

## Dining Philosophers analogy

Codexion is a variation of the classic Dining Philosophers concurrency
problem. Philosophers sit around a table and need two shared forks to eat.
Here, coders sit in a circular workspace and need two shared USB dongles to
compile.

The states and resources correspond as follows:

| Codexion | Dining Philosophers |
| --- | --- |
| compiling | eating |
| debugging | sleeping |
| refactoring | thinking |
| dongles | forks |
| burnout | death |

The analogy demonstrates a common concurrency problem: several independent
workers compete for a limited set of neighboring resources. An incorrect
solution can cause data races, deadlocks, or starvation. Codexion extends the
classic problem with dongle cooldowns and explicit FIFO or EDF scheduling.

## Concurrency theory

A **process** is a running program with its own virtual memory and operating
system resources. Starting `./codexion` creates one process.

A **thread** is an execution path inside a process. Threads in the same process
share allocated memory, global process resources, and open files, but each
thread has its own stack and current execution state. Codexion creates one
thread for each coder and a separate monitor thread.

Threads allow several activities to make progress during the same period. They
may run in parallel on different CPU cores, but the operating system may also
pause and resume them in any order. The program must therefore remain correct
for every possible execution order.

**Asynchronous programming** is a broader programming model in which work can
start without forcing the caller to wait for its completion immediately. It
may be implemented with threads, an event loop, operating system events, or a
combination of these techniques. Threads are an execution mechanism; asynchronous
programming describes how independent work and waiting are organized.

A **mutex** is neither a process nor a thread. It is a synchronization lock
protecting shared data. Only one thread can hold a particular mutex at a time.
Codexion uses mutexes to protect dongles, coder state, the stop flag, scheduler
operations, and output. Without these locks, two threads could modify the same
state simultaneously and create a data race.

A **semaphore** is another synchronization mechanism. Unlike a mutex, which
normally allows one owner into a protected section, a semaphore contains a
counter and may allow a limited number of threads to access a resource at the
same time. Semaphores are useful for resource pools, producer-consumer queues,
and limiting concurrent work. Codexion does not use semaphores because the
subject requires dongle state to be protected with mutexes and uses condition
variables for waiting.

A **condition variable** lets a thread sleep until shared state may have
changed. Waiting coder threads do not need to constantly compete for dongles.
They sleep on the scheduler condition and wake when dongles are released, the
simulation starts, or the simulation stops.

The two scheduling policies define different priority rules:

- **FIFO (First In, First Out):** the request that arrived first has priority,
  like customers waiting in one queue.
- **EDF (Earliest Deadline First):** the coder with the nearest burnout
  deadline has priority. The deadline is calculated as
  `last_compile_start + time_to_burnout`.

FIFO focuses on arrival fairness. EDF focuses on urgency and attempts to serve
the coder that is closest to burnout.

## Priority heap

A binary heap is a tree-shaped priority queue stored inside an array. The
highest-priority request remains near the root, so insertion and priority
removal normally take `O(log n)` time.

Codexion implements its own heap because C89 has no standard priority queue.
FIFO compares request arrival order, while EDF compares burnout deadlines.
When a request is inserted or removed, it moves up or down until the heap
priority rule is restored.

## Instructions

Build the program:

```sh
make
```

Run it with eight mandatory arguments:

```sh
./codexion number_of_coders time_to_burnout time_to_compile \
time_to_debug time_to_refactor number_of_compiles_required \
dongle_cooldown scheduler
```

- `number_of_coders`: number of coder threads and also the number of dongles.
  It must be greater than zero. With one coder, only one dongle exists, so the
  coder cannot acquire two dongles and eventually burns out.
- `time_to_burnout`: maximum time in milliseconds between the beginning of two
  compilations. For the first compilation, the timer starts at the common
  simulation start. If a coder does not start compiling before this deadline,
  the coder burns out.
- `time_to_compile`: compilation duration in milliseconds. The coder must hold
  both neighboring dongles during this complete interval.
- `time_to_debug`: debugging duration in milliseconds. The coder does not hold
  dongles during this phase.
- `time_to_refactor`: refactoring duration in milliseconds. After this phase,
  the coder immediately requests dongles again.
- `number_of_compiles_required`: required number of successful compilations for
  every coder. The simulation stops successfully when all coders reach it.
- `dongle_cooldown`: time in milliseconds during which a released dongle cannot
  be used again. Zero is valid and disables the waiting period.
- `scheduler`: arbitration policy used for competing requests. It must be
  exactly `fifo` or `edf` in lowercase.

All numeric values must be integers. Every value must be greater than zero
except `dongle_cooldown`, which may be zero.

Example:

```sh
./codexion 4 800 200 200 200 3 10 edf
```

Useful development commands:

```sh
make stress
make leaks
norminette coders
```

## Blocking cases handled

- Deadlock prevention: dongles are acquired as an atomic pair under the
  scheduler mutex, and individual dongle mutexes use ascending dongle-ID order.
- Starvation prevention: every request enters a priority heap. FIFO uses request
  arrival order, while EDF uses the coder's next burnout deadline.
- Parallel progress: the scheduler selects a non-conflicting set of requests, so
  coders using separate dongles may compile concurrently.
- Cooldown handling: each released dongle stores an absolute `cooldown_until`
  timestamp and remains unavailable until that time.
- Precise burnout detection: a dedicated monitor checks coder deadlines every
  500 microseconds and stops the simulation on burnout.
- Serialized logging: one mutex protects complete output lines, and normal
  messages are suppressed after the simulation stops.
- Partial initialization and thread failures: initialized resources and started
  threads are tracked and cleaned up before returning an error.

The design breaks Coffman's hold-and-wait condition because a coder never keeps
one dongle while waiting for another. Circular wait is also prevented by the
global pair arbitration and consistent dongle lock ordering.

## Thread synchronization mechanisms

`pthread_mutex_t` protects:

- each dongle's availability and cooldown timestamp;
- each coder's compile count and last compile start;
- the global stop flag;
- scheduler heap operations;
- serialized logging.

`pthread_cond_t` is shared by the scheduler. Coder threads sleep while their
request is not selected or their dongles are unavailable. Releases, shutdown,
simulation start, and monitor ticks broadcast the condition so waiting coders
can re-evaluate their request.

All coder and monitor threads wait behind a start gate. The main thread sets one
common start timestamp, resets every initial deadline, and then broadcasts the
start condition. This prevents thread-creation time from affecting burnout.

## Resources

- POSIX threads: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html
- `pthread_create(3)` and `pthread_join(3)` Linux manual pages
- `pthread_mutex_lock(3)` and `pthread_cond_wait(3)` Linux manual pages
- `gettimeofday(2)` Linux manual page
- Binary heap overview: https://en.wikipedia.org/wiki/Binary_heap
- Coffman deadlock conditions:
  https://en.wikipedia.org/wiki/Deadlock#Necessary_conditions

AI was used as a learning and review assistant for architecture discussion, synchronization explanations, failure-path review, test planning, and English correction.

## What This Project Teaches

Codexion teaches thread lifecycle management, shared-state synchronization,
deadlock and starvation prevention, priority scheduling, precise timing, and
reliable cleanup. The same ideas are used in servers, databases, job queues,
device management, worker pools, and other systems where concurrent tasks
compete for limited resources.
