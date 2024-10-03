#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>

double timeGetTime() {
    struct timeval time;
    struct timezone zone;
    gettimeofday(&time, &zone);
    return time.tv_sec + time.tv_usec * 1e-6;
}

const long int VERYBIG = 100000;

int main(void) {
    int i;
    long int j, k, sum;
    double sumx, sumy, total, z;
    double starttime, elapsedtime;
    // ---------------------------------------------------------------------
    // Output a start message
    printf("OpenMP Parallel Timings for %ld iterations using dynamic scheduling (chunk size = 2000) \n\n", VERYBIG);

    // repeat experiment several times
    for (i = 0; i < 6; i++) {
        // get starting time
        starttime = timeGetTime();
        // reset check sum and total
        sum = 0;
        total = 0.0;

// Work loop, do some work by looping VERYBIG times
#pragma omp parallel num_threads(4)
        {

        double start_time_local = timeGetTime();
#pragma omp for private(sumx, sumy, k) reduction(+: sum, total) schedule(dynamic, 2000) nowait
            for (int j = 0; j < VERYBIG; j++) {
                // increment check sum
                sum += 1;

                // Calculate first arithmetic series
                sumx = 0.0;
                for (k = 0; k < j; k++)
                    sumx = sumx + (double)k;

                // Calculate second arithmetic series
                sumy = 0.0;
                for (k = j; k > 0; k--)
                    sumy = sumy + (double)k;

                if (sumx > 0.0)
                    total = total + 1.0 / sqrt(sumx);
                if (sumy > 0.0)
                    total = total + 1.0 / sqrt(sumy);
            }

            printf("Time Elapsed in Thread %d : %10d mSecs Total=%lf Check Sum = %ld\n",
                   omp_get_thread_num(), (int)((timeGetTime() - start_time_local) * 1000), total, sum);
        }
        // get ending time and use it to determine elapsed time
        elapsedtime = timeGetTime() - starttime;

        // report elapsed time
        printf("Time Elapsed %10d mSecs Total=%lf Check Sum = %ld\n\n",
               (int)(elapsedtime * 1000), total, sum);
    }

    // return integer as required by function header
    return 0;
}
// **********************************************************************
