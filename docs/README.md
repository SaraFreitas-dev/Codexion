# 📚 Study Docs — Codexion (42 Porto)

This folder contains study notes prepared before tackling **Codexion**, a POSIX-threads
project modeled on the classic *dining philosophers* problem — coders competing for
shared USB dongles instead of philosophers competing for forks.

Each file below covers one concept needed to understand and defend the project.

## 🗂️ Topics

| File | Covers |
|---|---|
| [`01-dining-philosophers.md`](./01-dining-philosophers.md) | The classic problem, Coffman's 4 conditions, deadlock vs starvation vs livelock |
| [`02-pthreads-sync.md`](./02-pthreads-sync.md) | `pthread_mutex_t`, `pthread_cond_t`, wait/signal/broadcast, critical sections |
| [`03-scheduling-fifo-edf.md`](./03-scheduling-fifo-edf.md) | FIFO vs Earliest Deadline First scheduling, tie-breaking, real-time scheduling basics |
| [`04-heap-priority-queue.md`](./04-heap-priority-queue.md) | Binary min-heap implemented from scratch, insert/extract-min, use as a scheduler queue |
| [`05-timing-precision.md`](./05-timing-precision.md) | `gettimeofday()` vs `clock_gettime()`, `usleep()` drift, hitting the 10ms burnout window |
| [`06-concurrency-debugging.md`](./06-concurrency-debugging.md) | Valgrind, Helgrind, ThreadSanitizer — catching leaks, races, and deadlocks |

## 🎯 How this maps to Codexion

| Codexion concept | Study topic |
|---|---|
| Dongles = forks, coders = philosophers | `01` |
| Mutex per dongle, cond var for waiting queue | `02` |
| `scheduler` argument (`fifo` / `edf`) | `03` |
| "You must implement a priority queue (heap)" | `04` |
| Burnout log within 10ms, monitor thread | `05` |
| Norm + no leaks + no races during defense | `06` |

## ✅ Suggested study order

1. `01` → understand *why* the problem is hard
2. `02` → learn the C tools to solve it
3. `03` and `04` → the two mandatory scheduling/data-structure requirements
4. `05` → the precision constraints that make testing tricky
5. `06` → how to verify your own solution before submitting
