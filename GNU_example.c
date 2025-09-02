#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

typedef struct s_thread_info
{
    pthread_t thread_id;       /* ID returned by pthread_create() */
    int       thread_num;      /* Application-defined thread # */
    char      *argv_string;    /* From command-line argument */
}   t_thread_info;

void    handle_err_en(int en, const char *msg)
{
    errno = en;
    perror(msg);
    exit(EXIT_FAILURE);
}

void handle_err(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static void *thread_start(void *arg)
{
    t_thread_info   *tinfo;
    char            *uarg;

    tinfo = arg;
    printf("Thread %d: top of stack near %p; argv_string=%s\n",
           tinfo->thread_num, &tinfo, tinfo->argv_string);
    uarg = strdup(tinfo->argv_string);
    if (uarg == NULL)
        handle_err("strdup");
    for (char *p = uarg; *p!= '\0'; p++)
        *p = toupper(*p);
    return (uargv);
}

int main(int argc, char **argv)
{
    int             s;
    int             opt;
    void            *res;
    size_t          num_threads;
    pthread_attr_t  attr;
    t_thread_info   *tinfo;

    stack_size = -1;
    while ((opt = getopt(argc, argv, "s:")) != -1)
    {
        switch (opt)
        {
            case 's':
                stack_size = strtoul(optarg, NULL, 0);
                if (errno == ERANGE)
                    handle_err("strtoul");
                break;
            default:
                fprintf(stderr, "Usage: %s [-s stack-size] arg...\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    num_threads = argc - optind;
    s = pthread_attr_init(&attr);
    if (s != 0)
        handle_err_en(s, "pthread_attr_init");
    if (stack_size > 0)
    {
        s = pthread_attr_setstacksize(&attr, stack_size);
        if (s != 0)
            handle_err_en(s, "pthread_attr_setstacksize");
    }
    tinfo = calloc(num_threads, sizeof(t_thread_info));
    if (tinfo == NULL)
        handle_err("calloc");
    for (size_t tnum = 0; tnum < num_threads; tnum++)
    {
        tinfo[tnum].thread_num = tnum + 1;
        tinfo[tnum].argv_string = argv[optind + tnum];
        s = pthread_create(&tinfo[tnum].thread_id, &attr,
                           &thread_start, &tinfo[tnum]);
        if (s != 0)
            handle_err_en(s, "pthread_create");
    }
    s = pthread_attr_destroy(&attr);
    if (s != 0)
        handle_err_en(s, "pthread_attr_destroy");
    for (size_t tnum = 0; tnum < num_threads; tnum++)
    {
        s = pthread_join(tinfo[tnum].thread_id, &res);
        if (s != 0)
            handle_err_en(s, "pthread_join");
        printf("Joined with thread %d; returned value was %s\n",
               tinfo[tnum].thread_num, (char *)res);
        free(res);
    }
    free(tinfo);
    exit(EXIT_SUCCESS);
}
