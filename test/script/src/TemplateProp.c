#include <script.h>

typedef struct {
    float x;
    float y;
} PointValue;

#define POINT_TEMPLATE\
    HEAD "struct Position(x: f32, y: f32)"\
    LINE "template Point {"\
    LINE "  prop x: f32 = 1"\
    LINE "  prop y: f32 = 2"\
    LINE "  Position: {$x, $y}"\
    LINE "}"

static void test_point(
    ecs_world_t *world,
    const char *entity,
    float x,
    float y)
{
    ecs_entity_t e = ecs_lookup(world, entity);
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, x);
    test_flt(pt->y, y);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, x);
    test_flt(pos->y, y);
}

void TemplateProp_prop_template_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(point != 0);
    test_assert(foo != 0);

    const EcsStruct *st = ecs_get(world, foo, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "point");
    test_uint(m->type, point);

    const EcsComponent *c = ecs_get(world, foo, EcsComponent);
    test_int(c->size, 8);

    ecs_entity_t e = ecs_lookup(world, "e");
    const PointValue *pt = ecs_get_id(world, e, foo);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    test_assert(!ecs_has_id(world, e, point));

    ecs_fini(world);
}

void TemplateProp_prop_template_type_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "e { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PointValue *pt = ecs_get_id(world, e, foo);
    test_assert(pt != NULL);
    test_flt(pt->x, 1);
    test_flt(pt->y, 2);

    ecs_fini(world);
}

void TemplateProp_prop_template_type_default_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point = {x: 100, y: 200}"
    LINE "}"
    LINE "e { Foo: {} }"
    LINE "f { Foo: {point: {y: 20}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PointValue *pt = ecs_get_id(world, e, foo);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 200);

    ecs_entity_t f = ecs_lookup(world, "f");
    pt = ecs_get_id(world, f, foo);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 20);

    ecs_fini(world);
}

void TemplateProp_prop_template_type_partial_default_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point = {y: 200}"
    LINE "}"
    LINE "e { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PointValue *pt = ecs_get_id(world, e, foo);
    test_assert(pt != NULL);
    test_flt(pt->x, 1);
    test_flt(pt->y, 200);

    ecs_fini(world);
}

void TemplateProp_use_as_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 6);

    ecs_fini(world);
}

#define LIGHT_TEMPLATES\
    HEAD "struct Position(x: f32, y: f32)"\
    LINE "struct StreetLight(on_off: bool)"\
    LINE "template MyStreetLight : StreetLight {"\
    LINE "  prop scale: f32 = 2"\
    LINE "  if $on_off {"\
    LINE "    Position: {$scale, $scale + 1}"\
    LINE "  }"\
    LINE "}"

typedef struct {
    bool on_off;
    float scale;
} MyStreetLightValue;

