#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <stdint.h>

#if defined(__GNUC__)
#define THREAD_LOCAL __thread
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define THREAD_LOCAL _Thread_local
#else
#error "No thread-local storage available"
#endif

static inline double get_time_sec(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Microsecond-precision helpers (gettimeofday-based)
static inline uint64_t now_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ull + (uint64_t)tv.tv_usec;
}

// Monotonic wrapper to avoid backward jumps from NTP/time updates
static inline uint64_t now_us_monotonic(void)
{
    static THREAD_LOCAL uint64_t last = 0;
    uint64_t t = now_us();
    if (t < last)
        t = last;
    else
        last = t;
    return t;
}

static inline double us_to_sec(uint64_t us) { return (double)us * 1e-6; }

void preciseSleep(double seconds)
{
    if (!(seconds > 0.0))
        return;

    const uint64_t SPIN_US = 1000; // final 1ms spin for precision
    uint64_t start = now_us_monotonic();
    uint64_t deadline = start + (uint64_t)(seconds * 1e6);

    for (;;)
    {
        uint64_t now = now_us_monotonic();
        if (now >= deadline)
            break;

        uint64_t rem = deadline - now;
        if (rem > SPIN_US)
        {
            // Sleep most of what's left, leave SPIN_US for the busy spin
            useconds_t to_sleep = (useconds_t)(rem - SPIN_US);
            // Bound sleep to reasonable chunks to avoid long oversleeps
            if (to_sleep > 1e6)
                to_sleep = 1e6; // max 1s per usleep
            usleep(to_sleep);
        }
        else
        {
            // Final accurate spin
            while (now_us_monotonic() < deadline)
            { /* busy */
            }
            break;
        }
    }
}

struct stats
{
    double min, max, sum, sumsq;
    long iters;
    int id;
};

struct arg
{
    int id;
    int iters;
    double target;
    struct stats *s;
};

void *worker(void *v)
{
    struct arg *a = v;
    struct stats *s = a->s;
    s->id = a->id;
    s->min = 1e9;
    s->max = 0;
    s->sum = s->sumsq = 0;
    s->iters = 0;

    // Log start and report more frequently
    fprintf(stderr, "thread %d: starting\n", a->id);
    int report_every = (a->iters >= 100) ? (a->iters / 100) : 1;

    for (int i = 0; i < a->iters; ++i)
    {
        uint64_t t0 = now_us_monotonic();
        preciseSleep(a->target);
        uint64_t t1 = now_us_monotonic();
        double elapsed = us_to_sec(t1 - t0);

        if (elapsed < s->min)
            s->min = elapsed;
        if (elapsed > s->max)
            s->max = elapsed;
        s->sum += elapsed;
        s->sumsq += elapsed * elapsed;
        s->iters++;

        if ((i + 1) % report_every == 0)
        {
            double last_ms = elapsed * 1e3;
            double avg_ms = (s->sum / s->iters) * 1e3;
            fprintf(stderr, "thread %d: %d/%d (last=%.6f ms, avg=%.6f ms)\n",
                    a->id, i + 1, a->iters, last_ms, avg_ms);
        }
    }
    fprintf(stderr, "thread %d: done\n", a->id);
    return NULL;
}

int main(int argc, char **argv)
{
    int nthreads = (argc > 1) ? atoi(argv[1]) : 4;
    int iters = (argc > 2) ? atoi(argv[2]) : 5000;
    double target = (argc > 3) ? atof(argv[3]) : 0.016;

    double expected = iters * target;
    fprintf(stderr, "Starting: %d threads, %d iterations each, target=%.3f sec (expected per-thread ~%.1fs)\n",
            nthreads, iters, target, expected);

    pthread_t *t = calloc(nthreads, sizeof(*t));
    struct arg *a = calloc(nthreads, sizeof(*a));
    struct stats *stats = calloc(nthreads, sizeof(*stats));

    for (int i = 0; i < nthreads; ++i)
    {
        a[i].id = i;
        a[i].iters = iters;
        a[i].target = target;
        a[i].s = &stats[i];
        if (pthread_create(&t[i], NULL, worker, &a[i]))
        {
            perror("pthread_create");
            return 1;
        }
    }
    for (int i = 0; i < nthreads; ++i)
        pthread_join(t[i], NULL);

    for (int i = 0; i < nthreads; ++i)
    {
        struct stats *s = &stats[i];
        if (s->iters == 0)
        {
            printf("thread %d: iters=%ld (no samples)\n", s->id, s->iters);
            continue;
        }
        double avg = s->sum / s->iters;
        double var = s->sumsq / s->iters - avg * avg;
        if (var < 0)
            var = 0;
        printf("thread %d: iters=%ld min=%.6fms avg=%.6fms max=%.6fms std=%.6fms\n",
               s->id, s->iters, s->min * 1e3, avg * 1e3, s->max * 1e3, sqrt(var) * 1e3);
    }

    free(t);
    free(a);
    free(stats);
    return 0;
}