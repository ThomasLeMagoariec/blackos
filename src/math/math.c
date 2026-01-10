#include "math.h"


size_t math_divRoundUp(size_t number, size_t divisor) {
    return (number + divisor - 1) / divisor;
}
