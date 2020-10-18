//
// Created by freddie on 11.10.20.
//

#include "stdfuncs_wrappers.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


bool Error_occurred = false;

void * calloc_wrapper (
        size_t num,
        size_t size) {
    Error_occurred = false;
    void *ptr = calloc (num, size);

    if (ptr == nullptr) {
        FPRINTF_ERROR (
                "Allocation failure. Cannot allocate batch of %zu bytes",
                num * size);
        Error_occurred = true;
    }
    return ptr;
}

FILE * fopen_wrapper (
        const char *fname,
        const char *mode) {
    Error_occurred = false;
    FILE *file = fopen(fname, mode);

    if (file == nullptr) {
        FPRINTF_ERROR (
                "Cannot open file \'%s\' in \'%s\' mode",
                fname,
                mode);
        Error_occurred = true;
    }

    return file;
}

long ftell_wrapper (FILE *stream) {
    Error_occurred = false;
    long pos = ftell(stream);

    if (pos == -1L) {
        FPRINTF_ERROR ("A file error has occurred.");
        Error_occurred = true;
    }
    return  pos;
}


int fseek_wrapper(
        FILE     *stream,
        long int offset,
        int      whence) {
    Error_occurred = false;

    int err = fseek(stream, offset, whence);

    if (err != 0) {
        Error_occurred = true;
        switch (errno) {
            case EBADF:
                FPRINTF_ERROR ("The stream specified is not a seekable stream.");
                break;
            case EINVAL:
                FPRINTF_ERROR ("The whence argument to fseek() was not SEEK_SET, SEEK_END, or SEEK_CUR."
                               "\nOr: the resulting file offset would be negative");
                break;
            default:
                FPRINTF_ERROR ("Unknown error.");
                break;
        }
    }
    return err;
}

int fclose_wrapper(FILE *stream) {
    Error_occurred = false;
    int err = fclose(stream);

    if (err != 0) {
        FPRINTF_ERROR("Cannot close file")
        Error_occurred = true;
    }

    return err;

}




