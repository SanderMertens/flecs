#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void ValueType_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

typedef struct {
    float x;
    float y;
} PositionValue;

typedef struct {
    ecs_value_t facade;
    int32_t n;
} BuildingValue;

#define FACADE_HEAD \
    HEAD "struct Position(x: f32, y: f32)" \
    LINE "struct Facade(height: f32)" \
    LINE "template Brick : Facade {" \
    LINE "  Position: {height, 1}" \
    LINE "}" \
    LINE "template Glass : Facade {" \
    LINE "  Position: {height, 2}" \
    LINE "}"

void ValueType_const_primitive(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "const x: value(i32) = 10"
    LINE "const y: value(f32) = 20"
    LINE "e { Position: {x, y} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    {
        ecs_entity_t pe = ecs_lookup(world, "e");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 10);
        test_flt(p->y, 20);
    }

    ecs_fini(world);
}

void ValueType_const_enum(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "enum Color {"
    LINE "  constant Red"
    LINE "  constant Green"
    LINE "  constant Blue"
    LINE "}"
    LINE "const c: value(Color) = Blue"
    LINE "e { c }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t color = ecs_lookup(world, "Color");
    test_assert(e != 0);
    test_assert(color != 0);
    const int32_t *c = ecs_get_id(world, e, color);
    test_assert(c != NULL);
    test_int(*c, 2);

    ecs_fini(world);
}

void ValueType_const_string(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Label(value: string)"
    LINE "const s: value(string) = \"hello\""
    LINE "e { Label: {s} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t label = ecs_lookup(world, "Label");
    test_assert(e != 0);
    test_assert(label != 0);
    char* const *l = ecs_get_id(world, e, label);
    test_assert(l != NULL);
    test_str(*l, "hello");

    ecs_fini(world);
}

void ValueType_const_struct_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "const f: value(Facade) = Brick(height: 3)"
    LINE "e { f }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_lookup(world, "Brick")));
    {
        ecs_entity_t pe = ecs_lookup(world, "e");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 3);
        test_flt(p->y, 1);
    }

    ecs_fini(world);
}

void ValueType_const_struct_type_only(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "const f: value(Facade) = Glass"
    LINE "e { f }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_lookup(world, "Glass")));
    {
        ecs_entity_t pe = ecs_lookup(world, "e");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 0);
        test_flt(p->y, 2);
    }

    ecs_fini(world);
}

void ValueType_const_struct_unrelated(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Other {}"
    LINE "const f: value(Facade) = Other";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_const_unresolved_param(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const f: value(DoesNotExist) = 10";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_const_param_on_non_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "const f: i32(Position) = 10";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_const_param_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const f: value(value) = 10";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_const_as_component_w_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "const f: value(Facade) = Brick(height: 3)"
    LINE "e { f: {height: 5} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_lookup(world, "Brick")));
    {
        ecs_entity_t pe = ecs_lookup(world, "e");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 5);
        test_flt(p->y, 1);
    }

    ecs_fini(world);
}

void ValueType_const_member_access(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "const f: value(Facade) = Brick(height: 3)"
    LINE "e { Position: {f.height, f.height + 1} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    {
        ecs_entity_t pe = ecs_lookup(world, "e");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 3);
        test_flt(p->y, 4);
    }

    ecs_fini(world);
}

void ValueType_const_vector_index(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "const fs: value(Facade)[] = [Brick, Glass(height: 4)]"
    LINE "a { fs[0] }"
    LINE "b { fs[1] }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    {
        ecs_entity_t pe = ecs_lookup(world, "a");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 0);
        test_flt(p->y, 1);
    }
    {
        ecs_entity_t pe = ecs_lookup(world, "b");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 4);
        test_flt(p->y, 2);
    }

    ecs_fini(world);
}

void ValueType_const_vector_for(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "const fs: value(Facade)[] = [Brick, Glass]"
    LINE "for f in fs {"
    LINE "  \"e_{f}\" { f: {height: 9} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    {
        ecs_entity_t pe = ecs_lookup(world, "e_Brick");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 9);
        test_flt(p->y, 1);
    }
    {
        ecs_entity_t pe = ecs_lookup(world, "e_Glass");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 9);
        test_flt(p->y, 2);
    }

    ecs_fini(world);
}

