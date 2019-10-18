#ifndef EXERCISE_I__REPORT_H_
#define EXERCISE_I__REPORT_H_

#include <stddef.h>
#include "attributes.h"
#include "rb_tree.h"
#include "bloom_filter.h"

__NO_RETURN void fatal(const char *fmt, ...) __FORMAT(printf, 1, 2);

void report_error(const char *fmt, ...) __FORMAT(printf, 1, 2);

void check_voters_in_file(int out_fd, bloom_filter *bf, rb_tree *tree,
                          const char *filename) __NON_NULL(2, 3);

#endif //EXERCISE_I__REPORT_H_
