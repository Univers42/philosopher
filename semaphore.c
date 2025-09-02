#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
/**
 * A simple semaphore implementation using mutex and condition variable.
 * Uses mutex and condition variable for atomic operations.
 */
typedef struct s_semaphore
{
    int             value;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
}   t_semaphore;

void semaphore_init(t_semaphore *s, int initial_value)
{
    s->value = initial_value;
    pthread_mutex_init(&s->mutex, NULL);
    pthread_cond_init(&s->cond, NULL);
}

// Wait operation
void P(t_semaphore *s)
{
    pthread_mutex_lock(&s->mutex);
    while (s->value <= 0)
        pthread_cond_wait(&s->cond, &s->mutex);
    s->value--;         //atomically decrement
    pthread_mutex_unlock(&s->mutex);
}

//Signal Operation (V operation)
void V(t_semaphore *s)
{
    pthread_mutex_lock(&s->mutex);
    s->value++;
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->mutex);
}

void semaphore_destroy(t_semaphore *s)
{
    pthread_mutex_destroy(&s->mutex);
    pthread_cond_destroy(&s->cond);
}

t_semaphore binary_sem;
int shared_resource = 0;

void *worker_thread(void *arg)
{
    int thread_id = *((int *)arg);
    printf("Thread %d: Waiting to enter critical section [alias : shared_resource]\n", thread_id);
    P(&binary_sem); // Wait (P operation)
    printf("Thread %d: Entered critical section\n", thread_id);
    shared_resource++;
    printf("Thread %d: Updated shared_resource to %d\n", thread_id, shared_resource);
    sleep(2);
    printf("Thread %d: Leaving critical section\n", thread_id);
    V(&binary_sem); // Signal (V operation)
    return NULL;
}

int main()
{
    pthread_t threads[5];
    int thread_ids[5] = {1, 2, 3, 4, 5};
    semaphore_init(&binary_sem, 1);
    for (int i = 0; i < 5; i++)
        pthread_create(&threads[i], NULL, worker_thread, &thread_ids[i]);
    for (int i = 0; i < 5; i++)
        pthread_join(threads[i], NULL);
    printf("Final value of shared ressource: %d\n", shared_resource);
    semaphore_destroy(&binary_sem);
    return 0;
}
