#include <meta.h>

typedef struct {
    int32_t value;
} OpaqueInt;

static int OpaqueInt_serialize(
    const ecs_serializer_t *s,
    const void *ptr)
{
    return s->value(s, ecs_id(ecs_i32_t), &((const OpaqueInt*)ptr)->value);
}

static void OpaqueInt_assign_int(
    void *ptr,
    int64_t value)
{
    ((OpaqueInt*)ptr)->value = (int32_t)value;
}

void ValueType_init_unconstrained(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, {
        .entity = ecs_entity(world, { .name = "T" })
    });
    test_assert(t != 0);

    const EcsType *mt = ecs_get(world, t, EcsType);
    test_assert(mt != NULL);
    test_int(mt->kind, EcsValueType);

    const EcsComponent *c = ecs_get(world, t, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, ECS_SIZEOF(ecs_value_t));
    test_int(c->alignment, ECS_ALIGNOF(ecs_value_t));

    const EcsValue *v = ecs_get(world, t, EcsValue);
    test_assert(v != NULL);
    test_uint(v->type, 0);

    ecs_fini(world);
}

void ValueType_init_w_constraint(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, {
        .entity = ecs_entity(world, { .name = "T" }),
        .type = ecs_id(ecs_i32_t)
    });
    test_assert(t != 0);

    const EcsType *mt = ecs_get(world, t, EcsType);
    test_assert(mt != NULL);
    test_int(mt->kind, EcsValueType);

    const EcsValue *v = ecs_get(world, t, EcsValue);
    test_assert(v != NULL);
    test_uint(v->type, ecs_id(ecs_i32_t));

    const ecs_type_info_t *ti = ecs_get_type_info(world, t);
    test_assert(ti != NULL);
    test_assert(ti->hooks.copy != NULL);
    test_assert(ti->hooks.move != NULL);
    test_assert(ti->hooks.dtor != NULL);
    test_assert(ti->hooks.cmp != NULL);
    test_assert(ti->hooks.equals != NULL);

    ecs_fini(world);
}

void ValueType_init_w_tag_constraint(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_new(world);

    ecs_log_set_level(-4);
    ecs_entity_t t = ecs_value_type(world, { .type = tag });
    test_assert(t == 0);

    ecs_fini(world);
}

void ValueType_init_w_value_constraint(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_value_t) });
    test_assert(t == 0);

    ecs_fini(world);
}

void ValueType_init_w_constrained_value_constraint(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t inner = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });
    test_assert(inner != 0);

    ecs_log_set_level(-4);
    ecs_entity_t t = ecs_value_type(world, { .type = inner });
    test_assert(t == 0);

    ecs_fini(world);
}

void ValueType_builtin_accepts_any(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    test_bool(true, ecs_value_type_accepts(
        world, ecs_id(ecs_value_t), ecs_id(ecs_i32_t)));
    test_bool(true, ecs_value_type_accepts(
        world, ecs_id(ecs_value_t), ecs_id(ecs_string_t)));
    test_bool(true, ecs_value_type_accepts(world, ecs_id(ecs_value_t), s));
    test_bool(false, ecs_value_type_accepts(
        world, ecs_id(ecs_value_t), ecs_id(ecs_value_t)));
    test_bool(false, ecs_value_type_accepts(world, ecs_id(ecs_value_t), 0));

    ecs_fini(world);
}

void ValueType_primitive_accepts_exact(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });
    test_assert(t != 0);

    test_bool(true, ecs_value_type_accepts(world, t, ecs_id(ecs_i32_t)));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_i64_t)));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_u32_t)));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_f32_t)));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_string_t)));

    ecs_entity_t alias = ecs_primitive(world, { .kind = EcsI32 });
    test_bool(false, ecs_value_type_accepts(world, t, alias));

    ecs_fini(world);
}

