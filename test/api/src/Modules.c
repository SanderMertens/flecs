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

static
void AddVtoP(EcsRows *rows) {
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);    
    
    ECS_IMPORT_COLUMN(rows, SimpleModule, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, entities[i], Velocity);
    }
}

void Modules_import_module_from_system() {
    EcsWorld *world = ecs_init();

    ECS_IMPORT(world, SimpleModule, 0);
    ECS_SYSTEM(world, AddVtoP, EcsOnFrame, Position, $SimpleModule);

    void *module_ptr = ecs_get_singleton_ptr(world, SimpleModule);
    test_assert(module_ptr != NULL);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Modules_import_from_on_add_system() {
    EcsWorld *world = ecs_init();

    ECS_IMPORT(world, SimpleModule, 0);
    ECS_SYSTEM(world, AddVtoP, EcsOnAdd, Position, $SimpleModule);

    void *module_ptr = ecs_get_singleton_ptr(world, SimpleModule);
    test_assert(module_ptr != NULL);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Modules_import_from_on_set_system() {
    EcsWorld *world = ecs_init();

    ECS_IMPORT(world, SimpleModule, 0);
    ECS_SYSTEM(world, AddVtoP, EcsOnSet, Position, $SimpleModule);

    void *module_ptr = ecs_get_singleton_ptr(world, SimpleModule);
    test_assert(module_ptr != NULL);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_has(world, e, Velocity));

    ecs_set(world, e, Position, {10, 20});

    test_assert( ecs_has(world, e, Velocity));

    ecs_fini(world);
}
