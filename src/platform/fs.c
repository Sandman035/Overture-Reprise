#include "platform/fs.h"
#include "core/log.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: test and make it better you know
void* read_file(const char* path, size_t* size) {
    FILE *fp = fopen(path, "rb");

    if (fp == NULL) {
        WARN("Could not open file: %s.", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);

    *size = ftell(fp);

    rewind(fp);

    void* buffer = malloc(*size);
    fread(buffer, *size, 1, fp);

    fclose(fp);

    TRACE("Read file: %s.", path);

    return buffer;
}
