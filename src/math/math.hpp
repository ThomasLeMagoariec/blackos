#ifndef MATH_H
#define MATH_H

#ifdef __cpp_lib_bitops
#   include <bit>
#   define CountLeadingZeros(x) std::countl_zeros(x)
#else
#   define CountLeadingZeros(x) __builtin_clz(x)
#endif

#include <stdint.h>

uint32_t RoundToPowerOf2(uint32_t x);
uint64_t RoundToPowerOf2(uint64_t x);

template<typename T>
inline T IsPowerOf2(T number)
{
    return ((number & (number - 1)) == 0);
}

template<typename T>
inline T DivRoundUp(T number, T divisor)
{
    return (number + divisor - 1) / divisor;
}


template<typename T>
inline T Log2(T number)
{
    return (8 * sizeof(T)) - CountLeadingZeros(number) - 1;
}


#endif /* MATH_H */
