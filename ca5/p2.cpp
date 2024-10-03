#include <sys/time.h>
#include <iostream>
#include "pthread.h"

#define VECTOR_SIZE	(1 << 20)

using namespace std;

float *numbers, *numbers_2;

struct thread_args {
    int start;
    int end;
};

void swap(float* a, float* b)
{
	float t = *a;
	*a = *b;
	*b = t;
}


float partition (float arr[], int startingIndex, int finishingIndex) 
{ 
    float pivot = arr[finishingIndex];
    int i = (startingIndex - 1);
  
    for (int j = startingIndex; j <= finishingIndex - 1; j++) 
    {
        if (arr[j] <= pivot) 
        { 
            i++;
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[finishingIndex]); 
    return (i + 1); 
} 
  

void serialQuickSort(float arr[], int startingIndex, int finishingIndex) 
{ 
    if (startingIndex < finishingIndex) 
    { 
        float pivot = partition(arr, startingIndex, finishingIndex); 
        serialQuickSort(arr, startingIndex, pivot - 1); 
        serialQuickSort(arr, pivot + 1, finishingIndex); 
    } 
}


void* threadQuickSort(void* arg) {
    thread_args* arg_str = (thread_args*) arg;
    int startingIndex = arg_str->start;
    int finishingIndex = arg_str->end;
    // cout << " YO " << startingIndex << " AS " << finishingIndex << " VS " << VECTOR_SIZE << " A " << (startingIndex < 0) << " B " << (finishingIndex >= VECTOR_SIZE) << endl;
    if (startingIndex < finishingIndex)
	{
        float pivot = partition(numbers_2, startingIndex, finishingIndex);
        // cout << "pivot done " << startingIndex << " ASASAAS " << finishingIndex << endl;
        // Threading overhead outgrows its speedup as the array becomes smaller.
        // Serial quicksort will be used when the array size becomes smaller than 10000.
        if ((finishingIndex - startingIndex) < 10000) {
            // cout << "shoot " << endl;
            serialQuickSort(numbers_2, startingIndex, pivot - 1);
            serialQuickSort(numbers_2, pivot + 1, finishingIndex);
        }
        else {
            pthread_t thread_1, thread_2;
            thread_args *thread_1_args, *thread_2_args;
            thread_1_args = new thread_args;
            thread_1_args->start = startingIndex;
            thread_1_args->end = pivot - 1;
            pthread_create(&thread_1, NULL, threadQuickSort, (void*) thread_1_args);
            thread_2_args = new thread_args;
            thread_2_args->start = pivot + 1;
            thread_2_args->end = finishingIndex;
            pthread_create(&thread_2, NULL, threadQuickSort, (void*) thread_2_args);
            pthread_join(thread_1, NULL);
            pthread_join(thread_2, NULL);
        }
	}
    pthread_exit(NULL);
}

int main() {
   	numbers = new float [VECTOR_SIZE];
    numbers_2 = new float [VECTOR_SIZE];

	if (!numbers) {
		cout <<  "Memory allocation error!!\n";
		return 1;
	}

	for (int i = 0; i < VECTOR_SIZE; i++) {
		numbers[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100000.0));
        numbers_2[i] = numbers[i];
	}

    struct timeval serial_start, serial_end;
    gettimeofday(&serial_start, NULL);

    serialQuickSort(numbers, 0, VECTOR_SIZE - 1);

    gettimeofday(&serial_end, NULL);

    struct timeval parallel_start, parallel_end;
    gettimeofday(&parallel_start, NULL);

    thread_args* starting_args = new thread_args;
    starting_args->start = 0;
    starting_args->end = VECTOR_SIZE - 1;
    pthread_t sort_thread;
    pthread_create(&sort_thread, NULL, threadQuickSort, (void*) starting_args);
    pthread_join(sort_thread, NULL);
    gettimeofday(&parallel_end, NULL);

    bool isCorrect = true;
    for (int i = 0 ; i < VECTOR_SIZE ; i++) {
        if (numbers[i] != numbers_2[i]) {
            isCorrect = false;
            cout << "Results are not the same!" << endl;
            cout << "Index: " << i << "Serial Result: " << numbers[i] << "Parallel Result: " << numbers_2[i] << endl;
        }
    }
    if (isCorrect) {
        cout << "Parallel and Serial results are equal" << endl;
    }

    long serial_seconds = (serial_end.tv_sec - serial_start.tv_sec);
    long serial_micros = ((serial_seconds * 1000000) + serial_end.tv_usec) - (serial_start.tv_usec);
    long parallel_seconds = (parallel_end.tv_sec - parallel_start.tv_sec);
    long parallel_micros = ((parallel_seconds * 1000000) + parallel_end.tv_usec) - (parallel_start.tv_usec);
    float speedup = (float)serial_micros / parallel_micros;
    cout << "Serial Micros: " << serial_micros << '\n';
    cout << "Parallel Micros: " << parallel_micros << '\n';
    cout << "Speedup:" << speedup << '\n';
}