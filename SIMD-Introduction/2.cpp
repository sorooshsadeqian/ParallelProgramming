#include "x86intrin.h"
#include <iostream>
#include <string>

using namespace std;

#define UNSIGNED_BYTE "u8"
#define SIGNED_BYTE "i8"
#define UNSIGNED_WORD "u16"
#define SIGNED_WORD "i16"
#define UNSIGNED_DOUBLE_WORD "u32"
#define SIGNED_DOUBLE_WORD "i32"
#define UNSIGNED_QUAD_WORD "u64"
#define SIGNED_QUAD_WORD "i64"

typedef union {
    __m128i int128;

    unsigned char m128_u8[16];
    signed char m128_i8[16];

    unsigned short m128_u16[8];
    signed short m128_i16[8];

    unsigned int m128_u32[4];
    signed int m128_i32[4];

    unsigned long m128_u64[2];
    signed long m128_i64[2];

} intVec;

typedef union {
    __m128 float128;

    float m128_f4[4];
} floatVec;

void print_u8(__m128i a) {
    intVec tmp;
    tmp.int128 = a;
    printf("[");
    for (int i = 15; i > 0; i--) {
        printf("%X, ", tmp.m128_u8[i]);
    }
    printf("%X]\n\n", tmp.m128_u8[0]);
}

void print_i8(__m128i a) {
    intVec tmp;
    tmp.int128 = a;
    printf("[");
    for (int i = 15; i > 0; i--) {
        printf("%d, ", tmp.m128_i8[i]);
    }
    printf("%d]\n\n", tmp.m128_i8[0]);
}

void print_u16(__m128i a) {
    intVec tmp;
    tmp.int128 = a;
    printf("[");
    for (int i = 7; i > 0; i--) {
        printf("%X, ", tmp.m128_u16[i]);
    }
    printf("%X]\n\n", tmp.m128_u16[0]);
}

void print_i16(__m128i a) {
    intVec tmp;
    tmp.int128 = a;
    printf("[");
    for (int i = 7; i > 0; i--) {
        printf("%d, ", tmp.m128_i16[i]);
    }
    printf("%d]\n\n", tmp.m128_i16[0]);
}

void print_u32(__m128i a) {
    intVec tmp;
    tmp.int128 = a;
    printf("[");
    for (int i = 3; i > 0; i--) {
        printf("%X, ", tmp.m128_u32[i]);
    }
    printf("%X]\n\n", tmp.m128_u32[0]);
}

void print_i32(__m128i a) {
    intVec tmp;
    tmp.int128 = a;
    tmp.int128 = a;
    printf("[");
    for (int i = 3; i > 0; i--) {
        printf("%d, ", tmp.m128_i32[i]);
    }
    printf("%d]\n\n", tmp.m128_i32[0]);
}

void print_u64(__m128i a) {
    intVec tmp;
    tmp.int128 = a;
    printf("[%lX, %lX]\n\n", tmp.m128_u64[1], tmp.m128_u64[0]);
}

void print_i64(__m128i a) {
    intVec tmp;
    tmp.int128 = a;
    printf("[%ld, %ld]\n\n", tmp.m128_i64[1], tmp.m128_i64[0]);
}

void print_int_vector(__m128i a, string type) {
    if (type == UNSIGNED_BYTE) {
        print_u8(a);
    } else if (type == SIGNED_BYTE) {
        print_i8(a);
    } else if (type == UNSIGNED_WORD) {
        print_u16(a);
    } else if (type == SIGNED_WORD) {
        print_i16(a);
    } else if (type == UNSIGNED_DOUBLE_WORD) {
        print_u32(a);
    } else if (type == SIGNED_DOUBLE_WORD) {
        print_i32(a);
    } else if (type == UNSIGNED_QUAD_WORD) {
        print_u64(a);
    } else if (type == SIGNED_QUAD_WORD) {
        print_i64(a);
    } else {
        printf("Wrong type. Choices are: u8, i8, u16, i16, u32, i32, u64 and i64");
    }
}

unsigned char intArray[16] = {0X00, 0X11, 0X22, 0X33, 0X44, 0X55, 0X66, 0X77,
                              0X88, 0X99, 0XAA, 0XBB, 0XCC, 0XDD, 0XEE, 0XFF};

void print_spfp_vector(__m128 a) {
    floatVec tmp;
    tmp.float128 = a;
    printf("[");
    for (int i = 3; i > 0; i--) {
        printf("%f, ", tmp.m128_f4[i]);
    }
    printf("%f]\n\n", tmp.m128_f4[0]);
}

float flArray[4] = {1.1, 2.0, 3.0, 4.0};

void test_int_print() {
    __m128i a = _mm_load_si128((const __m128i *)intArray);

    printf("Unsigned byte: ");
    print_int_vector(a, "u8");

    printf("Unsigned word: ");
    print_int_vector(a, "u16");

    printf("Unsigned double word: ");
    print_int_vector(a, "u32");

    printf("Unsigned quad word: ");
    print_int_vector(a, "u64");

    printf("Signed byte: ");
    print_int_vector(a, "i8");

    printf("Signed word: ");
    print_int_vector(a, "i16");

    printf("Signed double word: ");
    print_int_vector(a, "i32");

    printf("Signed quad word: ");
    print_int_vector(a, "i64");
}

void test_float_print() {
    __m128 a = _mm_load_ps(flArray);

    printf("Single Precision Floating Point: ");
    print_spfp_vector(a);
}

int main(void) {
    test_int_print();
    test_float_print();
    return 0;
}
