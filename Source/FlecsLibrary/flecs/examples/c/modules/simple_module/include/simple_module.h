#ifndef SIMPLE_MODULE_H
#define SIMPLE_MODULE_H

/* This generated file contains includes for project dependencies */
#include "simple_module/bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Component types
typedef struct {
    double x, y;
} Position, Velocity;

// Global variables that hold component ids
extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Velocity);

// Module import function
void SimpleModuleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif
