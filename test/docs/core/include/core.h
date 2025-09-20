#ifndef CORE_H
#define CORE_H

/* This generated file contains includes for project dependencies */
#include <core/bake_config.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct Gravity {
    float value;
} Gravity;

static inline void Move(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x * it->delta_time;
        p[i].y += v[i].y * it->delta_time;
    }
}

static inline void OnSetPosition(ecs_iter_t *it) {}

#ifdef __cplusplus
}
#endif

#endif

