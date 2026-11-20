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

void AnonymousType_to_str_type_args(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "const a: vector(f32) = [1, 2]"
        LINE "const b: map(i32, string) = [1: \"a\"]"
        LINE "const c: vector() = []",
        NULL, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_str(str,
    HEAD "{"
    LINE "  const: a : vector(@f32) = {1, 2}"
    LINE "  const: b : map(@i32, @string) = {1:a}"
    LINE "  const: c : vector() = {}"
    LINE "}"
    LINE "");
    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void AnonymousType_invalid_type_arg(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const t = vector($undefined)", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AnonymousType_struct_member_collection_shorthand(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: f32[], c: f32[3])";

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

    if (mc->count) {
        test_uint(mc->type, ecs_id(ecs_f32_t));
        test_int(mc->count, 3);
    } else {
        const EcsArray *ac = ecs_get(world, mc->type, EcsArray);
        test_assert(ac != NULL);
        test_uint(ac->type, ecs_id(ecs_f32_t));
        test_int(ac->count, 3);
    }

    ecs_fini(world);
}

void AnonymousType_struct_member_nested_collection_shorthand(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: f32[][], b: f32[3][], c: f32[][2])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "A.a");
    ecs_entity_t b = ecs_lookup(world, "A.b");
    ecs_entity_t c = ecs_lookup(world, "A.c");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(c != 0);

    const EcsMember *ma = ecs_get(world, a, EcsMember);
    test_assert(ma != NULL);
    const EcsVector *va = ecs_get(world, ma->type, EcsVector);
    test_assert(va != NULL);
    const EcsVector *va_elem = ecs_get(world, va->type, EcsVector);
    test_assert(va_elem != NULL);
    test_uint(va_elem->type, ecs_id(ecs_f32_t));

    const EcsMember *mb = ecs_get(world, b, EcsMember);
    test_assert(mb != NULL);
    const EcsVector *vb = ecs_get(world, mb->type, EcsVector);
    test_assert(vb != NULL);
    const EcsArray *ab_elem = ecs_get(world, vb->type, EcsArray);
    test_assert(ab_elem != NULL);
    test_uint(ab_elem->type, ecs_id(ecs_f32_t));
    test_int(ab_elem->count, 3);

    const EcsMember *mc = ecs_get(world, c, EcsMember);
    test_assert(mc != NULL);
    const EcsArray *ac = ecs_get(world, mc->type, EcsArray);
    test_assert(ac != NULL);
    test_int(ac->count, 2);
    const EcsVector *vc_elem = ecs_get(world, ac->type, EcsVector);
    test_assert(vc_elem != NULL);
    test_uint(vc_elem->type, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void AnonymousType_struct_member_shorthand_w_member_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: {type: f32[]})";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "A.a");
    test_assert(a != 0);

    const EcsMember *ma = ecs_get(world, a, EcsMember);
    test_assert(ma != NULL);
    const EcsVector *va = ecs_get(world, ma->type, EcsVector);
    test_assert(va != NULL);
    test_uint(va->type, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void AnonymousType_struct_member_shorthand_w_local_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct P(x: f32)"
    LINE "struct A(a: P[], b: P[2])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t p = ecs_lookup(world, "P");
    ecs_entity_t a = ecs_lookup(world, "A.a");
    ecs_entity_t b = ecs_lookup(world, "A.b");
    test_assert(p != 0);
    test_assert(a != 0);
    test_assert(b != 0);

    const EcsMember *ma = ecs_get(world, a, EcsMember);
    test_assert(ma != NULL);
    const EcsVector *va = ecs_get(world, ma->type, EcsVector);
    test_assert(va != NULL);
    test_uint(va->type, p);

    const EcsMember *mb = ecs_get(world, b, EcsMember);
    test_assert(mb != NULL);
    const EcsArray *ab = ecs_get(world, mb->type, EcsArray);
    test_assert(ab != NULL);
    test_uint(ab->type, p);
    test_int(ab->count, 2);

    ecs_fini(world);
}

