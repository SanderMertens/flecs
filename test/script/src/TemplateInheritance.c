#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void TemplateInheritance_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

typedef struct {
    float x;
    float y;
} PositionBase;

typedef struct {
    float x;
    float y;
    float z;
} Position3D;

static ecs_script_future_t *ti_futures[64];
static int32_t ti_future_count;
static int32_t ti_cancel_count;
static int32_t ti_args[64];
static ecs_entity_t ti_entities[64];

static void ti_fetch_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    int32_t index = ti_future_count ++;
    ti_entities[index] = ctx->entity;
    if (argc) {
        ti_args[index] = *(int32_t*)argv[0].ptr;
    }
    ti_futures[index] = future;
}

static void ti_cancel_callback(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)future;
    ti_cancel_count ++;
}

void TemplateInheritance_base_template_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, base, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);

    const EcsComponent *c = ecs_get(world, base, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 4);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(0 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "x");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 0);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "z");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 4);

    const EcsScript *s = ecs_get(world, derived, EcsScript);
    test_assert(s != NULL);
    test_assert(s->template_ != NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const float *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v[0], 10);
    test_flt(v[1], 30);

    ecs_fini(world);
}

void TemplateInheritance_base_template_two_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10, y: 20, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 12);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(0 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "x");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 0);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "y");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 4);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(2 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 2);
    test_str(m->name, "z");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 8);

    ecs_entity_t e = ecs_lookup(world, "e");
    const Position3D *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 20);
    test_flt(v->z, 30);

    ecs_fini(world);
}

void TemplateInheritance_base_template_no_own_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "e { Derived: {x: 10, y: 20} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(0 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "x");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 0);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "y");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 4);

    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 20);

    ecs_fini(world);
}

void TemplateInheritance_base_template_prop_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {z: 30} }"
    LINE "f { Derived: {y: 20} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    const Position3D *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v->x, 1);
    test_flt(v->y, 2);
    test_flt(v->z, 30);

    v = ecs_get_id(world, f, derived);
    test_assert(v != NULL);
    test_flt(v->x, 1);
    test_flt(v->y, 20);
    test_flt(v->z, 3);

    ecs_fini(world);
}

void TemplateInheritance_base_template_prop_default_in_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  child { Position: {$x + $z, $y + $z} }"
    LINE "}"
    LINE "e { Derived: {z: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const PositionBase *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 11);
    test_flt(p->y, 12);

    ecs_fini(world);
}

void TemplateInheritance_base_template_prop_set_at_instantiate(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  child { Position: {$x + $z, $y + $z} }"
    LINE "}"
    LINE "e { Derived: {x: 10, y: 20, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const PositionBase *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 40);
    test_flt(p->y, 50);

    ecs_fini(world);
}

void TemplateInheritance_base_template_prop_w_expr_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const K = 10"
    LINE "template Base {"
    LINE "  prop x: f32 = $K * 2"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = $x + 1"
    LINE "}"
    LINE "e { Derived: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v[0], 20);
    test_flt(v[1], 21);

    ecs_fini(world);
}

void TemplateInheritance_base_template_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "Tag {}"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  Tag"
    LINE "  Position: {$x, $x}"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10} }"
    LINE "f { Base: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t tag = ecs_lookup(world, "Tag");
    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    test_assert(ecs_has_id(world, e, derived));
    test_assert(ecs_has_id(world, e, base));
    test_assert(ecs_has_id(world, e, tag));
    test_assert(ecs_has_id(world, e, position));

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 10);

    test_assert(ecs_has_id(world, f, base));
    test_assert(!ecs_has_id(world, f, derived));
    test_assert(ecs_has_id(world, f, tag));
    test_assert(ecs_has_id(world, f, position));

    p = ecs_get_id(world, f, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 10);

    ecs_fini(world);
}

void TemplateInheritance_base_template_string_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop name: string = \"hello\""
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {z: 1} }"
    LINE "f { Derived: {name: \"world\"} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    typedef struct {
        ecs_string_t name;
        ecs_f32_t z;
    } Derived;

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, ECS_SIZEOF(Derived));

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(0 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "name");
    test_uint(m->type, ecs_id(ecs_string_t));
    test_int(m->offset, 0);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "z");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, (int32_t)offsetof(Derived, z));

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    char *str = ecs_ptr_to_expr(world, derived, ecs_get_id(world, e, derived));
    test_str(str, "{name: \"hello\", z: 1}");
    ecs_os_free(str);

    str = ecs_ptr_to_expr(world, derived, ecs_get_id(world, f, derived));
    test_str(str, "{name: \"world\", z: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void TemplateInheritance_base_template_entity_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "Bar {}"
    LINE "template Base {"
    LINE "  prop target: entity = Foo"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  (ChildOf, $target)"
    LINE "}"
    LINE "e { Derived: {} }"
    LINE "f { Derived: {target: Bar} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t e = ecs_lookup(world, "Foo.e");
    ecs_entity_t f = ecs_lookup(world, "Bar.f");
    test_assert(e != 0);
    test_assert(f != 0);
    test_assert(ecs_has_pair(world, e, EcsChildOf, foo));
    test_assert(ecs_has_pair(world, f, EcsChildOf, bar));

    ecs_fini(world);
}

void TemplateInheritance_base_struct(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Base(x: f32, y: f32)"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  Position: {$x + $z, $y}"
    LINE "}"
    LINE "e { Derived: {x: 10, y: 20} }"
    LINE "f { Derived: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 12);

    ecs_entity_t e = ecs_lookup(world, "e");
    const Position3D *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 20);
    test_flt(v->z, 3);

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 13);
    test_flt(p->y, 20);

    ecs_entity_t f = ecs_lookup(world, "f");
    v = ecs_get_id(world, f, derived);
    test_assert(v != NULL);
    test_flt(v->x, 0);
    test_flt(v->y, 0);
    test_flt(v->z, 3);

    ecs_fini(world);
}

void TemplateInheritance_base_component_defined_in_c(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionBase);

    ecs_struct(world, {
        .entity = ecs_id(PositionBase),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Derived : PositionBase {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10, y: 20, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, ecs_id(PositionBase)));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 12);

    ecs_entity_t e = ecs_lookup(world, "e");
    const Position3D *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 20);
    test_flt(v->z, 30);

    ecs_fini(world);
}