void ValueType_primitive_set_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_int(&cur, 10));

    test_uint(v.type, ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_int(*(int32_t*)v.ptr, 10);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_primitive_set_float(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_float(&cur, 10));

    test_uint(v.type, ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_int(*(int32_t*)v.ptr, 10);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_primitive_set_out_of_range(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_u8_t) });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_set_int(&cur, 1000));

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_primitive_set_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_string_t) });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_string(&cur, "Hello"));

    test_uint(v.type, ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "Hello");

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_primitive_set_string_to_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_string(&cur, "10"));

    test_uint(v.type, ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_int(*(int32_t*)v.ptr, 10);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_primitive_set_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_entity_t) });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_entity(&cur, e));

    test_uint(v.type, ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, e);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_primitive_set_type_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_entity_t) });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_entity(&cur, ecs_id(ecs_i32_t)));

    test_uint(v.type, ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, ecs_id(ecs_i32_t));

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_primitive_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "i32"));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_pop(&cur));

    test_uint(v.type, ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_int(*(int32_t*)v.ptr, 10);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_primitive_member_mismatch(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_push(&cur));

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_member(&cur, "i64"));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_enum_accepts_exact(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t color = ecs_enum(world, {
        .constants = {{ "Red" }, { "Green" }, { "Blue" }}
    });

    ecs_entity_t other = ecs_enum(world, {
        .constants = {{ "Red" }, { "Green" }, { "Blue" }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = color });
    test_assert(t != 0);

    test_bool(true, ecs_value_type_accepts(world, t, color));
    test_bool(false, ecs_value_type_accepts(world, t, other));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_i32_t)));

    ecs_fini(world);
}

void ValueType_enum_set_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t color = ecs_enum(world, {
        .constants = {{ "Red" }, { "Green" }, { "Blue" }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = color });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_string(&cur, "Blue"));

    test_uint(v.type, color);
    test_assert(v.ptr != NULL);
    test_int(*(int32_t*)v.ptr, 2);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_enum_set_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t color = ecs_enum(world, {
        .constants = {{ "Red" }, { "Green" }, { "Blue" }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = color });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_int(&cur, 1));

    test_uint(v.type, color);
    test_assert(v.ptr != NULL);
    test_int(*(int32_t*)v.ptr, 1);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_bitmask_accepts_exact(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t toppings = ecs_bitmask(world, {
        .constants = {{ "Bacon" }, { "Lettuce" }, { "Tomato" }}
    });

    ecs_entity_t other = ecs_bitmask(world, {
        .constants = {{ "Bacon" }, { "Lettuce" }, { "Tomato" }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = toppings });
    test_assert(t != 0);

    test_bool(true, ecs_value_type_accepts(world, t, toppings));
    test_bool(false, ecs_value_type_accepts(world, t, other));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_u32_t)));

    ecs_fini(world);
}

void ValueType_bitmask_set_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t toppings = ecs_bitmask(world, {
        .constants = {{ "Bacon" }, { "Lettuce" }, { "Tomato" }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = toppings });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_string(&cur, "Bacon|Tomato"));

    test_uint(v.type, toppings);
    test_assert(v.ptr != NULL);
    test_uint(*(uint32_t*)v.ptr, 1 | 4);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_struct_accepts_exact(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });
    test_assert(t != 0);

    test_bool(true, ecs_value_type_accepts(world, t, base));

    ecs_fini(world);
}

void ValueType_struct_accepts_derived(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });
    test_bool(true, ecs_value_type_accepts(world, t, derived));

    ecs_fini(world);
}

void ValueType_struct_accepts_derived_chain(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived_2 = ecs_entity(world, { .name = "Derived2" });
    ecs_add_pair(world, derived_2, EcsIsA, derived);
    ecs_struct(world, {
        .entity = derived_2,
        .members = {{ "z", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });
    test_bool(true, ecs_value_type_accepts(world, t, derived_2));

    ecs_entity_t t2 = ecs_value_type(world, { .type = derived });
    test_bool(true, ecs_value_type_accepts(world, t2, derived_2));
    test_bool(false, ecs_value_type_accepts(world, t2, base));

    ecs_fini(world);
}

void ValueType_struct_rejects_base(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = derived });
    test_bool(false, ecs_value_type_accepts(world, t, base));

    ecs_fini(world);
}

void ValueType_struct_rejects_unrelated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t other = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Other" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });
    test_bool(false, ecs_value_type_accepts(world, t, other));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_i32_t)));

    ecs_fini(world);
}

