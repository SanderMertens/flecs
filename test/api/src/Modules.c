#include <include/api.h>

/* -- Begin module code -- */

typedef struct SimpleModuleHandles {
    ECS_DECLARE_COMPONENT(Position);
    ECS_DECLARE_COMPONENT(Velocity);
} SimpleModuleHandles;

#define SimpleModule_ImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Position);\
    ECS_IMPORT_COMPONENT(handles, Velocity);

void SimpleModule(
    EcsWorld *world, 
    int flags, 
    void *handles)
{
    SimpleModuleHandles *handles_out = handles;

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SET_COMPONENT(handles_out, Position);
    ECS_SET_COMPONENT(handles_out, Velocity);
}

/* -- End module code -- */

void Modules_simple_module() {
    EcsWorld *world = ecs_init();

    ECS_IMPORT(world, SimpleModule, 0);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    
    ecs_add(world, e, Velocity);
    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}