void TemplateInheritance_chain(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template A {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { C: {} }"
    LINE "f { C: {x: 10, y: 20, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "A");
    ecs_entity_t b = ecs_lookup(world, "B");
    ecs_entity_t c = ecs_lookup(world, "C");
    test_assert(ecs_has_pair(world, b, EcsIsA, a));
    test_assert(ecs_has_pair(world, c, EcsIsA, b));

    const EcsStruct *st = ecs_get(world, a, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);

    const EcsComponent *comp = ecs_get(world, a, EcsComponent);
    test_assert(comp != NULL);
    test_int(comp->size, 4);

    st = ecs_get(world, b, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    comp = ecs_get(world, b, EcsComponent);
    test_assert(comp != NULL);
    test_int(comp->size, 8);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    comp = ecs_get(world, c, EcsComponent);
    test_assert(comp != NULL);
    test_int(comp->size, 12);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_assert(0 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "x");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 0);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "y");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 4);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_assert(2 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 2);
    test_str(m->name, "z");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 8);

    ecs_entity_t e = ecs_lookup(world, "e");
    const Position3D *v = ecs_get_id(world, e, c);
    test_assert(v != NULL);
    test_flt(v->x, 1);
    test_flt(v->y, 2);
    test_flt(v->z, 3);

    ecs_entity_t f = ecs_lookup(world, "f");
    v = ecs_get_id(world, f, c);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 20);
    test_flt(v->z, 30);

    ecs_fini(world);
}

void TemplateInheritance_chain_props_in_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template A {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop z: f32 = 3"
    LINE "  child { Position: {$x + $y, $z} }"
    LINE "}"
    LINE "e { C: {x: 10, y: 20, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const PositionBase *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 30);
    test_flt(p->y, 30);

    ecs_fini(world);
}

void TemplateInheritance_chain_partial_defaults(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template A {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { C: {y: 20} }"
    LINE "f { C: {x: 10, z: 30} }"
    LINE "g { C: {z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t c = ecs_lookup(world, "C");

    ecs_entity_t e = ecs_lookup(world, "e");
    const Position3D *v = ecs_get_id(world, e, c);
    test_assert(v != NULL);
    test_flt(v->x, 1);
    test_flt(v->y, 20);
    test_flt(v->z, 3);

    ecs_entity_t f = ecs_lookup(world, "f");
    v = ecs_get_id(world, f, c);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 2);
    test_flt(v->z, 30);

    ecs_entity_t g = ecs_lookup(world, "g");
    v = ecs_get_id(world, g, c);
    test_assert(v != NULL);
    test_flt(v->x, 1);
    test_flt(v->y, 2);
    test_flt(v->z, 30);

    ecs_fini(world);
}

void TemplateInheritance_chain_defaults_in_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template A {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop z: f32 = 3"
    LINE "  child { Position: {$x + $y, $z} }"
    LINE "}"
    LINE "e { C: {} }"
    LINE "f { C: {y: 20} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");

    ecs_entity_t e_child = ecs_lookup(world, "e.child");
    test_assert(e_child != 0);
    const PositionBase *p = ecs_get_id(world, e_child, position);
    test_assert(p != NULL);
    test_flt(p->x, 3);
    test_flt(p->y, 3);

    ecs_entity_t f_child = ecs_lookup(world, "f.child");
    test_assert(f_child != 0);
    p = ecs_get_id(world, f_child, position);
    test_assert(p != NULL);
    test_flt(p->x, 21);
    test_flt(p->y, 3);

    ecs_fini(world);
}

void TemplateInheritance_two_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template D1 : Base {"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template D2 : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { D1: {} }"
    LINE "f { D2: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t d1 = ecs_lookup(world, "D1");
    ecs_entity_t d2 = ecs_lookup(world, "D2");

    const EcsStruct *st = ecs_get(world, base, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);

    const EcsComponent *c = ecs_get(world, base, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 4);

    st = ecs_get(world, d1, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    c = ecs_get(world, d1, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    st = ecs_get(world, d2, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    c = ecs_get(world, d2, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    st = ecs_get(world, d1, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "y");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 4);

    st = ecs_get(world, d2, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "z");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 4);

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, d1);
    test_flt(v[0], 1);
    test_flt(v[1], 2);

    ecs_entity_t f = ecs_lookup(world, "f");
    v = ecs_get_id(world, f, d2);
    test_flt(v[0], 10);
    test_flt(v[1], 3);

    ecs_fini(world);
}

void TemplateInheritance_base_prop_change_updates_instance(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  child { Position: {$x, $z} }"
    LINE "}"
    LINE "e { Derived: {x: 10, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const PositionBase *p = ecs_get_id(world, child, position);
    test_flt(p->x, 10);
    test_flt(p->y, 30);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Derived: {x: 20, z: 30} }", &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "e.child") == child);
    p = ecs_get_id(world, child, position);
    test_flt(p->x, 20);
    test_flt(p->y, 30);

    float new_value[2] = { 40, 30 };
    ecs_set_id(world, e, derived, sizeof(float) * 2, new_value);

    p = ecs_get_id(world, child, position);
    test_flt(p->x, 40);
    test_flt(p->y, 30);

    ecs_fini(world);
}

void TemplateInheritance_base_prop_used_by_derived_prop_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 5"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = $x * 2"
    LINE "}"
    LINE "e { Derived: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, derived);
    test_flt(v[0], 5);
    test_flt(v[1], 10);

    ecs_fini(world);
}

void TemplateInheritance_base_w_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  mut counter: i32 = 0"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base_mut = ecs_lookup(world, "Base.mut");
    ecs_entity_t derived_mut = ecs_lookup(world, "Derived.mut");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base_mut != 0);
    test_assert(derived_mut != 0);
    test_assert(derived != 0);
    test_assert(!ecs_has_pair(world, derived, EcsWith, base_mut));
    test_assert(ecs_has_pair(world, derived, EcsWith, derived_mut));
    test_assert(ecs_has_pair(world, derived_mut, EcsIsA, base_mut));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(ecs_has_id(world, e, derived));
    test_assert(ecs_has_id(world, e, derived_mut));
    test_assert(ecs_has_id(world, e, base_mut));

    const ecs_type_t *type = ecs_get_type(world, e);
    for (int32_t i = 0; i < type->count; i ++) {
        test_assert(type->array[i] != base_mut);
    }

    const int32_t *counter = ecs_get_id(world, e, base_mut);
    test_assert(counter != NULL);
    test_int(*counter, 0);
    test_assert(counter == ecs_get_id(world, e, derived_mut));

    ecs_fini(world);
}

