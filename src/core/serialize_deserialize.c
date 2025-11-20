#include "core/serialize_deserialize.h"

size_t serialize(struct struct_fmt *fmt, void *structure, unsigned char *buffer) { 
    size_t pos = 0;

    for (size_t i = 0; i < fmt->num_members; i++) {
        memcpy(buffer+pos, ((unsigned char*)structure)+fmt->offsets[i], fmt->sizes[i]);
        pos += fmt->sizes[i];
    }

    return pos;
}

// maybe malloc and return structure pointer instead??? might be a better way or maybe not
size_t deserialize(struct struct_fmt *fmt, unsigned char *buffer, void *structure) {
    size_t pos = 0;

    for (size_t i = 0; i < fmt->num_members; i++) {
        memcpy(((unsigned char*)structure)+fmt->offsets[i], buffer+pos, fmt->sizes[i]);
        pos += fmt->sizes[i];
    }

    return pos;
}
