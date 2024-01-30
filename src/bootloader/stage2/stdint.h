#pragma once

//! int types
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed long int int32_t;
typedef unsigned long int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;


//! booleans
typedef uint8_t bool
#define true 1
#define false 0

#define NULL ((void*)0)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