static void test_light(
    ecs_world_t *world,
    const char *entity,
    bool on_off,
    float scale)
{
    ecs_entity_t e = ecs_lookup(world, entity);
    test_assert(e != 0);
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t interface = ecs_lookup(world, "StreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(light != 0 && interface != 0 && position != 0);

    const MyStreetLightValue *v = ecs_get_id(world, e, light);
    test_assert(v != NULL);
    test_bool(v->on_off, on_off);
    test_flt(v->scale, scale);
    test_assert(!ecs_has_id(world, e, interface));

    const PointValue *p = ecs_get_id(world, e, position);
    if (on_off) {
        test_assert(p != NULL);
        test_flt(p->x, scale);
        test_flt(p->y, scale + 1);
    } else {
        test_assert(p == NULL);
    }
}

void TemplateProp_interface_prop_self_template(void) {
    test_quarantine("1 Sep 2026");

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Iface(on: bool)"
    LINE "template Light : Iface {"
    LINE "  prop inner : template Iface = Light"
    LINE "  child { inner: {on: true} }"
    LINE "}"
    LINE "e { Light: {} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_empty_initializer_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Iface(on: bool)"
    LINE "template Light : Iface {"
    LINE "  Bulb {}"
    LINE "}"
    LINE "template Road {"
    LINE "  prop light : template Iface"
    LINE "  lamp { light: {} }"
    LINE "}"
    LINE "Road r(light: Light)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_in_with_w_initializer_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Iface(on: bool)"
    LINE "template Light : Iface {"
    LINE "  Bulb {}"
    LINE "}"
    LINE "template Road {"
    LINE "  prop light : template Iface"
    LINE "  with light(on: true) {"
    LINE "    lamp {}"
    LINE "  }"
    LINE "}"
    LINE "Road r(light: Light)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_dollar_initializer(void) {
    test_quarantine("1 Sep 2026");

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Iface(on: bool)"
    LINE "template Light : Iface {"
    LINE "  Bulb {}"
    LINE "}"
    LINE "template Road {"
    LINE "  prop light : template Iface"
    LINE "  lamp { $light: {on: true} }"
    LINE "}"
    LINE "Road r(light: Light)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t lamp = ecs_lookup(world, "r.lamp");
    test_assert(lamp != 0);
    test_assert(ecs_lookup(world, "r.lamp.Bulb") != 0);

    ecs_entity_t light = ecs_lookup(world, "Light");
    const bool *v = ecs_get_id(world, lamp, light);
    test_assert(v != NULL);
    test_bool(v[0], true);

    ecs_fini(world);
}

void TemplateProp_interface_prop_invalid_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct StreetLight(on_off: bool)"
    LINE "template Road {"
    LINE "  prop light : template StreetLight"
    LINE "  lamp { light }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL,
        "Road r(light: flecs)", NULL) != 0);
    ecs_log_set_level(-1);

    test_assert(ecs_lookup(world, "r.lamp") == 0);

    ecs_fini(world);
}

void TemplateProp_interface_prop_unrelated_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct StreetLight(on_off: bool)"
    LINE "struct Other(x: f32)"
    LINE "template OtherT : Other {"
    LINE "  Bulb {}"
    LINE "}"
    LINE "template Road {"
    LINE "  prop light : template StreetLight"
    LINE "  lamp { light }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL,
        "Road r(light: OtherT)", NULL) != 0);
    ecs_log_set_level(-1);

    test_assert(ecs_lookup(world, "r.lamp") == 0);

    ecs_fini(world);
}

void TemplateProp_interface_prop_missing_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct StreetLight(on_off: bool)"
    LINE "template Road {"
    LINE "  prop light : template StreetLight"
    LINE "  lamp { light }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, "Road r()", NULL) != 0);
    ecs_log_set_level(-1);

    test_assert(ecs_lookup(world, "r.lamp") == 0);

    ecs_fini(world);
}

void TemplateProp_interface_prop_instantiates_passed_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_light(world, "e.lamp", true, 2);

    ecs_entity_t road = ecs_lookup(world, "Road");
    ecs_entity_t e = ecs_lookup(world, "e");
    const ecs_entity_t *rv = ecs_get_id(world, e, road);
    test_assert(rv != NULL);
    test_assert(rv[0] == ecs_lookup(world, "MyStreetLight"));

    ecs_fini(world);
}

void TemplateProp_interface_prop_paren_syntax(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "Road r(street_light: MyStreetLight)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_light(world, "r.lamp", true, 2);

    ecs_fini(world);
}

void TemplateProp_interface_prop_false_branch(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: false} }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_light(world, "e.lamp", false, 2);

    ecs_fini(world);
}

void TemplateProp_interface_prop_with_other_components(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} Velocity: {10, 20} }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_light(world, "e.lamp", true, 2);

    ecs_entity_t velocity = ecs_lookup(world, "Velocity");
    const PointValue *vel = ecs_get_id(world, ecs_lookup(world, "e.lamp"), velocity);
    test_assert(vel != NULL);
    test_flt(vel->x, 10);
    test_flt(vel->y, 20);

    ecs_fini(world);
}

void TemplateProp_interface_prop_default_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight = MyStreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_light(world, "e.lamp", true, 2);

    ecs_fini(world);
}

void TemplateProp_interface_prop_as_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_light(world, "e.lamp", false, 2);

    ecs_fini(world);
}

