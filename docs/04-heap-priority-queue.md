# 🌳 Binary Heap as a Priority Queue

## 📖 Why Codexion needs one

Both `fifo` and `edf` scheduling need to answer one question fast, repeatedly:
**"which waiting coder should get this dongle next?"**

A naive approach (loop through all waiters to find the minimum) works but is
`O(n)` per pick. A **binary heap** gives `O(log n)` insert and extract — and,
more importantly, the subject **forbids** using a standard library priority queue
(there isn't one in C anyway, but the point stands: you must build it yourself).

## 🌲 What a binary heap is

A **min-heap** is a complete binary tree where every parent node is **smaller**
(higher priority) than its children. It's usually stored as a flat array, not
actual tree nodes:

```
index:      0   1   2   3   4   5   6
value:      3   5   8   9   7  10  12

        parent(i) = (i - 1) / 2
        left(i)   = 2*i + 1
        right(i)  = 2*i + 2
```

For Codexion, "value" is the **priority key**:
- FIFO mode → arrival timestamp (earlier = smaller = higher priority)
- EDF mode → deadline (`last_compile_start + time_to_burnout`), earlier = higher priority

## ⬆️⬇️ The two core operations

### Insert (`sift-up` / `bubble-up`)
1. Add the new element at the end of the array
2. Compare with its parent; if smaller, swap
3. Repeat until it's in the right place (or reaches the root)

### Extract-min (`sift-down` / `bubble-down`)
1. Save the root (this is the minimum — the answer)
2. Move the **last** element into the root position
3. Compare with children; swap with the smaller child if it violates heap order
4. Repeat until the heap property holds again

Both operations are `O(log n)` because the tree height is `log n`.

## 🧮 Minimal C sketch

```c
typedef struct s_heap
{
    t_entry *data;     // array of {priority, coder_id}
    int      size;
    int      capacity;
}   t_heap;

void    heap_push(t_heap *h, t_entry e);
t_entry heap_pop(t_heap *h);   // removes and returns the minimum
```

Key things to get right:
- **Resizing** the underlying array (or pre-allocate `number_of_coders` slots — the
  max number of simultaneous waiters is bounded and known upfront)
- Comparator logic differs between `fifo` (timestamp) and `edf` (deadline) — this
  can be a function pointer or a simple `if` branch on the `scheduler` argument
- Protecting the heap itself with a mutex, since multiple coder threads may push/pop
  concurrently

## 🔗 Why this matters for Codexion

- "You must implement a priority queue (heap) for FIFO/EDF scheduling (no standard
  library priority queue may be used)" is an explicit mandatory requirement
- The heap is what backs **each dongle's waiting line** — every time a dongle is
  requested, the requester is pushed; every time it's released, the top of the
  heap is popped and woken up (via its condition variable)
- Getting the comparator wrong (e.g., max-heap instead of min-heap, or wrong deadline
  formula) silently breaks fairness without crashing — worth testing explicitly

## 📚 Further reading
- Cormen et al., *Introduction to Algorithms* — heap chapter (build-heap, heapify)
- Any visualization tool for heaps (e.g., VisuAlgo) to see sift-up/down in action