void ValueType_struct_member_derived(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "Derived"));
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "x"));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_member(&cur, "y"));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_uint(v.type, derived);
    test_assert(v.ptr != NULL);
    test_int(((int32_t*)v.ptr)[0], 10);
    test_int(((int32_t*)v.ptr)[1], 20);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_struct_member_unrelated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Other" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_push(&cur));

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_member(&cur, "Other"));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_struct_set_entity_type_only(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_entity(&cur, derived));

    test_uint(v.type, derived);
    test_assert(v.ptr == NULL);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_struct_set_entity_unrelated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t other = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Other" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_set_entity(&cur, other));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_struct_set_entity_replaces_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "s", ecs_id(ecs_string_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "Base"));
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_string(&cur, "Hello"));
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_uint(v.type, base);
    test_assert(v.ptr != NULL);

    cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_entity(&cur, base));

    test_uint(v.type, base);
    test_assert(v.ptr == NULL);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_struct_set_string_type_only(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_string(&cur, "Derived"));

    test_uint(v.type, derived);
    test_assert(v.ptr == NULL);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_struct_set_string_unresolved(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_set_string(&cur, "DoesNotExist"));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_struct_set_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_set_int(&cur, 10));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_array_accepts_exact(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array(world, {
        .type = ecs_id(ecs_i32_t), .count = 3 });
    ecs_entity_t other = ecs_array(world, {
        .type = ecs_id(ecs_i32_t), .count = 3 });

    ecs_entity_t t = ecs_value_type(world, { .type = a });
    test_assert(t != 0);

    test_bool(true, ecs_value_type_accepts(world, t, a));
    test_bool(false, ecs_value_type_accepts(world, t, other));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_i32_t)));

    ecs_fini(world);
}

void ValueType_array_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "A" }),
        .type = ecs_id(ecs_i32_t), .count = 3 });

    ecs_entity_t t = ecs_value_type(world, { .type = a });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "A"));
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 30));
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_uint(v.type, a);
    test_assert(v.ptr != NULL);
    test_int(((int32_t*)v.ptr)[0], 10);
    test_int(((int32_t*)v.ptr)[1], 20);
    test_int(((int32_t*)v.ptr)[2], 30);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_array_set_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "A" }),
        .type = ecs_id(ecs_i32_t), .count = 3 });

    ecs_entity_t t = ecs_value_type(world, { .type = a });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);

    ecs_value_t src = ecs_value_new(world, a);
    ((int32_t*)src.ptr)[0] = 1;
    ((int32_t*)src.ptr)[1] = 2;
    ((int32_t*)src.ptr)[2] = 3;
    test_int(0, ecs_meta_set_value(&cur, &src));
    ecs_value_fini(world, &src);

    test_uint(v.type, a);
    test_assert(v.ptr != NULL);
    test_int(((int32_t*)v.ptr)[0], 1);
    test_int(((int32_t*)v.ptr)[1], 2);
    test_int(((int32_t*)v.ptr)[2], 3);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_vector_accepts_exact(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vt = ecs_vector(world, { .type = ecs_id(ecs_i32_t) });
    ecs_entity_t other = ecs_vector(world, { .type = ecs_id(ecs_i32_t) });

    ecs_entity_t t = ecs_value_type(world, { .type = vt });
    test_assert(t != 0);

    test_bool(true, ecs_value_type_accepts(world, t, vt));
    test_bool(false, ecs_value_type_accepts(world, t, other));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_i32_t)));

    ecs_fini(world);
}

void ValueType_vector_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vt = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "V" }),
        .type = ecs_id(ecs_i32_t) });

    ecs_entity_t t = ecs_value_type(world, { .type = vt });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "V"));
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_uint(v.type, vt);
    test_assert(v.ptr != NULL);
    ecs_vec_t *vec = v.ptr;
    test_int(ecs_vec_count(vec), 2);
    test_int(ecs_vec_get_t(vec, int32_t, 0)[0], 10);
    test_int(ecs_vec_get_t(vec, int32_t, 1)[0], 20);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_map_accepts_exact(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .key_type = ecs_id(ecs_i64_t), .type = ecs_id(ecs_i32_t) });
    ecs_entity_t other = ecs_map_type(world, {
        .key_type = ecs_id(ecs_i64_t), .type = ecs_id(ecs_i32_t) });

    ecs_entity_t t = ecs_value_type(world, { .type = m });
    test_assert(t != 0);

    test_bool(true, ecs_value_type_accepts(world, t, m));
    test_bool(false, ecs_value_type_accepts(world, t, other));

    ecs_fini(world);
}

