#ifndef EXERCISE_I__REPORT_H_
#define EXERCISE_I__REPORT_H_

#include "attributes.h"

__NO_RETURN void fatal(const char *fmt, ...) __FORMAT(printf, 1, 2);
void report_error(const char *fmt, ...) __FORMAT(printf, 1, 2);

#endif //EXERCISE_I__REPORT_H_
