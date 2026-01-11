#ifndef MATH_H
#define MATH_H

#include <stddef.h>


template<typename T>
inline T DivRoundUp(T number, T divisor)
{
    return (number + divisor - 1) / divisor;
}


#endif /* MATH_H */
