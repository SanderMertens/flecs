#ifndef ${id upper}_H
#define ${id upper}_H

/* This generated file contains includes for project dependencies */
#include "${id dash}/bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
typedef int MyComponent;
*/

typedef struct ${id pascalcase} {
    /*
     * ECS_DECLARE_ENTITY(MyTag);
     * ECS_DECLARE_COMPONENT(MyComponent);
     */
} ${id pascalcase};

void ${id pascalcase}Import(
    ecs_world_t *world);

#define ${id pascalcase}ImportHandles(handles) /*\
    * ECS_IMPORT_ENTITY(handles, MyTag);\
    * ECS_IMPORT_COMPONENT(handles, MyComponent); 
    */

#ifdef __cplusplus
}
#endif

#endif
