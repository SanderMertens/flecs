#ifndef API_H
#define API_H

/* This generated file contains includes for project dependencies */
#include "bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SYS_COLUMNS (20)
#define MAX_ENTITIES (64)

typedef struct SysTestData {
    EcsEntity system;
    uint32_t offset;
    uint32_t count;
    uint32_t column_count;
    uint32_t invoked;
    EcsEntity e[MAX_ENTITIES];
    EcsEntity c[MAX_SYS_COLUMNS];
    EcsEntity s[MAX_SYS_COLUMNS];
    void *param;
} SysTestData;

typedef struct Position {
    float x;
    float y;
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

typedef float Mass;

typedef float Rotation;

void ProbeSystem(EcsRows *rows);

#ifdef __cplusplus
}
#endif

#endif