void ValueType_map_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "M" }),
        .key_type = ecs_id(ecs_i64_t), .type = ecs_id(ecs_i32_t) });

    ecs_entity_t t = ecs_value_type(world, { .type = m });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "M"));
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "10"));
    test_int(0, ecs_meta_set_int(&cur, 100));
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_uint(v.type, m);
    test_assert(v.ptr != NULL);
    ecs_map_t *map = v.ptr;
    test_int(ecs_map_count(map), 1);
    ecs_map_val_t *value = ecs_map_get(map, 10);
    test_assert(value != NULL);
    test_int(*(int32_t*)value, 100);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_opaque_accepts_exact(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueInt);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueInt),
        .type.as_type = ecs_id(ecs_i32_t),
        .type.serialize = OpaqueInt_serialize,
        .type.assign_int = OpaqueInt_assign_int
    });

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(OpaqueInt) });
    test_assert(t != 0);

    test_bool(true, ecs_value_type_accepts(world, t, ecs_id(OpaqueInt)));
    test_bool(false, ecs_value_type_accepts(world, t, ecs_id(ecs_i32_t)));

    ecs_fini(world);
}

void ValueType_opaque_set_int(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueInt);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueInt),
        .type.as_type = ecs_id(ecs_i32_t),
        .type.serialize = OpaqueInt_serialize,
        .type.assign_int = OpaqueInt_assign_int
    });

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(OpaqueInt) });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_int(&cur, 10));

    test_uint(v.type, ecs_id(OpaqueInt));
    test_assert(v.ptr != NULL);
    test_int(((OpaqueInt*)v.ptr)->value, 10);

    char *json = ecs_ptr_to_json(world, t, &v);
    test_assert(json != NULL);
    test_str(json, "{\"OpaqueInt\":10}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_set_value_derived(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    int32_t src[] = {10, 20};
    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_value(&cur, &(ecs_value_t){derived, src}));

    test_uint(v.type, derived);
    test_assert(v.ptr != NULL);
    test_int(((int32_t*)v.ptr)[0], 10);
    test_int(((int32_t*)v.ptr)[1], 20);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_set_value_unrelated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t other = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Other" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    int32_t src = 10;
    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_set_value(&cur, &(ecs_value_t){other, &src}));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_set_value_convert_primitive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    int64_t src = 10;
    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_value(&cur,
        &(ecs_value_t){ecs_id(ecs_i64_t), &src}));

    test_uint(v.type, ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_int(*(int32_t*)v.ptr, 10);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_set_value_from_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    int32_t data[] = {10, 20};
    ecs_value_t src = ecs_value_init(world, derived, data);

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_value(&cur, &(ecs_value_t){t, &src}));

    test_uint(v.type, derived);
    test_assert(v.ptr != NULL);
    test_assert(v.ptr != src.ptr);
    test_int(((int32_t*)v.ptr)[0], 10);
    test_int(((int32_t*)v.ptr)[1], 20);

    ecs_value_fini(world, &src);
    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_set_value_type_only(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t src = { .type = derived };
    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_value(&cur, &(ecs_value_t){t, &src}));

    test_uint(v.type, derived);
    test_assert(v.ptr == NULL);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_set_value_type_only_to_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t src = { .type = base };
    ecs_entity_t e = 0;
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &e);
    test_int(0, ecs_meta_set_value(&cur, &(ecs_value_t){t, &src}));
    test_uint(e, base);

    ecs_fini(world);
}

void ValueType_set_null(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = ecs_value_new(world, ecs_id(ecs_i32_t));
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_null(&cur));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_struct_w_value_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {
            { "a", ecs_id(ecs_i32_t) },
            { "v", t },
            { "b", ecs_id(ecs_i32_t) }
        }
    });

    typedef struct { int32_t a; ecs_value_t v; int32_t b; } S;
    S value = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, s, &value);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 1));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "Derived"));
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 2));
    test_int(0, ecs_meta_pop(&cur));

    test_int(value.a, 1);
    test_int(value.b, 2);
    test_uint(value.v.type, derived);
    test_assert(value.v.ptr != NULL);
    test_int(((int32_t*)value.v.ptr)[0], 10);
    test_int(((int32_t*)value.v.ptr)[1], 20);

    ecs_ptr_fini(world, s, &value);

    ecs_fini(world);
}

void ValueType_struct_w_value_member_unrelated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t other = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Other" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {{ "v", t }}
    });

    ecs_value_t value = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, s, &value);
    test_int(0, ecs_meta_push(&cur));

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_set_entity(&cur, other));

    test_uint(value.type, 0);
    test_assert(value.ptr == NULL);

    ecs_fini(world);
}

