#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>

double get_time_sec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

void precise_sleep(double seconds)
{
    // Coarse sleep if needed
    if (seconds > 0.002)
        usleep((useconds_t)((seconds - 0.001) * 1e6)); // leave ~1ms for spin

    // Spin-wait for the remainder
    double start = get_time_sec();
    while ((get_time_sec() - start) < seconds)
        ;
}

int main(void)
{
    while (1)
    {
        double start = get_time_sec();
        precise_sleep(0.016);
        double end = get_time_sec();
        printf("Slept for %.6f seconds (%.2f)ms\n", end - start, (end - start) * 1e3);
    }
}