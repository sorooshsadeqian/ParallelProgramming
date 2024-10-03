 #include <sys/time.h>
#include <iostream>
#include <cstdlib>
#include <omp.h>

#define VECTOR_SIZE	2 << 20
using namespace std;

int main(int argc, char * argv[]) {
    srand(time(NULL));

	float *numbers = new float [VECTOR_SIZE];

	if (!numbers) {
		cout <<  "Memory allocation error!!\n";
		return 1;
	}

	// Initialize vectors with random numbers
	for (long long int i = 0; i < VECTOR_SIZE; i++) {
		numbers[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100000.0));
	}


    struct timeval serial_start, serial_end;
    gettimeofday(&serial_start, NULL);

    long long int max_index_serial = 0;
    for (long long int i = 0; i < VECTOR_SIZE; i++) {
        if (numbers[i] > numbers[max_index_serial]) {
            max_index_serial = i;
        }
    }

    gettimeofday(&serial_end, NULL);


    struct timeval parallel_start, parallel_end;
    gettimeofday(&parallel_start, NULL);

    long long int max_index_parallel = 0;
    #pragma omp parallel num_threads(4)
    {
        int max_index_local = max_index_parallel;
        #pragma omp for nowait
            for (long long int i = 1; i < VECTOR_SIZE; i++) {        
                if (numbers[i] > numbers[max_index_local]) {
                    max_index_local = i;
                }
            }

        #pragma omp critical 
        {
            if (numbers[max_index_local] > numbers[max_index_parallel]) {
                max_index_parallel = max_index_local;
            }
        }
    }

    
    gettimeofday(&parallel_end, NULL);

    long serial_seconds = (serial_end.tv_sec - serial_start.tv_sec);
    long serial_micros = ((serial_seconds * 1000000) + serial_end.tv_usec) - (serial_start.tv_usec);
    long parallel_seconds = (parallel_end.tv_sec - parallel_start.tv_sec);
    long parallel_micros = ((parallel_seconds * 1000000) + parallel_end.tv_usec) - (parallel_start.tv_usec);
    float speedup = (float)serial_micros / parallel_micros;

    cout << "Serial result: numbers[" << max_index_serial << "] = " << numbers[max_index_serial] << '\n';
    cout << "Parallel result: numbers[" << max_index_parallel << "] = " << numbers[max_index_parallel] << '\n' << endl;

    cout << "Serial Micros: " << serial_micros << '\n';
    cout << "Parallel Micros: " << parallel_micros << '\n';
    cout << "Speedup:" << speedup << '\n';

	return 0;
}