void TemplateProp_interface_prop_from_c(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t road = ecs_lookup(world, "Road");
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set_id(world, e, road, sizeof(ecs_entity_t), &light);

    test_light(world, "e.lamp", true, 2);

    ecs_fini(world);
}

void TemplateProp_interface_prop_change_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template OtherStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 5"
    LINE "  if $on_off {"
    LINE "    Position: {100, 200}"
    LINE "  }"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_light(world, "e.lamp", true, 2);

    test_assert(ecs_script_run(world, NULL,
        "e { Road: {street_light: OtherStreetLight} }", NULL) == 0);

    ecs_entity_t lamp = ecs_lookup(world, "e.lamp");
    ecs_entity_t other = ecs_lookup(world, "OtherStreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    const MyStreetLightValue *v = ecs_get_id(world, lamp, other);
    test_assert(v != NULL);
    test_bool(v->on_off, true);
    test_flt(v->scale, 5);
    const PointValue *p = ecs_get_id(world, lamp, position);
    test_assert(p != NULL);
    test_flt(p->x, 100);
    test_flt(p->y, 200);

    ecs_fini(world);
}

void TemplateProp_interface_prop_not_derived_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template Unrelated {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {street_light: Unrelated} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_not_template_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "struct DerivedStruct : StreetLight(z: f32)"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {street_light: DerivedStruct} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_unset_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_unknown_member_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LIGHT_TEMPLATES
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {scale: 3} }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_tag_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { $point }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 6);

    ecs_fini(world);
}

void TemplateProp_use_as_tag_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 1, 2);

    ecs_fini(world);
}

void TemplateProp_use_as_tag_on_instance(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  point"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e", 5, 6);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_partial(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 10, 6);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_partial_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { $point: {y: 20} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 20);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_full(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10, y: 20} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 10, 20);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {10, 20} }"
    LINE "  child2 { point: {10} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 10, 20);
    test_point(world, "e.child2", 10, 6);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  prop offset: f32 = 100"
    LINE "  child { point: {x: $point.x + $offset} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 105, 6);

    ecs_fini(world);
}

void TemplateProp_use_w_empty_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 6);

    ecs_fini(world);
}

void TemplateProp_use_in_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  with point {"
    LINE "    child {}"
    LINE "    child2 {}"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 6);
    test_point(world, "e.child2", 5, 6);

    ecs_fini(world);
}

void TemplateProp_use_in_with_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  with $point {"
    LINE "    child {}"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 6);

    ecs_fini(world);
}

void TemplateProp_use_in_with_w_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  with point(x: 42) {"
    LINE "    child {}"
    LINE "  }"
    LINE "  with point(y: 43) {"
    LINE "    child2 {}"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 42, 6);
    test_point(world, "e.child2", 5, 43);

    ecs_fini(world);
}

void TemplateProp_use_in_with_w_positional_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  with point(42) {"
    LINE "    child {}"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 42, 6);

    ecs_fini(world);
}

void TemplateProp_member_access_in_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  const px = $point.x"
    LINE "  child { Position: {$px, $point.y} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(!ecs_has_id(world, child, point));

    const PointValue *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 5);
    test_flt(p->y, 6);

    ecs_fini(world);
}

void TemplateProp_prop_change_updates_children(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "  child2 { point }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 10, 6);
    test_point(world, "e.child2", 5, 6);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t child2 = ecs_lookup(world, "e.child2");

    test_assert(ecs_script_run(world, NULL,
        "e { Foo: {point: {x: 7, y: 8}} }", NULL) == 0);

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_lookup(world, "e.child2") == child2);
    test_point(world, "e.child", 10, 8);
    test_point(world, "e.child2", 7, 8);

    ecs_fini(world);
}

void TemplateProp_prop_change_from_c_updates_children(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 6);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    PointValue new_value = { 20, 30 };
    ecs_set_id(world, e, foo, sizeof(PointValue), &new_value);

    test_point(world, "e.child", 20, 30);

    ecs_fini(world);
}

