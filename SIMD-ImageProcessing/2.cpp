#include <opencv2/highgui.hpp>
#include <tmmintrin.h>
#include <emmintrin.h>
#include <iostream>
#include <sys/time.h>

// #define ALPHA 0.5
#define SATURATED_MAX 255

int main(int argc, char **argv)
{


    struct timeval serial_start, serial_end, parallel_start, parallel_end;

    cv::Mat img_1 = cv::imread("q2-1.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img_2 = cv::imread("q2-2.png", cv::IMREAD_GRAYSCALE);

    unsigned int NCOLS = img_2.cols;
    unsigned int NROWS = img_2.rows;

    cv::Mat out_img = cv::imread("q2-1.png", cv::IMREAD_GRAYSCALE);
    cv::Mat out_img_parallel = cv::imread("q2-1.png", cv::IMREAD_GRAYSCALE);

    unsigned char *in_image, *in_image2;
    unsigned char *out_image;

    in_image = (unsigned char *)img_1.data;
    in_image2 = (unsigned char *)img_2.data;
    out_image = (unsigned char *)out_img.data;

    gettimeofday(&serial_start, NULL);
    for (unsigned int row = 0; row < NROWS; row++)
        for (unsigned int col = 0; col < NCOLS; col++) {
            unsigned char res = *(in_image + row * img_1.cols + col) + (*(in_image2 + row * NCOLS + col) >> 1);
            *(out_image + row * img_1.cols + col) = res < *(in_image + row * img_1.cols + col) ? SATURATED_MAX : res;
        }
    gettimeofday(&serial_end, NULL);

    __m128i *pSrc1, *pSrc2;
    __m128i *pRes;
    __m128i m1, m2, m3, c127;

    c127 = _mm_set1_epi8(0x7f);

    pSrc1 = (__m128i *) img_1.data;
    pSrc2 = (__m128i *) img_2.data;
    pRes = (__m128i *) out_img_parallel.data;


    gettimeofday(&parallel_start, NULL);
    for (unsigned int i = 0; i < NROWS; i++)
    	for (unsigned int j = 0; j < NCOLS / 16; j++) {
    		m1 = _mm_loadu_si128(pSrc1 + i * img_1.cols/16 + j) ;
            m2 = _mm_loadu_si128(pSrc2 + i * NCOLS/16 + j);

            m2 = _mm_srli_epi16(m2, 1);
            m2 = _mm_and_si128(m2, c127);

            m3 = _mm_adds_epu8(m1, m2);

    		_mm_storeu_si128 (pRes + i * img_1.cols/16 + j, m3);
    	}

    gettimeofday(&parallel_end, NULL);

    cv::namedWindow("serial output", cv::WINDOW_AUTOSIZE);
    cv::imshow("serial output", out_img);
    cv::namedWindow("parallel output", cv::WINDOW_AUTOSIZE);
    cv::imshow("parallel output", out_img_parallel);

    auto const diff = out_img != out_img_parallel;
    if (cv::countNonZero(diff) == 0){
        std::cout << "The pictures are the same." << std::endl;
    }
    else {
        std::cout << "The pictures are different." << std::endl;
    }

    long serial_seconds = (serial_end.tv_sec - serial_start.tv_sec);
    long serial_micros = ((serial_seconds * 1000000) + serial_end.tv_usec) - (serial_start.tv_usec);
    long parallel_seconds = (parallel_end.tv_sec - parallel_start.tv_sec);
    long parallel_micros = ((parallel_seconds * 1000000) + parallel_end.tv_usec) - (parallel_start.tv_usec);
    float speedup = (float)serial_micros / parallel_micros;
    printf("Serial Micros: %ld\n", serial_micros);
    printf("Parallel Micros: %ld\n", parallel_micros);
    printf("Speedup: %f\n", speedup);
    cv::waitKey(0);

}