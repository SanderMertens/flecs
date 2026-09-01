#include <script.h>

typedef struct {
    float x;
    float y;
} PositionBase;

typedef struct {
    float x;
    float y;
    float z;
} Position3D;

static void test_member(
    ecs_world_t *world,
    ecs_entity_t type,
    int32_t index,
    const char *name,
    ecs_entity_t member_type,
    int32_t offset)
{
    const EcsStruct *st = ecs_get(world, type, EcsStruct);
    test_assert(st != NULL);
    test_assert(index < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, index);
    test_str(m->name, name);
    test_uint(m->type, member_type);
    test_int(m->offset, offset);
}

static void test_struct(
    ecs_world_t *world,
    ecs_entity_t type,
    int32_t member_count,
    ecs_size_t size)
{
    const EcsStruct *st = ecs_get(world, type, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), member_count);

    const EcsComponent *c = ecs_get(world, type, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, size);
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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    test_struct(world, base, 1, 4);
    test_struct(world, derived, 2, 8);
    test_member(world, derived, 0, "x", ecs_id(ecs_f32_t), 0);
    test_member(world, derived, 1, "z", ecs_id(ecs_f32_t), 4);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);
    test_struct(world, derived, 3, 12);
    test_member(world, derived, 0, "x", ecs_id(ecs_f32_t), 0);
    test_member(world, derived, 1, "y", ecs_id(ecs_f32_t), 4);
    test_member(world, derived, 2, "z", ecs_id(ecs_f32_t), 8);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(derived != 0);
    test_struct(world, derived, 2, 8);
    test_member(world, derived, 0, "x", ecs_id(ecs_f32_t), 0);
    test_member(world, derived, 1, "y", ecs_id(ecs_f32_t), 4);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, derived);
    test_assert(v != NULL);
    test_flt(v[0], 20);
    test_flt(v[1], 21);

    ecs_fini(world);
}

void TemplateInheritance_base_template_body_not_inherited(void) {
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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tag = ecs_lookup(world, "Tag");
    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    test_assert(ecs_has_id(world, e, derived));
    test_assert(!ecs_has_id(world, e, base));
    test_assert(!ecs_has_id(world, e, tag));
    test_assert(!ecs_has_id(world, e, position));

    test_assert(ecs_has_id(world, f, base));
    test_assert(ecs_has_id(world, f, tag));
    test_assert(ecs_has_id(world, f, position));

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_struct(world, derived, 2, 16);
    test_member(world, derived, 0, "name", ecs_id(ecs_string_t), 0);
    test_member(world, derived, 1, "z", ecs_id(ecs_f32_t), 8);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));
    test_struct(world, derived, 3, 12);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, ecs_id(PositionBase)));
    test_struct(world, derived, 3, 12);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "A");
    ecs_entity_t b = ecs_lookup(world, "B");
    ecs_entity_t c = ecs_lookup(world, "C");
    test_assert(ecs_has_pair(world, b, EcsIsA, a));
    test_assert(ecs_has_pair(world, c, EcsIsA, b));

    test_struct(world, a, 1, 4);
    test_struct(world, b, 2, 8);
    test_struct(world, c, 3, 12);
    test_member(world, c, 0, "x", ecs_id(ecs_f32_t), 0);
    test_member(world, c, 1, "y", ecs_id(ecs_f32_t), 4);
    test_member(world, c, 2, "z", ecs_id(ecs_f32_t), 8);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const PositionBase *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 30);
    test_flt(p->y, 30);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t d1 = ecs_lookup(world, "D1");
    ecs_entity_t d2 = ecs_lookup(world, "D2");
    test_struct(world, base, 1, 4);
    test_struct(world, d1, 2, 8);
    test_struct(world, d2, 2, 8);
    test_member(world, d1, 1, "y", ecs_id(ecs_f32_t), 4);
    test_member(world, d2, 1, "z", ecs_id(ecs_f32_t), 4);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const PositionBase *p = ecs_get_id(world, child, position);
    test_flt(p->x, 10);
    test_flt(p->y, 30);

    test_assert(ecs_script_run(world, NULL,
        "e { Derived: {x: 20, z: 30} }", NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t base_mut = ecs_lookup(world, "Base.mut");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base_mut != 0);
    test_assert(derived != 0);
    test_assert(!ecs_has_pair(world, derived, EcsWith, base_mut));
    test_struct(world, derived, 2, 8);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(ecs_has_id(world, e, derived));
    test_assert(!ecs_has_id(world, e, base_mut));

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t derived_mut = ecs_lookup(world, "Derived.mut");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived_mut != 0);
    test_assert(ecs_has_pair(world, derived, EcsWith, derived_mut));
    test_struct(world, derived, 2, 8);

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
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tc = ecs_lookup(world, "flecs.script.typecache");
    test_assert(tc != 0);
    ecs_iter_t it = ecs_children(world, tc);
    int32_t count_before = 0;
    while (ecs_children_next(&it)) {
        count_before += it.count;
    }

    test_assert(ecs_script_run(world, NULL,
        "L b(lanes: 2, lane: [{5}, {6}, {7}, {8}])", NULL) == 0);
    test_assert(ecs_script_run(world, NULL,
        "L c(lanes: 3, lane: [{9}, {10}, {11}, {12}])", NULL) == 0);

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
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
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
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
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
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
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
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
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
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
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
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
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
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_struct(world, derived, 2, 8);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));
    test_struct(world, derived, 2, 8);

    ecs_fini(world);
}

void TemplateInheritance_base_in_module(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module shapes"
    LINE "template Base {"
    LINE "  prop x: f32 = 1"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    expr =
    HEAD "using shapes"
    LINE "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "shapes.Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));
    test_struct(world, derived, 2, 8);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    expr =
    HEAD "template Derived : shapes.Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {x: 10} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    expr =
    HEAD "template Derived : Base {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "e { Derived: {y: 20} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_struct(world, derived, 3, 12);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));
    test_struct(world, derived, 2, 8);

    test_assert(ecs_script_run(world, NULL, "e { Derived: {} }", NULL) == 0);
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

    ecs_entity_t s = ecs_script(world, {
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
    test_struct(world, derived, 2, 8);

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

    ecs_entity_t s = ecs_script(world, {
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

    test_assert(ecs_script_run(world, NULL, "e { Derived: {} }", NULL) == 0);

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

    ecs_script_t *s = ecs_script_parse(world, NULL, expr, NULL, NULL);
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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 110);
    test_flt(p->y, 30);

    ecs_fini(world);
}