void TemplateProp_multiple_template_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop a : template Point"
    LINE "  prop b : template Point"
    LINE "  child_a { a }"
    LINE "  child_b { b }"
    LINE "}"
    LINE "e { Foo: {a: {x: 1, y: 2}, b: {x: 3, y: 4}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    const EcsComponent *c = ecs_get(world, foo, EcsComponent);
    test_int(c->size, 16);

    test_point(world, "e.child_a", 1, 2);
    test_point(world, "e.child_b", 3, 4);

    ecs_fini(world);
}

void TemplateProp_template_prop_w_other_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop count: i32 = 5"
    LINE "  prop point : template Point"
    LINE "  prop name: string = \"foo\""
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {point: {x: 1, y: 2}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    char *str = ecs_ptr_to_expr(world, foo, ecs_get_id(world, e, foo));
    test_str(str, "{count: 5, point: {x: 1, y: 2}, name: \"foo\"}");
    ecs_os_free(str);

    test_point(world, "e.child", 1, 2);

    ecs_fini(world);
}

void TemplateProp_template_prop_w_string_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Label(text: string)"
    LINE "template Named {"
    LINE "  prop name: string = \"default\""
    LINE "  Label: {$name}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop named : template Named"
    LINE "  child { named }"
    LINE "  child2 { named: {name: \"other\"} }"
    LINE "}"
    LINE "e { Foo: {named: {name: \"hello\"}} }"
    LINE "f { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t label = ecs_lookup(world, "Label");
    ecs_entity_t named = ecs_lookup(world, "Named");

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_str(*(char**)ecs_get_id(world, child, label), "hello");
    test_str(*(char**)ecs_get_id(world, child, named), "hello");

    child = ecs_lookup(world, "e.child2");
    test_assert(child != 0);
    test_str(*(char**)ecs_get_id(world, child, label), "other");

    child = ecs_lookup(world, "f.child");
    test_assert(child != 0);
    test_str(*(char**)ecs_get_id(world, child, label), "default");

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    char *str = ecs_ptr_to_expr(world, foo, ecs_get_id(world, e, foo));
    test_str(str, "{named: {name: \"hello\"}}");
    ecs_os_free(str);

    ecs_delete(world, e);
    ecs_delete(world, ecs_lookup(world, "f"));

    ecs_fini(world);
}

void TemplateProp_inherited_template_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "template Bar : Foo {"
    LINE "  prop z: f32 = 3"
    LINE "  child { point: {y: $z} }"
    LINE "  child2 { point }"
    LINE "}"
    LINE "e { Bar: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 3);
    test_point(world, "e.child2", 5, 6);

    ecs_fini(world);
}

void TemplateProp_inherited_template_prop_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "template Bar : Foo {"
    LINE "  prop z: f32 = 3"
    LINE "  child { $point: {y: $z} }"
    LINE "  child2 { $point }"
    LINE "}"
    LINE "e { Bar: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 3);
    test_point(world, "e.child2", 5, 6);

    ecs_fini(world);
}

void TemplateProp_inherited_template_prop_chain(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "template Bar : Foo {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Baz : Bar {"
    LINE "  child { point: {x: $z} }"
    LINE "}"
    LINE "e { Baz: {point: {x: 5, y: 6}, z: 30} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 30, 6);

    ecs_fini(world);
}

void TemplateProp_prop_of_derived_template_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Point3D : Point {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point3D"
    LINE "  child { point: {z: 30} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t point3d = ecs_lookup(world, "Point3D");
    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(!ecs_has_id(world, child, point));

    const float *v = ecs_get_id(world, child, point3d);
    test_assert(v != NULL);
    test_flt(v[0], 5);
    test_flt(v[1], 6);
    test_flt(v[2], 30);

    ecs_fini(world);
}

void TemplateProp_nested_template_instantiation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "template Bar {"
    LINE "  prop x: f32 = 7"
    LINE "  foo { Foo: {point: {x: $x, y: 8}} }"
    LINE "}"
    LINE "e { Bar: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.foo.child", 7, 8);

    ecs_fini(world);
}

