#include "stdio.h"
#include <cmath>
#include <sys/time.h>
#include <x86intrin.h>

const int VECTOR_SIZE = 2 << 20;

int main(void) {

    float *floatArray = new float[VECTOR_SIZE];
    if (!floatArray) {
        printf("Memory allocation error!!\n");
        return 1;
    }

    // Initialize vectors with random numbers
    for (long i = 0; i < VECTOR_SIZE; i++) {
        floatArray[i] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100.0));
    }

    struct timeval serialStart, serialEnd;
    gettimeofday(&serialStart, NULL);
    // Serial implementation
    float serialSumTemp[4];
    serialSumTemp[0] = serialSumTemp[1] = serialSumTemp[2] = serialSumTemp[3] = 0.0;
    for (long i = 0; i < VECTOR_SIZE; i += 4) {
        for (int j = 0; j < 4; j++) {
            serialSumTemp[j] += floatArray[i + j];
        }
    }
    float serialSum = serialSumTemp[0] + serialSumTemp[1] + serialSumTemp[2] + serialSumTemp[3];
    float serialMean = serialSum / VECTOR_SIZE;

    serialSumTemp[0] = serialSumTemp[1] = serialSumTemp[2] = serialSumTemp[3] = 0.0;
    for (long i = 0; i < VECTOR_SIZE; i += 4) {
        for (int j = 0; j < 4; j++) {
            float diff = floatArray[i + j] - serialMean;
            serialSumTemp[j] += (diff * diff);
        }
    }
    serialSum = serialSumTemp[0] + serialSumTemp[1] + serialSumTemp[2] + serialSumTemp[3];
    float serialStandardDeviation = sqrt(serialSum / VECTOR_SIZE);
    gettimeofday(&serialEnd, NULL);
    long serialSeconds = (serialEnd.tv_sec - serialStart.tv_sec);
	long serialMicros = ((serialSeconds * 1000000) + serialEnd.tv_usec) - (serialStart.tv_usec);

    struct timeval parallelStart, parallelEnd;
    gettimeofday(&parallelStart, NULL);
    // Parallel implementation
    __m128 sum = _mm_set1_ps(0.0f);
    for (long i = 0; i < VECTOR_SIZE; i += 4) {
        __m128 vec = _mm_loadu_ps(&floatArray[i]);
        sum = _mm_add_ps(sum, vec);
    }
    sum = _mm_hadd_ps(sum, sum);
    sum = _mm_hadd_ps(sum, sum);
    __m128 count = _mm_set1_ps(VECTOR_SIZE);
    __m128 mean = _mm_div_ps(sum, count);

    __m128 standardDeviation = _mm_set1_ps(0.0f);
    for (long i = 0; i < VECTOR_SIZE; i += 4) {
        __m128 vec = _mm_loadu_ps(&floatArray[i]);

        __m128 tmp = _mm_sub_ps(vec, mean);
        tmp = _mm_mul_ps(tmp, tmp);

        standardDeviation = _mm_add_ps(standardDeviation, tmp);
    }
    standardDeviation = _mm_hadd_ps(standardDeviation, standardDeviation);
    standardDeviation = _mm_hadd_ps(standardDeviation, standardDeviation);
    standardDeviation = _mm_div_ps(standardDeviation, count);
    standardDeviation = _mm_sqrt_ps(standardDeviation);

    float parallelMean = _mm_cvtss_f32(mean);
    float parallelStandardDeviation = _mm_cvtss_f32(standardDeviation);
    gettimeofday(&parallelEnd, NULL);
    long parallelSeconds = (parallelEnd.tv_sec - parallelStart.tv_sec);
	long parallelMicros = ((parallelSeconds * 1000000) + parallelEnd.tv_usec) - (parallelStart.tv_usec);
    float speedup = (float) serialMicros / parallelMicros;

    printf("Serial Mean: %f\n", serialMean);
    printf("Serial Standard Deviation: %f\n", serialStandardDeviation);
    printf("Serial Time: %ld\n", serialMicros);
    printf("Parallel Mean: %f\n", parallelMean);
    printf("Parallel Standard Deviation: %f\n", parallelStandardDeviation);
    printf("Parallel Time: %ld\n", parallelMicros);
    printf("Speedup: %f\n", speedup);

    return 0;
}
