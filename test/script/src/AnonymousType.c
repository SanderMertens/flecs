#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void AnonymousType_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

void AnonymousType_const_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Values(v: vector(f32))"
    LINE "const x: vector(f32) = [1, 2, 3]"
    LINE "e { Values: {x} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t values = ecs_lookup(world, "Values");
    test_assert(e != 0);
    test_assert(values != 0);
    const ecs_vec_t *v = ecs_get_id(world, e, values);
    test_assert(v != NULL);
    test_int(ecs_vec_count(v), 3);
    test_flt(ecs_vec_get_t(v, float, 0)[0], 1);
    test_flt(ecs_vec_get_t(v, float, 1)[0], 2);
    test_flt(ecs_vec_get_t(v, float, 2)[0], 3);

    ecs_fini(world);
}

void AnonymousType_const_array(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "const x: array(f32, 2) = [10, 20]"
    LINE "e { Position: {x[0], x[1]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(e != 0);
    test_assert(position != 0);
    const float *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p[0], 10);
    test_flt(p[1], 20);

    ecs_fini(world);
}

void AnonymousType_const_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Label(value: string)"
    LINE "const x: map(i32, string) = [10: \"foo\", 20: \"bar\"]"
    LINE "e { Label: {x[20]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t label = ecs_lookup(world, "Label");
    test_assert(e != 0);
    test_assert(label != 0);
    char* const *l = ecs_get_id(world, e, label);
    test_assert(l != NULL);
    test_str(*l, "bar");

    ecs_fini(world);
}

void AnonymousType_const_map_named_args(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Label(value: string)"
    LINE "const x: map(key_type: i32, type: string) = [10: \"foo\"]"
    LINE "e { Label: {x[10]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t label = ecs_lookup(world, "Label");
    test_assert(e != 0);
    test_assert(label != 0);
    char* const *l = ecs_get_id(world, e, label);
    test_assert(l != NULL);
    test_str(*l, "foo");

    ecs_fini(world);
}

void AnonymousType_const_value_empty_args(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "const x: value() = 10"
    LINE "e { Position: {x, x} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(e != 0);
    test_assert(position != 0);
    const float *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p[0], 10);
    test_flt(p[1], 10);

    ecs_fini(world);
}

void AnonymousType_const_vector_of_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template Brick : Facade {"
    LINE "  Position: {height, 1}"
    LINE "}"
    LINE "const fs: vector(value(Facade)) = [Brick(height: 3)]"
    LINE "e { fs[0] }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(e != 0);
    test_assert(position != 0);
    test_assert(ecs_has_id(world, e, ecs_lookup(world, "Brick")));
    const float *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p[0], 3);
    test_flt(p[1], 1);

    ecs_fini(world);
}

void AnonymousType_same_as_shorthand(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: vector(f32), c: array(f32, 3))"
    LINE "const b: f32[] = []"
    LINE "const d: f32[3] = [1, 2, 3]"
    LINE "const e: vector(f32) = b"
    LINE "const f: array(f32, 3) = d"
    LINE "export const types: entity[] = [vector(f32), array(f32, 3)]";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "A.a");
    ecs_entity_t c = ecs_lookup(world, "A.c");
    test_assert(a != 0);
    test_assert(c != 0);

    const EcsMember *ma = ecs_get(world, a, EcsMember);
    const EcsMember *mc = ecs_get(world, c, EcsMember);
    test_assert(ma != NULL);
    test_assert(mc != NULL);

    const EcsVector *va = ecs_get(world, ma->type, EcsVector);
    test_assert(va != NULL);
    test_uint(va->type, ecs_id(ecs_f32_t));

    const EcsArray *ac = ecs_get(world, mc->type, EcsArray);
    test_assert(ac != NULL);
    test_uint(ac->type, ecs_id(ecs_f32_t));
    test_int(ac->count, 3);

    ecs_entity_t types = ecs_lookup(world, "types");
    test_assert(types != 0);
    ecs_value_t tv = ecs_const_var_get(world, types);
    test_assert(tv.ptr != NULL);
    const ecs_vec_t *vec = tv.ptr;
    test_int(ecs_vec_count(vec), 2);
    test_uint(ecs_vec_get_t(vec, ecs_entity_t, 0)[0], ma->type);
    test_uint(ecs_vec_get_t(vec, ecs_entity_t, 1)[0], mc->type);

    ecs_fini(world);
}