void TemplateInheritance_derived_w_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  mut counter: i32 = 10"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived_mut = ecs_lookup(world, "Derived.mut");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived_mut != 0);
    test_assert(ecs_has_pair(world, derived, EcsWith, derived_mut));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(ecs_has_id(world, e, derived));
    test_assert(ecs_has_id(world, e, derived_mut));
    const int32_t *counter = ecs_get_id(world, e, derived_mut);
    test_assert(counter != NULL);
    test_int(*counter, 10);

    ecs_fini(world);
}

void TemplateInheritance_derived_prop_same_name_as_base(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop x: f32 = 5"
    LINE "}"
    LINE "e { Derived: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v[0], 5);

    ecs_fini(world);
}

void TemplateInheritance_delete_base_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);

    ecs_delete(world, base);
    test_assert(!ecs_is_alive(world, base));
    test_assert(ecs_is_alive(world, derived));

    ecs_fini(world);
}

void TemplateInheritance_mut_shadows_base_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template A {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template B : A {"
    LINE "  mut x: f32 = 2"
    LINE "}"
    LINE "e { B: {} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_base_w_count_one_array_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct OneArr(v: {type: f32, count: 1})"
    LINE "template T : OneArr {"
    LINE "  child { Position: {v[0], 0} }"
    LINE "}"
    LINE "T a(v: [7])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t child = ecs_lookup(world, "a.child");
    test_assert(position != 0);
    test_assert(child != 0);

    const float *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p[0], 7);
    test_flt(p[1], 0);

    ecs_fini(world);
}

void TemplateInheritance_base_w_anonymous_array_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t elem = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t light = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Light" }),
        .members = {
            {"lanes", ecs_id(ecs_i32_t)},
            {"lane", elem, .count = 4}
        }
    });
    test_assert(light != 0);

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template L : Light {"
    LINE "  child { Position: {lane[0].x, lanes} }"
    LINE "}"
    LINE "L a(lanes: 1, lane: [{1}, {2}, {3}, {4}])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t tc = ecs_lookup(world, "flecs.script.typecache");
    test_assert(tc != 0);
    ecs_iter_t it = ecs_children(world, tc);
    int32_t count_before = 0;
    while (ecs_children_next(&it)) {
        count_before += it.count;
    }

    test_assert(ecs_script_run_w_desc(world, NULL,
        "L b(lanes: 2, lane: [{5}, {6}, {7}, {8}])", &ir_desc, NULL) == 0);
    test_assert(ecs_script_run_w_desc(world, NULL,
        "L c(lanes: 3, lane: [{9}, {10}, {11}, {12}])", &ir_desc, NULL) == 0);

    it = ecs_children(world, tc);
    int32_t count_after = 0;
    while (ecs_children_next(&it)) {
        count_after += it.count;
    }
    test_int(count_before, count_after);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t a_child = ecs_lookup(world, "a.child");
    ecs_entity_t b_child = ecs_lookup(world, "b.child");
    ecs_entity_t c_child = ecs_lookup(world, "c.child");
    test_assert(a_child != 0);
    test_assert(b_child != 0);
    test_assert(c_child != 0);

    const float *pv = ecs_get_id(world, a_child, position);
    test_assert(pv != NULL);
    test_flt(pv[0], 1);
    test_flt(pv[1], 1);
    pv = ecs_get_id(world, b_child, position);
    test_assert(pv != NULL);
    test_flt(pv[0], 5);
    test_flt(pv[1], 2);
    pv = ecs_get_id(world, c_child, position);
    test_assert(pv != NULL);
    test_flt(pv[0], 9);
    test_flt(pv[1], 3);

    ecs_fini(world);
}

void TemplateInheritance_base_not_a_struct(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Base {}"
    LINE "template Derived : Base {"
    LINE "  prop x: f32 = 5"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_base_is_prefab(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "prefab Base"
    LINE "template Derived : Base {"
    LINE "  prop x: f32 = 5"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_base_is_enum(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "enum Color { Red, Green }"
    LINE "template Derived : Color {"
    LINE "  prop x: f32 = 5"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_base_unresolved(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Derived : Base {"
    LINE "  prop x: f32 = 5"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_base_declared_after_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Derived : Base {"
    LINE "  prop z: f32 = 5"
    LINE "}"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_self_inherit(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Derived : Derived {"
    LINE "  prop x: f32 = 5"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_missing_base_name(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Derived : {"
    LINE "  prop x: f32 = 5"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_newline_before_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base"
    LINE "{"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, derived);
    test_flt(v[0], 1);
    test_flt(v[1], 3);

    ecs_fini(world);
}

void TemplateInheritance_no_space_around_colon(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived:Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    ecs_fini(world);
}

void TemplateInheritance_base_in_module(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module shapes"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    expr =
    HEAD "using shapes"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "shapes.Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, derived);
    test_flt(v[0], 10);
    test_flt(v[1], 3);

    ecs_fini(world);
}

void TemplateInheritance_base_in_module_path(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module shapes"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    expr =
    HEAD "template Derived : shapes.Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "shapes.Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, derived);
    test_flt(v[0], 10);
    test_flt(v[1], 3);

    ecs_fini(world);
}

void TemplateInheritance_base_in_separate_script(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    expr =
    HEAD "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {y: 20} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 12);

    ecs_entity_t e = ecs_lookup(world, "e");
    const Position3D *v = ecs_get_id(world, e, derived);
    test_flt(v->x, 1);
    test_flt(v->y, 20);
    test_flt(v->z, 3);

    ecs_fini(world);
}

void TemplateInheritance_run_script_twice(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    test_assert(ecs_script_run_w_desc(world, NULL, "e { Derived: {} }", &ir_desc, NULL) == 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, derived);
    test_flt(v[0], 1);
    test_flt(v[1], 3);

    ecs_fini(world);
}

void TemplateInheritance_managed_script_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled,
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    test_assert(ecs_script_update(world, s, 0, expr) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const float *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v[0], 10);
    test_flt(v[1], 3);

    ecs_fini(world);
}

void TemplateInheritance_managed_script_update_base_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}";

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled,
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 5"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}";

    test_assert(ecs_script_update(world, s, 0, expr) == 0);

    test_assert(ecs_script_run_w_desc(world, NULL, "e { Derived: {} }", &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v[0], 5);
    test_flt(v[1], 3);

    ecs_fini(world);
}

void TemplateInheritance_ast_to_str(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}";

    ecs_script_t *s = ecs_script_parse(world, NULL, expr, &ir_desc, NULL);
    test_assert(s != NULL);

    char *str = ecs_script_ast_to_str(s, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "Derived : Base") != NULL);
    ecs_os_free(str);

    ecs_script_free(s);

    ecs_fini(world);
}

void TemplateInheritance_derived_instance_json(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    char *json = ecs_entity_to_json(world, e, &(ecs_entity_to_json_desc_t){
        .serialize_values = true
    });
    test_assert(json != NULL);
    test_assert(strstr(json, "\"Derived\":{\"x\":10, \"z\":3}") != NULL);
    ecs_os_free(json);

    ecs_fini(world);
}

void TemplateInheritance_derived_instance_set_from_c(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  child { Position: {$x + $y, $z} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t position = ecs_lookup(world, "Position");

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    Position3D v = { 10, 20, 30 };
    ecs_set_id(world, e, derived, sizeof(Position3D), &v);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    const PositionBase *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 30);
    test_flt(p->y, 30);

    ecs_entity_t f = ecs_entity(world, { .name = "f" });
    ecs_add_id(world, f, derived);
    ecs_modified_id(world, f, derived);
    const Position3D *fv = ecs_get_id(world, f, derived);
    test_flt(fv->x, 1);
    test_flt(fv->y, 2);
    test_flt(fv->z, 3);

    child = ecs_lookup(world, "f.child");
    test_assert(child != 0);
    p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 3);
    test_flt(p->y, 3);

    ecs_fini(world);
}

void TemplateInheritance_const_before_prop_in_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  const c = 10"
    LINE "  prop z: f32 = 3"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_const_after_prop_in_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  const c = $x + $z"
    LINE "  Position: {$c, $c}"
    LINE "}"
    LINE "e { Derived: {x: 10, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 40);
    test_flt(p->y, 40);

    ecs_fini(world);
}

void TemplateInheritance_derived_from_template_w_captured_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "const K = 100"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  Position: {$x + $K, $z}"
    LINE "}"
    LINE "e { Derived: {x: 10, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 110);
    test_flt(p->y, 30);

    ecs_fini(world);
}

