# SystemC TLM Assignment

This repository contains the implementations for the three tasks of the SystemC/TLM assignment.

## Repository

GitHub Repository: https://github.com/k4beer/A_1

---

## Prerequisites

Make sure the following are installed:

* `g++`
* SystemC
* TLM (included with SystemC)

The compilation commands below assume that SystemC is already installed and available to the linker as `-lsystemc`.

---

## 1. Clone the Repository

Clone the repository using:

```bash
git clone https://github.com/k4beer/A_1.git
cd A_1
```

---

# Task 1

Task 1 uses the following source files:

* `SC_main.cpp`
* `Software.cpp`
* `CPU.cpp`
* `Memory.cpp`

### Compile

```bash
g++ SC_main.cpp Software.cpp CPU.cpp Memory.cpp -o sim -lsystemc
```

### Run

```bash
./sim
```

---

# Task 2

Task 2 uses a separate SystemC main file while reusing the `Software`, `CPU`, and `Memory` implementations.

Source files:

* `SC_main_T2.cpp`
* `Software.cpp`
* `CPU.cpp`
* `Memory.cpp`

### Compile

```bash
g++ SC_main_T2.cpp Software.cpp CPU.cpp Memory.cpp -o sim -lsystemc
```

### Run

```bash
./sim
```

---

# Task 3 – Temporal Decoupling

Task 3 implements the temporal-decoupling version using separate Software and CPU modules.

Source files:

* `SC_main_temporal.cpp`
* `SoftwareTemporal.cpp`
* `CPUTemporal.cpp`
* `Memory.cpp`

### Compile

```bash
g++ SC_main_temporal.cpp SoftwareTemporal.cpp CPUTemporal.cpp Memory.cpp -o sim -lsystemc
```

### Run

```bash
./sim
```

---

## Quick Reference

| Task   | Compilation Command                                                                         | Run     |
| ------ | ------------------------------------------------------------------------------------------- | ------- |
| Task 1 | `g++ SC_main.cpp Software.cpp CPU.cpp Memory.cpp -o sim -lsystemc`                          | `./sim` |
| Task 2 | `g++ SC_main_T2.cpp Software.cpp CPU.cpp Memory.cpp -o sim -lsystemc`                       | `./sim` |
| Task 3 | `g++ SC_main_temporal.cpp SoftwareTemporal.cpp CPUTemporal.cpp Memory.cpp -o sim -lsystemc` | `./sim` |

> **Note:** Each compilation command produces an executable named `sim`. If you compile a different task, the previous `sim` executable will be overwritten.
