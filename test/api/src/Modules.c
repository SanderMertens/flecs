#include <api.h>

void Modules_setup() {
}

/* -- Begin module code -- */

typedef struct SimpleFooComponent {
    float value;
} SimpleFooComponent;


typedef struct NestedComponent {
    float value;
} NestedComponent;

static
void Move(ecs_iter_t *it) { }

static
void SimpleFooSystem(ecs_iter_t *it) { }

static
void SimpleFooTrigger(ecs_iter_t *it) { }

typedef struct NestedModule {
    ECS_DECLARE_COMPONENT(NestedComponent);
} NestedModule;

typedef struct SimpleModule {
    ECS_DECLARE_COMPONENT(Position);
    ECS_DECLARE_COMPONENT(Velocity);
    ECS_DECLARE_COMPONENT(SimpleFooComponent);
    ECS_DECLARE_ENTITY(Tag);
    ECS_DECLARE_ENTITY(SimpleFooTag);
    ECS_DECLARE_ENTITY(Entity);
    ECS_DECLARE_ENTITY(SimpleFooEntity);
    ECS_DECLARE_ENTITY(Move);
    ECS_DECLARE_ENTITY(SimpleFooSystem);
    ECS_DECLARE_ENTITY(SimpleFooPrefab);
    ECS_DECLARE_ENTITY(SimpleFooPipeline);
    ECS_DECLARE_ENTITY(SimpleFooTrigger);
    ECS_DECLARE_ENTITY(Simple_underscore);
} SimpleModule;

#define NestedModuleImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, NestedComponent);\

#define SimpleModuleImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Position);\
    ECS_IMPORT_COMPONENT(handles, Velocity);\
    ECS_IMPORT_COMPONENT(handles, SimpleFooComponent);\
    ECS_IMPORT_ENTITY(handles, Tag);\
    ECS_IMPORT_ENTITY(handles, SimpleFooTag);\
    ECS_IMPORT_ENTITY(handles, Entity);\
    ECS_IMPORT_ENTITY(handles, SimpleFooEntity);\
    ECS_IMPORT_ENTITY(handles, Move);\
    ECS_IMPORT_ENTITY(handles, SimpleFooSystem);\
    ECS_IMPORT_ENTITY(handles, SimpleFooPrefab);\
    ECS_IMPORT_ENTITY(handles, SimpleFooPipeline);\
    ECS_IMPORT_ENTITY(handles, SimpleFooTrigger);\
    ECS_IMPORT_ENTITY(handles, Simple_underscore);

void NestedModuleImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, NestedModule);

    ecs_set_name_prefix(world, "Nested");

    ECS_COMPONENT(world, NestedComponent);

    ECS_EXPORT_COMPONENT(NestedComponent);
}

void SimpleModuleImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, SimpleModule);

    ECS_IMPORT(world, NestedModule);

    ecs_set_name_prefix(world, "Simple");

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, SimpleFooComponent);

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Entity, 0);
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    ECS_TAG(world, SimpleFooTag);
    ECS_ENTITY(world, SimpleFooEntity, 0);
    ECS_PREFAB(world, SimpleFooPrefab, 0);
    ECS_SYSTEM(world, SimpleFooSystem, EcsOnUpdate, Position);
    ECS_PIPELINE(world, SimpleFooPipeline, Tag);
    ECS_TRIGGER(world, SimpleFooTrigger, EcsOnAdd, Position);
    ECS_TAG(world, Simple_underscore);

    ECS_EXPORT_COMPONENT(Position);
    ECS_EXPORT_COMPONENT(Velocity);
    ECS_EXPORT_COMPONENT(SimpleFooComponent);
    ECS_EXPORT_ENTITY(Tag);
    ECS_EXPORT_ENTITY(SimpleFooTag);
    ECS_EXPORT_ENTITY(Entity);
    ECS_EXPORT_ENTITY(SimpleFooEntity);
    ECS_EXPORT_ENTITY(Move);
    ECS_EXPORT_ENTITY(SimpleFooSystem);
    ECS_EXPORT_ENTITY(SimpleFooPrefab);
    ECS_EXPORT_ENTITY(SimpleFooPipeline);
    ECS_EXPORT_ENTITY(SimpleFooTrigger);
    ECS_EXPORT_ENTITY(Simple_underscore);
}

