#include <meta.h>

static void meta_test_map(
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

void MapTypes_map_string_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_string_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);
    test_str(ecs_get_name(world, m), "M");

    meta_test_map(world, m, ecs_id(ecs_string_t), ecs_id(ecs_i32_t));

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

void MapTypes_map_string_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_string_t)}
        }
    });

    test_assert(t != 0);

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .entity = ecs_entity(world, {.name = "M"}),
        .key_type = ecs_id(ecs_string_t),
        .type = t
    });

    test_assert(m != 0);
    test_str(ecs_get_name(world, m), "M");

    meta_test_map(world, m, ecs_id(ecs_string_t), t);

    ecs_fini(world);
}

void MapTypes_map_string_key_layout(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_string_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    ecs_map_t value = {0};
    test_assert(ecs_ptr_from_json(world, m, &value, 
        "{\"foo\": 10, \"bar\": 20}", NULL) != NULL);

    test_int(ecs_map_count(&value), 2);

    bool foo_found = false, bar_found = false;
    ecs_map_iter_t it = ecs_map_iter(&value);
    while (ecs_map_next(&it)) {
        const char *key = (const char*)(uintptr_t)ecs_map_key(&it);
        int32_t elem = *(int32_t*)&it.res[1];
        if (!ecs_os_strcmp(key, "foo")) {
            test_int(elem, 10);
            foo_found = true;
        } else if (!ecs_os_strcmp(key, "bar")) {
            test_int(elem, 20);
            bar_found = true;
        } else {
            test_assert(false);
        }
    }

    test_assert(foo_found);
    test_assert(bar_found);

    ecs_ptr_fini(world, m, &value);

    ecs_fini(world);
}

void MapTypes_map_string_key_populate_from_c(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_string_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    ecs_map_t value;
    ecs_map_init(&value, NULL);
    char *foo = ecs_os_strdup("foo");
    ecs_map_insert(&value, (ecs_map_key_t)(uintptr_t)foo, 10);

    ecs_map_t copy = {0};
    ecs_ptr_copy(world, m, &copy, &value);
    test_int(ecs_map_count(&copy), 1);

    ecs_map_iter_t it = ecs_map_iter(&copy);
    test_assert(ecs_map_next(&it));
    const char *key = (const char*)(uintptr_t)ecs_map_key(&it);
    test_str(key, "foo");
    test_assert(key != foo);
    test_int(*(int32_t*)&it.res[1], 10);
    test_assert(!ecs_map_next(&it));

    char *json = ecs_ptr_to_json(world, m, &value);
    test_str(json, "{\"foo\":10}");
    ecs_os_free(json);

    ecs_ptr_fini(world, m, &copy);
    ecs_ptr_fini(world, m, &value);

    ecs_fini(world);
}
