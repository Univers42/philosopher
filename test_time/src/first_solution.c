#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

double get_time_sec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

int main(void)
{
    double prevClock = get_time_sec();
    double  nextClock;
    double  deltaTime;
    double  frameClock;
    double  sleepSecs;

    while (1)
    {
        nextClock = get_time_sec();
        deltaTime = nextClock - prevClock;
        printf(" frame time: %.2lf ms\n", deltaTime * 1e3);
        // Simulate updateGame() here if needed
        frameClock = get_time_sec();
        sleepSecs = 1.0 / 60 - (frameClock - nextClock);
        if (sleepSecs > 0)
            usleep((useconds_t)(sleepSecs * 1e6));
        prevClock = nextClock;
    }
    return 0;
}