void ValueType_const_vector_unrelated(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Other {}"
    LINE "const fs: value(Facade)[] = [Brick, Other]";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_value_type_decl(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "value FacadeRef(type: Facade)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t t = ecs_lookup(world, "FacadeRef");
    test_assert(t != 0);
    const EcsValue *v = ecs_get(world, t, EcsValue);
    test_assert(v != NULL);
    test_uint(v->type, ecs_lookup(world, "Facade"));

    const EcsType *mt = ecs_get(world, t, EcsType);
    test_assert(mt != NULL);
    test_int(mt->kind, EcsValueType);

    ecs_fini(world);
}

void ValueType_value_type_decl_unconstrained(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "value Any()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t t = ecs_lookup(world, "Any");
    test_assert(t != 0);
    const EcsValue *v = ecs_get(world, t, EcsValue);
    test_assert(v != NULL);
    test_uint(v->type, 0);

    const EcsType *mt = ecs_get(world, t, EcsType);
    test_assert(mt != NULL);
    test_int(mt->kind, EcsValueType);

    ecs_fini(world);
}

void ValueType_value_type_decl_self(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "value Self(type: Self)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_struct_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "value FacadeRef(type: Facade)"
    LINE "struct Building(facade: FacadeRef, n: i32)"
    LINE "e { Building: {facade: Glass(height: 3), n: 2} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t building = ecs_lookup(world, "Building");
    ecs_entity_t glass = ecs_lookup(world, "Glass");
    test_assert(e != 0);
    test_assert(building != 0);
    test_assert(glass != 0);

    const BuildingValue *b = ecs_get_id(world, e, building);
    test_assert(b != NULL);
    test_int(b->n, 2);
    test_uint(b->facade.type, glass);
    test_assert(b->facade.ptr != NULL);
    test_flt(*(float*)b->facade.ptr, 3);

    test_assert(!ecs_has_id(world, e, glass));

    ecs_fini(world);
}

void ValueType_struct_member_type_only(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "value FacadeRef(type: Facade)"
    LINE "struct Building(facade: FacadeRef, n: i32)"
    LINE "e { Building: {facade: Glass, n: 2} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t building = ecs_lookup(world, "Building");
    test_assert(e != 0);
    test_assert(building != 0);

    const BuildingValue *b = ecs_get_id(world, e, building);
    test_assert(b != NULL);
    test_int(b->n, 2);
    test_uint(b->facade.type, ecs_lookup(world, "Glass"));
    test_assert(b->facade.ptr == NULL);

    ecs_fini(world);
}

void ValueType_struct_member_unrelated(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Other {}"
    LINE "value FacadeRef(type: Facade)"
    LINE "struct Building(facade: FacadeRef, n: i32)"
    LINE "e { Building: {facade: Other, n: 2} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_template_prop_named_value_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "value FacadeRef(type: Facade)"
    LINE "template Tower {"
    LINE "  prop facade: FacadeRef = Glass"
    LINE "  floor { facade: {height: 7} }"
    LINE "}"
    LINE "Tower t()"
    LINE "Tower u(facade: Brick(height: 1))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    {
        ecs_entity_t pe = ecs_lookup(world, "t.floor");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 7);
        test_flt(p->y, 2);
    }
    {
        ecs_entity_t pe = ecs_lookup(world, "u.floor");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 7);
        test_flt(p->y, 1);
    }

    ecs_fini(world);
}

