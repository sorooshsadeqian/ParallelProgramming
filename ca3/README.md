# CA3 - SIMD Image Processing

## The first program (1.cpp)
Calculating the difference of two frames is one of the ways to find out if an object has moved. In this program we try to speed up the subtracting operation using SIMD instructions and calculate the speedup.

## The second program (2.cpp)
We use this equation: Result = Img1 + Img2 * a (where a is a coefficient with a positive value less than 1)
We tried to speed up the operation of calculating the result using SIMD operations and stated the speedup at the end.