/* -- End module code -- */

void Modules_simple_module() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    
    ecs_add(world, e, Velocity);
    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

static
void AddVtoP(ecs_iter_t *it) {
    ECS_IMPORT_TERM(it, SimpleModule, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

void Modules_import_module_from_system() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);
    ECS_SYSTEM(world, AddVtoP, EcsOnUpdate, simple.module.Position, simple.module:simple.module);

    const void *module_ptr = ecs_get(world, ecs_id(SimpleModule), SimpleModule);
    test_assert(module_ptr != NULL);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

ecs_entity_t import_module(ecs_world_t *world) {
    ECS_IMPORT(world, SimpleModule);
    return ecs_id(SimpleModule);
}

void Modules_import_again() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    test_assert(ecs_id(SimpleModule) != 0);
    test_assert(ecs_id(SimpleModule) == import_module(world));
    
    ecs_fini(world);
}

void Modules_scoped_component() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.Position");
    test_assert(e != 0);
    test_assert(e == ecs_id(Position));
    
    ecs_fini(world);
}

void Modules_scoped_tag() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.Tag");
    test_assert(e != 0);
    test_assert(e == Tag);
    
    ecs_fini(world);
}

void Modules_scoped_system() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.Move");
    test_assert(e != 0);
    test_assert(e == Move);
    
    ecs_fini(world);
}

void Modules_scoped_entity() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.Entity");
    test_assert(e != 0);
    test_assert(e == Entity);
    
    ecs_fini(world);
}

void Modules_name_prefix_component() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.FooComponent");
    test_assert(e != 0);
    test_assert(e == ecs_id(SimpleFooComponent));
    
    ecs_fini(world);
}

void Modules_name_prefix_tag() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.FooTag");
    test_assert(e != 0);
    test_assert(e == SimpleFooTag);
    
    ecs_fini(world);
}

void Modules_name_prefix_system() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.FooSystem");
    test_assert(e != 0);
    test_assert(e == SimpleFooSystem);
    
    ecs_fini(world);
}

void Modules_name_prefix_entity() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.FooEntity");
    test_assert(e != 0);
    test_assert(e == SimpleFooEntity);
    
    ecs_fini(world);
}

void Modules_name_prefix_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.FooPrefab");
    test_assert(e != 0);
    test_assert(e == SimpleFooPrefab);
    
    ecs_fini(world);
}

void Modules_name_prefix_pipeline() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.FooPipeline");
    test_assert(e != 0);
    test_assert(e == SimpleFooPipeline);
    
    ecs_fini(world);
}

void Modules_name_prefix_trigger() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.FooTrigger");
    test_assert(e != 0);
    test_assert(e == SimpleFooTrigger);
    
    ecs_fini(world);
}

void Modules_name_prefix_underscore() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "simple.module.underscore");
    test_assert(e != 0);
    test_assert(e == Simple_underscore);
    
    ecs_fini(world);
}

void Modules_lookup_by_symbol() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_symbol(world, "Position", true);
    test_assert(e != 0);
    test_assert(e == ecs_id(Position));

    e = ecs_lookup_symbol(world, "SimpleFooComponent", true);
    test_assert(e != 0);
    test_assert(e == ecs_id(SimpleFooComponent));

    ecs_fini(world);
}

void Modules_nested_module() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t e = ecs_lookup_fullpath(world, "nested.module.Component");
    test_assert(e != 0);

    char *path = ecs_get_fullpath(world, e);
    test_str(path, "nested.module.Component");
    ecs_os_free(path);

    ecs_fini(world);
}

void Modules_module_tag_on_namespace() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);

    ecs_entity_t mid = ecs_lookup_fullpath(world, "simple.module");
    test_assert(mid != 0);
    test_assert(ecs_has_id(world, mid, EcsModule));

    ecs_entity_t nid = ecs_lookup_fullpath(world, "simple");
    test_assert(nid != 0);
    test_assert(ecs_has_id(world, nid, EcsModule));

    ecs_fini(world);
}
