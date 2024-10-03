#include 	"stdio.h"
#include 	"x86intrin.h"
#include    <sys/time.h>

#define		VECTOR_SIZE		  	1048576

int main (void) {
    struct timeval serial_start, serial_end, parallel_start, parallel_end;

	float *fArray;
	fArray = new float [VECTOR_SIZE];

	if (!fArray) {
		printf ("Memory allocation error!!\n");
		return 1;
	}
	// Initialize vectors with random numbers
	for (long i = 0; i < VECTOR_SIZE; i++)
		fArray[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));
	// Serial implementation
    gettimeofday(&serial_start, NULL);
    float serial_max = fArray[1];
    long serial_max_index = 0;
    for (long i = 0 ; i < VECTOR_SIZE ; i++) {
        if (fArray[i] > serial_max) {
            serial_max = fArray[i];
            serial_max_index = i;
        }
    }
    gettimeofday(&serial_end, NULL);
    long serial_seconds = (serial_end.tv_sec - serial_start.tv_sec);
	long serial_micros = ((serial_seconds * 1000000) + serial_end.tv_usec) - (serial_start.tv_usec);

	// Parallel implementation
    gettimeofday(&parallel_start, NULL);
    __m128 max_pack, fresh_pack, index, incr, packed_max_index, comparisonVec;
    packed_max_index = index = _mm_set_ps (3.0, 2.0, 1.0, 0.0);
    incr  = _mm_set1_ps (4.0);

    max_pack = _mm_loadu_ps(&fArray[0]);
	for (long i = 4; i < VECTOR_SIZE; i+=4) {
		fresh_pack = _mm_loadu_ps (&fArray[i]);
        index = _mm_add_ps(index, incr);
        comparisonVec = _mm_cmpgt_ps(fresh_pack, max_pack);
        packed_max_index = _mm_blendv_ps(packed_max_index, index, comparisonVec);
		max_pack = _mm_max_ps (max_pack, fresh_pack);
    }
    float parallel_max_index, parallel_max = max_pack[0];
    for (int i = 0 ; i < 4 ; i++) {
        if (max_pack[i] > parallel_max) {
            parallel_max = max_pack[i];
            parallel_max_index = packed_max_index[i];
        }
    }
    gettimeofday(&parallel_end, NULL);
    long parallel_seconds = (parallel_end.tv_sec - parallel_start.tv_sec);
	long parallel_micros = ((parallel_seconds * 1000000) + parallel_end.tv_usec) - (parallel_start.tv_usec);
    float speedup = (float)serial_micros / parallel_micros;
    printf("Serial Result: %f\n", serial_max);
    printf("Serial Index: %ld\n", serial_max_index);
    printf("Serial Micros: %ld\n", serial_micros);
    printf("Parallel Result: %f\n", parallel_max);
    printf("Parallel Index: %f\n", parallel_max_index);
    printf("Parallel Micros: %ld\n", parallel_micros);
    printf("Speedup: %f\n", speedup);
	return 0;
}