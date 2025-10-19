# Philosophers Output Modes

This document explains the three output modes available in the philosophers program.

## Configuration (in philo.h)

```c
# define DEBUG_MODE       0    // 0 = off, 1 = on
# define ENHANCED_OUTPUT  1    // 0 = standard, 1 = enhanced
```

---

## Mode 1: Standard Output (School Compliant)

**Settings:** `DEBUG_MODE=0` and `ENHANCED_OUTPUT=0`

**Purpose:** Meets 42 School requirements exactly

**Example:**
```
0 1 has taken a fork
0 3 has taken a fork
0 1 is eating
0 3 is eating
200 1 is sleeping
200 3 is sleeping
200 2 has taken a fork
206 2 has taken a fork
206 2 is eating
400 1 is thinking
400 3 is thinking
406 2 is sleeping
```

**Characteristics:**
- Plain text format: `timestamp philosopher_id action`
- No colors, no icons
- Minimal visual processing
- Easy to parse programmatically
- Required for evaluation

---

## Mode 2: Enhanced Output (Recommended)

**Settings:** `DEBUG_MODE=0` and `ENHANCED_OUTPUT=1` *(DEFAULT)*

**Purpose:** Beautiful, readable output for development and presentation

**Example:**
```
🍴      0ms │ Philo   1 │ has taken a fork
🍴      0ms │ Philo   3 │ has taken a fork
🍝      0ms │ Philo   1 │ is eating
🍝      0ms │ Philo   3 │ is eating
😴    200ms │ Philo   1 │ is sleeping
😴    200ms │ Philo   3 │ is sleeping
🍴    200ms │ Philo   2 │ has taken a fork
🍴    206ms │ Philo   2 │ has taken a fork
🍝    206ms │ Philo   2 │ is eating
🤔    400ms │ Philo   1 │ is thinking
🤔    400ms │ Philo   3 │ is thinking
😴    406ms │ Philo   2 │ is sleeping
```

**Characteristics:**
- **Icon indicators:**
  - 🍴 = Taking fork (Yellow)
  - 🍝 = Eating (Green)
  - 😴 = Sleeping (Cyan)
  - 🤔 = Thinking (Magenta)
  - 💀 = Death (Red)
- **Column alignment:** Easy to follow philosopher actions
- **Right-aligned timing:** Better visual scanning
- **Color coding:** Quick action identification
- **Still precise:** All timing information preserved

---

## Mode 3: Debug Mode (Full Diagnostics)

**Settings:** `DEBUG_MODE=1` (ENHANCED_OUTPUT ignored)

**Purpose:** Deep debugging and state analysis

**Example:**
```
[ℹ️  INFO] [     0ms] Philo   1: Acquired left fork
🍴 [     0ms] Philo   1: has taken a fork
[ℹ️  INFO] [     0ms] Philo   1: Acquired right fork
🍴 [     0ms] Philo   1: has taken a fork
🍝 [     0ms] Philo   1: is eating
[ℹ️  INFO] [   200ms] Philo   1: Released forks
😴 [   200ms] Philo   1: is sleeping
🤔 [   400ms] Philo   1: is thinking
[⚠️  WARN] [   750ms] Philo   2: Time since meal: 750ms
[💀 DEATH] [   801ms] Philo   2: Death detected by monitor
💀 [   801ms] Philo   2: died
```

**Characteristics:**
- **Log levels:**
  - `[🔍 DEBUG]` - General debug info
  - `[ℹ️  INFO]` - State changes
  - `[⚠️  WARN]` - Warning conditions
  - `[❌ ERROR]` - Errors
  - `[💀 DEATH]` - Death events
- **Internal operations visible:** Fork locks, state transitions
- **Millisecond precision:** 6-digit formatting
- **State tracking:** See simulation state changes
- **Performance impact:** Slightly slower due to extra logging

---

## State Machine

```
SIM_STATE_OK (0)
    ↓
SIM_STATE_RUNNING (1) ──────→ SIM_STATE_COMPLETED (2)  [All meals done]
    ↓
    └──────────────────────→ SIM_STATE_DEATH (3)       [Philosopher died]

Log Levels:
- SIM_STATE_INFO (100)     - Informational
- SIM_STATE_WARNING (200)  - Warnings
- SIM_STATE_ERROR (300)    - Errors
```

---

## When to Use Each Mode

### Standard Mode
- ✅ 42 School evaluation
- ✅ Automated testing scripts
- ✅ Performance benchmarking
- ✅ Output parsing/analysis

### Enhanced Mode (Default)
- ✅ Development and testing
- ✅ Demonstrations
- ✅ Understanding program behavior
- ✅ Spotting timing issues visually
- ✅ General use

### Debug Mode
- ✅ Finding race conditions
- ✅ Debugging deadlocks
- ✅ Understanding fork acquisition order
- ✅ Tracing death detection
- ✅ Development only

---

## Quick Reference

| Mode | DEBUG_MODE | ENHANCED_OUTPUT | Use Case |
|------|------------|-----------------|----------|
| Standard | 0 | 0 | School evaluation |
| Enhanced | 0 | 1 | Daily use (DEFAULT) |
| Debug | 1 | * | Troubleshooting |

---

## Examples by Scenario

### 1. No Death Scenario (5 philos, 800/200/200)

**Enhanced Output:**
```
🍴      0ms │ Philo   1 │ has taken a fork
🍴      0ms │ Philo   1 │ has taken a fork
🍝      0ms │ Philo   1 │ is eating
🍴      0ms │ Philo   3 │ has taken a fork
🍴    100ms │ Philo   3 │ has taken a fork
🍝    100ms │ Philo   3 │ is eating
😴    200ms │ Philo   1 │ is sleeping
...continues smoothly...
```

### 2. Death Scenario (4 philos, 310/200/100)

**Enhanced Output:**
```
🍴      0ms │ Philo   2 │ has taken a fork
🍴      0ms │ Philo   2 │ has taken a fork
🍝      0ms │ Philo   2 │ is eating
🍴    200ms │ Philo   4 │ has taken a fork
💀    315ms │ Philo   1 │ died
```

### 3. Single Philosopher (1 philo, 800/200/200)

**Enhanced Output:**
```
🍴      0ms │ Philo   1 │ has taken a fork
💀    801ms │ Philo   1 │ died
```

---

## Notes

- Enhanced mode maintains exact timing accuracy
- Colors work on all modern terminals
- Icons require UTF-8 terminal support
- Switching modes only requires recompilation
- All modes are norminette compliant