void ValueType_vector_of_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t other = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Other" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });
    ecs_entity_t vt = ecs_vector(world, { .type = t });
    test_assert(vt != 0);

    ecs_vec_t vec = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, vt, &vec);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_entity(&cur, base));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "Derived"));
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_next(&cur));

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_set_entity(&cur, other));
    ecs_log_set_level(-1);

    test_int(0, ecs_meta_pop(&cur));

    test_assert(ecs_vec_count(&vec) >= 2);
    ecs_value_t *elems = ecs_vec_first(&vec);
    test_uint(elems[0].type, base);
    test_assert(elems[0].ptr == NULL);
    test_uint(elems[1].type, derived);
    test_assert(elems[1].ptr != NULL);
    test_int(((int32_t*)elems[1].ptr)[0], 10);
    test_int(((int32_t*)elems[1].ptr)[1], 20);

    ecs_ptr_fini(world, vt, &vec);

    ecs_fini(world);
}

void ValueType_unconstrained_set_entity_type_only(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_value_t), &v);
    test_int(0, ecs_meta_set_entity(&cur, base));

    test_uint(v.type, base);
    test_assert(v.ptr == NULL);

    cur = ecs_meta_cursor(world, ecs_id(ecs_value_t), &v);
    test_int(0, ecs_meta_set_entity(&cur, ecs_id(ecs_i32_t)));

    test_uint(v.type, ecs_id(ecs_i32_t));
    test_assert(v.ptr == NULL);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_unconstrained_set_entity_not_a_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_value_t), &v);
    test_int(0, ecs_meta_set_entity(&cur, e));

    test_uint(v.type, ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, e);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_unconstrained_set_string_type_only(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_value_t), &v);
    test_int(0, ecs_meta_set_string(&cur, "Base"));

    test_uint(v.type, base);
    test_assert(v.ptr == NULL);

    cur = ecs_meta_cursor(world, ecs_id(ecs_value_t), &v);
    test_int(0, ecs_meta_set_string(&cur, "Hello"));

    test_uint(v.type, ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "Hello");

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_unconstrained_set_entity_zero(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = ecs_value_new(world, ecs_id(ecs_i32_t));
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_value_t), &v);
    test_int(0, ecs_meta_set_entity(&cur, 0));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_type_only_api(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = ecs_value_new(world, ecs_id(ecs_string_t));
    *(char**)v.ptr = ecs_os_strdup("Hello");

    ecs_value_set_type(world, &v, ecs_id(ecs_i32_t));
    test_uint(v.type, ecs_id(ecs_i32_t));
    test_assert(v.ptr == NULL);

    ecs_value_t c = {0};
    ecs_value_copy(world, &c, &v);
    test_uint(c.type, ecs_id(ecs_i32_t));
    test_assert(c.ptr == NULL);

    ecs_value_t m = {0};
    ecs_value_move(world, &m, &c);
    test_uint(m.type, ecs_id(ecs_i32_t));
    test_assert(m.ptr == NULL);
    test_uint(c.type, 0);

    ecs_value_set_type(world, &m, 0);
    test_uint(m.type, 0);
    test_assert(m.ptr == NULL);

    ecs_value_fini(world, &v);
    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_type_only_copy_overwrites_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = ecs_value_new(world, ecs_id(ecs_string_t));
    *(char**)v.ptr = ecs_os_strdup("Hello");

    ecs_value_t src = { .type = ecs_id(ecs_i32_t) };
    ecs_value_copy(world, &v, &src);
    test_uint(v.type, ecs_id(ecs_i32_t));
    test_assert(v.ptr == NULL);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_type_only_compare(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t a = { .type = ecs_id(ecs_i32_t) };
    ecs_value_t b = { .type = ecs_id(ecs_i32_t) };
    ecs_value_t c = ecs_value_new(world, ecs_id(ecs_i32_t));

    test_int(0, ecs_value_compare(world, &a, &b));
    test_bool(true, ecs_value_equals(world, &a, &b));
    test_assert(0 != ecs_value_compare(world, &a, &c));
    test_bool(false, ecs_value_equals(world, &a, &c));

    ecs_value_fini(world, &c);

    ecs_fini(world);
}

void ValueType_component_lifecycle(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "s", ecs_id(ecs_string_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "t", ecs_id(ecs_string_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, {
        .entity = ecs_entity(world, { .name = "T" }),
        .type = base
    });

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {{ "a", t }, { "b", t }}
    });

    typedef struct { ecs_value_t a; ecs_value_t b; } S;

    ecs_entity_t e = ecs_new(world);
    S *ptr = ecs_ensure_id(world, e, s, sizeof(S));
    test_assert(ptr != NULL);
    test_uint(ptr->a.type, 0);
    test_assert(ptr->a.ptr == NULL);

    const char *strs[] = {"Hello", "World"};
    ecs_value_set(world, &ptr->a, derived, (char*[]){
        ECS_CONST_CAST(char*, strs[0]), ECS_CONST_CAST(char*, strs[1])});
    ecs_value_set_type(world, &ptr->b, derived);
    ecs_modified_id(world, e, s);

    ecs_entity_t f = ecs_clone(world, 0, e, true);
    const S *cptr = ecs_get_id(world, f, s);
    test_assert(cptr != NULL);
    test_uint(cptr->a.type, derived);
    test_assert(cptr->a.ptr != NULL);
    test_assert(cptr->a.ptr != ptr->a.ptr);
    test_str(((char**)cptr->a.ptr)[0], "Hello");
    test_str(((char**)cptr->a.ptr)[1], "World");
    test_uint(cptr->b.type, derived);
    test_assert(cptr->b.ptr == NULL);

    ecs_add_id(world, e, ecs_new(world));
    ptr = ecs_get_mut_id(world, e, s);
    test_uint(ptr->a.type, derived);
    test_str(((char**)ptr->a.ptr)[0], "Hello");

    ecs_delete(world, e);
    ecs_delete(world, f);

    ecs_fini(world);
}

