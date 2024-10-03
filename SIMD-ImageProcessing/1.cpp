#include <opencv2/highgui.hpp>
#include <tmmintrin.h>
#include <iostream>
#include <sys/time.h>

int main(int argc, char **argv)
{

    struct timeval serial_start, serial_end, parallel_start, parallel_end;

    cv::Mat img_1 = cv::imread("q1-1.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img_2 = cv::imread("q1-2.png", cv::IMREAD_GRAYSCALE);

    unsigned int NCOLS = img_1.cols;
    unsigned int NROWS = img_1.rows;

    cv::Mat out_img(NROWS, NCOLS, CV_8U);
    cv::Mat out_img2(NROWS, NCOLS, CV_8U);

    unsigned char *in_image, *in_image2;
    unsigned char *out_image;

    in_image = (unsigned char *)img_1.data;
    in_image2 = (unsigned char *)img_2.data;
    out_image = (unsigned char *)out_img.data;

    gettimeofday(&serial_start, NULL);
    for (unsigned int row = 0; row < NROWS; row++)
        for (unsigned int col = 0; col < NCOLS; col++) {
            int res = *(in_image + row * NCOLS + col) - *(in_image2 + row * NCOLS + col);
            *(out_image + row * NCOLS + col) = (unsigned char)(res > 0 ? res : -1 * res);
        }
    gettimeofday(&serial_end, NULL);

    __m128i *pSrc1, *pSrc2;
    __m128i *pRes;
    __m128i m1, m2, m3;

    pSrc1 = (__m128i *) img_1.data;
    pSrc2 = (__m128i *) img_2.data;
    pRes = (__m128i *) out_img2.data;

    gettimeofday(&parallel_start, NULL);
    for (unsigned int i = 0; i < NROWS; i++)
    	for (unsigned int j = 0; j < NCOLS / 16; j++) {
    		m1 = _mm_loadu_si128(pSrc1 + i * NCOLS/16 + j) ;
            m2 = _mm_loadu_si128(pSrc2 + i * NCOLS/16 + j);
            m3 = _mm_sub_epi8(m1, m2);
    		m3 = _mm_abs_epi8(m3);
    		_mm_storeu_si128 (pRes + i * NCOLS/16 + j, m3);
    	}
    gettimeofday(&parallel_end, NULL);

    cv::namedWindow("serial output", cv::WINDOW_AUTOSIZE);
    cv::imshow("serial output", out_img);
    cv::namedWindow("parallel output", cv::WINDOW_AUTOSIZE);
    cv::imshow("parallel output", out_img2);

    auto const diff = out_img != out_img2;
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