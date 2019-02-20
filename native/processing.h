
#ifndef PROCESSING_API_H
#define PROCESSING_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct region {
    int x; int y; int w; int h;
} region;

typedef struct flow {
    int x; int y;
} flow;

int processing_initialize();

region processing_detect_face(void *obj);
flow processing_calculate_flow(void *obj, region r);

#ifdef __cplusplus
}

#endif

#endif
