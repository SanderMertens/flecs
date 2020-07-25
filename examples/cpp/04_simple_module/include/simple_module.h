#ifndef SIMPLE_MODULE_H
#define SIMPLE_MODULE_H

/* This generated file contains includes for project dependencies */
#include "simple_module/bake_config.h"

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct SimpleModule {
    SimpleModule(flecs::world& world);
    flecs::entity position;
    flecs::entity velocity;
    flecs::entity move;
};

#endif