void TemplateProp_template_prop_in_for_loop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  for i in 0..3 {"
    LINE "    \"child_$i\" { point: {x: $i} }"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child_0", 0, 6);
    test_point(world, "e.child_1", 1, 6);
    test_point(world, "e.child_2", 2, 6);

    ecs_fini(world);
}

void TemplateProp_template_prop_in_if(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  prop flag: bool = true"
    LINE "  if $flag {"
    LINE "    child { point }"
    LINE "  } else {"
    LINE "    child { point: {x: 100} }"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }"
    LINE "f { Foo: {point: {x: 5, y: 6}, flag: false} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 5, 6);
    test_point(world, "f.child", 100, 6);

    ecs_fini(world);
}

void TemplateProp_use_wo_template_keyword(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : Point"
    LINE "  child { point }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_prop_wo_template_keyword_as_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : Point"
    LINE "  child { Position: {$point.x, $point.y} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    const PointValue *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 5);
    test_flt(p->y, 6);

    ecs_fini(world);
}

void TemplateProp_type_not_a_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "Tag {}"
    LINE "template Foo {"
    LINE "  prop point : template Tag"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_type_is_primitive(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop point : template f32"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_type_unresolved(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop point : template Point"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_type_is_self(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop point : template Foo"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_mut_template_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  mut point : template Point = {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_const_template_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "const point : template Point = {}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_missing_type_name(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_pair_first(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { (point, Position) }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_pair_second(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { (Position, point) }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_pair_first_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { ($point, Position) }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_entity_kind(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  point child"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_w_wrong_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {z: 10} }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_prop_template_type_wo_default_required(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.child", 1, 2);

    ecs_fini(world);
}

void TemplateProp_run_script_twice(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_script_run(world, NULL,
        "e { Foo: {point: {x: 5, y: 6}} }", NULL) == 0);
    test_point(world, "e.child", 10, 6);

    ecs_fini(world);
}

void TemplateProp_managed_script_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);
    test_point(world, "e.child", 10, 6);

    test_assert(ecs_script_update(world, s, 0, expr) == 0);
    test_point(world, "e.child", 10, 6);

    expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 20} }"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_update(world, s, 0, expr) == 0);
    test_point(world, "e.child", 20, 6);

    ecs_fini(world);
}

void TemplateProp_ast_to_str(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "}";

    ecs_script_t *s = ecs_script_parse(world, NULL, expr, NULL, NULL);
    test_assert(s != NULL);

    char *str = ecs_script_ast_to_str(s, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "point : template Point") != NULL);
    ecs_os_free(str);

    ecs_script_free(s);

    ecs_fini(world);
}

void TemplateProp_instance_to_json(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "e { Foo: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    char *json = ecs_entity_to_json(world, e, &(ecs_entity_to_json_desc_t){
        .serialize_values = true
    });
    test_assert(json != NULL);
    test_assert(strstr(json, "\"Foo\":{\"point\":{\"x\":5, \"y\":6}}") != NULL);
    ecs_os_free(json);

    ecs_fini(world);
}


#define POINT3D_TEMPLATE\
    LINE "template Point3D : Point {"\
    LINE "  prop z: f32 = 3"\
    LINE "}"

#define INNER_TEMPLATE\
    LINE "template Inner {"\
    LINE "  prop point : template Point"\
    LINE "  leaf { point }"\
    LINE "  leaf2 { point: {x: 100} }"\
    LINE "}"

