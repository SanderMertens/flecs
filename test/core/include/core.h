#ifndef CORE_H
#define CORE_H

/* This generated file contains includes for project dependencies */
#include <core/bake_config.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SYS_COLUMNS (20)
#define MAX_ENTITIES (256)
#define MAX_INVOCATIONS (1024)

/* Multiline strings */
#define HEAD
#define LINE "\n"

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

typedef struct Color {
    float r;
    float g;
    float b;
    float a;
} Color;

typedef struct Self {
    ecs_entity_t value;
} Self;

void probe_system_w_ctx(
    ecs_iter_t *it,
    Probe *ctx);

void probe_iter(ecs_iter_t *it);

void probe_has_entity(Probe *probe, ecs_entity_t e);

void install_test_abort(void);

const ecs_entity_t* bulk_new_w_type(
    ecs_world_t *world, ecs_entity_t type_ent, int32_t count);

#ifdef __cplusplus
}
#endif

#endif