void TemplateInheritance_derived_prop_override_base_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop x: f32 = 5"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "b { Base: {} }"
    LINE "e { Derived: {} }"
    LINE "f { Derived: {x: 10} }"
    LINE "g { Derived: {y: 20, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 12);

    const PositionBase *pb = ecs_get_id(world, ecs_lookup(world, "b"), base);
    test_assert(pb != NULL);
    test_flt(pb->x, 1);
    test_flt(pb->y, 2);

    const Position3D *v = ecs_get_id(world, ecs_lookup(world, "e"), derived);
    test_assert(v != NULL);
    test_flt(v->x, 5);
    test_flt(v->y, 2);
    test_flt(v->z, 3);

    v = ecs_get_id(world, ecs_lookup(world, "f"), derived);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 2);
    test_flt(v->z, 3);

    v = ecs_get_id(world, ecs_lookup(world, "g"), derived);
    test_assert(v != NULL);
    test_flt(v->x, 5);
    test_flt(v->y, 20);
    test_flt(v->z, 30);

    ecs_fini(world);
}

void TemplateInheritance_derived_prop_override_struct_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Rgb(r: f32, g: f32, b: f32)"
    LINE "template Theme {"
    LINE "  prop panel: Rgb = {0.1, 0.2, 0.3}"
    LINE "  child { Rgb: {$panel.r, $panel.g, $panel.b} }"
    LINE "}"
    LINE "template Light : Theme {"
    LINE "  prop panel: Rgb = {1, 2, 3}"
    LINE "  child { Rgb: {$panel.r, $panel.g, $panel.b} }"
    LINE "}"
    LINE "Theme t"
    LINE "Light l"
    LINE "Light m(panel: {4, 5, 6})";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t rgb = ecs_lookup(world, "Rgb");
    ecs_entity_t light = ecs_lookup(world, "Light");
    test_assert(rgb != 0);
    test_assert(light != 0);

    const float *v = ecs_get_id(world, ecs_lookup(world, "t.child"), rgb);
    test_assert(v != NULL);
    test_flt(v[0], 0.1);
    test_flt(v[1], 0.2);
    test_flt(v[2], 0.3);

    v = ecs_get_id(world, ecs_lookup(world, "l"), light);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 3);

    v = ecs_get_id(world, ecs_lookup(world, "l.child"), rgb);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 3);

    v = ecs_get_id(world, ecs_lookup(world, "m.child"), rgb);
    test_assert(v != NULL);
    test_flt(v[0], 4);
    test_flt(v[1], 5);
    test_flt(v[2], 6);

    ecs_fini(world);
}

void TemplateInheritance_derived_prop_override_used_in_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop x: f32 = 5"
    LINE "  prop z: f32 = $x * 2"
    LINE "  child { Position: {$x, $z} }"
    LINE "}"
    LINE "Derived e"
    LINE "Derived f(x: 7)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionBase *p = ecs_get_id(
        world, ecs_lookup(world, "e.child"), position);
    test_assert(p != NULL);
    test_flt(p->x, 5);
    test_flt(p->y, 10);

    p = ecs_get_id(world, ecs_lookup(world, "f.child"), position);
    test_assert(p != NULL);
    test_flt(p->x, 7);
    test_flt(p->y, 10);

    ecs_fini(world);
}

void TemplateInheritance_derived_prop_override_chain(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template A {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop x: f32 = 2"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop x: f32 = 3"
    LINE "}"
    LINE "template D : B {"
    LINE "}"
    LINE "a { A: {} }"
    LINE "b { B: {} }"
    LINE "c { C: {} }"
    LINE "d { D: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    const float *v = ecs_get_id(
        world, ecs_lookup(world, "a"), ecs_lookup(world, "A"));
    test_assert(v != NULL);
    test_flt(v[0], 1);

    v = ecs_get_id(world, ecs_lookup(world, "b"), ecs_lookup(world, "B"));
    test_assert(v != NULL);
    test_flt(v[0], 2);

    v = ecs_get_id(world, ecs_lookup(world, "c"), ecs_lookup(world, "C"));
    test_assert(v != NULL);
    test_flt(v[0], 3);

    v = ecs_get_id(world, ecs_lookup(world, "d"), ecs_lookup(world, "D"));
    test_assert(v != NULL);
    test_flt(v[0], 2);

    ecs_fini(world);
}

void TemplateInheritance_derived_prop_override_wrong_type_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop x: i32 = 5"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_derived_prop_override_no_default_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop x: f32"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateInheritance_base_body_default_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived }"
    LINE "f { Derived: {z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(position != 0);
    test_assert(e != 0);
    test_assert(f != 0);

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    p = ecs_get_id(world, f, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateInheritance_base_body_partial_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {y: 20} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 20);

    ecs_fini(world);
}

