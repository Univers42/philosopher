# philosopher

- Allow at most four philosophers to be sitting simul   taneously at the table.
- Allow a philosopher to pick up his forks only if both are available. (to do this he must pick them up in a critical section)
- Use an asymmetric solution; that is, an odd philosopher picks up first his left fork and then his right, while an even philosopher picks up first his right fork and then his left fork
Avoiding deadlocks is one of the requirements to do for the philosopher problem.
# KEYWORDS
synchronization problems
concurrent processes
semaphore is simply a variable which is non-negative and shared between threads. This variable is used to solve the `critical section` problem 
and to achieve process synchronization int he multiprocessingenvironment
`wait()` and `signal`
wait is denoted from the `P` -> means `to test`
signal -> `V` which means `to increment`

they come from `proberen` and `verhogen`

All the mmodifications to the integer value of the semaphore in the wait() and signal() operations
must be executed indivisibly. 
That is, when one process modifies the semaphore value, no other process can simulateneously modify that same semaphore value.
# types of semaphore

## binary semaphore:
The value of a binary semaphore can range only between 0 and 1. ON some systems, binary semaphores are known as mutex locks, at they are locks that
provide mutual exclusion