void TemplateProp_pass_to_child_template_same_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t inner = ecs_lookup(world, "Inner");
    ecs_entity_t e_inner = ecs_lookup(world, "e.inner");
    test_assert(e_inner != 0);
    const PointValue *pt = ecs_get_id(world, e_inner, inner);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    test_point(world, "e.inner.leaf", 5, 6);
    test_point(world, "e.inner.leaf2", 100, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_same_type_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { $point }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.inner.leaf", 5, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {$point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.inner.leaf", 5, 6);
    test_point(world, "e.inner.leaf2", 100, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_w_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: {x: $point.x + 1, y: $point.y * 2}} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.inner.leaf", 6, 12);
    test_point(world, "e.inner.leaf2", 100, 12);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_w_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  with Inner(point: $point) {"
    LINE "    inner {}"
    LINE "  }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.inner.leaf", 5, 6);
    test_point(world, "e.inner.leaf2", 100, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.inner.leaf", 1, 2);
    test_point(world, "e.inner.leaf2", 100, 2);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.inner.leaf", 5, 6);

    ecs_entity_t inner = ecs_lookup(world, "e.inner");
    ecs_entity_t leaf = ecs_lookup(world, "e.inner.leaf");

    test_assert(ecs_script_run(world, NULL,
        "e { Outer: {point: {x: 7, y: 8}} }", NULL) == 0);

    test_assert(ecs_lookup(world, "e.inner") == inner);
    test_assert(ecs_lookup(world, "e.inner.leaf") == leaf);
    test_point(world, "e.inner.leaf", 7, 8);
    test_point(world, "e.inner.leaf2", 100, 8);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_from_c(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t outer = ecs_lookup(world, "Outer");
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    PointValue value = { 5, 6 };
    ecs_set_id(world, e, outer, sizeof(PointValue), &value);

    test_point(world, "e.inner.leaf", 5, 6);
    test_point(world, "e.inner.leaf2", 100, 6);

    value.x = 7;
    value.y = 8;
    ecs_set_id(world, e, outer, sizeof(PointValue), &value);

    test_point(world, "e.inner.leaf", 7, 8);
    test_point(world, "e.inner.leaf2", 100, 8);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_two_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Middle {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  middle { Middle: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.middle.inner.leaf", 5, 6);
    test_point(world, "e.middle.inner.leaf2", 100, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_and_use_in_outer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  point: {y: 50}"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e", 5, 50);
    test_point(world, "e.inner.leaf", 5, 6);

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    POINT3D_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6, z: 7}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t inner = ecs_lookup(world, "Inner");
    ecs_entity_t point3d = ecs_lookup(world, "Point3D");
    ecs_entity_t outer = ecs_lookup(world, "Outer");

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *ov = ecs_get_id(world, e, outer);
    test_assert(ov != NULL);
    test_flt(ov[0], 5);
    test_flt(ov[1], 6);
    test_flt(ov[2], 7);

    ecs_entity_t e_inner = ecs_lookup(world, "e.inner");
    const PointValue *pt = ecs_get_id(world, e_inner, inner);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    ecs_entity_t leaf = ecs_lookup(world, "e.inner.leaf");
    test_assert(!ecs_has_id(world, leaf, point3d));
    test_point(world, "e.inner.leaf", 5, 6);
    test_point(world, "e.inner.leaf2", 100, 6);

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    POINT3D_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  inner { Inner: {$point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6, z: 7}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.inner.leaf", 5, 6);
    test_point(world, "e.inner.leaf2", 100, 6);

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base_w_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    POINT3D_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  with Inner(point: $point) {"
    LINE "    inner {}"
    LINE "  }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6, z: 7}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.inner.leaf", 5, 6);
    test_point(world, "e.inner.leaf2", 100, 6);

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    POINT3D_TEMPLATE
    INNER_TEMPLATE
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6, z: 7}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t point3d = ecs_lookup(world, "Point3D");
    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, point3d);
    test_assert(v != NULL);
    test_flt(v[0], 5);
    test_flt(v[1], 6);
    test_flt(v[2], 7);
    test_point(world, "e.inner.leaf", 5, 6);

    ecs_entity_t leaf = ecs_lookup(world, "e.inner.leaf");

    test_assert(ecs_script_run(world, NULL,
        "e { Outer: {point: {x: 8, y: 9, z: 10}} }", NULL) == 0);

    v = ecs_get_id(world, e, point3d);
    test_flt(v[0], 8);
    test_flt(v[1], 9);
    test_flt(v[2], 10);
    test_assert(ecs_lookup(world, "e.inner.leaf") == leaf);
    test_point(world, "e.inner.leaf", 8, 9);
    test_point(world, "e.inner.leaf2", 100, 9);

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    POINT3D_TEMPLATE
    LINE "template Inner3D {"
    LINE "  prop point : template Point3D"
    LINE "  leaf { point }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  inner { Inner3D: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6, z: 7}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t point3d = ecs_lookup(world, "Point3D");
    ecs_entity_t leaf = ecs_lookup(world, "e.inner.leaf");
    test_assert(leaf != 0);
    const float *v = ecs_get_id(world, leaf, point3d);
    test_assert(v != NULL);
    test_flt(v[0], 5);
    test_flt(v[1], 6);
    test_flt(v[2], 7);
    test_assert(!ecs_has_id(world, leaf, ecs_lookup(world, "Point")));

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base_two_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    POINT3D_TEMPLATE
    INNER_TEMPLATE
    LINE "template Middle {"
    LINE "  prop point : template Point3D"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  middle { Middle: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: {x: 5, y: 6, z: 7}} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_point(world, "e.middle.inner.leaf", 5, 6);
    test_point(world, "e.middle.inner.leaf2", 100, 6);

    ecs_fini(world);
}

void TemplateProp_pass_base_to_child_template_derived_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    POINT3D_TEMPLATE
    LINE "template Inner3D {"
    LINE "  prop point : template Point3D"
    LINE "  leaf { point }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner3D: {point: $point} }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_pass_unrelated_to_child_template_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    POINT_TEMPLATE
    INNER_TEMPLATE
    LINE "template Other {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop other : template Other"
    LINE "  inner { Inner: {point: $other} }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_bool_prop_mul_flt_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Light {"
    LINE "  prop on_off: bool = true"
    LINE "  Position: {6 * $on_off, 1}"
    LINE "}"
    LINE "on { Light: {on_off: true} }"
    LINE "off { Light: {on_off: false} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    ecs_entity_t on = ecs_lookup(world, "on");
    test_assert(on != 0);
    const PointValue *on_pos = ecs_get_id(world, on, position);
    test_assert(on_pos != NULL);
    test_flt(on_pos->x, 6);
    test_flt(on_pos->y, 1);

    ecs_entity_t off = ecs_lookup(world, "off");
    test_assert(off != 0);
    const PointValue *off_pos = ecs_get_id(world, off, position);
    test_assert(off_pos != NULL);
    test_flt(off_pos->x, 0);
    test_flt(off_pos->y, 1);

    ecs_fini(world);
}

