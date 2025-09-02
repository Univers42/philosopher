#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

pthread_t tid[2];

int counter = 0;

void *trythis(void *arg)
{
    (void)arg;
    unsigned long i = 0;
    counter += 1;
    printf("\n Job %d has started\n", counter);

    for (i = 0; i < (0xFFFFFFFF); i++)
        ;

    printf("\n Job %d has finished\n", counter);

    return NULL;
}

int main()
{
    for (int i = 0; i < 2; i++)
    {
        int err = pthread_create(&(tid[i]), NULL, &trythis, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
    }
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    return (0);
}
