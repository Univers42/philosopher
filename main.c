#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define MAX_THREADS 3

typedef struct s_proc {
    pthread_t       tid[MAX_THREADS];  // Fixed: array size should match MAX_THREADS
    int             counter;
    pthread_mutex_t lock;
} t_proc;

// Global variable to pass proc structure to threads
t_proc *g_proc;

void *trythis(void *arg) {
    unsigned long   i;
    int             job_id;
    
    (void)arg;  // Suppress unused parameter warning
    
    pthread_mutex_lock(&g_proc->lock);
    g_proc->counter += 1;
    job_id = g_proc->counter;  // Store job ID locally
    printf("\n Job %d has started\n", job_id);
    pthread_mutex_unlock(&g_proc->lock);  // CRITICAL: Unlock before long computation
    
    // Do the long computation outside the lock to allow concurrency
    i = 0;
    while (i < 0xFFFFFFFF)
        i++;
    
    pthread_mutex_lock(&g_proc->lock);
    printf("\n Job %d has finished\n", job_id);
    pthread_mutex_unlock(&g_proc->lock);
    
    return (NULL);
}

int main() {
    t_proc          proc;
    int             err;
    int             i;
    
    g_proc = &proc;  // Set global pointer
    proc.counter = 0;  // Initialize counter
    
    pthread_mutex_init(&proc.lock, NULL);
    
    // Create all threads
    i = 0;
    while (i < MAX_THREADS)
    {
        err = pthread_create(&(proc.tid[i]), NULL, trythis, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }
    
    // Join all threads properly
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(proc.tid[i], NULL);
    }
    
    pthread_mutex_destroy(&proc.lock);
    return (0);
}
