# ⏱️ Scheduling Policies: FIFO vs EDF

## 📖 The problem they solve

When multiple coders request the **same dongle** at the same time, someone has to
decide who gets it first. Codexion requires this arbitration to be **fair** and
**deterministic**, following one of two policies chosen at runtime.

## 🥇 FIFO — First In, First Out

The simplest fairness policy: whoever **asked first**, gets served first.

- Each dongle needs a **request queue** (or timestamped requests)
- Requests are granted strictly in arrival order
- Simple to reason about, simple to implement
- **Downside:** doesn't account for *urgency* — a coder about to burn out waits
  exactly as long as everyone else in line

## 🚨 EDF — Earliest Deadline First

A classic **real-time scheduling** algorithm: whoever has the **most urgent deadline**
gets served first, regardless of arrival order.

**Deadline formula (Codexion):**
```
deadline = last_compile_start + time_to_burnout
```

- Every coder implicitly carries a deadline: the moment they'll burn out if they
  don't start compiling again
- When multiple coders request a dongle, the one with the **earliest** deadline wins
- This is the same principle used in real-time operating systems to schedule tasks
  with hard time constraints

**Why EDF matters:** it directly targets **starvation prevention** — a coder who's
been waiting a long time (and is close to burnout) gets prioritized over one who
just started their cycle.

## ⚖️ Tie-breaking

The subject notes that identical deadlines "may rarely occur in practice," but a
**deterministic tie-breaker is still required** — e.g., fall back to coder number,
or to arrival order (FIFO) as a secondary key. Without this, two coders with equal
deadlines could be handled inconsistently between runs, which breaks reproducibility
during testing/defense.

## 🧠 FIFO vs EDF — comparison

| | FIFO | EDF |
|---|---|---|
| Fairness basis | Arrival order | Urgency (deadline) |
| Complexity | Simple queue | Priority queue (heap) needed |
| Real-time guarantees | None | Can prevent starvation under feasible load |
| Used in | Basic job queues, printers | Real-time OS task schedulers, Codexion's `edf` mode |

## 🔗 Why this matters for Codexion

- The `scheduler` argument (`fifo` or `edf`) changes **how dongle requests are
  ordered**, not the dongle logic itself
- EDF mode is what makes the **liveness guarantee** ("no coder should starve, given
  feasible parameters") provable — it's the textbook algorithm for exactly this claim
- Both policies need an underlying **priority queue** to efficiently pick the next
  request (see `04-heap-priority-queue.md`)

## 📚 Further reading
- Liu, C. L. & Layland, J. (1973) — *Scheduling Algorithms for Multiprogramming in a
  Hard-Real-Time Environment* (the original EDF paper)
- Real-time systems textbooks covering rate-monotonic vs EDF scheduling