void ValueType_ser_json(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    int32_t data[] = {10, 20};
    ecs_value_t v = ecs_value_init(world, derived, data);

    char *json = ecs_ptr_to_json(world, t, &v);
    test_assert(json != NULL);
    test_str(json, "{\"Derived\":{\"x\":10, \"y\":20}}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_ser_json_type_only(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = { .type = base };

    char *json = ecs_ptr_to_json(world, t, &v);
    test_assert(json != NULL);
    test_str(json, "\"Base\"");
    ecs_os_free(json);

    ecs_fini(world);
}

void ValueType_ser_json_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = {0};

    char *json = ecs_ptr_to_json(world, t, &v);
    test_assert(json != NULL);
    test_str(json, "null");
    ecs_os_free(json);

    ecs_fini(world);
}

void ValueType_ser_json_struct_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {{ "a", t }, { "b", t }, { "c", t }}
    });

    int32_t x = 10;
    ecs_value_t value[3] = {
        { .type = base, .ptr = &x },
        { .type = base },
        {0}
    };

    char *json = ecs_ptr_to_json(world, s, value);
    test_assert(json != NULL);
    test_str(json, "{\"a\":{\"Base\":{\"x\":10}}, \"b\":\"Base\", \"c\":null}");
    ecs_os_free(json);

    ecs_fini(world);
}