void TemplateInheritance_base_body_and_derived_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  Position: {$x, 0}"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  Velocity: {$x, $z}"
    LINE "}"
    LINE "e { Derived: {x: 10, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t e = ecs_lookup(world, "e");

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 0);

    const PositionBase *v = ecs_get_id(world, e, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 30);

    ecs_fini(world);
}

void TemplateInheritance_base_body_multi_level(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "struct Mass(value: f32)"
    LINE "template A {"
    LINE "  prop x: f32 = 1"
    LINE "  Position: {$x, 0}"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop y: f32 = 2"
    LINE "  Velocity: {$x, $y}"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop z: f32 = 3"
    LINE "  Mass: {$x + $y + $z}"
    LINE "}"
    LINE "e { C: {x: 10, y: 20, z: 30} }"
    LINE "f { B: {x: 100, y: 200} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t mass = ecs_lookup(world, "Mass");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    test_assert(ecs_has_id(world, e, ecs_lookup(world, "A")));
    test_assert(ecs_has_id(world, e, ecs_lookup(world, "B")));
    test_assert(ecs_has_id(world, e, ecs_lookup(world, "C")));

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 0);

    const PositionBase *v = ecs_get_id(world, e, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 20);

    const float *m = ecs_get_id(world, e, mass);
    test_assert(m != NULL);
    test_flt(*m, 60);

    p = ecs_get_id(world, f, position);
    test_assert(p != NULL);
    test_flt(p->x, 100);
    test_flt(p->y, 0);

    v = ecs_get_id(world, f, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 100);
    test_flt(v->y, 200);

    test_assert(!ecs_has_id(world, f, mass));

    ecs_fini(world);
}

void TemplateInheritance_base_body_multi_level_default_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "struct Mass(value: f32)"
    LINE "template A {"
    LINE "  prop x: f32 = 1"
    LINE "  Position: {$x, 0}"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop y: f32 = 2"
    LINE "  Velocity: {$x, $y}"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop z: f32 = 3"
    LINE "  Mass: {$x + $y + $z}"
    LINE "}"
    LINE "e { C }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t mass = ecs_lookup(world, "Mass");
    ecs_entity_t e = ecs_lookup(world, "e");

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 0);

    const PositionBase *v = ecs_get_id(world, e, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 1);
    test_flt(v->y, 2);

    const float *m = ecs_get_id(world, e, mass);
    test_assert(m != NULL);
    test_flt(*m, 6);

    ecs_fini(world);
}

void TemplateInheritance_base_body_prop_override(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop x: f32 = 5"
    LINE "}"
    LINE "e { Derived }"
    LINE "f { Base }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 5);
    test_flt(p->y, 2);

    p = ecs_get_id(world, f, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateInheritance_base_body_prop_override_provided_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop x: f32 = 5"
    LINE "}"
    LINE "e { Derived: {x: 10} }"
    LINE "f { Derived: {y: 20} }"
    LINE "g { Derived: {x: 10, y: 20} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");
    ecs_entity_t g = ecs_lookup(world, "g");

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 2);

    p = ecs_get_id(world, f, position);
    test_assert(p != NULL);
    test_flt(p->x, 5);
    test_flt(p->y, 20);

    p = ecs_get_id(world, g, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 20);

    ecs_fini(world);
}

void TemplateInheritance_base_body_prop_override_multi_level(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "template A {"
    LINE "  prop x: f32 = 1"
    LINE "  Position: {$x, 0}"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop x: f32 = 2"
    LINE "  Velocity: {$x, 0}"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template D : C {"
    LINE "  prop x: f32 = 4"
    LINE "}"
    LINE "a { A }"
    LINE "b { B }"
    LINE "c { C }"
    LINE "d { D }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t c = ecs_lookup(world, "c");
    ecs_entity_t d = ecs_lookup(world, "d");

    const PositionBase *p = ecs_get_id(world, a, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_assert(!ecs_has_id(world, a, velocity));

    p = ecs_get_id(world, b, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    const PositionBase *v = ecs_get_id(world, b, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 2);

    p = ecs_get_id(world, c, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    v = ecs_get_id(world, c, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 2);

    p = ecs_get_id(world, d, position);
    test_assert(p != NULL);
    test_flt(p->x, 4);
    v = ecs_get_id(world, d, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 4);

    ecs_fini(world);
}

void TemplateInheritance_base_body_prop_override_multi_level_provided_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "template A {"
    LINE "  prop x: f32 = 1"
    LINE "  Position: {$x, 0}"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop x: f32 = 2"
    LINE "  Velocity: {$x, 0}"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template D : C {"
    LINE "  prop x: f32 = 4"
    LINE "}"
    LINE "b { B: {x: 10} }"
    LINE "c { C: {x: 20} }"
    LINE "d { D: {x: 30} }"
    LINE "d2 { D: {z: 40} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t c = ecs_lookup(world, "c");
    ecs_entity_t d = ecs_lookup(world, "d");
    ecs_entity_t d2 = ecs_lookup(world, "d2");

    const PositionBase *p = ecs_get_id(world, b, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    const PositionBase *v = ecs_get_id(world, b, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 10);

    p = ecs_get_id(world, c, position);
    test_assert(p != NULL);
    test_flt(p->x, 20);
    v = ecs_get_id(world, c, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 20);

    p = ecs_get_id(world, d, position);
    test_assert(p != NULL);
    test_flt(p->x, 30);
    v = ecs_get_id(world, d, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 30);

    p = ecs_get_id(world, d2, position);
    test_assert(p != NULL);
    test_flt(p->x, 4);
    v = ecs_get_id(world, d2, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 4);

    ecs_fini(world);
}

void TemplateInheritance_base_body_prop_change(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  Position: {$x, 0}"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  Velocity: {$z, 0}"
    LINE "}"
    LINE "e { Derived: {x: 10, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);

    float new_value[2] = { 40, 30 };
    ecs_set_id(world, e, derived, sizeof(float) * 2, new_value);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 40);
    const PositionBase *v = ecs_get_id(world, e, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 30);

    float new_value_2[2] = { 40, 50 };
    ecs_set_id(world, e, derived, sizeof(float) * 2, new_value_2);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 40);
    v = ecs_get_id(world, e, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 50);

    ecs_fini(world);
}

void TemplateInheritance_base_body_prop_change_multi_level(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "struct Mass(value: f32)"
    LINE "template A {"
    LINE "  prop x: f32 = 1"
    LINE "  Position: {$x, 0}"
    LINE "}"
    LINE "template B : A {"
    LINE "  prop y: f32 = 2"
    LINE "  Velocity: {$x, $y}"
    LINE "}"
    LINE "template C : B {"
    LINE "  prop z: f32 = 3"
    LINE "  Mass: {$x + $y + $z}"
    LINE "}"
    LINE "e { C: {x: 10, y: 20, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t mass = ecs_lookup(world, "Mass");
    ecs_entity_t c = ecs_lookup(world, "C");
    ecs_entity_t e = ecs_lookup(world, "e");

    float new_value[3] = { 11, 20, 30 };
    ecs_set_id(world, e, c, sizeof(float) * 3, new_value);

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 11);
    const PositionBase *v = ecs_get_id(world, e, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 11);
    test_flt(v->y, 20);
    const float *m = ecs_get_id(world, e, mass);
    test_assert(m != NULL);
    test_flt(*m, 61);

    float new_value_2[3] = { 11, 21, 30 };
    ecs_set_id(world, e, c, sizeof(float) * 3, new_value_2);

    p = ecs_get_id(world, e, position);
    test_flt(p->x, 11);
    v = ecs_get_id(world, e, velocity);
    test_flt(v->x, 11);
    test_flt(v->y, 21);
    m = ecs_get_id(world, e, mass);
    test_flt(*m, 62);

    ecs_fini(world);
}

void TemplateInheritance_derived_body_overrides_base_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop w: f32 = 2"
    LINE "  Position: {$x, $w}"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  Position: {$x, 100}"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 100);

    float new_value[2] = { 10, 50 };
    ecs_set_id(world, e, derived, sizeof(float) * 2, new_value);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 100);

    float new_value_2[2] = { 20, 50 };
    ecs_set_id(world, e, derived, sizeof(float) * 2, new_value_2);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 20);
    test_flt(p->y, 100);

    ecs_fini(world);
}

