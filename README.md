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

In this project, the program is a single process that creates one pthread per
coder and one monitor pthread. Shared state is coordinated with mutexes and one
scheduler condition variable: mutexes protect the dongles, coder state, stop
state, scheduler heap, and output, while the condition variable lets waiting
coders sleep until a resource, cooldown, start, or shutdown event changes the
scheduling decision.

## Priority heap

A binary heap is a tree-shaped priority queue stored inside an array. The
highest-priority request remains near the root, so insertion and priority
removal normally take `O(log n)` time.

Codexion implements its own heap because C89 has no standard priority queue.
FIFO compares request arrival order, while EDF compares burnout deadlines.
When a request is inserted or removed, it moves up or down until the heap
priority rule is restored.

Each heap node stores a waiting coder request: the coder pointer, an increasing
arrival order, the current EDF deadline, and temporary selection flags used by
the scheduler. The root is the best request according to the active policy, but
the scheduler also scans the heap to select several non-conflicting requests in
one pass. This allows coder 1 and coder 3, for example, to compile at the same
time if they do not share a dongle.

```text
Coder wants to compile
        |
        v
Create request
  - coder id
  - FIFO arrival order
  - EDF deadline = last_compile_start + time_to_burnout
        |
        v
Insert into binary heap
        |
        v
Heap compares requests
  fifo: lower arrival_order wins
  edf : lower deadline wins
        |
        v
Scheduler selection pass
  1. clear considered/selected flags
  2. repeatedly find the best unconsidered request
  3. select it only if its dongles do not conflict
     with already selected requests
        |
        v
Can this coder run now?
   | yes                         | no
   v                             v
Take both dongles          Sleep on condition variable
Remove request             Wake on release/start/stop/tick
Compile
```

For FIFO, equal urgency is handled naturally by arrival order. For EDF, the
main priority is the earliest burnout deadline; when deadlines are equal, the
current implementation falls back to arrival order.

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
- Data race prevention: every shared field is read and written through its
  corresponding mutex, including dongle availability, cooldown timestamps, coder
  compile counters, coder deadlines, the stop flag, heap state, and logs.
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

Coffman's four deadlock conditions are addressed as follows:

| Coffman condition | Meaning | How Codexion handles it |
| --- | --- | --- |
| Mutual exclusion | A dongle cannot be used by two coders at the same time. | This condition remains true by design and is protected by each dongle mutex. |
| Hold and wait | A coder holds one resource while waiting for another. | Broken: a coder only succeeds when both dongles can be taken together, and does not keep one dongle while waiting for the other. |
| No preemption | A resource cannot be forcibly taken away. | This remains true; coders release dongles after compiling or when stopping. The design avoids depending on preemption. |
| Circular wait | Coders form a cycle while each waits for a resource held by the next. | Prevented by pair arbitration under the scheduler mutex and consistent dongle lock ordering. |

The most important broken condition is hold and wait: no coder can reserve one
dongle and block forever waiting for the second. Circular wait is also avoided
by global scheduling and ordered dongle locking. Data races are handled
separately from deadlocks: mutexes protect the shared memory itself, while the
scheduler policy decides which coder may proceed.

## Thread synchronization mechanisms

The simulation has three kinds of execution contexts. The main thread parses
arguments, initializes shared state, creates all worker threads, opens the start
gate, joins finished threads, and performs cleanup. Each coder thread represents
one coder and repeatedly tries to acquire two neighboring dongles, compile,
debug, and refactor. The monitor thread runs separately and checks whether all
coders have finished or whether one coder has reached its burnout deadline.

The program uses mutexes for ownership of shared memory and a condition variable
for scheduler wakeups. The mutexes are intentionally small in scope: each one
protects one category of state, so unrelated work can continue in parallel.

| Synchronization object | Type | Protected state or responsibility |
| --- | --- | --- |
| `dongle->mutex` | `pthread_mutex_t` | One dongle's `available` flag and `cooldown_until` timestamp. |
| `coder->state_mutex` | `pthread_mutex_t` | One coder's `compiles_done` and `last_compile_start`. |
| `simulation->stop_mutex` | `pthread_mutex_t` | The global `stop` flag checked by coders, monitor, sleeps, and logging. |
| `simulation->scheduler_mutex` | `pthread_mutex_t` | Request heap changes, scheduler selection, start gate `ready`, and condition-variable waits. |
| `simulation->log_mutex` | `pthread_mutex_t` | Complete output lines, preventing interleaved logs. |
| `simulation->scheduler_cond` | `pthread_cond_t` | Wakeup channel for coders waiting on scheduler decisions, cooldown changes, start, stop, or monitor ticks. |

Coder and monitor threads also wait behind the same start gate. The main thread
sets one common start timestamp, resets every initial deadline, sets `ready`,
and broadcasts `scheduler_cond`. This prevents thread-creation time from
affecting burnout deadlines.

Race conditions are prevented by reading and writing shared state only while the
matching mutex is held. For example, the monitor reads each coder's last compile
start through `state_mutex`, and coder threads update that same value through
the same mutex before logging `is compiling`.

## Pthread API used

Codexion uses the pthread API directly. Most pthread functions return `0` on
success and a non-zero error code on failure. Mutex and condition variable
objects are passed by address because the function updates the object itself.

| Function | Signature | What it does | Arguments used in Codexion |
| --- | --- | --- | --- |
| `pthread_create` | `int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);` | Starts a new thread that runs `start_routine(arg)`. | `thread` stores the created thread id; `attr` is `NULL` for default attributes; `start_routine` is `coder_routine` or `monitor_routine`; `arg` points to the coder or simulation data. |
| `pthread_join` | `int pthread_join(pthread_t thread, void **retval);` | Waits until a thread finishes and reclaims its resources. | `thread` is a previously created coder or monitor thread; `retval` is `NULL` because the routines do not return a useful value. |
| `pthread_mutex_init` | `int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);` | Initializes a mutex before it can be locked. | `mutex` points to a mutex field; `attr` is `NULL` for default mutex behavior. |
| `pthread_mutex_lock` | `int pthread_mutex_lock(pthread_mutex_t *mutex);` | Locks the mutex. If another thread owns it, the caller waits. | `mutex` is the lock protecting the shared state about to be read or changed. |
| `pthread_mutex_unlock` | `int pthread_mutex_unlock(pthread_mutex_t *mutex);` | Releases a mutex previously locked by the same thread. | `mutex` is the same lock acquired before the protected work. |
| `pthread_mutex_destroy` | `int pthread_mutex_destroy(pthread_mutex_t *mutex);` | Destroys a mutex after no thread can use it anymore. | Called during cleanup for initialized mutexes. |
| `pthread_cond_init` | `int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);` | Initializes a condition variable. | `cond` is `simulation->scheduler_cond`; `attr` is `NULL` for default behavior. |
| `pthread_cond_wait` | `int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);` | Atomically unlocks `mutex`, sleeps until signaled or broadcast, then locks `mutex` again before returning. | `cond` is `scheduler_cond`; `mutex` is `scheduler_mutex`, which protects the condition being checked. |
| `pthread_cond_broadcast` | `int pthread_cond_broadcast(pthread_cond_t *cond);` | Wakes all threads waiting on the condition variable. | Used when start, stop, dongle release, or monitor ticks may let waiting coders make progress. |
| `pthread_cond_destroy` | `int pthread_cond_destroy(pthread_cond_t *cond);` | Destroys a condition variable after all waiters are gone. | Called during simulation cleanup. |

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
