#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

double get_time_sec()
{
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + tv.tv_usec / 1e6);
}

void spin_lock()
{
    const double frame_duration = 1.0 / 60.0; // 60 FPS
    double next_frame_time = get_time_sec();

    while (1)
    {
        double now = get_time_sec();
        double delta_time = now - next_frame_time + frame_duration;
        printf(" frame time: %.2lf ms\n", delta_time * 1e3);

        next_frame_time += frame_duration;

        double sleep_time = next_frame_time - get_time_sec();
        if (sleep_time > 0.002) // sleep if more than 2ms left
            usleep((useconds_t)((sleep_time - 0.001) * 1e6)); // leave ~1ms for spin

        // Spin-wait for the remaining time
        while (get_time_sec() < next_frame_time)
            ;
    }
}

int main(int argc, char **argv)
{
    spin_lock();
    return 0;
}