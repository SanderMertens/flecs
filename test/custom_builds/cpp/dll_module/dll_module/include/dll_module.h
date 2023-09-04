#ifndef DLL_MODULE_H
#define DLL_MODULE_H

/* This generated file contains includes for project dependencies */
#include "dll_module/bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace movement 
{
    struct Position {
        float x;
        float y;
    };

    struct Velocity {
        float x;
        float y;
    };

    struct DLL_MODULE_API module {
        module(flecs::world& world);
    };
}

#ifdef __cplusplus
}
#endif

#endif

