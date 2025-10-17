#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <thread>
#include <chrono>
using namespace std;
using namespace std::chrono;

double schedulerPeriod = 1e-3; // 1ms

struct SleepData {
    int sampleSize = 0; // how many times we tried to sleep
    double target  = 0; // how much we aimed to sleep for
    double meanerr = 0; // how much were we off on average
    double stddev  = 0; // how big was the variance between calls
    double minerr  = +INFINITY; // minimum error we had
    double maxerr  = -INFINITY; // maximum error we had
};

SleepData measureAccuracy(void(*sleep)(double sec), double sec, int runs) {
    SleepData data;

	sleep(sec); // do one cold run

    double mean = 0, m2 = 0;
	for (int i = 1; i <= runs; ++i) {
		auto start = high_resolution_clock::now();
		sleep(sec);
        auto end = high_resolution_clock::now();
        double elapsed = (end - start).count() / 1e9;
		
        double delta = elapsed - mean;
        mean += delta / i;
        m2   += delta * (elapsed - mean);

        double error = elapsed - sec;
		data.maxerr = fmax(data.maxerr, error);
        data.minerr = fmin(data.minerr, error);
	}

    data.sampleSize = runs;
    data.target = sec;
    data.meanerr = mean - sec;
    data.stddev = sqrt(m2 / (runs + 1));
    return data;
}

void fpsTest(void(*sleep)(double sec), int runs, FILE *csv) {

    printf("\n--- 60fps game loop test ---\n");

    fprintf(csv, "frame time (ms)\n");
    printf("frame time (ms)\n");

    for (int i = 0; i < runs; ++i) {
        auto start = high_resolution_clock::now();
        sleep(1 / 60.0);
        auto end = high_resolution_clock::now();
        double duration = (end - start).count() / 1e9;
        printf("%.2lf\n", duration * 1e3);
        fprintf(csv, "%.2lf\n", duration * 1e3);
    }
}

void accuracyTest(void(*sleep)(double sec), int runs, FILE *csv) {
	printf("\n--- exact accuracy test ---\n");

    fprintf(csv, "target (ns), error (ns), stddev (ns), min error (ns), max error (ns)\n");
	printf("target (ns), error (ns), stddev (ns), min error (ns), max error (ns)\n");

	for (int64_t ns = 1; ns <= 1'000'000'000; ns *= 10) {
        SleepData dat = measureAccuracy(sleep, ns / 1e9, runs);
        printf("%lld, %.1lf, %.1lf, %.1lf, %.1lf\n", ns,
            dat.meanerr * 1e9,
            dat.stddev  * 1e9,
            dat.minerr  * 1e9,
            dat.maxerr  * 1e9);
        fprintf(csv, "%lld, %.1lf, %.1lf, %.1lf, %.1lf\n", ns,
            dat.meanerr * 1e9, 
            dat.stddev  * 1e9, 
            dat.minerr  * 1e9, 
            dat.maxerr  * 1e9);
    }
}

double getProcessTime() {
    static DWORD pid = GetCurrentProcessId();
    static HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

    FILETIME creation, exit, kernel, user;
    GetProcessTimes(process, &creation, &exit, &kernel, &user);
    uint64_t t1 = (uint64_t(kernel.dwHighDateTime) << 32) | kernel.dwLowDateTime;
    uint64_t t2 = (uint64_t(user.dwHighDateTime) << 32) | user.dwLowDateTime;
    return (t1 + t2) / 1e7;
}

void cpuUsageFPSTest(const char *name, void(*sleep)(double sec), int runs) {
    double startProcess = getProcessTime();
    auto start = high_resolution_clock::now();

    for (int i = 0; i < runs; ++i)
        sleep(1 / 60.0);
    
    double endProcess = getProcessTime();
    double duration = (high_resolution_clock::now() - start).count() / 1e9;
    double cpuUsage = 100 * (getProcessTime() - startProcess) / duration;

    printf("%s, %.1lf\n", name, cpuUsage);
}

void cpuUsageTest(void(*sleep)(double sec), double duration, FILE *csv) {
    printf("\n--- cpu usage test ---\n");

    fprintf(csv, "target (ms), cpu usage (%%)\n");
    printf("target (ms), cpu usage (%%)\n");

    for (int64_t ms = 1; ms <= 25; ++ms) {
        double processStart = getProcessTime();

        auto start = high_resolution_clock::now();
        while ((high_resolution_clock::now() - start).count() / 1e9 < duration)
            sleep(ms / 1e3);
        auto end = high_resolution_clock::now();

        double cpuTime = getProcessTime() - processStart;
        double allTime = (end - start).count() / 1e9;

        printf("%lld, %.1lf\n", ms, 100 * cpuTime / allTime);
        fprintf(csv, "%lld, %.1lf\n", ms, 100 * cpuTime / allTime);
    }
}

