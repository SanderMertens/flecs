#include <meta.h>

static
void meta_test_map(
    ecs_world_t *world,
    ecs_entity_t t,
    ecs_entity_t key_type,
    ecs_entity_t value_type)
{
    const EcsComponent *ct = ecs_get(world, t, EcsComponent);
    test_assert(ct != NULL);
    test_int(ct->size, ECS_SIZEOF(ecs_map_t));
    test_int(ct->alignment, ECS_ALIGNOF(ecs_map_t));

    const EcsType *mt = ecs_get(world, t, EcsType);
    test_assert(mt != NULL);
    test_assert(mt->kind == EcsMapType);

    const EcsMap *m = ecs_get(world, t, EcsMap);
    test_assert(m != NULL);
    test_assert(m->key_type == key_type);
    test_assert(m->type == value_type);
}

void MapTypes_map_i32_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_str(ecs_get_name(world, m), "M");

    meta_test_map(world, m, ecs_id(ecs_i32_t), ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void MapTypes_map_i64_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_i64_t),
        .type = ecs_id(ecs_string_t)
    });

    test_assert(m != 0);
    test_str(ecs_get_name(world, m), "M");

    meta_test_map(world, m, ecs_id(ecs_i64_t), ecs_id(ecs_string_t));

    ecs_fini(world);
}

void MapTypes_map_entity_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_entity_t),
        .type = t
    });

    test_assert(m != 0);
    test_str(ecs_get_name(world, m), "M");

    meta_test_map(world, m, ecs_id(ecs_entity_t), t);

    ecs_fini(world);
}

void MapTypes_map_w_short_notation(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_bool_t)
    });

    test_assert(m != 0);
    test_str(ecs_get_name(world, m), "M");
    test_assert(ecs_has(world, m, EcsMap));

    ecs_fini(world);
}

void MapTypes_map_no_key_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_assert(ecs_get(world, m, EcsType) == NULL);

    ecs_fini(world);
}

void MapTypes_map_no_value_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_assert(ecs_get(world, m, EcsType) == NULL);

    ecs_fini(world);
}

void MapTypes_map_invalid_key_type_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_string_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_assert(ecs_get(world, m, EcsType) == NULL);

    ecs_fini(world);
}

void MapTypes_map_invalid_key_type_f32(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_f32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_assert(ecs_get(world, m, EcsType) == NULL);

    ecs_fini(world);
}

void MapTypes_map_invalid_key_type_f64(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_f64_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_assert(ecs_get(world, m, EcsType) == NULL);

    ecs_fini(world);
}

void MapTypes_map_invalid_key_type_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = t,
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_assert(ecs_get(world, m, EcsType) == NULL);

    ecs_fini(world);
}

void MapTypes_map_enum_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
        .constants = {
            {"Red"}, {"Green"}, {"Blue"}
        }
    });

    test_assert(c != 0);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = c,
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_str(ecs_get_name(world, m), "M");

    meta_test_map(world, m, c, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void MapTypes_map_bitmask_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .entity = ecs_entity(world, {.name = "Toppings"}),
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    test_assert(b != 0);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = b,
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_str(ecs_get_name(world, m), "M");

    meta_test_map(world, m, b, ecs_id(ecs_i32_t));

    ecs_fini(world);
}