void AnonymousType_struct_member_shorthand_w_const_count(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const N = 2"
    LINE "struct A(a: f32[N], b: f32[N + 1])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "A.a");
    ecs_entity_t b = ecs_lookup(world, "A.b");
    test_assert(a != 0);
    test_assert(b != 0);

    const EcsMember *ma = ecs_get(world, a, EcsMember);
    test_assert(ma != NULL);
    const EcsArray *aa = ecs_get(world, ma->type, EcsArray);
    test_assert(aa != NULL);
    test_uint(aa->type, ecs_id(ecs_f32_t));
    test_int(aa->count, 2);

    const EcsMember *mb = ecs_get(world, b, EcsMember);
    test_assert(mb != NULL);
    const EcsArray *ab = ecs_get(world, mb->type, EcsArray);
    test_assert(ab != NULL);
    test_uint(ab->type, ecs_id(ecs_f32_t));
    test_int(ab->count, 3);

    ecs_fini(world);
}

void AnonymousType_type_arg_collection_shorthand(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: vector(f32[]), b: array(f32[], 2), c: map(i32, f32[3]))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "A.a");
    ecs_entity_t b = ecs_lookup(world, "A.b");
    ecs_entity_t c = ecs_lookup(world, "A.c");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(c != 0);

    const EcsMember *ma = ecs_get(world, a, EcsMember);
    test_assert(ma != NULL);
    const EcsVector *va = ecs_get(world, ma->type, EcsVector);
    test_assert(va != NULL);
    const EcsVector *va_elem = ecs_get(world, va->type, EcsVector);
    test_assert(va_elem != NULL);
    test_uint(va_elem->type, ecs_id(ecs_f32_t));

    const EcsMember *mb = ecs_get(world, b, EcsMember);
    test_assert(mb != NULL);
    const EcsArray *ab = ecs_get(world, mb->type, EcsArray);
    test_assert(ab != NULL);
    test_int(ab->count, 2);
    const EcsVector *vb_elem = ecs_get(world, ab->type, EcsVector);
    test_assert(vb_elem != NULL);
    test_uint(vb_elem->type, ecs_id(ecs_f32_t));

    const EcsMember *mc = ecs_get(world, c, EcsMember);
    test_assert(mc != NULL);
    const EcsMap *mapc = ecs_get(world, mc->type, EcsMap);
    test_assert(mapc != NULL);
    test_uint(mapc->key_type, ecs_id(ecs_i32_t));
    const EcsArray *ac_elem = ecs_get(world, mapc->type, EcsArray);
    test_assert(ac_elem != NULL);
    test_uint(ac_elem->type, ecs_id(ecs_f32_t));
    test_int(ac_elem->count, 3);

    ecs_fini(world);
}

