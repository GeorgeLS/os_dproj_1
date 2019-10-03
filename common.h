#ifndef EXERCISE_I__COMMON_H_
#define EXERCISE_I__COMMON_H_

#include <stddef.h>
#include <stdint.h>

#define internal static
#define global static
#define local static

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef uint8_t byte;

typedef size_t (*hash_function) (byte *data, size_t bytes);

#endif //EXERCISE_I__COMMON_H_
