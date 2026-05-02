/* stdint.h shim for MSVC 2008 (which predates C99 stdint.h) */
#ifndef _STDINT_H_SHIM_
#define _STDINT_H_SHIM_

#if defined(_MSC_VER)

typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

typedef __int8  int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;

typedef unsigned __int8  uint_least8_t;
typedef unsigned __int16 uint_least16_t;
typedef unsigned __int32 uint_least32_t;
typedef unsigned __int64 uint_least64_t;

typedef __int8  int_least8_t;
typedef __int16 int_least16_t;
typedef __int32 int_least32_t;
typedef __int64 int_least64_t;

typedef unsigned __int8  uint_fast8_t;
typedef unsigned __int32 uint_fast16_t;
typedef unsigned __int32 uint_fast32_t;
typedef unsigned __int64 uint_fast64_t;

typedef __int8  int_fast8_t;
typedef __int32 int_fast16_t;
typedef __int32 int_fast32_t;
typedef __int64 int_fast64_t;

#ifdef _WIN64
typedef unsigned __int64 uintptr_t;
typedef __int64          intptr_t;
typedef unsigned __int64 uintmax_t;
typedef __int64          intmax_t;
#else
typedef unsigned __int32 uintptr_t;
typedef __int32          intptr_t;
typedef unsigned __int64 uintmax_t;
typedef __int64          intmax_t;
#endif

#define INT8_MIN    (-127i8 - 1)
#define INT16_MIN   (-32767i16 - 1)
#define INT32_MIN   (-2147483647i32 - 1)
#define INT64_MIN   (-9223372036854775807i64 - 1)

#define INT8_MAX    127i8
#define INT16_MAX   32767i16
#define INT32_MAX   2147483647i32
#define INT64_MAX   9223372036854775807i64

#define UINT8_MAX   0xffui8
#define UINT16_MAX  0xffffui16
#define UINT32_MAX  0xffffffffui32
#define UINT64_MAX  0xffffffffffffffffui64

#define SIZE_MAX    UINT32_MAX

#endif /* _MSC_VER */
#endif /* _STDINT_H_SHIM_ */
