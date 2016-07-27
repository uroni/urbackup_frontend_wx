#pragma once

#ifdef __linux__
typedef long long int __int64;
typedef unsigned long long int uint64;
#elif _WIN32
typedef unsigned __int64 uint64;
#else
typedef long long int __int64;
typedef unsigned long long int uint64;
#endif

#ifndef NULL
#define NULL    0
#endif

typedef __int64 int64;
typedef unsigned char uchar;
typedef int _i32;
typedef __int64 _i64;
typedef unsigned int _u32;
typedef unsigned short _u16;
typedef short _i16;