void TemplateProp_bool_prop_in_flt_member_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Light {"
    LINE "  prop on_off: bool = true"
    LINE "  Position: {1, $on_off}"
    LINE "}"
    LINE "on { Light: {on_off: true} }"
    LINE "off { Light: {on_off: false} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    ecs_entity_t on = ecs_lookup(world, "on");
    test_assert(on != 0);
    const PointValue *on_pos = ecs_get_id(world, on, position);
    test_assert(on_pos != NULL);
    test_flt(on_pos->x, 1);
    test_flt(on_pos->y, 1);

    ecs_entity_t off = ecs_lookup(world, "off");
    test_assert(off != 0);
    const PointValue *off_pos = ecs_get_id(world, off, position);
    test_assert(off_pos != NULL);
    test_flt(off_pos->x, 1);
    test_flt(off_pos->y, 0);

    ecs_fini(world);
}

void TemplateProp_bool_prop_mul_flt_member_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Light {"
    LINE "  prop on_off: bool = true"
    LINE "  const strength = 6 * $on_off"
    LINE "  Position: {$strength, 1}"
    LINE "}"
    LINE "on { Light: {on_off: true} }"
    LINE "off { Light: {on_off: false} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    ecs_entity_t on = ecs_lookup(world, "on");
    test_assert(on != 0);
    const PointValue *on_pos = ecs_get_id(world, on, position);
    test_assert(on_pos != NULL);
    test_flt(on_pos->x, 6);

    ecs_entity_t off = ecs_lookup(world, "off");
    test_assert(off != 0);
    const PointValue *off_pos = ecs_get_id(world, off, position);
    test_assert(off_pos != NULL);
    test_flt(off_pos->x, 0);

    ecs_fini(world);
}
