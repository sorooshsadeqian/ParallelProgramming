 #include <sys/time.h>
#include <iostream>
#include <cstdlib>
#include <omp.h>

#define VECTOR_SIZE	1 << 20
using namespace std;

void swap(float* a, float* b)
{
	float t = *a;
	*a = *b;
	*b = t;
}


float partition (float arr[], long long int startingIndex, long long int finishingIndex) 
{ 
    float pivot = arr[finishingIndex];
    long long int i = (startingIndex - 1);
  
    for (long long int j = startingIndex; j <= finishingIndex - 1; j++) 
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
  

void serialQuickSort(float arr[], long long int startingIndex, long long int finishingIndex) 
{ 
    if (startingIndex < finishingIndex) 
    { 
        float pivot = partition(arr, startingIndex, finishingIndex); 
        serialQuickSort(arr, startingIndex, pivot - 1); 
        serialQuickSort(arr, pivot + 1, finishingIndex); 
    } 
}

void parallelserialQuickSort(float arr[], long long int startingIndex, long long int finishingIndex) {
    if (startingIndex < finishingIndex)
	{
        float pivot = partition(arr, startingIndex, finishingIndex);

        // Threading overhead outgrows its speedup as the array becomes smaller.
        // Serial quicksort will be used when the array size becomes smaller than 10000.
        if ((finishingIndex - startingIndex) < 10000) {
            serialQuickSort(arr, startingIndex, pivot - 1);
            serialQuickSort(arr, pivot + 1, finishingIndex);
        }

        else {
            #pragma omp task firstprivate(arr,startingIndex,pivot)
                {
                    parallelserialQuickSort(arr,startingIndex, pivot - 1);
                }

            #pragma omp task firstprivate(arr, finishingIndex,pivot)
                {
                    parallelserialQuickSort(arr, pivot + 1, finishingIndex);
                }
        }
	}
}

int main(int argc, char * argv[]) {
    srand(time(NULL));

	float *numbers = new float [VECTOR_SIZE];
    float *numbers_2 = new float [VECTOR_SIZE];

	if (!numbers) {
		cout <<  "Memory allocation error!!\n";
		return 1;
	}

	for (long long int i = 0; i < VECTOR_SIZE; i++) {
		numbers[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100000.0));
        numbers_2[i] = numbers[i];
	}

    struct timeval serial_start, serial_end;
    gettimeofday(&serial_start, NULL);

    serialQuickSort(numbers, 0, VECTOR_SIZE - 1);

    gettimeofday(&serial_end, NULL);

    struct timeval parallel_start, parallel_end;
    gettimeofday(&parallel_start, NULL);

    #pragma omp parallel num_threads(4)
    {
        #pragma omp single nowait 
        {
            parallelserialQuickSort(numbers_2, 0, VECTOR_SIZE - 1);
        }
    }
    
    gettimeofday(&parallel_end, NULL);

    bool isCorrect = true;
    for (long long int i = 0 ; i < VECTOR_SIZE ; i++) {
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

	return 0;
}