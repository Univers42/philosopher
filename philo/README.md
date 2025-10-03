interesting attribute to investigate are pthread_attr_setschedpolicy + pthread_attr_setschedparam

good mutex is about ordering strategy (avoid deadlocks)
precise sleeping (avoid overshoot)
m

# To debug deadlock

- pthread_mutex_attr_t, let us confiure mutex behavior: - pthread_mutex_errorcheck -> catched deadlocks/self-locks in debug - pthread_mutex_recursive -> allows recursive locks (rarely useful here)
  The trick for dining philosopher is to always pick first the left fork, then the right fork,
  except on ephilosopher who flips it

# Timing Considerations

## Death Detection

- Death checker runs every 5ms (balance between responsiveness and overhead)
- With 200 philosophers, 1ms checks cause too much scheduler thrashing
- 5ms is sufficient: even with 800ms time_to_die, we catch deaths within 0.6% tolerance

## Precise Sleep

- Adaptive intervals: coarse when far from target, fine when close
- Prevents overshooting eating/sleeping durations
- Critical for tight timing scenarios (e.g., 5 philos with 800/200/200)

## Last Meal Timestamp

- Updated when entering HUNGRY state (start of wait for forks)
- Gives full time_to_die window to acquire resources
- Prevents starvation deaths during fork contention
- Matches classic dining philosophers semantics: death = inability to start eating
  7801 3 is eating
  DEBUG: Philo 5 died - now=1759503440680 last_meal=1759503439875 diff=805 limit=800 state=1
  7806 5 died
