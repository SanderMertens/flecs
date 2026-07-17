#include <meta.h>
#include "flecs.h"

typedef struct {
    ecs_i32_t a;
    ecs_i32_t b;
} StructWithInts;

typedef struct {
    ecs_string_t a;
    ecs_i32_t b;
} StructWithStrings;

typedef struct {
    StructWithInts inner;
    ecs_i32_t x;
} NestedStruct;

typedef struct {
    int16_t value;
} SetRttOpaque;

static
int value_cmp(ecs_world_t *world, ecs_entity_t type, const void *a,
    const void *b)
{
    return ecs_value_compare(world,
        &(ecs_value_t){.type = type, .ptr = ECS_CONST_CAST(void*, a)},
        &(ecs_value_t){.type = type, .ptr = ECS_CONST_CAST(void*, b)});
}

static
bool value_eq(ecs_world_t *world, ecs_entity_t type, const void *a,
    const void *b)
{
    return ecs_value_equals(world,
        &(ecs_value_t){.type = type, .ptr = ECS_CONST_CAST(void*, a)},
        &(ecs_value_t){.type = type, .ptr = ECS_CONST_CAST(void*, b)});
}

static
ecs_entity_t define_struct_with_ints(ecs_world_t *world) {
    ECS_COMPONENT(world, StructWithInts);
    ecs_struct(world, {
        .entity = ecs_id(StructWithInts),
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });
    return ecs_id(StructWithInts);
}

void SetRttHooks_compare_struct_with_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = define_struct_with_ints(world);

    const ecs_type_info_t *ti = ecs_get_type_info(world, t);
    test_assert(!(ti->hooks.cmp && !(ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL)));

    test_int(ecs_set_rtt_compare(world, t), 0);

    StructWithInts a = {10, 20};
    StructWithInts b = {10, 25};
    StructWithInts c = {10, 20};

    test_assert(value_cmp(world, t, &a, &b) < 0);
    test_assert(value_cmp(world, t, &b, &a) > 0);
    test_int(value_cmp(world, t, &a, &c), 0);
    test_int(value_cmp(world, t, &a, &a), 0);

    ecs_fini(world);
}

void SetRttHooks_equals_struct_with_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = define_struct_with_ints(world);

    test_int(ecs_set_rtt_equals(world, t), 0);

    StructWithInts a = {10, 20};
    StructWithInts b = {10, 25};
    StructWithInts c = {10, 20};

    test_bool(value_eq(world, t, &a, &b), false);
    test_bool(value_eq(world, t, &a, &c), true);
    test_bool(value_eq(world, t, &a, &a), true);

    ecs_fini(world);
}

void SetRttHooks_compare_generates_equals(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = define_struct_with_ints(world);

    test_int(ecs_set_rtt_compare(world, t), 0);

    const ecs_type_info_t *ti = ecs_get_type_info(world, t);
    test_assert(ti->hooks.cmp != NULL);
    test_assert(!(ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL));
    test_assert(ti->hooks.equals != NULL);
    test_assert(!(ti->hooks.flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL));

    StructWithInts a = {10, 20};
    StructWithInts b = {10, 20};
    StructWithInts c = {10, 25};

    test_bool(value_eq(world, t, &a, &b), true);
    test_bool(value_eq(world, t, &a, &c), false);

    ecs_fini(world);
}

void SetRttHooks_equals_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StructWithStrings);
    ecs_struct(world, {
        .entity = ecs_id(StructWithStrings),
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });
    ecs_entity_t t = ecs_id(StructWithStrings);

    test_int(ecs_set_rtt_equals(world, t), 0);

    StructWithStrings a = {(char*)"hello", 10};
    StructWithStrings b = {(char*)"hello", 10};
    StructWithStrings c = {(char*)"world", 10};
    StructWithStrings d = {(char*)"hello", 20};

    test_bool(value_eq(world, t, &a, &b), true);
    test_bool(value_eq(world, t, &a, &c), false);
    test_bool(value_eq(world, t, &a, &d), false);

    ecs_fini(world);
}