void AnonymousType_named_type_collection_shorthand(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "vector V(type: f32[])"
    LINE "array Arr(type: f32[], count: 2)"
    LINE "map M(key_type: i32, type: f32[3])"
    LINE "value Val(type: f32[])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t v = ecs_lookup(world, "V");
    ecs_entity_t arr = ecs_lookup(world, "Arr");
    ecs_entity_t m = ecs_lookup(world, "M");
    ecs_entity_t val = ecs_lookup(world, "Val");
    test_assert(v != 0);
    test_assert(arr != 0);
    test_assert(m != 0);
    test_assert(val != 0);

    const EcsVector *vv = ecs_get(world, v, EcsVector);
    test_assert(vv != NULL);
    const EcsVector *vv_elem = ecs_get(world, vv->type, EcsVector);
    test_assert(vv_elem != NULL);
    test_uint(vv_elem->type, ecs_id(ecs_f32_t));

    const EcsArray *aa = ecs_get(world, arr, EcsArray);
    test_assert(aa != NULL);
    test_int(aa->count, 2);
    const EcsVector *aa_elem = ecs_get(world, aa->type, EcsVector);
    test_assert(aa_elem != NULL);
    test_uint(aa_elem->type, ecs_id(ecs_f32_t));

    const EcsMap *mm = ecs_get(world, m, EcsMap);
    test_assert(mm != NULL);
    test_uint(mm->key_type, ecs_id(ecs_i32_t));
    const EcsArray *mm_elem = ecs_get(world, mm->type, EcsArray);
    test_assert(mm_elem != NULL);
    test_uint(mm_elem->type, ecs_id(ecs_f32_t));
    test_int(mm_elem->count, 3);

    const EcsValue *vt = ecs_get(world, val, EcsValue);
    test_assert(vt != NULL);
    const EcsVector *vt_elem = ecs_get(world, vt->type, EcsVector);
    test_assert(vt_elem != NULL);
    test_uint(vt_elem->type, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void AnonymousType_const_w_type_arg_collection_shorthand(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: f32)"
    LINE "const x: vector(f32[]) = [[1, 2], [3]]"
    LINE "e { Result: {x[0][1] + x[1][0]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const float *r = ecs_get_id(world, e, result);
    test_assert(r != NULL);
    test_flt(r[0], 5);

    ecs_fini(world);
}

void AnonymousType_entity_value_collection_shorthand(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "export const types: entity[] = [f32[], f32[3]]";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t types = ecs_lookup(world, "types");
    test_assert(types != 0);
    ecs_value_t tv = ecs_const_var_get(world, types);
    test_assert(tv.ptr != NULL);
    const ecs_vec_t *vec = tv.ptr;
    test_int(ecs_vec_count(vec), 2);

    const EcsVector *v = ecs_get(world,
        ecs_vec_get_t(vec, ecs_entity_t, 0)[0], EcsVector);
    test_assert(v != NULL);
    test_uint(v->type, ecs_id(ecs_f32_t));

    const EcsArray *a = ecs_get(world,
        ecs_vec_get_t(vec, ecs_entity_t, 1)[0], EcsArray);
    test_assert(a != NULL);
    test_uint(a->type, ecs_id(ecs_f32_t));
    test_int(a->count, 3);

    ecs_fini(world);
}

void AnonymousType_invalid_collection_shorthand_count(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "struct A(a: f32[0])", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AnonymousType_collection_shorthand_on_non_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "struct A(a: flecs.core.ChildOf[])", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AnonymousType_const_map_string_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Label(value: string)"
    LINE "const x: map(string, string) = [\"foo\": \"hello\", \"bar\": \"world\"]"
    LINE "e { Label: {x[\"bar\"]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t label = ecs_lookup(world, "Label");
    test_assert(e != 0);
    test_assert(label != 0);
    char* const *l = ecs_get_id(world, e, label);
    test_assert(l != NULL);
    test_str(*l, "world");

    ecs_fini(world);
}

void AnonymousType_struct_member_map_string_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Inventory(items: map(string, i32))"
    LINE "e { Inventory: {items: [\"apple\": 3, \"pear\": 5]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t inventory = ecs_lookup(world, "Inventory");
    test_assert(e != 0);
    test_assert(inventory != 0);

    const EcsStruct *st = ecs_get(world, inventory, EcsStruct);
    test_assert(st != NULL);
    ecs_entity_t map_type = ecs_vec_first_t(&st->members, ecs_member_t)->type;
    const EcsMap *m = ecs_get(world, map_type, EcsMap);
    test_assert(m != NULL);
    test_uint(m->key_type, ecs_id(ecs_string_t));
    test_uint(m->type, ecs_id(ecs_i32_t));

    const ecs_map_t *ptr = ecs_get_id(world, e, inventory);
    test_assert(ptr != NULL);
    test_int(ecs_map_count(ptr), 2);
    test_assert(script_map_has(world, map_type, ptr, "\"apple\": 3"));
    test_assert(script_map_has(world, map_type, ptr, "\"pear\": 5"));

    ecs_fini(world);
}
