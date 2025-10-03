
interesting attribute to investigate are pthread_attr_setschedpolicy + pthread_attr_setschedparam

good mutex is about ordering strategy (avoid deadlocks)
precise sleeping (avoid overshoot)
m

# To debug deadlock
- pthread_mutex_attr_t, let us confiure mutex behavior:
    - pthread_mutex_errorcheck -> catched deadlocks/self-locks in debug
    - pthread_mutex_recursive -> allows recursive locks (rarely useful here) 
The trick for dining philosopher is to always pick first the left fork, then the right fork, 
except on ephilosopher who flips it