void ValueType_deser_json(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    const char *r = ecs_ptr_from_json(world, t, &v,
        "{\"Derived\":{\"x\":10, \"y\":20}}", NULL);
    test_assert(r != NULL);
    test_str(r, "");

    test_uint(v.type, derived);
    test_assert(v.ptr != NULL);
    test_int(((int32_t*)v.ptr)[0], 10);
    test_int(((int32_t*)v.ptr)[1], 20);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_deser_json_type_only(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    const char *r = ecs_ptr_from_json(world, t, &v, "\"Derived\"", NULL);
    test_assert(r != NULL);

    test_uint(v.type, derived);
    test_assert(v.ptr == NULL);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_deser_json_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = ecs_value_new(world, ecs_id(ecs_i32_t));
    const char *r = ecs_ptr_from_json(world, t, &v, "null", NULL);
    test_assert(r != NULL);

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_deser_json_unrelated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Other" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_log_set_level(-4);
    const char *r = ecs_ptr_from_json(world, t, &v, "\"Other\"", NULL);
    test_assert(r == NULL);

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_deser_json_primitive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = {0};
    const char *r = ecs_ptr_from_json(world, t, &v, "10", NULL);
    test_assert(r != NULL);

    test_uint(v.type, ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_int(*(int32_t*)v.ptr, 10);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void ValueType_json_roundtrip(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {{ "a", t }, { "b", t }, { "c", t }, { "d", ecs_id(ecs_value_t) }}
    });

    int32_t data[] = {10, 20};
    ecs_value_t value[4] = {
        { .type = derived, .ptr = data },
        { .type = derived },
        {0},
        { .type = base }
    };

    char *json = ecs_ptr_to_json(world, s, value);
    test_assert(json != NULL);
    test_str(json, "{\"a\":{\"Derived\":{\"x\":10, \"y\":20}}, \"b\":\"Derived\", "
        "\"c\":null, \"d\":\"Base\"}");

    ecs_value_t out[4] = {0};
    const char *r = ecs_ptr_from_json(world, s, out, json, NULL);
    test_assert(r != NULL);
    ecs_os_free(json);

    test_uint(out[0].type, derived);
    test_assert(out[0].ptr != NULL);
    test_int(((int32_t*)out[0].ptr)[0], 10);
    test_int(((int32_t*)out[0].ptr)[1], 20);
    test_uint(out[1].type, derived);
    test_assert(out[1].ptr == NULL);
    test_uint(out[2].type, 0);
    test_assert(out[2].ptr == NULL);
    test_uint(out[3].type, base);
    test_assert(out[3].ptr == NULL);

    ecs_ptr_fini(world, s, out);

    ecs_fini(world);
}

void ValueType_type_info_json(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {{ "a", t }, { "b", ecs_id(ecs_value_t) }}
    });

    char *json = ecs_type_info_to_json(world, s);
    test_assert(json != NULL);
    test_str(json, "{\"a\":[\"value\", \"Base\"], \"b\":[\"value\"]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void ValueType_ptr_to_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {{ "a", t }, { "b", t }, { "c", t }}
    });

    int32_t data[] = {10, 20};
    ecs_value_t value[3] = {
        { .type = derived, .ptr = data },
        { .type = derived },
        {0}
    };

    char *expr = ecs_ptr_to_expr(world, s, value);
    test_assert(expr != NULL);
    test_str(expr, "{a: {Derived: {x: 10, y: 20}}, b: Derived, c: #0}");
    ecs_os_free(expr);

    ecs_fini(world);
}

static ecs_entity_t ValueType_lookup_custom(
    ecs_world_t *world,
    const char *value,
    void *ctx)
{
    (void)world;
    if (!ecs_os_strcmp(value, "Custom")) {
        return *(ecs_entity_t*)ctx;
    }
    return 0;
}

void ValueType_init_w_dead_constraint(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t dead = ecs_new(world);
    ecs_delete(world, dead);

    ecs_log_set_level(-4);
    ecs_entity_t t = ecs_value_type(world, { .type = dead });
    test_assert(t == 0);

    ecs_fini(world);
}

void ValueType_struct_set_value_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t derived = ecs_entity(world, { .name = "Derived" });
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_struct(world, {
        .entity = derived,
        .members = {{ "y", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    const char *name = "Derived";
    test_int(0, ecs_meta_set_value(&cur,
        &(ecs_value_t){ecs_id(ecs_string_t), &name}));
    test_uint(v.type, derived);
    test_assert(v.ptr == NULL);

    const char *null_name = NULL;
    cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_value(&cur,
        &(ecs_value_t){ecs_id(ecs_string_t), &null_name}));
    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_set_value_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t v = ecs_value_new(world, ecs_id(ecs_i32_t));
    ecs_value_t src = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_value(&cur, &(ecs_value_t){t, &src}));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_set_value_empty_to_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_value_type(world, { .type = ecs_id(ecs_i32_t) });

    ecs_value_t src = {0};
    ecs_entity_t e = 0;
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &e);

    ecs_log_set_level(-4);
    test_assert(0 != ecs_meta_set_value(&cur, &(ecs_value_t){t, &src}));
    test_uint(e, 0);

    ecs_fini(world);
}

void ValueType_struct_set_string_zero(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Base" }),
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = { .type = base };
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    test_int(0, ecs_meta_set_string(&cur, "#0"));

    test_uint(v.type, 0);
    test_assert(v.ptr == NULL);

    ecs_fini(world);
}

void ValueType_struct_set_string_lookup_action(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .members = {{ "x", ecs_id(ecs_i32_t) }}
    });

    ecs_entity_t t = ecs_value_type(world, { .type = base });

    ecs_value_t v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &v);
    cur.lookup_action = ValueType_lookup_custom;
    cur.lookup_ctx = &base;
    test_int(0, ecs_meta_set_string(&cur, "Custom"));

    test_uint(v.type, base);
    test_assert(v.ptr == NULL);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}
