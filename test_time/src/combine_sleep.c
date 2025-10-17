#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

double get_time_sec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

//not thread safe
void preciseSleep(double seconds)
{
    static double estimate = 0.005;
    static double mean = 0.005;
    static double m2 = 0.0;
    static long count = 1;

    while (seconds > estimate) {
        double start = get_time_sec();
        usleep(1000); // sleep for 1 ms
        double end = get_time_sec();

        double observed = end - start;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2   += delta * (observed - mean);
        double stddev = (count > 1) ? sqrt(m2 / (count - 1)) : 0.0;
        estimate = mean + stddev;
    }

    // spin lock for the remainder
    double spin_start = get_time_sec();
    while ((get_time_sec() - spin_start) < seconds)
        ; // busy wait
}

// ...existing code...
// ...existing code...

int main(void)
{
    while (1) {
        double start = get_time_sec();
        preciseSleep(0.016); // target ~16ms (60 FPS)
        double end = get_time_sec();
        printf("Slept for %.6f seconds (%.2f ms)\n", end - start, (end - start) * 1e3);
    }
    return (0);
}
