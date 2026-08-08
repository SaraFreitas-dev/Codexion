# ⏲️ Timing Precision & the 10ms Burnout Window

## 📖 Why timing is tricky here

Codexion has a hard, testable constraint: a burnout log **must appear within 10ms**
of the actual burnout moment. That's tight enough that sloppy timing choices
(wrong clock, coarse sleep granularity, a monitor thread that polls too slowly)
will fail the defense even if the logic is otherwise correct.

## 🕐 `gettimeofday()` vs `clock_gettime()`

| | `gettimeofday()` | `clock_gettime()` |
|---|---|---|
| Resolution | Microseconds (usually) | Nanoseconds (usually) |
| Clock source | Wall-clock (`CLOCK_REALTIME` equivalent) | Selectable: `CLOCK_REALTIME`, `CLOCK_MONOTONIC`, etc. |
| Affected by system time changes (NTP sync, manual clock changes) | Yes | Only if using `CLOCK_REALTIME`; `CLOCK_MONOTONIC` is immune |
| Status | Considered legacy, but explicitly **allowed and recommended** for this project | More modern/precise, needed for `pthread_cond_timedwait`'s `timespec` |

The subject explicitly says real-time measurement via `gettimeofday()` is fine here
for simplicity — this isn't a project about squeezing out nanosecond accuracy, it's
about **correct logic** with reasonable timing.

### What `gettimeofday()` actually gives you

It doesn't hand you a single number — it fills a `struct timeval` with **two**
separate fields:

```c
struct timeval
{
	time_t      tv_sec;    // whole seconds since Jan 1, 1970
	suseconds_t tv_usec;   // microseconds within that second (0-999999)
};
```

```c
struct timeval tv;

gettimeofday(&tv, NULL);
```

You can't subtract two `struct timeval` directly, and you can't log `tv_sec` and
`tv_usec` separately and expect it to match the subject's `timestamp_in_ms` format —
you need to **convert both fields into a single number of milliseconds first**:

```c
long	now_ms;

now_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
```

- `tv_sec * 1000` — turns whole seconds into milliseconds
- `tv_usec / 1000` — turns the leftover microseconds into milliseconds (integer
  division drops anything below 1ms, which is fine at this precision)

### Getting a timestamp relative to simulation start

The subject's example log starts at `0`, not at some huge epoch number like
`1754582400000` — that means every timestamp you print is **relative to when the
simulation began**, not the absolute system clock.

The pattern is always the same, everywhere in the program:
1. **Once**, at the very start of `main` (or wherever you initialize `t_simulation`),
   call `gettimeofday()` and convert it to `start_time_ms` using the formula above —
   store that value in the shared struct
2. **Every time** you need "the current timestamp" for a log line, call
   `gettimeofday()` again, convert it the same way, and **subtract** the stored
   `start_time_ms`

```c
long	elapsed_ms;

elapsed_ms = now_ms - simul->start_time_ms;
```

That `elapsed_ms` is exactly the `timestamp_in_ms` the subject wants printed —
`0` right at the start, `201` two hundred-and-one milliseconds later, and so on.

**Common mistake:** forgetting to convert `tv_sec`/`tv_usec` into the *same* unit
before subtracting, or mixing a value computed with `gettimeofday()` against one
computed with `clock_gettime()` somewhere else — the two clocks aren't guaranteed
to agree on the same "zero point," so subtracting one from the other gives garbage.
Pick one function, use it everywhere, always convert to milliseconds the same way.

## 💤 `usleep()` and its limits

`usleep(microseconds)` suspends a thread for **at least** the given time — the OS
scheduler doesn't guarantee it wakes up exactly on time. Sleeping in long fixed
chunks (e.g., `usleep(time_to_burnout * 1000)`) means your wakeup could be
**anywhere up to that long late**, which risks blowing the 10ms window.

**Practical fix:** don't sleep for the entire duration in one shot. Either:
- Sleep in **small increments** (e.g., 1–5ms) and re-check the elapsed time each loop, or
- Use `pthread_cond_timedwait` with a precise deadline, which wakes up close to
  the actual target time rather than "whenever the sleep happens to end"

## 🛰️ The monitor thread's job

A **separate thread** (not one of the coder threads) is responsible for watching
everyone's last-compile-start timestamp and comparing it against `time_to_burnout`.
It needs to:
1. Check frequently enough that it never misses the 10ms window
2. Log the burnout message immediately upon detection
3. Signal the rest of the program to stop (all coder threads, cleanly)

A polling loop with a short sleep interval (a few ms) is a simple, defensible
approach here — precise enough to hit the deadline, cheap enough not to burn CPU.

## 🧠 Practical checklist

- [ ] Pick one clock function and use it **consistently** everywhere (mixing
      `gettimeofday()` and `clock_gettime()` invites subtle bugs)
- [ ] Never sleep for a whole phase duration in one blocking call if precision matters
- [ ] Monitor thread polls at a granularity well under 10ms (e.g., 1–2ms)
- [ ] Timestamps in logs are relative to the **simulation start**, not absolute epoch time

## 🔗 Why this matters for Codexion

- The whole grading of the "hardest" behavioral requirement (precise burnout
  detection) hinges on timing discipline, not clever locking
- This is also where test flakiness usually comes from — allow yourself a small
  tolerance when testing locally, since hardware/OS scheduling adds jitter, but
  don't design the *logic* around imprecision

## 📚 Further reading
- `man gettimeofday`, `man clock_gettime`, `man usleep`
- POSIX real-time clocks documentation