void SetRttHooks_compare_nested_struct(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StructWithInts);
    ecs_struct(world, {
        .entity = ecs_id(StructWithInts),
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    ECS_COMPONENT(world, NestedStruct);
    ecs_struct(world, {
        .entity = ecs_id(NestedStruct),
        .members = {
            {"inner", ecs_id(StructWithInts)},
            {"x", ecs_id(ecs_i32_t)},
        }
    });
    ecs_entity_t t = ecs_id(NestedStruct);

    const ecs_type_info_t *inner_ti =
        ecs_get_type_info(world, ecs_id(StructWithInts));
    test_assert(!(inner_ti->hooks.cmp &&
        !(inner_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL)));

    test_int(ecs_set_rtt_compare(world, t), 0);

    inner_ti = ecs_get_type_info(world, ecs_id(StructWithInts));
    test_assert(inner_ti->hooks.cmp != NULL);
    test_assert(!(inner_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL));

    NestedStruct a = {{10, 20}, 1};
    NestedStruct b = {{10, 20}, 1};
    NestedStruct c = {{10, 25}, 1};
    NestedStruct d = {{10, 20}, 2};

    test_int(value_cmp(world, t, &a, &b), 0);
    test_assert(value_cmp(world, t, &a, &c) < 0);
    test_assert(value_cmp(world, t, &a, &d) < 0);

    ecs_fini(world);
}

void SetRttHooks_equals_nested_struct(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StructWithInts);
    ecs_struct(world, {
        .entity = ecs_id(StructWithInts),
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    ECS_COMPONENT(world, NestedStruct);
    ecs_struct(world, {
        .entity = ecs_id(NestedStruct),
        .members = {
            {"inner", ecs_id(StructWithInts)},
            {"x", ecs_id(ecs_i32_t)},
        }
    });
    ecs_entity_t t = ecs_id(NestedStruct);

    test_int(ecs_set_rtt_equals(world, t), 0);

    const ecs_type_info_t *inner_ti =
        ecs_get_type_info(world, ecs_id(StructWithInts));
    test_assert(inner_ti->hooks.equals != NULL);
    test_assert(!(inner_ti->hooks.flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL));

    NestedStruct a = {{10, 20}, 1};
    NestedStruct b = {{10, 20}, 1};
    NestedStruct c = {{10, 25}, 1};

    test_bool(value_eq(world, t, &a, &b), true);
    test_bool(value_eq(world, t, &a, &c), false);

    ecs_fini(world);
}

static
ecs_entity_t define_struct_without_hooks(ecs_world_t *world) {
    ECS_COMPONENT(world, StructWithInts);
    ecs_struct(world, {
        .entity = ecs_id(StructWithInts),
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });
    return ecs_id(StructWithInts);
}

void SetRttHooks_compare_array(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t elem = define_struct_without_hooks(world);

    ecs_entity_t t = ecs_array(world, {
        .type = elem,
        .count = 3
    });

    const ecs_type_info_t *elem_ti = ecs_get_type_info(world, elem);
    test_assert(!(elem_ti->hooks.cmp &&
        !(elem_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL)));

    test_int(ecs_set_rtt_compare(world, t), 0);

    elem_ti = ecs_get_type_info(world, elem);
    test_assert(elem_ti->hooks.cmp != NULL);
    test_assert(!(elem_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL));

    StructWithInts a[3] = {{1, 1}, {2, 2}, {3, 3}};
    StructWithInts b[3] = {{1, 1}, {2, 2}, {3, 3}};
    StructWithInts c[3] = {{1, 1}, {2, 2}, {3, 4}};

    test_int(value_cmp(world, t, a, b), 0);
    test_assert(value_cmp(world, t, a, c) < 0);
    test_bool(value_eq(world, t, a, b), true);
    test_bool(value_eq(world, t, a, c), false);

    ecs_fini(world);
}

void SetRttHooks_compare_vector(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t elem = define_struct_without_hooks(world);

    ecs_entity_t t = ecs_vector(world, {
        .type = elem
    });

    const ecs_type_info_t *elem_ti = ecs_get_type_info(world, elem);
    test_assert(!(elem_ti->hooks.cmp &&
        !(elem_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL)));

    test_int(ecs_set_rtt_compare(world, t), 0);

    elem_ti = ecs_get_type_info(world, elem);
    test_assert(elem_ti->hooks.cmp != NULL);

    const ecs_type_info_t *ti = ecs_get_type_info(world, t);

    ecs_vec_t a, b, c;
    ecs_vec_init_t(NULL, &a, StructWithInts, 2);
    ecs_vec_init_t(NULL, &b, StructWithInts, 2);
    ecs_vec_init_t(NULL, &c, StructWithInts, 2);
    ecs_vec_set_count_t(NULL, &a, StructWithInts, 2);
    ecs_vec_set_count_t(NULL, &b, StructWithInts, 2);
    ecs_vec_set_count_t(NULL, &c, StructWithInts, 2);
    StructWithInts *av = ecs_vec_first(&a);
    StructWithInts *bv = ecs_vec_first(&b);
    StructWithInts *cv = ecs_vec_first(&c);
    av[0] = (StructWithInts){1, 1}; av[1] = (StructWithInts){2, 2};
    bv[0] = (StructWithInts){1, 1}; bv[1] = (StructWithInts){2, 2};
    cv[0] = (StructWithInts){1, 1}; cv[1] = (StructWithInts){2, 3};

    test_int(ti->hooks.cmp(&a, &b, ti), 0);
    test_assert(ti->hooks.cmp(&a, &c, ti) < 0);
    test_bool(ti->hooks.equals(&a, &b, ti), true);
    test_bool(ti->hooks.equals(&a, &c, ti), false);

    ecs_vec_fini_t(NULL, &a, StructWithInts);
    ecs_vec_fini_t(NULL, &b, StructWithInts);
    ecs_vec_fini_t(NULL, &c, StructWithInts);

    ecs_fini(world);
}

void SetRttHooks_compare_map(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = define_struct_without_hooks(world);

    ecs_entity_t t = ecs_map_type(world, {
        .key_type = ecs_id(ecs_u64_t),
        .type = value
    });

    const ecs_type_info_t *value_ti = ecs_get_type_info(world, value);
    test_assert(!(value_ti->hooks.cmp &&
        !(value_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL)));

    test_int(ecs_set_rtt_compare(world, t), 0);

    value_ti = ecs_get_type_info(world, value);
    test_assert(value_ti->hooks.cmp != NULL);

    const ecs_type_info_t *ti = ecs_get_type_info(world, t);
    test_assert(ti->hooks.cmp != NULL);
    test_assert(ti->hooks.equals != NULL);

    ecs_map_t a, b, c;
    ecs_map_init(&a, NULL);
    ecs_map_init(&b, NULL);
    ecs_map_init(&c, NULL);
    *(StructWithInts*)ecs_map_ensure(&a, 1) = (StructWithInts){10, 10};
    *(StructWithInts*)ecs_map_ensure(&b, 1) = (StructWithInts){10, 10};
    *(StructWithInts*)ecs_map_ensure(&c, 1) = (StructWithInts){10, 20};

    test_int(ti->hooks.cmp(&a, &b, ti), 0);
    test_assert(ti->hooks.cmp(&a, &c, ti) != 0);
    test_bool(ti->hooks.equals(&a, &b, ti), true);
    test_bool(ti->hooks.equals(&a, &c, ti), false);

    ecs_map_fini(&a);
    ecs_map_fini(&b);
    ecs_map_fini(&c);

    ecs_fini(world);
}

void SetRttHooks_compare_opaque_without_hook_fails(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, SetRttOpaque);

    ecs_entity_t descriptor = ecs_struct(world, {
        .members = {
            {.name = "value", .type = ecs_id(ecs_i16_t)},
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(SetRttOpaque),
        .type = {.as_type = descriptor}
    });

    typedef struct {
        SetRttOpaque op;
    } StructWithOpaque;

    ECS_COMPONENT(world, StructWithOpaque);
    ecs_struct(world, {
        .entity = ecs_id(StructWithOpaque),
        .members = {
            {"op", ecs_id(SetRttOpaque)},
        }
    });

    test_int(ecs_set_rtt_compare(world, ecs_id(SetRttOpaque)), -1);
    test_int(ecs_set_rtt_compare(world, ecs_id(StructWithOpaque)), -1);

    const ecs_type_info_t *ti =
        ecs_get_type_info(world, ecs_id(StructWithOpaque));
    test_assert(!(ti->hooks.cmp &&
        !(ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL)));

    ecs_fini(world);
}

void SetRttHooks_compare_primitive_noop(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_set_rtt_compare(world, ecs_id(ecs_i32_t)), 0);
    test_int(ecs_set_rtt_equals(world, ecs_id(ecs_i32_t)), 0);

    ecs_i32_t a = 10, b = 20, c = 10;
    test_assert(value_cmp(world, ecs_id(ecs_i32_t), &a, &b) < 0);
    test_bool(value_eq(world, ecs_id(ecs_i32_t), &a, &c), true);

    ecs_fini(world);
}

void SetRttHooks_compare_idempotent(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = define_struct_with_ints(world);

    test_int(ecs_set_rtt_compare(world, t), 0);
    test_int(ecs_set_rtt_compare(world, t), 0);
    test_int(ecs_set_rtt_compare(world, t), 0);

    StructWithInts a = {10, 20};
    StructWithInts b = {10, 25};

    test_assert(value_cmp(world, t, &a, &b) < 0);

    ecs_fini(world);
}

void SetRttHooks_value_compare(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t a = 10, b = 20, c = 10;
    test_assert(value_cmp(world, ecs_id(ecs_i32_t), &a, &b) < 0);
    test_assert(value_cmp(world, ecs_id(ecs_i32_t), &b, &a) > 0);
    test_int(value_cmp(world, ecs_id(ecs_i32_t), &a, &c), 0);

    ecs_fini(world);
}

void SetRttHooks_value_equals(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t a = 10, b = 20, c = 10;
    test_bool(value_eq(world, ecs_id(ecs_i32_t), &a, &b), false);
    test_bool(value_eq(world, ecs_id(ecs_i32_t), &a, &c), true);

    ecs_fini(world);
}

void SetRttHooks_value_different_types(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t i = 10;
    ecs_u32_t u = 10;

    ecs_value_t a = {.type = ecs_id(ecs_i32_t), .ptr = &i};
    ecs_value_t b = {.type = ecs_id(ecs_u32_t), .ptr = &u};

    test_assert(ecs_value_compare(world, &a, &b) != 0);
    test_bool(ecs_value_equals(world, &a, &b), false);

    ecs_value_t a2 = {.type = ecs_id(ecs_i32_t), .ptr = &i};
    test_int(ecs_value_compare(world, &a, &a2), 0);
    test_bool(ecs_value_equals(world, &a, &a2), true);

    ecs_fini(world);
}
