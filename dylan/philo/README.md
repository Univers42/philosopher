The death test is simply: a philosopher dies if now − last_meal_start > T_DIE. You set last_meal_start at the start of eat.

The key is the worst-case gap between two eat starts. With 2 forks per philosopher, after finishing:

You eat for T_EAT.
You sleep for T_SLEEP.
Then you may have to wait for a neighbor still eating to release a fork. That wait is up to max(0, T_EAT − T_SLEEP).
So a tight upper bound for the gap is: T_gap ≈ T_EAT + max(T_SLEEP, T_EAT)

Survival requires: T_DIE > T_gap (+ small jitter)

Apply to your tests:

4 410 200 200 T_gap = 200 + max(200, 200) = 400; T_DIE = 410 > 400 ⇒ no death.

4 310 200 100 T_gap = 200 + max(100, 200) = 400; T_DIE = 310 < 400 ⇒ one dies.

Why “same proportion” feels misleading: Both have T_DIE − (T_EAT + T_SLEEP) = 10 ms, but the extra wait term is max(T_EAT, T_SLEEP) − T_SLEEP, which is 0 in the first case and 100 ms in the second. That non-linear max() term makes proportions not preserve the outcome.

Practical rule of thumb: Choose T_DIE ≥ T_EAT + max(T_EAT, T_SLEEP) + margin (to cover scheduling/print/jitter).




It’s not a simple “odd → 3×, even → 2×” rule. The schedule depends on ring parity and whether sleep is shorter than eat.

Intuition

A philosopher dies if time_since(last_meal_start) > T_DIE.
The worst-case gap between two “start eating” events is: gap ≈ T_EAT + wait_for_forks + T_SLEEP (sleep can occur before or after waiting).
wait_for_forks depends on contention topology:
Even N: ring splits into two alternating sets; you only ever wait behind one neighbor’s eat.
Odd N: no perfect alternation; between your two turns, both neighbors can eat in sequence, costing an extra T_EAT.
Useful heuristic bounds

Even N: gap_even ≈ T_EAT + max(T_SLEEP, T_EAT)
Odd N: gap_odd ≈ T_EAT + max(T_SLEEP, 2·T_EAT)
Then require T_DIE > gap + margin (for scheduling/printing jitter).

Checks against your cases

4 410 200 200 (N even): 200 + max(200, 200) = 400 < 410 → live.
4 310 200 100 (N even): 200 + max(100, 200) = 400 > 310 → a death.
5 800 200 200 (N odd): 200 + max(200, 400) = 600 < 800 → live.
6 800 300 200 (N even): 300 + max(200, 300) = 600 < 800 → usually OK, but I/O and thread jitter can push effective gap upward; if you see deaths here, raise the margin (e.g., +100–200 ms) or reduce printing.
Why “same proportion” fails The max() term is non-linear. When T_EAT > T_SLEEP, the extra contention cost grows with T_EAT, and ring parity changes whether you pay for one neighbor’s eat (even) or both neighbors’ eats (odd).

