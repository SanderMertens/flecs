#include <api.h>

ECS_TAG_DECLARE(MyTag);
ECS_ENTITY_DECLARE(MyEntity);
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);
ECS_TYPE_DECLARE(Movable);

static
ecs_entity_t create_entity(ecs_world_t *world) {
    return ecs_new(world, Position);
}

static
ecs_entity_t create_entity_w_entity(ecs_world_t *world) {
    return ecs_new_w_id(world, ecs_id(Position));
}

static
ecs_entity_t create_tag_entity(ecs_world_t *world) {
    return ecs_new(world, MyTag);
}

static
ecs_entity_t create_tag_entity_w_entity(ecs_world_t *world) {
    return ecs_new_w_id(world, MyTag);
}

static ecs_entity_t return_entity() {
    return MyEntity;
}

static 
ecs_entity_t create_type_entity(ecs_world_t *world) {
    return ecs_new(world, Movable);
}

void GlobalComponentIds_declare() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ecs_entity_t e = create_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void GlobalComponentIds_declare_w_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ecs_entity_t e = create_entity_w_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void GlobalComponentIds_declare_2_world() {    
    ecs_world_t *world_1 = ecs_init();

    ECS_COMPONENT_DEFINE(world_1, Position);
    ECS_COMPONENT_DEFINE(world_1, Velocity);

    ecs_entity_t e_pos = ecs_id(Position);
    ecs_entity_t e_vel = ecs_id(Velocity);

    ecs_world_t *world_2 = ecs_init();

    /* Declare in reverse order, ensure that ids are still the same */
    ECS_COMPONENT_DEFINE(world_2, Velocity);
    ECS_COMPONENT_DEFINE(world_2, Position);

    test_assert(e_pos == ecs_id(Position));
    test_assert(e_vel == ecs_id(Velocity));

    ecs_fini(world_1);
    ecs_fini(world_2);
}

void GlobalComponentIds_declare_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG_DEFINE(world, MyTag);

    ecs_entity_t e = create_tag_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, MyTag));

    ecs_fini(world);
}

void GlobalComponentIds_declare_tag_w_entity() {
    ecs_world_t *world = ecs_init();

    ECS_TAG_DEFINE(world, MyTag);

    ecs_entity_t e = create_tag_entity_w_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, MyTag));

    ecs_fini(world);
}

void GlobalComponentIds_declare_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_ENTITY_DEFINE(world, MyEntity, Position);

    ecs_entity_t e = return_entity();
    test_assert(e != 0);
    test_assert(e == MyEntity);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void GlobalComponentIds_declare_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);
    ECS_TYPE_DEFINE(world, Movable, Position, Velocity);

    ecs_entity_t e = create_type_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}