void TemplateInheritance_base_body_children(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  child_a { Position: {$x, 0} }"
    LINE "  shared { Position: {$x, 1} }"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  child_b { Position: {$z, 0} }"
    LINE "  shared { Velocity: {$z, 1} }"
    LINE "}"
    LINE "e { Derived: {x: 10, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    ecs_entity_t shared = ecs_lookup(world, "e.shared");
    test_assert(child_a != 0);
    test_assert(child_b != 0);
    test_assert(shared != 0);

    const PositionBase *p = ecs_get_id(world, child_a, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);

    p = ecs_get_id(world, child_b, position);
    test_assert(p != NULL);
    test_flt(p->x, 30);

    p = ecs_get_id(world, shared, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 1);

    const PositionBase *v = ecs_get_id(world, shared, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 30);
    test_flt(v->y, 1);

    ecs_remove_id(world, e, derived);

    test_assert(ecs_lookup(world, "e.child_a") == 0);
    test_assert(ecs_lookup(world, "e.child_b") == 0);
    test_assert(ecs_lookup(world, "e.shared") == 0);

    ecs_fini(world);
}

void TemplateInheritance_remove_derived_cleans_up_base_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  Position: {$x, 0}"
    LINE "  child { Position: {$x, 0} }"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "  Velocity: {$z, 0}"
    LINE "}"
    LINE "e { Derived: {x: 10, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    test_assert(ecs_has_id(world, e, position));
    test_assert(ecs_has_id(world, e, velocity));
    test_assert(ecs_lookup(world, "e.child") != 0);

    ecs_remove_id(world, e, derived);

    test_assert(!ecs_has_id(world, e, derived));
    test_assert(!ecs_has_id(world, e, base));
    test_assert(!ecs_has_id(world, e, position));
    test_assert(!ecs_has_id(world, e, velocity));
    test_assert(ecs_lookup(world, "e.child") == 0);

    ecs_fini(world);
}

void TemplateInheritance_base_mut_in_derived_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  mut counter: i32 = 10"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  Position: {counter, 0}"
    LINE "}"
    LINE "e { Derived }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);

    ecs_fini(world);
}

void TemplateInheritance_base_mut_and_derived_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  mut a: i32 = 10"
    LINE "  child { Position: {a, 0} }"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  mut b: i32 = 20"
    LINE "  Position: {a, b}"
    LINE "}"
    LINE "e { Derived }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t base_mut = ecs_lookup(world, "Base.mut");
    ecs_entity_t derived_mut = ecs_lookup(world, "Derived.mut");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(base_mut != 0);
    test_assert(derived_mut != 0);
    test_assert(child != 0);
    test_assert(ecs_has_pair(world, derived_mut, EcsIsA, base_mut));

    const EcsStruct *st = ecs_get(world, base_mut, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);

    st = ecs_get(world, derived_mut, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "a");
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "b");

    const int32_t *m = ecs_get_id(world, e, derived_mut);
    test_assert(m != NULL);
    test_int(m[0], 10);
    test_int(m[1], 20);

    const int32_t *bm = ecs_get_id(world, e, base_mut);
    test_assert(bm == m);

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 20);

    p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);

    ecs_fini(world);
}

void TemplateInheritance_base_mut_multi_level(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Mass(value: f32)"
    LINE "template A {"
    LINE "  mut a: i32 = 1"
    LINE "}"
    LINE "template B : A {"
    LINE "  mut b: i32 = 2"
    LINE "}"
    LINE "template C : B {"
    LINE "  mut c: i32 = 3"
    LINE "  Position: {a, b}"
    LINE "  Mass: {c}"
    LINE "}"
    LINE "e { C }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t mass = ecs_lookup(world, "Mass");
    ecs_entity_t a_mut = ecs_lookup(world, "A.mut");
    ecs_entity_t b_mut = ecs_lookup(world, "B.mut");
    ecs_entity_t c_mut = ecs_lookup(world, "C.mut");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(ecs_has_pair(world, b_mut, EcsIsA, a_mut));
    test_assert(ecs_has_pair(world, c_mut, EcsIsA, b_mut));

    test_assert(ecs_has_id(world, e, a_mut));
    test_assert(ecs_has_id(world, e, b_mut));
    test_assert(ecs_has_id(world, e, c_mut));

    const int32_t *m = ecs_get_id(world, e, c_mut);
    test_assert(m != NULL);
    test_int(m[0], 1);
    test_int(m[1], 2);
    test_int(m[2], 3);
    test_assert(m == ecs_get_id(world, e, a_mut));
    test_assert(m == ecs_get_id(world, e, b_mut));

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    const float *ms = ecs_get_id(world, e, mass);
    test_assert(ms != NULL);
    test_flt(*ms, 3);

    ecs_fini(world);
}

