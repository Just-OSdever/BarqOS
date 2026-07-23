# Page Frame Allocator

## 📅 June 30, 2026

> I got an idea to create my own memory allocator!

### BCA — Black Cursor Allocator

#### The Logic

- Keep a moving cursor through physical memory.
- Store every reserved or allocated region inside a **Black List**.
- When a collision occurs, jump to the end of that block.
- Continue searching until a free range is found.

### Goals

- No bitmap scanning.
- No free list.
- Very low fragmentation.
- Extremely simple algorithm.
- Easy to debug.
- Fast searching.

---

## 📅 July 1–5, 2026

### Initial PMM

Implemented the first version of the Page Frame Allocator.

Added:

- Limine Memory Map support.
- Higher Half Direct Map (HHDM).
- `to_phys()`
- `to_virt()`
- Black List initialization.

---

## 📅 July 6–8, 2026

### Building the Black List

Implemented:

- Counting every non-usable memory region.
- Storing each reserved range inside the Black List.
- Reserving the allocator's own array as another Black Block.

First successful allocations appeared.

---

## 📅 July 9–10, 2026

### Dynamic Black List

Realized that every allocation creates another Black Block.

Implemented:

- Dynamic Black List resizing.
- Array relocation.
- Memory copying.
- Updating allocator metadata.

Solved multiple bugs related to:

- wrong indexes
- wrong copy size
- cursor corruption

---

## 📅 July 11–13, 2026

### Physical vs Virtual

Most Page Faults were caused by mixing physical and virtual addresses.

Added helper functions:

```cpp
to_phys()
to_virt()
```

Audited almost every memory access.

Several #PFs disappeared.

---

## 📅 July 14, 2026

### Limine isn't enough

Discovered an important limitation.

The Limine Memory Map only describes memory during boot.

After initialization it doesn't know anything about allocations created by the allocator.

Decision:

> The Black List becomes the only source of truth after boot.

---

## 📅 July 15–17, 2026

### Debugging

Spent several days inside GDB.

Fixed:

- NULL pointer bugs
- Off-by-one errors
- Infinite loops
- Collision detection
- Cursor updates
- Wrong resize logic
- Dynamic array bugs

Finally...

The allocator successfully returned valid allocations.

🎉

---

## 📅 July 18–19, 2026

### Cleanup

Refactored:

- Collision detection
- Allocation loop
- Dynamic resizing

Started measuring allocation time using PIT ticks.

---

## 📅 July 20, 2026

### Bookmarks

Started designing a second allocation strategy.

Instead of permanently skipping memory after collisions:

- Save the previous cursor.
- Jump over the obstacle.
- Return later to continue searching.

The goal is to preserve skipped free regions.

---

## 📅 July 21, 2026 - 📅 July 23, 2026

### Bookmark implementation

Started implementing bookmarks directly inside RAM.

Current idea:

- Store the old cursor immediately after the collided block.
- Jump forward.
- Before returning from the allocator:
  - Restore the old cursor.
- Return the allocated pointer.
- The kernel is free to overwrite the bookmark afterwards.

Today's funniest bug:

```cpp
start = (uintptr_t)bookmark;
```
Spent a while debugging...

Then realized I was assigning the bookmark **to** `start` instead of writing `start` **into** the bookmark. 🤣
The corresct idea is:
```cpp
*(uintptr_t *)to_virt(bookmark_addr) = start;
```

### Fixing some #PF
there was  #PF after the first 5 allocation test (in the sixth)

##### **The reason** :
I forgot to Update **to new array addr in blacklist**, in `void get_new_array_and_add(uint64_t start , uint64_t end)` function

## Started making tests
#### the tests
    basic_test();
    pattern_test();
    overlap_test();
    alignment_test();
    random_sizes_test();
    stress_test();
    huge_alloc_test();

failed in :
alignment_test

---

## Current Status

### ✅ Completed

- Black List
- Dynamic resize
- Collision detection
- Physical / Virtual translation
- Working PMM
- Allocation timing

### 🚧 In Progress

- Bookmark allocator
- `free()`
- Stress testing
- Performance comparison
- Fragmentation benchmarks