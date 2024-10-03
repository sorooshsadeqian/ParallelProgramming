#include <sys/time.h>
#include <iostream>
#include "pthread.h"

#define VECTOR_SIZE	(1 << 20)
#define THREAD_COUNT 8
using namespace std;

float *numbers;

void* thread_get_max(void* thread_number) {
    int thread_id = *((int*) thread_number);
    int chunk_size = VECTOR_SIZE / THREAD_COUNT;
    int start_index = chunk_size * thread_id;
    int max_index_parallel;
    for (int i = start_index; i < start_index + chunk_size; i++) {
        if (numbers[i] > numbers[max_index_parallel]) {
            max_index_parallel = i;
        }
    }
    int *result = new int;
    *result = max_index_parallel;
    pthread_exit((void*) result);
}

int main() {
    srand(time(NULL));

    numbers = new float [VECTOR_SIZE];

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

    int thread_ids[THREAD_COUNT];
    pthread_t threads_handlers[THREAD_COUNT];
    
    struct timeval parallel_start, parallel_end;
    gettimeofday(&parallel_start, NULL);
    for (int i = 0; i < THREAD_COUNT; i++) {
        thread_ids[i] = i;
        pthread_create(&threads_handlers[i], NULL, thread_get_max, (void *)&thread_ids[i]);
    }
    void* thread_res;
    int* thread_max_index;
    int max_index_parallel = 0;
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads_handlers[i], &thread_res);
        thread_max_index = (int*) thread_res;
        if (numbers[*thread_max_index] > numbers[max_index_parallel]) {
            max_index_parallel = *thread_max_index;
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


}