void TemplateInheritance_base_mut_init_w_prop_override(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  mut m: f32 = x * 2"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop x: f32 = 5"
    LINE "  Position: {m, 0}"
    LINE "}"
    LINE "e { Derived }"
    LINE "f { Derived: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);

    p = ecs_get_id(world, f, position);
    test_assert(p != NULL);
    test_flt(p->x, 20);

    ecs_fini(world);
}

void TemplateInheritance_base_mut_set_from_c(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  mut a: i32 = 10"
    LINE "  child { Position: {a, 0} }"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  mut b: i32 = 20"
    LINE "  Position: {a, b}"
    LINE "}"
    LINE "e { Derived }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t derived_mut = ecs_lookup(world, "Derived.mut");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");

    int32_t value[2] = { 11, 20 };
    ecs_set_id(world, e, derived_mut, sizeof(int32_t) * 2, value);

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 11);
    test_flt(p->y, 20);

    p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 11);

    int32_t value_2[2] = { 11, 21 };
    ecs_set_id(world, e, derived_mut, sizeof(int32_t) * 2, value_2);

    p = ecs_get_id(world, e, position);
    test_flt(p->x, 11);
    test_flt(p->y, 21);

    p = ecs_get_id(world, child, position);
    test_flt(p->x, 11);

    ecs_fini(world);
}

void TemplateInheritance_mut_shadows_base_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  mut a: i32 = 10"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  mut a: i32 = 20"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void TemplateInheritance_prop_shadows_base_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Base {"
    LINE "  mut a: i32 = 10"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  prop a: i32 = 20"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void TemplateInheritance_base_async_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ti_futures);
    ecs_os_zeromem(ti_args);
    ecs_os_zeromem(ti_entities);
    ti_future_count = 0;
    ti_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ti_fetch_callback,
        .cancel = ti_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Position(x: f32, y: f32)"
            LINE "template Base {"
            LINE "  prop v: i32 = 1"
            LINE "  mut m: i32 = 0"
            LINE "  child { Position: {m, 0} }"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "template Derived : Base {"
            LINE "  prop z: f32 = 3"
            LINE "  Position: {m, $z}"
            LINE "}"
            LINE "e { Derived: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t derived_mut = ecs_lookup(world, "Derived.mut");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(derived_mut != 0);
    test_assert(child != 0);

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 0);
    test_flt(p->y, 3);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ti_future_count, 1);
    test_int(ti_args[0], 5);
    test_uint(ti_entities[0], e);

    ecs_value_t v = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ti_futures[0], &v), 0);
    ecs_script_future_release(ti_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    const int32_t *m = ecs_get_id(world, e, derived_mut);
    test_assert(m != NULL);
    test_int(*m, 7);

    const ecs_type_t *type = ecs_get_type(world, e);
    ecs_entity_t base_mut = ecs_lookup(world, "Base.mut");
    for (int32_t i = 0; i < type->count; i ++) {
        test_assert(type->array[i] != base_mut);
    }

    p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 7);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 7);
    test_flt(p->y, 3);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void TemplateInheritance_base_async_block_default_props(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ti_futures);
    ecs_os_zeromem(ti_args);
    ecs_os_zeromem(ti_entities);
    ti_future_count = 0;
    ti_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ti_fetch_callback,
        .cancel = ti_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template Base {"
            LINE "  prop v: i32 = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "template Derived : Base {"
            LINE "  prop v: i32 = 8"
            LINE "}"
            LINE "e { Derived }"
            LINE "f { Derived: {v: 9} }"
            LINE "g { Base }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 3);
    test_int(ti_future_count, 3);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");
    ecs_entity_t g = ecs_lookup(world, "g");
    int32_t i, found = 0;
    for (i = 0; i < 3; i ++) {
        if (ti_entities[i] == e) {
            test_int(ti_args[i], 8);
            found ++;
        } else if (ti_entities[i] == f) {
            test_int(ti_args[i], 9);
            found ++;
        } else if (ti_entities[i] == g) {
            test_int(ti_args[i], 1);
            found ++;
        }

        ecs_script_future_release(ti_futures[i]);
    }

    test_int(found, 3);

    ecs_fini(world);
}

void TemplateInheritance_base_async_block_multi_level(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ti_futures);
    ecs_os_zeromem(ti_args);
    ecs_os_zeromem(ti_entities);
    ti_future_count = 0;
    ti_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ti_fetch_callback,
        .cancel = ti_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Position(x: f32, y: f32)"
            LINE "template A {"
            LINE "  prop v: i32 = 1"
            LINE "  mut a: i32 = 0"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    a = r"
            LINE "  }"
            LINE "}"
            LINE "template B : A {"
            LINE "  prop w: i32 = 2"
            LINE "}"
            LINE "template C : B {"
            LINE "  mut c: i32 = 0"
            LINE "  Position: {a, c}"
            LINE "  async {"
            LINE "    const r = await fetch(w)"
            LINE "    c = r"
            LINE "  }"
            LINE "}"
            LINE "e { C: {v: 5, w: 6} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");

    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ti_future_count, 2);

    int32_t i;
    for (i = 0; i < 2; i ++) {
        test_uint(ti_entities[i], e);
        test_assert(ti_args[i] == 5 || ti_args[i] == 6);
        ecs_value_t v = ecs_value(ecs_i32_t, {ti_args[i] * 10});
        test_int(ecs_script_future_resolve(ti_futures[i], &v), 0);
        ecs_script_future_release(ti_futures[i]);
    }

    test_assert(ti_args[0] != ti_args[1]);

    test_int(ecs_script_tasks_progress(world), 2);

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 50);
    test_flt(p->y, 60);

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ti_future_count, 2);

    ecs_fini(world);
}

