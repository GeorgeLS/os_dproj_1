#ifndef EXERCISE_I__ATTRIBUTES_H_
#define EXERCISE_I__ATTRIBUTES_H_

#if defined(__GNUC__)

#define __NO_RETURN __attribute__((noreturn))
#define __NON_NULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define __ALWAYS_INLINE __attribute__((always_inline))
#define __FORMAT(format_type, index, first_to_check) __attribute__((format (format_type, index, first_to_check)))

#else

#define __NO_RETURN
#define __NON_NULL(...)
#define __ALWAYS_ININE
#define __FORMAT

#endif

#endif //EXERCISE_I__ATTRIBUTES_H_
