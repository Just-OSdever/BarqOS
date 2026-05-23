# BarqOS (BQ0 Beta) 🚀⚡

Welcome to **BarqOS** (Codename: **BQ0**), a minimal, ultra-fast 64-bit operating system built completely from scratch using C and x86_64 Assembly. 

BarqOS is developed under the umbrella of **Axiomil** as a modern hobbyist kernel designed for extreme performance, minimal resource consumption, and bare-metal reliability.

> ⚠️ **Note:** This is the open-source **BQ0 Beta** release, focused strictly on core kernel infrastructure, kernel-space stability, and developer testing. Future commercial/enterprise layers will remain proprietary.

---

## 🧠 The Vision
Most modern operating systems are bloated. BarqOS aims to prove that a sub-megabyte kernel, written with clean fundamentals, can deliver lightning-fast execution. The ultimate goal of the Axiomil ecosystem is to create a highly optimized environment where developers have total control over the hardware.

## 🛠️ Current Features (BQ0 Setup)
The BQ0 Beta strips away the overhead to focus on the absolute fundamentals:
* **The Boot & Core:** Native 64-bit kernel booting seamlessly.
* **Hardware Interrupts:** Fully custom IDT (Interrupt Descriptor Table) with robust PIC remap and hardware handlers (including standard Timer & Keyboard support).
* **Interactive Shell:** A minimalist command-line shell interface built directly into kernel space for direct testing.
* **Good FS (File System):** A custom, reliable file system designed for clean data block allocation, low latency, and corruption-free read/write operations.
* **Zero UI Overhead:** Purely `CLI / Text-Mode` to save 100% of the hardware resources for actual logic processing.

---

## 💻 Technical Specifications & Benchmarks
* **Language:** 100% Custom Assembly (x86_64) & Freestanding C.
* **Boot Environment:** Limine compatible.
* **Memory Footprint:** Under [??] MB of RAM at idle!

---

## 🚀 How to Run & Test
To test the BQ0 Beta on your local machine using the **QEMU emulator**, follow these steps:

1. Clone the repository:
```bash
   git clone [https://github.com/Marawan-games/BarqOS.git](https://github.com/Marawan-games/BarqOS.git)
```
2. Check you have qemu:
```bash
  qemu-system-x86_64 --version
```
if you don't have it, just search how to install it on your OS
