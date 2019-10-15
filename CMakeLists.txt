cmake_minimum_required(VERSION 3.14)
project(Exercise_I C)

set(CMAKE_C_STANDARD 11)

add_compile_options(-mavx -mavx2)

add_executable(Exercise_I main.c common.h report.c report.h attributes.h
        utils.c utils.h voter.c voter.h bloom_filter.c bloom_filter.h dynamic_buffer.c dynamic_buffer.h tokenizer.c tokenizer.h rb_tree.c rb_tree.h)