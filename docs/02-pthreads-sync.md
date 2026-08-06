# 🧵 POSIX Threads: Creation, Joining & Synchronization

## 🧠 Thread vs Mutex — the core difference

These are two completely different tools that work **together**, not in place of
each other. Mixing them up is the #1 source of confusion when starting out.

**Thread — a line of execution.** It's literally code running. Creating a thread
means you now have another "copy" of your program running independently, in
parallel with everything else.

```c
pthread_create(&thread, NULL, coder_routine, &coder_data);
```

**Mutex — a lock.** It doesn't make anything run — it **blocks access**. It's a
tool to stop two threads from touching the same thing at the same time.

```c
pthread_mutex_lock(&dongle->lock);
// only ONE thread at a time can be past this point
dongle->is_available = false;
pthread_mutex_unlock(&dongle->lock);
```

If thread A already holds the lock and thread B calls `pthread_mutex_lock` on the
same mutex, **thread B blocks (waits)** until thread A calls `unlock`.

### The simplest analogy

| Concept | Analogy |
|---|---|
| **Thread** | A person working |
| **Mutex** | A bathroom with a single key |
| **Shared resource** (`is_available`, `should_stop`, `stdout`) | The bathroom itself |

5 people (threads) share one office. There's only one bathroom (shared resource).
Without a key (mutex), two people could try to enter at once and collide. With the
key: whoever arrives first locks the door, uses it, leaves, and only then can the
next person get in.

### Why you always need both together

- **Threads without a mutex** = chaos. Multiple threads reading/writing the same
  variable at once = **race condition** (unpredictable outcome, bugs that are
  nearly impossible to reproduce)
- **A mutex without threads** = pointless. A mutex only matters when there's more
  than one line of execution competing for the same resource

### How this shows up in `codexion.h`

```c
typedef struct dongle
{
	...
	bool				is_available;   // the shared RESOURCE
	pthread_mutex_t		lock;           // the LOCK protecting it
}	t_dongle;

typedef struct coder
{
	...
	pthread_t			thread;         // the PERSON (line of execution)
	...
}	t_coder;

typedef struct simulation
{
	...
	bool				should_stop;    // another shared RESOURCE
	pthread_mutex_t		stop_lock;      // another LOCK
	pthread_mutex_t		log_lock;       // lock for printf
}	t_simulation;
```

Notice the pattern: **every piece of data shared between threads has its own
mutex sitting next to it.** That's the golden rule — if more than one thread can
touch a field, it needs a lock.

**One thing a mutex does NOT do:** it doesn't let a thread wait *efficiently* for
a state change — it only blocks for a brief moment of access. If a coder needs to
wait until a dongle actually becomes free (not just peek at its state once), that's
what condition variables are for — see the section below.

## 📖 Why threads need synchronization

