# Philosophers

In this project, I learnt about multithreaded programs, multi-processed programs and concurrency. 
The repo offers two solutions for the famous Dining Philosopher problem from Dijkstra. 
The project is split into two parts; the mandatory part where the philosophers are represented as threads and the bonus part where philosophers are processes. 
Here are the main differences between both parts.

***Mandatory Part*** <br />
The forks are represented by mutexes, therefore avoiding the risks of duplicated forks and data races.
The function checking if any philosopher died or ate enough is done by the main execution thread, whilst the other threads are in the routine.

***Bonus Part*** <br />
The forks are represented as semaphores.
The function checking if any philosopher died is embedded in the sleep function. Once a philosopher dies (i.e. a child process), it passes its exit code to his parent through waitpid and its macros.
In the case of DEATH = the parent analyses the received exit code and proceeds to kill of the remaining children processes.

***Learnings*** <br />
Upon thread creation, the start time of threads are not syncronized, this may lead to deadlocks if critical sections are not properly protected.
Some threads might be more "active" than others. To avoid starvation, a scheduler must be implemented to evenly distribute the CPU processing power to each thread.
Data races occur once a variable is read and written to by two different processes or threads. Mutual exclusions whether as mutexes or semaphores, protect variable from being accessed by multiple threads/processes at the same time. They make a program safe.

## Installation

```bash
git clone git@github.com:PGCL1/Philosophers
cd Philosophers/philo
make
```

## Usage

```bash
./philo philo_number time_to_die time_to_eat time_to_sleep [number_of_meals]
```

> [!NOTE]  
> All times passed as arguments should be in milliseconds, that way you won't have to wait ages to see the output :). In the example below, no one shoud die (see below): <br />
> ```bash
> ./philo 20 800 200 200 [5]
> ```

## Thank you

Thank you to the people that wrote these! Here's some ressources that really helped me doing this project:
* [Dining Philosopher Problem - Medium Article](https://medium.com/great-moments-in-computing-history/the-dining-philosophers-2e3da2847bac)
* [Threads & Concurrency](https://www.baeldung.com/cs/category/concurrency/tag/threads)
* [Scheduling Computing](https://en.wikipedia.org/wiki/Scheduling_(computing))

## My notes

All in all, this project for me, was a great learning path into concurrent programming, its advantages and drawbacks. It was also a stepping stone into understand deterministic programming and non-deterministic programming. I enjoyed my times with threads and processes and hope to see more in the future. Peace!

G