void accuratyRobustnessTest(void(*sleep)(double sec), int runs, FILE *csv) {
    printf("\n--- scheduler period robustness test ---\n");

    fprintf(csv, "scheduler period (ms), target (ns), error (ns), stddev (ns), min error (ns), max error (ns)\n");
    printf("scheduler period (ms), target (ns), error (ns), stddev (ns), min error (ns), max error (ns)\n");

    for (int p = 1; p <= 10; ++p) {
        schedulerPeriod = p / 1e3;
        for (int64_t ns = 2'000'000; ns <= 20'000'000; ns += 2'000'000) {
            SleepData dat = measureAccuracy(sleep, ns / 1e9, runs);
            printf("%d, %lld, %.1lf, %.1lf, %.1lf, %.1lf\n", p, ns,
                dat.meanerr * 1e9,
                dat.stddev  * 1e9,
                dat.minerr  * 1e9,
                dat.maxerr  * 1e9);
            fprintf(csv, "%d, %lld, %.1lf, %.1lf, %.1lf, %.1lf\n", p, ns,
                dat.meanerr * 1e9,
                dat.stddev  * 1e9,
                dat.minerr  * 1e9,
                dat.maxerr  * 1e9);
        }
    }
}

void systemSleep(double seconds) {
    this_thread::sleep_for(nanoseconds(int64_t(seconds * 1e9)));
}

void spinLock(double seconds) {
    auto start = high_resolution_clock::now();
    while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

void preciseSleep(double seconds) {
    using namespace std;
    using namespace chrono;

    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds > estimate) {
        auto start = high_resolution_clock::now();
        this_thread::sleep_for(milliseconds(1));
        auto end = high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2   += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = high_resolution_clock::now();
    while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

void timerSleep(double seconds) {
    using namespace std;
    using namespace chrono;

    static HANDLE timer = CreateWaitableTimerA(NULL, FALSE, NULL);
    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;
    
    while (seconds - estimate > 1e-7) {
        double toWait = seconds - estimate;
        LARGE_INTEGER due;
        due.QuadPart = -int64_t(toWait * 1e7);
        SetWaitableTimerEx(timer, &due, 0, NULL, NULL, NULL, 0);
        auto start = high_resolution_clock::now();
        WaitForSingleObject(timer, INFINITE);
        auto end = high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double error = observed - toWait;
        double delta = error - mean;
        mean += delta / count;
        m2   += delta * (error - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = high_resolution_clock::now();
    while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

void fakeSystemSleep(double seconds) {
    spinLock(seconds);
    double untilNextSchedule =
        schedulerPeriod * (rand() / (double)RAND_MAX);
    spinLock(untilNextSchedule);
    while ((rand() / (double)RAND_MAX) < 0.05)
        spinLock(schedulerPeriod);
}

void fakePreciseSleep(double seconds) {
    using namespace std;
    using namespace chrono;

    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds > estimate) {
        auto start = high_resolution_clock::now();
        fakeSystemSleep(1e-3);
        auto end = high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2 += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    spinLock(seconds);
}

int main() {
    //FILE *fpsSystem = fopen("fps-system-sleep.csv", "wt");
    //FILE *fpsSpinLock = fopen("fps-spin-lock.csv", "wt");
    //FILE *fpsPreciseSleep = fopen("fps-precise-sleep.csv", "wt");
    //FILE *fpsTimerSleep = fopen("fps-timer-sleep.csv", "wt");
    //FILE *accuracySystem = fopen("accuracy-system-sleep.csv", "wt");
    //FILE *accuracySpinLock = fopen("accuracy-spin-lock.csv", "wt");
    //FILE *accuracyPreciseSleep = fopen("accuracy-precise-sleep.csv", "wt");
    //FILE *accuracyTimerSleep = fopen("accuracy-timer-sleep.csv", "wt");
    //FILE *cpuPreciseSleep = fopen("cpu-precise-sleep.csv", "wt");
    //FILE *cpuTimerSleep = fopen("cpu-timer-sleep.csv", "wt");
    FILE *robustnessPreciseSleep = fopen("robustness-precise-sleep.csv", "wt");

    //fpsTest(systemSleep, 1000, fpsSystem);
    //fpsTest(spinLock, 1000, fpsSpinLock);
    //fpsTest(preciseSleep, 1000, fpsPreciseSleep);
    //fpsTest(timerSleep, 1000, fpsTimerSleep);
    //
    //accuracyTest(systemSleep, 100, accuracySystem);
    //accuracyTest(spinLock, 100, accuracySpinLock);
    //accuracyTest(preciseSleep, 100, accuracyPreciseSleep);
    //accuracyTest(timerSleep, 100, accuracyTimerSleep);
    //

    //printf("\n--- game loop cpu usage test ---\n");
    //cpuUsageFPSTest("system sleep", systemSleep, 1000);
    //cpuUsageFPSTest("spin lock", spinLock, 1000);
    //cpuUsageFPSTest("precise sleep", preciseSleep, 1000);
    //cpuUsageFPSTest("timer sleep", timerSleep, 1000);

    //cpuUsageTest(preciseSleep, 10, cpuPreciseSleep);
    //cpuUsageTest(timerSleep, 10, cpuTimerSleep);

    accuratyRobustnessTest(fakePreciseSleep, 20, robustnessPreciseSleep);

    return 0;
}