void AnonymousType_struct_member_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Lookup(values: map(i32, string))"
    LINE "e { Lookup: {values: [1: \"foo\", 2: \"bar\"]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t m = ecs_lookup(world, "Lookup.values");
    test_assert(m != 0);
    const EcsMember *member = ecs_get(world, m, EcsMember);
    test_assert(member != NULL);
    const EcsMap *map_type = ecs_get(world, member->type, EcsMap);
    test_assert(map_type != NULL);
    test_uint(map_type->key_type, ecs_id(ecs_i32_t));
    test_uint(map_type->type, ecs_id(ecs_string_t));

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const ecs_map_t *map = ecs_get_id(world, e, ecs_lookup(world, "Lookup"));
    test_assert(map != NULL);
    test_int(ecs_map_count(map), 2);
    const ecs_map_val_t *v = ecs_map_get(map, 2);
    test_assert(v != NULL);
    test_str(*(char**)v, "bar");

    ecs_fini(world);
}

void AnonymousType_struct_member_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template Brick : Facade {"
    LINE "  Position: {height, 1}"
    LINE "}"
    LINE "struct Theme(facade: value(Facade))"
    LINE "theme { Theme: {facade: Brick(height: 3)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t m = ecs_lookup(world, "Theme.facade");
    test_assert(m != 0);
    const EcsMember *member = ecs_get(world, m, EcsMember);
    test_assert(member != NULL);
    const EcsValue *vt = ecs_get(world, member->type, EcsValue);
    test_assert(vt != NULL);
    test_uint(vt->type, ecs_lookup(world, "Facade"));

    ecs_entity_t theme = ecs_lookup(world, "theme");
    test_assert(theme != 0);
    const ecs_value_t *v = ecs_get_id(world, theme, ecs_lookup(world, "Theme"));
    test_assert(v != NULL);
    test_uint(v->type, ecs_lookup(world, "Brick"));
    test_assert(v->ptr != NULL);
    test_flt(*(float*)v->ptr, 3);

    ecs_fini(world);
}

void AnonymousType_struct_member_value_unrelated(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Facade(height: f32)"
    LINE "struct Roof(pitch: f32)"
    LINE "template Flat : Roof {}"
    LINE "struct Theme(facade: value(Facade))"
    LINE "theme { Theme: {facade: Flat} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AnonymousType_struct_member_nested(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Facade(height: f32)"
    LINE "struct Theme(facades: map(i32, vector(value(Facade))))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t m = ecs_lookup(world, "Theme.facades");
    test_assert(m != 0);
    const EcsMember *member = ecs_get(world, m, EcsMember);
    test_assert(member != NULL);

    const EcsMap *map_type = ecs_get(world, member->type, EcsMap);
    test_assert(map_type != NULL);
    test_uint(map_type->key_type, ecs_id(ecs_i32_t));

    const EcsVector *vec_type = ecs_get(world, map_type->type, EcsVector);
    test_assert(vec_type != NULL);

    const EcsValue *value_type = ecs_get(world, vec_type->type, EcsValue);
    test_assert(value_type != NULL);
    test_uint(value_type->type, ecs_lookup(world, "Facade"));

    ecs_fini(world);
}

void AnonymousType_struct_member_w_count(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Facade(height: f32)"
    LINE "struct Theme(facades: {value(Facade), count: 3})";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t m = ecs_lookup(world, "Theme.facades");
    test_assert(m != 0);
    const EcsMember *member = ecs_get(world, m, EcsMember);
    test_assert(member != NULL);
    test_int(member->count, 3);
    const EcsValue *vt = ecs_get(world, member->type, EcsValue);
    test_assert(vt != NULL);
    test_uint(vt->type, ecs_lookup(world, "Facade"));

    const EcsComponent *c = ecs_get(world, ecs_lookup(world, "Theme"), EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 3 * ECS_SIZEOF(ecs_value_t));

    ecs_fini(world);
}