When multiple threads read/write shared data (like a dongle's "available" state)
without coordination, you get a **race condition** — the outcome depends on
unpredictable timing, and bugs become non-reproducible and terrifying.

## 🧬 Thread lifecycle: `pthread_create` & `pthread_join`

Before any locking matters, you need to actually spawn and clean up threads correctly.

### `pthread_create` — spawning a thread

```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                    void *(*start_routine)(void *), void *arg);
```

- `thread` — output param, gets filled with the new thread's ID
- `attr` — usually `NULL` (default attributes are fine here)
- `start_routine` — the function the thread will run; must have signature
  `void *fn(void *arg)`
- `arg` — whatever you want passed into that function (commonly a pointer to a
  per-coder struct with its id, dongles, timings, etc.)

```c
typedef struct s_coder
{
    int id;
    // ... dongle pointers, timing data, shared state pointers ...
}   t_coder;

pthread_t   threads[number_of_coders];
t_coder     coders[number_of_coders];

for (int i = 0; i < number_of_coders; i++)
{
    coders[i].id = i + 1;
    if (pthread_create(&threads[i], NULL, coder_routine, &coders[i]) != 0)
        // handle error — pthread_create returns non-zero on failure,
        // it does NOT set errno
        ;
}
```

**Common pitfalls:**
- Passing a pointer to a **loop variable** that changes before the thread reads it
  (classic bug) — always pass a pointer into an array/struct that outlives the loop
- Checking the return value: `pthread_create` returns an `int` error code directly,
  it does **not** use `errno`
- Every successfully created thread is either **joined** or **detached** — never
  just abandoned, or you leak resources

### `pthread_join` — waiting for a thread to finish

```c
int pthread_join(pthread_t thread, void **retval);
```

Blocks the calling thread until the target thread terminates. This is how `main()`
(or a coordinator) knows all coder threads have actually stopped before the program
exits — critical for clean shutdown and for Valgrind to report accurately (a thread
still technically "running" when `main()` returns can look like a leak or cause a crash).

```c
for (int i = 0; i < number_of_coders; i++)
    pthread_join(threads[i], NULL);   // NULL: we don't care about the return value here
```

**Why order matters in Codexion:** the simulation can stop either because a coder
burned out or because everyone hit `number_of_compiles_required`. Either way, every
coder thread needs a way to **notice** the stop condition (usually a shared
`should_stop` flag protected by a mutex) and exit its loop cleanly — `pthread_join`
only works if the thread routine actually *returns*, it won't forcibly kill a thread
stuck in an infinite loop.

## 🔐 Mutexes (`pthread_mutex_t`)

A mutex (mutual exclusion lock) ensures only **one thread at a time** can execute a
**critical section** — the code that touches shared data.

```c
pthread_mutex_t lock;

pthread_mutex_init(&lock, NULL);

pthread_mutex_lock(&lock);
// critical section: touch shared data here
pthread_mutex_unlock(&lock);

pthread_mutex_destroy(&lock);
```

**Rules of thumb:**
- Lock **right before** touching shared data, unlock **right after**
- Never leave a critical section without unlocking (even on error paths)
- Keep critical sections **short** — a compile-time mutex hog blocks everyone else
- Always init before use and destroy when done (leak/UB otherwise)

**Codexion use case:** each dongle needs its own mutex protecting `is_available`. The
**logging mutex** is separate — it protects the terminal/`stdout` so two threads'
messages never interleave mid-line.

## 🚦 Condition variables (`pthread_cond_t`)

A mutex alone can't make a thread **wait efficiently** for a condition to become
true (e.g., "wait until this dongle is free"). Busy-looping on a lock wastes CPU.
Condition variables solve this: a thread can sleep until another thread signals it.

```c
pthread_mutex_lock(&lock);
while (!dongle_is_available)
    pthread_cond_wait(&cond, &lock);   // atomically unlocks, sleeps, relocks on wakeup
// dongle is now available, mutex is held
pthread_mutex_unlock(&lock);
```

To wake a waiter:
```c
pthread_mutex_lock(&lock);
dongle_is_available = 1;
pthread_cond_signal(&cond);      // wake ONE waiter
// or pthread_cond_broadcast(&cond); to wake ALL waiters
pthread_mutex_unlock(&lock);
```

**Important:** always wait inside a `while` loop, not an `if` — **spurious wakeups**
are allowed by POSIX, and another thread might have grabbed the resource first
between your wakeup and your relock.

### `pthread_cond_timedwait`

Like `pthread_cond_wait`, but wakes up automatically after a deadline even if never
signaled. This is essential for Codexion's **monitor thread**, which must detect
burnout within 10ms — it can't just wait forever for a signal that may never come.

```c
struct timespec ts;
clock_gettime(CLOCK_REALTIME, &ts);
ts.tv_nsec += timeout_ms * 1000000;
// normalize overflow into tv_sec if needed
pthread_cond_timedwait(&cond, &lock, &ts);
```

## 🧠 Mutex vs Condition Variable — when to use which

| Need | Tool |
|---|---|
| Protect shared data from concurrent access | Mutex |
| Wait efficiently for a state change | Condition variable (used together with a mutex) |
| Wake up even if no signal ever comes (deadline) | `pthread_cond_timedwait` |

## 🔗 Why this matters for Codexion

- Each **coder** is its own thread, spawned with `pthread_create` and given a
  pointer to its own struct (id, timings, dongle references)
- Each **dongle** = a mutex + state (`available`/`taken`) + optional cond var for
  waiting coders
- The **logging mutex** guarantees serialized, non-interleaved output
- The **monitor thread** is itself just another thread, running its own loop and
  relying on timed waits (or frequent polling) to catch burnout within the strict
  10ms window
- `pthread_join` is used at the end (on every coder thread **and** the monitor
  thread) so the main thread waits for clean shutdown before the program exits —
  this requires each thread to actually detect the stop condition and return

## 📚 Further reading
- `man pthread_mutex_init`, `man pthread_cond_init`, `man pthread_cond_timedwait`
- Butenhof, D., *Programming with POSIX Threads*
