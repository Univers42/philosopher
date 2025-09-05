# Philosopher (42) — Current Prototype Summary

Overview
- Minimal, working dining philosophers prototype with starvation mitigation, fairness, and debug tooling.
- Portable helpers avoid direct use of POSIX pthread attribute APIs while exposing ft_pthread_attr_* utilities.
- Includes a waiter strategy, parity-based fork locking, odd-count desynchronization, and a monitor for death/completion.

Key design points
- No direct pthread_attr_* use. All attribute helpers operate on an internal registry keyed by pthread_attr_t*.
- Threads are created with pthread_create(..., NULL, ...). Detach semantics (if needed) are applied via runtime logic (pthread_detach).
- Ordered fork locking + waiter (N-1 slots) prevents deadlocks.
- Starvation reduction:
  - Parity-based fork order (even: right-first, odd: left-first).
  - Desynchronization for odd philosopher counts:
    - Even IDs delay start by time_to_eat.
    - Extra thinking after each sleep (time_to_eat ms) when N is odd.
  - Waiter grants slots fairly (round-robin pointer + pending tracking) with urgent override for the most starved pending philosopher.
  - Optional dynamic priority in custom attr, used by the waiter as a hint.

Files touched
- utils/philosopher.h
  - Increased N to 200.
  - Added t_sim (shared state): forks, logging, state mutexes, stop flag, meal goals, waiter fields (slots, slot mutex), fairness fields (urgent_id, next_id, pending[], pending_since[]), and philosophers pointer.
  - Added timing/logging APIs (get_time_ms, ft_usleep_ms, log_status) and a DEBUG logger (dbg + DBG macro).
  - Declared ft_pthread_attr_* utils and t_pthread_attr internal mirror of attributes.
  - Added per-philosopher pthread_attr_t in t_philo (used only by our utils, not passed to pthread_create).
- utils/pthread_attr.c
  - Implemented an internal registry for pthread attributes:
    - ft_pthread_attr_init/destroy, set/get detachstate/guardsize/stack/stacksize/scope/schedpolicy/schedparam/inheritsched, and validate/print.
    - No direct POSIX pthread_attr_* calls; data lives in our registry.
- utils/helpers.c
  - Argument parsing and validation (42-style).
  - Timing helpers: get_time_ms, ft_usleep_ms (stop-aware).
  - Thread-safe logging: log_status.
  - Debug logger: dbg (enabled via -DDEBUG).
- utils/philo.c
  - Simulation setup in main:
    - Allocates and initializes forks, print/state/slot mutexes, waiter slots (N-1), fairness arrays (pending[], pending_since[]), and stores philosophers array in sim.
    - Creates threads with NULL attributes (no real pthread_attr usage).
    - Monitor loop: detects death and completion; chooses urgent_id among pending philosophers; updates custom per-philo priorities as hints to the waiter.
  - Philosopher routine:
    - Odd/even staging to avoid synchronized cycles.
    - Waiter controls concurrent attempts to eat (N-1 slots).
    - Parity-based fork locking, eat/sleep/think loop, progress tracking.
    - Extra think for odd N after sleep to desynchronize further.
  - Fairness utilities:
    - Waiter uses pending[] and next_id for round-robin fairness, urgent_id for priority, and per-philo attr priority as a hint.

Build
- Requires Makefile at philo/ with standard 42 flags.
- Build:
  - make
  - make DEBUG=1 (adds -DDEBUG and enables debug logs to stderr)

Run
- Arguments: ./philo number_of_philos time_to_die time_to_eat time_to_sleep [meals_required]
- Examples:
  - ./philo 5 300 100 60
  - ./philo 5 300 100 60 4
  - ./philo 10 300 100 60

Notes and constraints
- Attribute utilities (ft_pthread_attr_*) are helpers only; they do not affect pthread_create. They serve for validation/printing and as hints in the waiter logic.
- Deadlocks prevented via waiter (N-1) and ordered locking.
- Starvation is mitigated using a combination of:
  - Staggered starts
  - Extra desync for odd counts
  - Fair waiter (pending[], next_id)
  - Urgent selection among pending (closest to time_to_die)
  - Optional priority hints (custom attrs)
- Debugging:
  - Enable DEBUG build to see [DBG ...] traces for waiter decisions, fork order, starvation warnings, and death timelines.

Limitations
- This is a prototype focused on making the current code build and run reliably with 42 constraints and utilities.
- Real pthread scheduling attributes are not applied to threads (by design, to avoid forbidden APIs).

For now, our attributes are useful: they steer who gets a slot first, combined with:
N-1 waiter
Parity-based fork ordering
Odd-count desynchronization
Urgent selection among pending
Round-robin fallback
