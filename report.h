#ifndef EXERCISE_I__REPORT_H_
#define EXERCISE_I__REPORT_H_

#include <stddef.h>
#include "attributes.h"
#include "rb_tree.h"
#include "bloom_filter.h"

__NO_RETURN void fatal(const char *fmt, ...) __FORMAT(printf, 1, 2);

void report_error(const char *fmt, ...) __FORMAT(printf, 1, 2);

#endif //EXERCISE_I__REPORT_H_
