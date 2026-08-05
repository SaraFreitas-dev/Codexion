# 🔍 Debugging Concurrency: Valgrind, Helgrind, ThreadSanitizer

## 📖 Why "it worked when I ran it" isn't enough

Concurrency bugs are famously non-deterministic — a race condition might not show
up in 100 runs and then crash the 101st during your defense. Testing alone can't
prove absence of races or leaks; you need tools built for this.

## 🧪 Valgrind (Memcheck) — memory leaks & invalid access

```bash
valgrind --leak-check=full --show-leak-kinds=all ./codexion 5 800 200 200 200 3 100 fifo
```

Detects:
- Memory leaks (unfree'd `malloc`)
- Use-after-free
- Reading uninitialized memory
- Invalid reads/writes (buffer overruns)

**Codexion relevance:** the Norm requires all heap memory to be freed — Valgrind
is how you verify it, especially since the program has threads exiting at
different points (burnout vs normal completion) and multiple code paths that all
need to clean up correctly.

## 🧵 Helgrind — race condition & lock-order detection

```bash
valgrind --tool=helgrind ./codexion 5 800 200 200 200 3 100 edf
```

Detects:
- **Data races**: two threads accessing the same memory without proper locking
- **Lock order inversions**: a common precursor to deadlock (thread A locks
  mutex1 then mutex2, while thread B locks mutex2 then mutex1 — a race waiting
  to become a deadlock)
- Misuse of `pthread` API (e.g., unlocking a mutex you don't hold)

## ⚡ ThreadSanitizer (TSan) — faster alternative

```bash
cc -fsanitize=thread -pthread -g *.c -o codexion_tsan
```

Compiles race detection directly into the binary (instead of running under an
emulator like Valgrind). Generally **faster** than Helgrind and often catches
races Helgrind misses, at the cost of higher memory usage during execution.

## 🧠 Valgrind vs Helgrind vs TSan — when to use which

| Tool | Catches | Speed | Notes |
|---|---|---|---|
| Valgrind (Memcheck) | Leaks, invalid memory access | Slow (~10-30x) | Run this first, always |
| Helgrind | Data races, lock ordering | Slow | Good on systems without sanitizer support |
| ThreadSanitizer | Data races | Faster than Valgrind | Needs recompilation with `-fsanitize=thread` |

## 🎯 A practical testing strategy for Codexion

1. **Functional first:** run with small, easy parameters (2-3 coders, generous
   timeouts) and manually verify the log sequence matches the expected format
2. **Stress test:** run with many coders and tight timeouts to try to force a
   burnout or a race
3. **Valgrind pass:** confirm zero leaks on both the "all compiled enough times"
   exit path and the "someone burned out" exit path
4. **Helgrind/TSan pass:** confirm no races on dongle state, logging, or the
   monitor's shared burnout flag
5. **Repeat under both schedulers** (`fifo` and `edf`) — a bug that only appears
   under `edf`'s deadline comparison logic is easy to miss if you only ever test
   `fifo`

## 🔗 Why this matters for Codexion

- The subject says a crash (segfault, double free) makes the whole project score
  **0** — these tools are how you catch that *before* the evaluator does
- During peer-evaluation, you're explicitly allowed to use your own tests and the
  evaluated peer's tests — having these tool runs ready builds confidence in what
  you present

## 📚 Further reading
- `valgrind --help`, Valgrind Quick Start Guide
- Clang/GCC `-fsanitize=thread` documentation
