#include <api.h>

static ECS_DECLARE(MyTag);
static ECS_DECLARE(MyEntity);
static ECS_COMPONENT_DECLARE(Position);
static ECS_COMPONENT_DECLARE(Velocity);

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

void GlobalComponentIds_declare() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);

    ecs_entity_t e = create_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void GlobalComponentIds_declare_w_entity() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);

    ecs_entity_t e = create_entity_w_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void GlobalComponentIds_declare_2_world() {    
    ecs_world_t *world_1 = ecs_mini();

    ECS_COMPONENT_DEFINE(world_1, Position);
    ECS_COMPONENT_DEFINE(world_1, Velocity);

    ecs_entity_t e_pos = ecs_id(Position);
    ecs_entity_t e_vel = ecs_id(Velocity);

    ecs_world_t *world_2 = ecs_mini();

    /* Declare in reverse order, ensure that ids are still the same */
    ECS_COMPONENT_DEFINE(world_2, Velocity);
    ECS_COMPONENT_DEFINE(world_2, Position);

    test_assert(e_pos == ecs_id(Position));
    test_assert(e_vel == ecs_id(Velocity));

    ecs_fini(world_1);
    ecs_fini(world_2);
}

void GlobalComponentIds_declare_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG_DEFINE(world, MyTag);

    ecs_entity_t e = create_tag_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, MyTag));

    ecs_fini(world);
}

void GlobalComponentIds_declare_tag_w_entity() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG_DEFINE(world, MyTag);

    ecs_entity_t e = create_tag_entity_w_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, MyTag));

    ecs_fini(world);
}

void GlobalComponentIds_declare_entity() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_ENTITY_DEFINE(world, MyEntity, Position);

    ecs_entity_t e = return_entity();
    test_assert(e != 0);
    test_assert(e == MyEntity);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void GlobalComponentIds_reuse_300_component_ids() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t *ids = ecs_os_malloc_n(ecs_entity_t, 300);

    for (int i = 0; i < 300; i ++) {
        ids[i] = ecs_component_init(world, &(ecs_component_desc_t){
            .type.size = 1,
            .type.alignment = 1
        });
    }

    const ecs_world_info_t *info = ecs_get_world_info(world);
    ecs_entity_t info_last_component_id = info->last_component_id;
    ecs_entity_t info_last_id = info->last_id;
    ecs_entity_t next_id = ecs_new_id(world);

    ecs_fini(world);

    world = ecs_mini();

    for (int i = 0; i < 300; i ++) {
        ids[i] = ecs_component_init(world, &(ecs_component_desc_t){
            .entity.entity = ids[i],
            .type.size = 1,
            .type.alignment = 1
        });
    }

    info = ecs_get_world_info(world);
    test_int(info->last_component_id, info_last_component_id);
    test_int(info->last_id, info_last_id);
    test_int(next_id, ecs_new_id(world));

    ecs_os_free(ids);

    ecs_fini(world);
}
