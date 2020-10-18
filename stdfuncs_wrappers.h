//
// Created by freddie on 11.10.20.
//

#ifndef TASK2_ONEGIN_STDFUNCS_WRAPPERS_H
#define TASK2_ONEGIN_STDFUNCS_WRAPPERS_H

#include <stdlib.h>
#include <stdio.h>

#define FPRINTF_ERROR(...)       \
    fprintf(stderr, "\nERROR %s: ", __FUNCTION__);          \
    fprintf(stderr, __VA_ARGS__);

#define FPRINTF_ERROR_AND_EXIT(...) \
    FPRINTF_ERROR(__VA_ARGS__); \
    exit(EXIT_FAILURE);

#define EXIT_IF_ERROR_OCCURRED() \
    if (Error_occurred) {      \
        exit(EXIT_FAILURE);   \
        }


extern bool Error_occurred;

void * calloc_wrapper(
        size_t num,
        size_t size);

FILE * fopen_wrapper (
        const char *fname,
        const char *mode);

long ftell_wrapper (FILE *stream);

int fseek_wrapper(
        FILE     *stream,
        long int offset,
        int      whence);

int fclose_wrapper(FILE *stream);

#endif //TASK2_ONEGIN_STDFUNCS_WRAPPERS_H
