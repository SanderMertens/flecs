#include <addons.h>

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

static ECS_COMPONENT_DECLARE(NestedComponent);

static ECS_COMPONENT_DECLARE(Position);
static ECS_COMPONENT_DECLARE(Velocity);
static ECS_COMPONENT_DECLARE(SimpleFooComponent);
static ECS_DECLARE(Tag);
static ECS_DECLARE(SimpleFooTag);
static ECS_DECLARE(Entity);
static ECS_DECLARE(SimpleFooEntity);
static ECS_DECLARE(SimpleFooPrefab);
static ECS_DECLARE(SimpleFooPipeline);
static ECS_DECLARE(Simple_underscore);

static ECS_SYSTEM_DECLARE(Move);
static ECS_SYSTEM_DECLARE(SimpleFooSystem);
static ECS_SYSTEM_DECLARE(SimpleFooTrigger);

void NestedModuleImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, NestedModule);

    ecs_set_name_prefix(world, "Nested");

    ECS_COMPONENT_DEFINE(world, NestedComponent);
}

void SimpleModuleImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, SimpleModule);

    ECS_IMPORT(world, NestedModule);

    ecs_set_name_prefix(world, "Simple");

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);
    ECS_COMPONENT_DEFINE(world, SimpleFooComponent);

    ECS_TAG_DEFINE(world, Tag);
    ECS_ENTITY_DEFINE(world, Entity, 0);
    
    ECS_SYSTEM_DEFINE(world, Move, EcsOnUpdate, Position, Velocity);
    ECS_SYSTEM_DEFINE(world, SimpleFooSystem, EcsOnUpdate, Position);
    ECS_OBSERVER_DEFINE(world, SimpleFooTrigger, EcsOnAdd, Position);

    ECS_TAG_DEFINE(world, SimpleFooTag);
    ECS_ENTITY_DEFINE(world, SimpleFooEntity, 0);
    ECS_PREFAB_DEFINE(world, SimpleFooPrefab, 0);
    ECS_PIPELINE_DEFINE(world, SimpleFooPipeline, flecs.system.System, Tag);
    ECS_TAG_DEFINE(world, Simple_underscore);
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
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

void Modules_import_module_from_system() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, SimpleModule);
    ECS_SYSTEM(world, AddVtoP, EcsOnUpdate, simple.module.Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Modules_import_again() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m1 = ECS_IMPORT(world, SimpleModule);
    ecs_entity_t m2 = ECS_IMPORT(world, SimpleModule);

    test_assert(m1 != 0);
    test_assert(m2 != 0);
    test_assert(m1 == m2);
    
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
    test_assert(e == ecs_id(Move));
    
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
    test_assert(e == ecs_id(SimpleFooSystem));
    
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
    test_assert(e == ecs_id(SimpleFooTrigger));
    
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

void Modules_module_tag_on_namespace_on_add() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new_id(world);
    test_assert(parent != 0);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    test_assert(child != 0);

    ecs_add_id(world, child, EcsModule);
    test_assert( ecs_has_id(world, child, EcsModule));
    test_assert( ecs_has_id(world, parent, EcsModule));

    ecs_fini(world);
}

void Modules_module_tag_on_namespace_on_add_2_levels() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t root = ecs_new_id(world);
    test_assert(root != 0);

    ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, root);
    test_assert(parent != 0);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    test_assert(child != 0);

    ecs_add_id(world, child, EcsModule);
    test_assert( ecs_has_id(world, child, EcsModule));
    test_assert( ecs_has_id(world, parent, EcsModule));
    test_assert( ecs_has_id(world, root, EcsModule));

    ecs_fini(world);
}