void AnonymousType_template_prop_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Label(value: string)"
    LINE "template T {"
    LINE "  prop names: map(i32, string) = [1: \"foo\"]"
    LINE "  child { Label: {names[1]} }"
    LINE "}"
    LINE "T t()"
    LINE "T u(names: [1: \"bar\"])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t label = ecs_lookup(world, "Label");
    test_assert(label != 0);

    ecs_entity_t t = ecs_lookup(world, "t.child");
    test_assert(t != 0);
    char* const *tl = ecs_get_id(world, t, label);
    test_assert(tl != NULL);
    test_str(*tl, "foo");

    ecs_entity_t u = ecs_lookup(world, "u.child");
    test_assert(u != 0);
    char* const *ul = ecs_get_id(world, u, label);
    test_assert(ul != NULL);
    test_str(*ul, "bar");

    ecs_fini(world);
}

void AnonymousType_type_as_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Holder(type: entity)"
    LINE "e { Holder: {map(i32, string)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const ecs_entity_t *h = ecs_get_id(world, e, ecs_lookup(world, "Holder"));
    test_assert(h != NULL);
    const EcsMap *map_type = ecs_get(world, *h, EcsMap);
    test_assert(map_type != NULL);
    test_uint(map_type->key_type, ecs_id(ecs_i32_t));
    test_uint(map_type->type, ecs_id(ecs_string_t));

    ecs_fini(world);
}

void AnonymousType_same_args_same_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: map(i32, string), b: map(i32, string), c: map(i32, f32))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    const EcsMember *a = ecs_get(world, ecs_lookup(world, "A.a"), EcsMember);
    const EcsMember *b = ecs_get(world, ecs_lookup(world, "A.b"), EcsMember);
    const EcsMember *c = ecs_get(world, ecs_lookup(world, "A.c"), EcsMember);
    test_assert(a != NULL);
    test_assert(b != NULL);
    test_assert(c != NULL);
    test_uint(a->type, b->type);
    test_assert(a->type != c->type);

    ecs_fini(world);
}

void AnonymousType_invalid_vector_args(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const x: vector() = []";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AnonymousType_invalid_array_count(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: array(f32, 0))";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AnonymousType_invalid_map_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: map(f32, i32))";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AnonymousType_invalid_value_of_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: value(value))";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AnonymousType_args_on_non_kind(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "const x: Position(f32) = {}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AnonymousType_value_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "e {"
    LINE "  value: {u32: 10}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const ecs_value_t *v = ecs_get_id(world, e, ecs_id(ecs_value_t));
    test_assert(v != NULL);
    test_uint(v->type, ecs_id(ecs_u32_t));
    test_assert(v->ptr != NULL);
    test_uint(*(uint32_t*)v->ptr, 10);

    ecs_fini(world);
}

void AnonymousType_serialize_type(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        HEAD "struct Facade(height: f32)"
        LINE "struct Theme(facades: map(i32, vector(value(Facade))), v: array(f32, 3))",
        NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Theme"));
    test_str(str,
        HEAD "struct Facade(height: f32)"
        LINE "struct Theme(facades: map(i32, vector(value(Facade))), v: array(f32, 3))"
        LINE);
    ecs_os_free(str);

    ecs_fini(world);
}

void AnonymousType_serialize_type_roundtrip(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        HEAD "struct Facade(height: f32)"
        LINE "value FacadeRef(type: Facade)"
        LINE "struct Theme(a: FacadeRef, b: map(i32, FacadeRef))",
        NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Theme"));
    test_assert(str != NULL);

    ecs_world_t *world2 = ecs_init();
    test_int(ecs_script_run(world2, NULL, str, NULL), 0);

    char *str2 = ecs_type_to_script(world2, ecs_lookup(world2, "Theme"));
    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);

    ecs_fini(world2);
    ecs_fini(world);
}
