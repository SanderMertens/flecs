#include <api.h>

static
void add_random(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    if (rand() % 2) {
        if (!entity) {
            entity = ecs_new_w_entity(world, component);
        } else {
            ecs_add_entity(world, entity, component);
        }
        test_assert( ecs_get_type(world, entity) != NULL);
        test_assert( ecs_has_entity(world, entity, component));
        test_assert( ecs_get_ptr_w_entity(world, entity, component) != NULL);
    }    
}

static
void set_random(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    void *ptr,
    void *expect,
    int32_t size)
{
    if (rand() % 2) {
        ecs_entity_t e = ecs_set_ptr_w_entity(world, entity, component, size, ptr);
        test_assert( ecs_get_type(world, e) != NULL);
        test_assert( ecs_has_entity(world, e, component));

        const void *comp_ptr = ecs_get_ptr_w_entity(world, e, component);
        test_assert(comp_ptr != NULL);

        test_assert( !memcmp(expect, comp_ptr, size) );
    }  
}

static
void Delete_above_1000(ecs_rows_t *rows) {
    int i;

    for (i = 0; i < rows->count; i ++) {
        if ((i + rows->frame_offset) > 1000) {
            ecs_delete(rows->world, rows->entities[i]);
        }
    }
}

static
void Add_random(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);

    int i;
    for (i = 0; i < rows->count; i ++) {
        add_random(rows->world, 0, ctx->component);
        add_random(rows->world, rows->entities[i], ctx->component_2);
        add_random(rows->world, rows->entities[i], ctx->component_3);
        ctx->entity_count ++;
    }
}

static
void Set_velocity_callback(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, v, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        v->x ++;
        v->y ++;
    }
}

static
void Set_random(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);             

    int i;
    for (i = 0; i < rows->count; i ++) {
        Position pos = {10, 20};
        set_random(rows->world, 0, ctx->component, &pos, &pos, sizeof(Position));

        Velocity vel = {30, 40};
        Velocity vel_expect = {31, 41};
        set_random(rows->world, rows->entities[i], ctx->component_2, &vel, &vel_expect,
            sizeof(Velocity));

        Rotation rot = {50};
        set_random(rows->world, rows->entities[i], ctx->component_3, &rot, &rot,
            sizeof(Rotation));

        ctx->entity_count ++;
    }
}

static
void create_delete_entity_random_components_staged(
    int32_t threads)
{
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_random, EcsOnUpdate, Position);
    ECS_SYSTEM(world, Delete_above_1000, EcsPostUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position), .component_2 = ecs_entity(Velocity), .component_3 = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_bulk_new(world, Position, 500);
    ecs_bulk_new(world, Type, 500);

    if (threads) {
        ecs_set_threads(world, threads);
    }

    uint64_t i;
    for (i = 0; i < 100; i ++) {
        ecs_progress(world, 1);
    }

    ecs_fini(world);
}

static
void set_entity_random_components(
    int32_t threads)
{
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Set_random, EcsOnUpdate, Position);
    ECS_TRIGGER(world, Set_velocity_callback, EcsOnSet, Velocity, NULL);
    ECS_SYSTEM(world, Delete_above_1000, EcsPostUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position), .component_2 = ecs_entity(Velocity), .component_3 = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_bulk_new(world, Position, 3);
    //ecs_bulk_new(world, Type, 500);

    if (threads) {
        ecs_set_threads(world, threads);
    }

    int i;
    for (i = 0; i < 100; i ++) {
        ecs_progress(world, 1);
    }

    ecs_fini(world);
}

void Stresstests_create_delete_entity_random_components() {
    // Implement testcase
}

void Stresstests_set_entity_random_components() {
    // Implement testcase
}

void Stresstests_create_delete_entity_random_components_staged() {
    create_delete_entity_random_components_staged(0);
}

void Stresstests_set_entity_random_components_staged() {
    set_entity_random_components(0);
}

void Stresstests_create_delete_entity_random_components_2_threads() {
    create_delete_entity_random_components_staged(2);
}

void Stresstests_set_entity_random_components_2_threads() {
    set_entity_random_components(2);
}

void Stresstests_create_delete_entity_random_components_6_threads() {
    create_delete_entity_random_components_staged(6);
}

void Stresstests_set_entity_random_components_6_threads() {
    set_entity_random_components(6);
}

void Stresstests_create_delete_entity_random_components_12_threads() {
    create_delete_entity_random_components_staged(12);
}

void Stresstests_set_entity_random_components_12_threads() {
    set_entity_random_components(12);
}