void TemplateInheritance_derived_async_block_assign_base_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ti_futures);
    ecs_os_zeromem(ti_args);
    ecs_os_zeromem(ti_entities);
    ti_future_count = 0;
    ti_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ti_fetch_callback,
        .cancel = ti_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Position(x: f32, y: f32)"
            LINE "template Base {"
            LINE "  mut m: i32 = 0"
            LINE "  child { Position: {m, 0} }"
            LINE "}"
            LINE "template Derived : Base {"
            LINE "  prop v: i32 = 1"
            LINE "  Position: {m, 0}"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "e { Derived: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ti_future_count, 1);
    test_int(ti_args[0], 5);

    ecs_value_t v = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ti_futures[0], &v), 0);
    ecs_script_future_release(ti_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    const PositionBase *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 7);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 7);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void TemplateInheritance_base_async_block_mut_change_keeps_derived_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ti_futures);
    ecs_os_zeromem(ti_args);
    ecs_os_zeromem(ti_entities);
    ti_future_count = 0;
    ti_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ti_fetch_callback,
        .cancel = ti_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Position(x: f32, y: f32)"
            LINE "template Base {"
            LINE "  prop v: i32 = 1"
            LINE "  mut m: i32 = 0"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "template Derived : Base {"
            LINE "  Position: {m, 0}"
            LINE "  async {"
            LINE "    await fetch(100)"
            LINE "  }"
            LINE "}"
            LINE "e { Derived: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");

    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ti_future_count, 2);

    int32_t base_future = ti_args[0] == 5 ? 0 : 1;
    test_int(ti_args[base_future], 5);
    test_int(ti_args[!base_future], 100);

    ecs_value_t v = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ti_futures[base_future], &v), 0);
    ecs_script_future_release(ti_futures[base_future]);

    test_int(ecs_script_tasks_progress(world), 1);

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 7);

    test_int(ti_cancel_count, 0);
    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ti_future_count, 2);

    ecs_script_future_release(ti_futures[!base_future]);

    ecs_fini(world);
}

void TemplateInheritance_base_async_block_prop_change_restarts(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ti_futures);
    ecs_os_zeromem(ti_args);
    ecs_os_zeromem(ti_entities);
    ti_future_count = 0;
    ti_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ti_fetch_callback,
        .cancel = ti_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template Base {"
            LINE "  prop v: i32 = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "template Derived : Base {"
            LINE "  prop z: i32 = 3"
            LINE "}"
            LINE "e { Derived: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ti_future_count, 1);
    test_int(ti_args[0], 5);

    int32_t value[2] = { 9, 3 };
    ecs_set_id(world, e, derived, sizeof(int32_t) * 2, value);

    test_int(ti_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ti_future_count, 2);
    test_int(ti_args[1], 9);

    int32_t value_2[2] = { 9, 4 };
    ecs_set_id(world, e, derived, sizeof(int32_t) * 2, value_2);

    test_int(ti_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ti_future_count, 2);

    ecs_script_future_release(ti_futures[0]);
    ecs_script_future_release(ti_futures[1]);

    ecs_fini(world);
}

void TemplateInheritance_base_async_block_remove_cancels(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ti_futures);
    ecs_os_zeromem(ti_args);
    ecs_os_zeromem(ti_entities);
    ti_future_count = 0;
    ti_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ti_fetch_callback,
        .cancel = ti_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template Base {"
            LINE "  prop v: i32 = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "template Derived : Base {"
            LINE "  prop z: i32 = 3"
            LINE "}"
            LINE "e { Derived: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ti_future_count, 1);
    ecs_script_future_release(ti_futures[0]);

    test_int(ti_cancel_count, 0);
    ecs_remove_id(world, e, derived);
    test_int(ti_cancel_count, 1);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void TemplateInheritance_base_async_block_delete_cancels(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ti_futures);
    ecs_os_zeromem(ti_args);
    ecs_os_zeromem(ti_entities);
    ti_future_count = 0;
    ti_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ti_fetch_callback,
        .cancel = ti_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template Base {"
            LINE "  prop v: i32 = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "template Derived : Base {"
            LINE "  prop z: i32 = 3"
            LINE "}"
            LINE "e { Derived: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ti_future_count, 1);
    ecs_script_future_release(ti_futures[0]);

    test_int(ti_cancel_count, 0);
    ecs_delete(world, e);
    test_int(ti_cancel_count, 1);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void TemplateInheritance_base_async_block_two_instances(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ti_futures);
    ecs_os_zeromem(ti_args);
    ecs_os_zeromem(ti_entities);
    ti_future_count = 0;
    ti_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ti_fetch_callback,
        .cancel = ti_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Position(x: f32, y: f32)"
            LINE "template Base {"
            LINE "  prop v: i32 = 1"
            LINE "  mut m: i32 = 0"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "template Derived : Base {"
            LINE "  Position: {m, 0}"
            LINE "}"
            LINE "a { Derived: {v: 5} }"
            LINE "b { Derived: {v: 6} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");

    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ti_future_count, 2);

    int32_t a_future = ti_entities[0] == a ? 0 : 1;
    test_uint(ti_entities[a_future], a);
    test_uint(ti_entities[!a_future], b);
    test_int(ti_args[a_future], 5);
    test_int(ti_args[!a_future], 6);

    ecs_value_t v = ecs_value(ecs_i32_t, {70});
    test_int(ecs_script_future_resolve(ti_futures[a_future], &v), 0);
    ecs_script_future_release(ti_futures[a_future]);

    test_int(ecs_script_tasks_progress(world), 1);

    const PositionBase *p = ecs_get_id(world, a, position);
    test_assert(p != NULL);
    test_flt(p->x, 70);
    p = ecs_get_id(world, b, position);
    test_assert(p != NULL);
    test_flt(p->x, 0);

    v = ecs_value(ecs_i32_t, {80});
    test_int(ecs_script_future_resolve(ti_futures[!a_future], &v), 0);
    ecs_script_future_release(ti_futures[!a_future]);

    test_int(ecs_script_tasks_progress(world), 1);

    p = ecs_get_id(world, a, position);
    test_flt(p->x, 70);
    p = ecs_get_id(world, b, position);
    test_flt(p->x, 80);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void TemplateInheritance_template_update_w_base(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "  prop w: f32 = 2"
    LINE "  Position: {$x, $w}"
    LINE "  Velocity: {$x, $w}"
    LINE "}"
    LINE "template Derived : Base {"
    LINE "  Position: {$x, 100}"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    ecs_remove_id(world, e, velocity);
    test_assert(!ecs_has_id(world, e, velocity));

    test_int(ecs_script_template_update(world, e, base), 0);

    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 100);

    const PositionBase *v = ecs_get_id(world, e, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 2);

    ecs_remove_id(world, e, velocity);
    test_int(ecs_script_template_update(world, e, derived), 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 100);

    v = ecs_get_id(world, e, velocity);
    test_assert(v != NULL);
    test_flt(v->x, 10);
    test_flt(v->y, 2);

    ecs_fini(world);
}
