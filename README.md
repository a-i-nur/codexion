*This project has been created as part of the 42 curriculum by aakhmeto.*

# Codexion

## Description

Codexion is a concurrent C simulation in which coder threads share a circular
set of USB dongles. Each coder needs two neighboring dongles to compile, then
debugs and refactors before requesting the dongles again.

The simulation supports FIFO and Earliest Deadline First (EDF) arbitration,
mandatory dongle cooldowns, precise burnout detection, and clean termination
when every coder completes the required number of compiles.

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

The scheduler must be `fifo` or `edf`. All time values are milliseconds.

Example:

```sh
./codexion 4 800 200 200 200 3 10 edf
```

Useful development commands:

```sh
make test_errors
make test_edge
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