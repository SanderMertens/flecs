#include <api.h>

/* -- Begin module code -- */

typedef struct SimpleModule {
    ECS_DECLARE_COMPONENT(Position);
    ECS_DECLARE_COMPONENT(Velocity);
} SimpleModule;

#define SimpleModuleImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Position);\
    ECS_IMPORT_COMPONENT(handles, Velocity);

void SimpleModuleImport(
    ecs_world_t *world, 
    int flags)
{
    ECS_MODULE(world, SimpleModule);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SET_COMPONENT(Position);
    ECS_SET_COMPONENT(Velocity);
}

/* -- End module code -- */

void Modules_simple_module() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule, 0);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    
    ecs_add(world, e, Velocity);
    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

static
void AddVtoP(ecs_rows_t *rows) {
    ECS_IMPORT_COLUMN(rows, SimpleModule, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], Velocity);
    }
}

void Modules_import_module_from_system() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule, 0);
    ECS_SYSTEM(world, AddVtoP, EcsOnUpdate, Position, $.SimpleModule);

    void *module_ptr = ecs_get_singleton_ptr(world, SimpleModule);
    test_assert(module_ptr != NULL);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Modules_import_from_on_add_system() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule, 0);
    ECS_SYSTEM(world, AddVtoP, EcsOnAdd, Position, $.SimpleModule);

    void *module_ptr = ecs_get_singleton_ptr(world, SimpleModule);
    test_assert(module_ptr != NULL);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Modules_import_from_on_set_system() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule, 0);
    ECS_SYSTEM(world, AddVtoP, EcsOnSet, Position, $.SimpleModule);

    void *module_ptr = ecs_get_singleton_ptr(world, SimpleModule);
    test_assert(module_ptr != NULL);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_has(world, e, Velocity));

    ecs_set(world, e, Position, {10, 20});

    test_assert( ecs_has(world, e, Velocity));

    ecs_fini(world);
}

ecs_entity_t import_module(ecs_world_t *world) {
    ECS_IMPORT(world, SimpleModule, 0);
    return ecs_entity(SimpleModule);
}

void Modules_import_again() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule, 0);

    test_assert(ecs_entity(SimpleModule) != 0);
    test_assert(ecs_entity(SimpleModule) == import_module(world));
    
    ecs_fini(world);
}
