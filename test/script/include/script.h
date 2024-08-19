#ifndef SCRIPT_H
#define SCRIPT_H

/* This generated file contains includes for project dependencies */
#include "script/bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Multiline strings */
#define HEAD
#define LINE "\n"

#define MAX_SYS_COLUMNS (20)
#define MAX_ENTITIES (256)
#define MAX_INVOCATIONS (1024)

typedef struct Probe {
    ecs_entity_t system;
    ecs_entity_t event;
    ecs_id_t event_id;
    int32_t offset;
    int32_t count;
    int32_t invoked;
    int32_t term_count;
    int32_t term_index;
    ecs_entity_t e[MAX_ENTITIES];
    ecs_entity_t c[MAX_INVOCATIONS][MAX_SYS_COLUMNS];
    ecs_entity_t s[MAX_INVOCATIONS][MAX_SYS_COLUMNS];
    void *param;
} Probe;

typedef struct IterData {
    ecs_entity_t component;
    ecs_entity_t component_2;
    ecs_entity_t component_3;
    ecs_entity_t new_entities[MAX_ENTITIES];
    int32_t entity_count;
} IterData;

typedef struct {
    float x, y;
} Position, Velocity;

typedef struct {
    int32_t x, y;
} PositionI;

typedef struct {
    float value;
} Mass;

typedef struct {
    float x, y;
} Point;

typedef struct {
    Point start, stop;
} Line;

typedef struct {
    PositionI start, stop;
} LineI;

typedef struct {
    float x, y, z;
} Vec3;

#ifdef __cplusplus
}
#endif

#endif