void ValueType_template_prop_primitive(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template T {"
    LINE "  prop n: value(i32) = 5"
    LINE "  child { n }"
    LINE "}"
    LINE "T t()"
    LINE "T u(n: 7)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t t = ecs_lookup(world, "t.child");
    test_assert(t != 0);
    const int32_t *tn = ecs_get_id(world, t, ecs_id(ecs_i32_t));
    test_assert(tn != NULL);
    test_int(*tn, 5);

    ecs_entity_t u = ecs_lookup(world, "u.child");
    test_assert(u != 0);
    const int32_t *un = ecs_get_id(world, u, ecs_id(ecs_i32_t));
    test_assert(un != NULL);
    test_int(*un, 7);

    ecs_fini(world);
}

void ValueType_template_prop_primitive_mismatch(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template T {"
    LINE "  prop n: value(i32) = 5"
    LINE "}"
    LINE "T t(n: \"hello\")";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_template_prop_enum(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "enum Color {"
    LINE "  constant Red"
    LINE "  constant Green"
    LINE "  constant Blue"
    LINE "}"
    LINE "template T {"
    LINE "  prop c: value(Color) = Green"
    LINE "  child { c }"
    LINE "}"
    LINE "T t()"
    LINE "T u(c: Red)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t color = ecs_lookup(world, "Color");
    test_assert(color != 0);

    ecs_entity_t t = ecs_lookup(world, "t.child");
    test_assert(t != 0);
    const int32_t *tc = ecs_get_id(world, t, color);
    test_assert(tc != NULL);
    test_int(*tc, 1);

    ecs_entity_t u = ecs_lookup(world, "u.child");
    test_assert(u != 0);
    const int32_t *uc = ecs_get_id(world, u, color);
    test_assert(uc != NULL);
    test_int(*uc, 0);

    ecs_fini(world);
}

void ValueType_template_prop_unconstrained_struct(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template T {"
    LINE "  prop o: value"
    LINE "  child { o }"
    LINE "}"
    LINE "T t(o: Position)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    {
        ecs_entity_t pe = ecs_lookup(world, "t.child");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 0);
        test_flt(p->y, 0);
    }

    ecs_fini(world);
}

void ValueType_template_prop_unconstrained_primitive(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template T {"
    LINE "  prop o: value"
    LINE "  child { o }"
    LINE "}"
    LINE "T t(o: 10)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "t.child");
    test_assert(child != 0);
    const int64_t *v = ecs_get_id(world, child, ecs_id(ecs_i64_t));
    test_assert(v != NULL);
    test_int(*v, 10);

    ecs_fini(world);
}

void ValueType_template_inherit_value_member_from_struct(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "value FacadeRef(type: Facade)"
    LINE "struct Base(facade: FacadeRef)"
    LINE "template Tower : Base {"
    LINE "  prop facade: FacadeRef = Brick"
    LINE "  floor { facade: {height: 6} }"
    LINE "}"
    LINE "Tower t()"
    LINE "Tower u(facade: Glass)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    {
        ecs_entity_t pe = ecs_lookup(world, "t.floor");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 6);
        test_flt(p->y, 1);
    }
    {
        ecs_entity_t pe = ecs_lookup(world, "u.floor");
        test_assert(pe != 0);
        const PositionValue *p = ecs_get_id(
            world, pe, ecs_lookup(world, "Position"));
        test_assert(p != NULL);
        test_flt(p->x, 6);
        test_flt(p->y, 2);
    }

    ecs_fini(world);
}

void ValueType_interpolate(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "const a: value(Facade) = Brick"
    LINE "const b: value(Facade) = Glass(height: 2)"
    LINE "const c: value = 10"
    LINE "\"e_{a}\" {}"
    LINE "\"e_{b}\" {}"
    LINE "\"e_{c}\" {}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "e_Brick") != 0);
    test_assert(ecs_lookup(world, "e_Glass") != 0);
    test_assert(ecs_lookup(world, "e_10") != 0);

    ecs_fini(world);
}

void ValueType_ptr_to_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "value FacadeRef(type: Facade)"
    LINE "struct Building(facade: FacadeRef, n: i32)"
    LINE "a { Building: {facade: Glass(height: 3), n: 2} }"
    LINE "b { Building: {facade: Brick, n: 3} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t building = ecs_lookup(world, "Building");
    test_assert(building != 0);

    char *str = ecs_ptr_to_expr(world, building,
        ecs_get_id(world, ecs_lookup(world, "a"), building));
    test_str(str, "{facade: {Glass: {height: 3}}, n: 2}");
    ecs_os_free(str);

    str = ecs_ptr_to_expr(world, building,
        ecs_get_id(world, ecs_lookup(world, "b"), building));
    test_str(str, "{facade: Brick, n: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void ValueType_expr_roundtrip(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "value FacadeRef(type: Facade)"
    LINE "struct Building(facade: FacadeRef, n: i32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t building = ecs_lookup(world, "Building");
    ecs_entity_t glass = ecs_lookup(world, "Glass");
    ecs_entity_t brick = ecs_lookup(world, "Brick");
    test_assert(building != 0);

    BuildingValue v = {0};
    const char *ptr = ecs_expr_run(world, "{facade: {Glass: {height: 3}}, n: 2}",
        &(ecs_value_t){ .type = building, .ptr = &v }, NULL);
    test_assert(ptr != NULL);
    test_uint(v.facade.type, glass);
    test_assert(v.facade.ptr != NULL);
    test_flt(*(float*)v.facade.ptr, 3);
    test_int(v.n, 2);

    ptr = ecs_expr_run(world, "{facade: Brick, n: 3}",
        &(ecs_value_t){ .type = building, .ptr = &v }, NULL);
    test_assert(ptr != NULL);
    test_uint(v.facade.type, brick);
    test_assert(v.facade.ptr == NULL);
    test_int(v.n, 3);

    ecs_ptr_fini(world, building, &v);

    ecs_fini(world);
}

void ValueType_serialize_type(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        HEAD "struct Facade(height: f32)"
        LINE "value FacadeRef(type: Facade)"
        LINE "struct Building(facade: FacadeRef)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Building"));
    test_str(str,
        HEAD "struct Facade(height: f32)"
        LINE "value FacadeRef(type: Facade)"
        LINE "struct Building(facade: FacadeRef)"
        LINE);
    ecs_os_free(str);

    ecs_fini(world);
}

void ValueType_ast_to_str(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "const a: value(Facade) = 10"
        LINE "const b: value(Facade)[] = [10]"
        LINE "const c: value(Facade)[2] = [10, 20]", NULL, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "a : value(@Facade) =") != NULL);
    test_assert(strstr(str, "b : value(@Facade)[] =") != NULL);
    test_assert(strstr(str, "c : value(@Facade)[2] =") != NULL);
    ecs_os_free(str);

    ecs_script_free(script);

    ecs_fini(world);
}

void ValueType_const_member_on_primitive_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "const x: value(i32) = 10"
    LINE "e { Position: {x.y, 0} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_const_from_unconstrained(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "struct Holder(f: value(Facade))"
    LINE "const a: value = Brick(height: 3)"
    LINE "e { Holder: {f: a} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const ecs_value_t *v = ecs_get_id(world, e, ecs_lookup(world, "Holder"));
    test_assert(v != NULL);
    test_uint(v->type, ecs_lookup(world, "Brick"));

    ecs_fini(world);
}

void ValueType_template_call_on_primitive_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const x: value(i32) = 10"
    LINE "const y = x(height: 3)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void ValueType_const_primitive_from_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "const x: value(i32) = Brick(height: 3)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}
