#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void TemplateProp_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

typedef struct {
    float x;
    float y;
} PointValue;

static void set_point_ref(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t component,
    float x,
    float y)
{
    ecs_entity_t point = ecs_lookup(world, "Point");
    test_assert(point != 0);
    PointValue *value = ecs_ptr_new(world, point);
    value->x = x;
    value->y = y;
    ecs_script_template_ref_t ref = { .type = point, .value = value };
    ecs_set_id(world, e, component, sizeof(ref), &ref);
    ecs_ptr_free(world, point, value);
}

void TemplateProp_prop_template_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(point != 0);
    test_assert(foo != 0);

    const EcsStruct *st = ecs_get(world, foo, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "point");
    test_uint(m->type, ecs_id(ecs_script_template_ref_t));

    const EcsComponent *c = ecs_get(world, foo, EcsComponent);
    test_int(c->size, ECS_SIZEOF(ecs_script_template_ref_t));

    ecs_entity_t e = ecs_lookup(world, "e");
    const ecs_script_template_ref_t *ref = ecs_get_id(world, e, foo);
    test_assert(ref != NULL);
    test_uint(ref->type, point);
    test_assert(ref->value != NULL);
    const PointValue *pt = ref->value;
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    test_assert(!ecs_has_id(world, e, point));

    ecs_fini(world);
}

void TemplateProp_prop_template_type_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "e { Foo: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    const ecs_script_template_ref_t *ref = ecs_get_id(world, e, foo);
    test_assert(ref != NULL);
    test_uint(ref->type, ecs_lookup(world, "Point"));
    test_assert(ref->value == NULL);

    ecs_fini(world);
}

void TemplateProp_use_as_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

typedef struct {
    bool on_off;
    float scale;
} MyStreetLightValue;

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
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_in_with_w_initializer(void) {
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

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t lamp = ecs_lookup(world, "r.lamp");
    ecs_entity_t light = ecs_lookup(world, "Light");
    test_assert(lamp != 0);
    test_assert(light != 0);
    test_assert(ecs_has_id(world, lamp, light));

    const bool *v = ecs_get_id(world, lamp, light);
    test_assert(v != NULL);
    test_bool(*v, true);

    test_assert(ecs_lookup(world, "r.lamp.Bulb") != 0);

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

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

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

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        "Road r(light: flecs)", &ir_desc, NULL) != 0);
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

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        "Road r(light: OtherT)", &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    test_assert(ecs_lookup(world, "r.lamp") == 0);

    ecs_fini(world);
}

void TemplateProp_interface_prop_missing_value_defaults_to_interface(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct StreetLight(on_off: bool)"
    LINE "template Road {"
    LINE "  prop light : template StreetLight"
    LINE "  lamp { light }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);
    test_assert(ecs_script_run_w_desc(world, NULL, "Road r()", &ir_desc, NULL) == 0);

    ecs_entity_t lamp = ecs_lookup(world, "r.lamp");
    ecs_entity_t street_light = ecs_lookup(world, "StreetLight");
    test_assert(lamp != 0);
    test_assert(street_light != 0);

    const bool *on_off = ecs_get_id(world, lamp, street_light);
    test_assert(on_off != NULL);
    test_bool(*on_off, false);

    ecs_fini(world);
}

void TemplateProp_interface_prop_instantiates_passed_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t inst = ecs_lookup(world, "e.lamp");
    test_assert(inst != 0);
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t interface = ecs_lookup(world, "StreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(light != 0 && interface != 0 && position != 0);

    const MyStreetLightValue *v = ecs_get_id(world, inst, light);
    test_assert(v != NULL);
    test_bool(v->on_off, true);
    test_flt(v->scale, 2);
    test_assert(ecs_has_id(world, inst, interface));

    const PointValue *p = ecs_get_id(world, inst, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    test_flt(p->y, 2 + 1);

    ecs_entity_t road = ecs_lookup(world, "Road");
    ecs_entity_t e = ecs_lookup(world, "e");
    const ecs_script_template_ref_t *rv = ecs_get_id(world, e, road);
    test_assert(rv != NULL);
    test_assert(rv->type == ecs_lookup(world, "MyStreetLight"));
    test_assert(rv->value == NULL);

    ecs_fini(world);
}

void TemplateProp_interface_prop_paren_syntax(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "Road r(street_light: MyStreetLight)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "r.lamp");
    test_assert(e != 0);
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t interface = ecs_lookup(world, "StreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(light != 0 && interface != 0 && position != 0);

    const MyStreetLightValue *v = ecs_get_id(world, e, light);
    test_assert(v != NULL);
    test_bool(v->on_off, true);
    test_flt(v->scale, 2);
    test_assert(ecs_has_id(world, e, interface));

    const PointValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    test_flt(p->y, 2 + 1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_false_branch(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: false} }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.lamp");
    test_assert(e != 0);
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t interface = ecs_lookup(world, "StreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(light != 0 && interface != 0 && position != 0);

    const MyStreetLightValue *v = ecs_get_id(world, e, light);
    test_assert(v != NULL);
    test_bool(v->on_off, false);
    test_flt(v->scale, 2);
    test_assert(ecs_has_id(world, e, interface));

    const PointValue *p = ecs_get_id(world, e, position);
    test_assert(p == NULL);

    ecs_fini(world);
}

void TemplateProp_interface_prop_with_other_components(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "struct Velocity(x: f32, y: f32)"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true}; Velocity: {10, 20} }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.lamp");
    test_assert(e != 0);
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t interface = ecs_lookup(world, "StreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(light != 0 && interface != 0 && position != 0);

    const MyStreetLightValue *v = ecs_get_id(world, e, light);
    test_assert(v != NULL);
    test_bool(v->on_off, true);
    test_flt(v->scale, 2);
    test_assert(ecs_has_id(world, e, interface));

    const PointValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    test_flt(p->y, 2 + 1);

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
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight = MyStreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.lamp");
    test_assert(e != 0);
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t interface = ecs_lookup(world, "StreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(light != 0 && interface != 0 && position != 0);

    const MyStreetLightValue *v = ecs_get_id(world, e, light);
    test_assert(v != NULL);
    test_bool(v->on_off, true);
    test_flt(v->scale, 2);
    test_assert(ecs_has_id(world, e, interface));

    const PointValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    test_flt(p->y, 2 + 1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_default_overridden(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template OtherStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 7"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight = MyStreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {street_light: OtherStreetLight} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t lamp = ecs_lookup(world, "e.lamp");
    test_assert(lamp != 0);

    ecs_entity_t other = ecs_lookup(world, "OtherStreetLight");
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    test_assert(other != 0 && light != 0);

    const MyStreetLightValue *v = ecs_get_id(world, lamp, other);
    test_assert(v != NULL);
    test_bool(v->on_off, true);
    test_flt(v->scale, 7);

    test_assert(!ecs_has_id(world, lamp, light));

    ecs_fini(world);
}

void TemplateProp_interface_prop_default_overridden_from_c(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template OtherStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 7"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight = MyStreetLight"
    LINE "  prop width: f32 = 3"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t road = ecs_lookup(world, "Road");
    ecs_entity_t other = ecs_lookup(world, "OtherStreetLight");
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    test_assert(road != 0 && other != 0 && light != 0);

    const ecs_type_info_t *ti = ecs_get_type_info(world, road);
    test_assert(ti != NULL);

    void *value = ecs_os_calloc(ti->size);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, road, value);
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "street_light"), 0);
    test_int(ecs_meta_set_entity(&cur, other), 0);
    test_int(ecs_meta_member(&cur, "width"), 0);
    test_int(ecs_meta_set_float(&cur, 5), 0);
    test_int(ecs_meta_pop(&cur), 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set_id(world, e, road, (size_t)ti->size, value);
    ecs_os_free(value);

    ecs_entity_t lamp = ecs_lookup(world, "e.lamp");
    test_assert(lamp != 0);

    const MyStreetLightValue *v = ecs_get_id(world, lamp, other);
    test_assert(v != NULL);
    test_bool(v->on_off, true);
    test_flt(v->scale, 7);

    test_assert(!ecs_has_id(world, lamp, light));

    ecs_fini(world);
}

void TemplateProp_interface_prop_as_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.lamp");
    test_assert(e != 0);
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t interface = ecs_lookup(world, "StreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(light != 0 && interface != 0 && position != 0);

    const MyStreetLightValue *v = ecs_get_id(world, e, light);
    test_assert(v != NULL);
    test_bool(v->on_off, false);
    test_flt(v->scale, 2);
    test_assert(ecs_has_id(world, e, interface));

    const PointValue *p = ecs_get_id(world, e, position);
    test_assert(p == NULL);

    ecs_fini(world);
}

void TemplateProp_interface_prop_from_c(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t road = ecs_lookup(world, "Road");
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_script_template_ref_t ref = { .type = light };
    ecs_set_id(world, e, road, sizeof(ecs_script_template_ref_t), &ref);

    ecs_entity_t inst = ecs_lookup(world, "e.lamp");
    test_assert(inst != 0);
    ecs_entity_t light_1 = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t interface = ecs_lookup(world, "StreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(light_1 != 0 && interface != 0 && position != 0);

    const MyStreetLightValue *v = ecs_get_id(world, inst, light_1);
    test_assert(v != NULL);
    test_bool(v->on_off, true);
    test_flt(v->scale, 2);
    test_assert(ecs_has_id(world, inst, interface));

    const PointValue *p = ecs_get_id(world, inst, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    test_flt(p->y, 2 + 1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_change_template(void) {
    test_quarantine("10 Sep 2026");

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
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

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.lamp");
    test_assert(e != 0);
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t interface = ecs_lookup(world, "StreetLight");
    ecs_entity_t pos_id = ecs_lookup(world, "Position");
    test_assert(light != 0 && interface != 0 && pos_id != 0);

    const MyStreetLightValue *val = ecs_get_id(world, e, light);
    test_assert(val != NULL);
    test_bool(val->on_off, true);
    test_flt(val->scale, 2);
    test_assert(!ecs_has_id(world, e, interface));

    const PointValue *pos = ecs_get_id(world, e, pos_id);
    test_assert(pos != NULL);
    test_flt(pos->x, 2);
    test_flt(pos->y, 2 + 1);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Road: {street_light: OtherStreetLight} }", &ir_desc, NULL) == 0);

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
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template Unrelated {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {street_light: Unrelated} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_not_template_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "struct DerivedStruct : StreetLight(z: f32)"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {street_light: DerivedStruct} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_unset_defaults_to_interface(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {on_off: true} }"
    LINE "}"
    LINE "e { Road: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t lamp = ecs_lookup(world, "e.lamp");
    ecs_entity_t street_light = ecs_lookup(world, "StreetLight");
    ecs_entity_t my_street_light = ecs_lookup(world, "MyStreetLight");
    test_assert(lamp != 0);
    test_assert(street_light != 0);
    test_assert(my_street_light != 0);

    const bool *on_off = ecs_get_id(world, lamp, street_light);
    test_assert(on_off != NULL);
    test_bool(*on_off, true);
    test_assert(!ecs_has_id(world, lamp, my_street_light));

    ecs_fini(world);
}

void TemplateProp_interface_prop_unknown_member_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  if $on_off {"
    LINE "    Position: {$scale, $scale + 1}"
    LINE "  }"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light: {scale: 3} }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_tag_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { $point }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_use_as_tag_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 1);
    test_flt(pt->y, 2);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 1);
    test_flt(pos->y, 2);

    ecs_fini(world);
}

void TemplateProp_use_as_tag_on_instance(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  point"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_partial(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 10);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 10);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_partial_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { $point: {y: 20} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 20);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 20);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_full(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10, y: 20} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 10);
    test_flt(pt->y, 20);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 10);
    test_flt(pos->y, 20);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {10, 20} }"
    LINE "  child2 { point: {10} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 10);
    test_flt(pt->y, 20);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 10);
    test_flt(pos->y, 20);

    e = ecs_lookup(world, "e.child2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 10);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 10);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_use_w_initializer_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  prop offset: f32 = 100"
    LINE "  child { point: {x: $point.x + $offset} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 105);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 105);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_use_w_empty_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_use_in_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  with point {"
    LINE "    child {}"
    LINE "    child2 {}"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "e.child2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_use_in_with_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  with $point {"
    LINE "    child {}"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_use_in_with_w_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  with point(x: 42) {"
    LINE "    child {}"
    LINE "  }"
    LINE "  with point(y: 43) {"
    LINE "    child2 {}"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 42);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 42);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "e.child2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 43);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 43);

    ecs_fini(world);
}

void TemplateProp_use_in_with_w_positional_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  with point(42) {"
    LINE "    child {}"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 42);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 42);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_member_access_in_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  const px = $point.x"
    LINE "  child { Position: {$px, $point.y} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

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

void TemplateProp_member_access_in_binary_expr_w_loop_var(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "interface {"
    LINE "  template Point {"
    LINE "    prop x: f32 = 4"
    LINE "    prop y: f32 = 2"
    LINE "  }"
    LINE "}"
    LINE "template Point : interface.Point {"
    LINE "  Position: {x, y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template interface.Point"
    LINE "  for i in 1..2 {"
    LINE "    child { Position: {point.x + (i - 1), point.y + (i - 1)} }"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: Point} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const PointValue *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_flt(p->x, 4);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateProp_prop_change_updates_children(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "  child2 { point }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 10);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 10);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "e.child2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t child2 = ecs_lookup(world, "e.child2");

    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Foo: {point: Point(x: 7, y: 8)} }", &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_lookup(world, "e.child2") == child2);

    e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 10);
    test_flt(pt->y, 8);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 10);
    test_flt(pos->y, 8);

    e = ecs_lookup(world, "e.child2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 7);
    test_flt(pt->y, 8);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 7);
    test_flt(pos->y, 8);

    ecs_fini(world);
}

void TemplateProp_prop_change_from_c_updates_children(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t inst = ecs_lookup(world, "e.child");
    test_assert(inst != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, inst, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, inst, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    set_point_ref(world, e, foo, 20, 30);

    inst = ecs_lookup(world, "e.child");
    test_assert(inst != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, inst, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 20);
    test_flt(pt->y, 30);

    pos = ecs_get_id(world, inst, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 20);
    test_flt(pos->y, 30);

    ecs_fini(world);
}

void TemplateProp_multiple_template_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop a : template Point"
    LINE "  prop b : template Point"
    LINE "  child_a { a }"
    LINE "  child_b { b }"
    LINE "}"
    LINE "e { Foo: {a: Point(x: 1, y: 2), b: Point(x: 3, y: 4)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    const EcsComponent *c = ecs_get(world, foo, EcsComponent);
    test_int(c->size, 2 * ECS_SIZEOF(ecs_script_template_ref_t));

    ecs_entity_t e = ecs_lookup(world, "e.child_a");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 1);
    test_flt(pt->y, 2);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 1);
    test_flt(pos->y, 2);

    e = ecs_lookup(world, "e.child_b");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 3);
    test_flt(pt->y, 4);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 3);
    test_flt(pos->y, 4);

    ecs_fini(world);
}

void TemplateProp_template_prop_w_other_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop count: i32 = 5"
    LINE "  prop point : template Point"
    LINE "  prop name: string = \"foo\""
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 1, y: 2)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    char *str = ecs_ptr_to_expr(world, foo, ecs_get_id(world, e, foo));
    test_str(str, "{count: 5, point: Point, name: \"foo\"}");
    ecs_os_free(str);

    ecs_entity_t inst = ecs_lookup(world, "e.child");
    test_assert(inst != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, inst, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 1);
    test_flt(pt->y, 2);

    const PointValue *pos = ecs_get_id(world, inst, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 1);
    test_flt(pos->y, 2);

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
    LINE "e { Foo: {named: Named(name: \"hello\")} }"
    LINE "f { Foo: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

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
    test_str(str, "{named: Named}");
    ecs_os_free(str);

    ecs_delete(world, e);
    ecs_delete(world, ecs_lookup(world, "f"));

    ecs_fini(world);
}

void TemplateProp_inherited_template_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "template Bar : Foo {"
    LINE "  prop z: f32 = 3"
    LINE "  child { point: {y: $z} }"
    LINE "  child2 { point }"
    LINE "}"
    LINE "e { Bar: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 3);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 3);

    e = ecs_lookup(world, "e.child2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_inherited_template_prop_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "template Bar : Foo {"
    LINE "  prop z: f32 = 3"
    LINE "  child { $point: {y: $z} }"
    LINE "  child2 { $point }"
    LINE "}"
    LINE "e { Bar: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 3);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 3);

    e = ecs_lookup(world, "e.child2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_inherited_template_prop_chain(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "template Bar : Foo {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Baz : Bar {"
    LINE "  child { point: {x: $z} }"
    LINE "}"
    LINE "e { Baz: {point: Point(x: 5, y: 6), z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 30);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 30);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_prop_of_derived_template_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Point3D : Point {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point3D"
    LINE "  child { point: {z: 30} }"
    LINE "}"
    LINE "e { Foo: {point: Point3D(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t point3d = ecs_lookup(world, "Point3D");
    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_has_id(world, child, point));

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
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "template Bar {"
    LINE "  prop x: f32 = 7"
    LINE "  foo { Foo: {point: Point(x: $x, y: 8)} }"
    LINE "}"
    LINE "e { Bar: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.foo.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 7);
    test_flt(pt->y, 8);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 7);
    test_flt(pos->y, 8);

    ecs_fini(world);
}

void TemplateProp_template_prop_in_for_loop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  for i in 0..3 {"
    LINE "    \"child_$i\" { point: {x: $i} }"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child_0");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 0);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 0);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "e.child_1");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 1);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 1);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "e.child_2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 2);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 2);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_template_prop_in_if(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  prop flag: bool = true"
    LINE "  if $flag {"
    LINE "    child { point }"
    LINE "  } else {"
    LINE "    child { point: {x: 100} }"
    LINE "  }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }"
    LINE "f { Foo: {point: Point(x: 5, y: 6), flag: false} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "f.child");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_use_wo_template_keyword(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : Point"
    LINE "  child { point }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_prop_wo_template_keyword_as_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : Point"
    LINE "  child { Position: {$point.x, $point.y} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

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
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "Tag {}"
    LINE "template Foo {"
    LINE "  prop point : template Tag"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
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
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
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
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
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
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_mut_template_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  mut point : template Point = {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_const_template_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "const point : template Point = {}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_missing_type_name(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template 5"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_pair_first(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { (point, Position) }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_pair_second(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { (Position, point) }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_pair_first_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { ($point, Position) }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_as_entity_kind(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  point child"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_use_w_wrong_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {z: 10} }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_prop_template_type_wo_default_required(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point }"
    LINE "}"
    LINE "e { Foo: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 1);
    test_flt(pt->y, 2);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 1);
    test_flt(pos->y, 2);

    ecs_fini(world);
}

void TemplateProp_run_script_twice(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Foo: {point: Point(x: 5, y: 6)} }", &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 10);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 10);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_managed_script_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled,
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    ecs_entity_t e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 10);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 10);
    test_flt(pos->y, 6);

    test_assert(ecs_script_update(world, s, 0, expr) == 0);

    e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 10);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 10);
    test_flt(pos->y, 6);

    expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 20} }"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_update(world, s, 0, expr) == 0);

    e = ecs_lookup(world, "e.child");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 20);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 20);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_ast_to_str(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "  child { point: {x: 10} }"
    LINE "}";

    ecs_script_t *s = ecs_script_parse(world, NULL, expr, &ir_desc, NULL);
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
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop point : template Point"
    LINE "}"
    LINE "e { Foo: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    char *json = ecs_entity_to_json(world, e, &(ecs_entity_to_json_desc_t){
        .serialize_values = true
    });
    test_assert(json != NULL);
    test_assert(strstr(json, "\"Foo\":{\"point\":\"Point\"}") != NULL);
    ecs_os_free(json);

    ecs_fini(world);
}


void TemplateProp_pass_to_child_template_same_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t inner = ecs_lookup(world, "Inner");
    ecs_entity_t e_inner = ecs_lookup(world, "e.inner");
    test_assert(e_inner != 0);
    const ecs_script_template_ref_t *ref = ecs_get_id(world, e_inner, inner);
    test_assert(ref != NULL);
    test_uint(ref->type, ecs_lookup(world, "Point"));
    const PointValue *pt = ref->value;
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    ecs_entity_t e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt_1 = ecs_get_id(world, e, point);
    test_assert(pt_1 != NULL);
    test_flt(pt_1->x, 5);
    test_flt(pt_1->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "e.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt_1 = ecs_get_id(world, e, point);
    test_assert(pt_1 != NULL);
    test_flt(pt_1->x, 100);
    test_flt(pt_1->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_same_type_dollar(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { $point }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {$point} }"
    LINE "}"
    LINE "e { Outer: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "e.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_w_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: Point(x: $point.x + 1, y: $point.y * 2)} }"
    LINE "}"
    LINE "e { Outer: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 6);
    test_flt(pt->y, 12);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 6);
    test_flt(pos->y, 12);

    e = ecs_lookup(world, "e.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 12);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 12);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_w_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  with Inner(point: $point) {"
    LINE "    inner {}"
    LINE "  }"
    LINE "}"
    LINE "e { Outer: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "e.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 1);
    test_flt(pt->y, 2);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 1);
    test_flt(pos->y, 2);

    e = ecs_lookup(world, "e.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 2);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 2);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_entity_t inner = ecs_lookup(world, "e.inner");
    ecs_entity_t leaf = ecs_lookup(world, "e.inner.leaf");

    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Outer: {point: Point(x: 7, y: 8)} }", &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "e.inner") == inner);
    test_assert(ecs_lookup(world, "e.inner.leaf") == leaf);

    e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 7);
    test_flt(pt->y, 8);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 7);
    test_flt(pos->y, 8);

    e = ecs_lookup(world, "e.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 8);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 8);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_from_c(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t outer = ecs_lookup(world, "Outer");
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    set_point_ref(world, e, outer, 5, 6);

    ecs_entity_t inst = ecs_lookup(world, "e.inner.leaf");
    test_assert(inst != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, inst, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, inst, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    inst = ecs_lookup(world, "e.inner.leaf2");
    test_assert(inst != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, inst, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, inst, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 6);

    set_point_ref(world, e, outer, 7, 8);

    inst = ecs_lookup(world, "e.inner.leaf");
    test_assert(inst != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, inst, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 7);
    test_flt(pt->y, 8);

    pos = ecs_get_id(world, inst, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 7);
    test_flt(pos->y, 8);

    inst = ecs_lookup(world, "e.inner.leaf2");
    test_assert(inst != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, inst, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 8);

    pos = ecs_get_id(world, inst, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 8);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_two_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Middle {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  middle { Middle: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.middle.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    e = ecs_lookup(world, "e.middle.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 100);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 100);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_pass_to_child_template_and_use_in_outer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  point: {y: 50}"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: Point(x: 5, y: 6)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 50);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 50);

    e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 5);
    test_flt(pos->y, 6);

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Point3D : Point {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: Point3D(x: 5, y: 6, z: 7)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t inner = ecs_lookup(world, "Inner");
    ecs_entity_t point3d = ecs_lookup(world, "Point3D");
    ecs_entity_t outer = ecs_lookup(world, "Outer");

    ecs_entity_t e = ecs_lookup(world, "e");
    const ecs_script_template_ref_t *oref = ecs_get_id(world, e, outer);
    test_assert(oref != NULL);
    test_uint(oref->type, point3d);
    const float *ov = oref->value;
    test_assert(ov != NULL);
    test_flt(ov[0], 5);
    test_flt(ov[1], 6);
    test_flt(ov[2], 7);

    ecs_entity_t e_inner = ecs_lookup(world, "e.inner");
    const ecs_script_template_ref_t *iref = ecs_get_id(world, e_inner, inner);
    test_assert(iref != NULL);
    test_uint(iref->type, point3d);
    const PointValue *pt = iref->value;
    test_assert(pt != NULL);
    test_flt(pt->x, 5);
    test_flt(pt->y, 6);

    ecs_entity_t leaf = ecs_lookup(world, "e.inner.leaf");
    test_assert(ecs_has_id(world, leaf, point3d));

    ecs_entity_t inst = ecs_lookup(world, "e.inner.leaf");
    test_assert(inst != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const float *pt_1_3d = ecs_get_id(world, inst, point3d);
    test_assert(pt_1_3d != NULL);
    test_flt(pt_1_3d[0], 5);
    test_flt(pt_1_3d[1], 6);
    test_flt(pt_1_3d[2], 7);
    test_assert(ecs_has_id(world, inst, point));
    {
        const PointValue *pos_v = ecs_get_id(world, inst, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 5);
        test_flt(pos_v->y, 6);
    }

    inst = ecs_lookup(world, "e.inner.leaf2");
    test_assert(inst != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt_1_3d = ecs_get_id(world, inst, point3d);
    test_assert(pt_1_3d != NULL);
    test_flt(pt_1_3d[0], 100);
    test_flt(pt_1_3d[1], 6);
    test_flt(pt_1_3d[2], 7);
    test_assert(ecs_has_id(world, inst, point));
    {
        const PointValue *pos_v = ecs_get_id(world, inst, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 100);
        test_flt(pos_v->y, 6);
    }

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Point3D : Point {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  inner { Inner: {$point} }"
    LINE "}"
    LINE "e { Outer: {point: Point3D(x: 5, y: 6, z: 7)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t point3d = ecs_lookup(world, "Point3D");

    ecs_entity_t e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const float *pt_3d = ecs_get_id(world, e, point3d);
    test_assert(pt_3d != NULL);
    test_flt(pt_3d[0], 5);
    test_flt(pt_3d[1], 6);
    test_flt(pt_3d[2], 7);
    test_assert(ecs_has_id(world, e, point));
    {
        const PointValue *pos_v = ecs_get_id(world, e, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 5);
        test_flt(pos_v->y, 6);
    }

    e = ecs_lookup(world, "e.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt_3d = ecs_get_id(world, e, point3d);
    test_assert(pt_3d != NULL);
    test_flt(pt_3d[0], 100);
    test_flt(pt_3d[1], 6);
    test_flt(pt_3d[2], 7);
    test_assert(ecs_has_id(world, e, point));
    {
        const PointValue *pos_v = ecs_get_id(world, e, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 100);
        test_flt(pos_v->y, 6);
    }

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base_w_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Point3D : Point {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  with Inner(point: $point) {"
    LINE "    inner {}"
    LINE "  }"
    LINE "}"
    LINE "e { Outer: {point: Point3D(x: 5, y: 6, z: 7)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t point3d = ecs_lookup(world, "Point3D");

    ecs_entity_t e = ecs_lookup(world, "e.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const float *pt_3d = ecs_get_id(world, e, point3d);
    test_assert(pt_3d != NULL);
    test_flt(pt_3d[0], 5);
    test_flt(pt_3d[1], 6);
    test_flt(pt_3d[2], 7);
    test_assert(ecs_has_id(world, e, point));
    {
        const PointValue *pos_v = ecs_get_id(world, e, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 5);
        test_flt(pos_v->y, 6);
    }

    e = ecs_lookup(world, "e.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt_3d = ecs_get_id(world, e, point3d);
    test_assert(pt_3d != NULL);
    test_flt(pt_3d[0], 100);
    test_flt(pt_3d[1], 6);
    test_flt(pt_3d[2], 7);
    test_assert(ecs_has_id(world, e, point));
    {
        const PointValue *pos_v = ecs_get_id(world, e, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 100);
        test_flt(pos_v->y, 6);
    }

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Point3D : Point {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  point"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: Point3D(x: 5, y: 6, z: 7)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t point3d = ecs_lookup(world, "Point3D");
    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, point3d);
    test_assert(v != NULL);
    test_flt(v[0], 5);
    test_flt(v[1], 6);
    test_flt(v[2], 7);

    ecs_entity_t inst = ecs_lookup(world, "e.inner.leaf");
    test_assert(inst != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const float *pt_3d = ecs_get_id(world, inst, point3d);
    test_assert(pt_3d != NULL);
    test_flt(pt_3d[0], 5);
    test_flt(pt_3d[1], 6);
    test_flt(pt_3d[2], 7);
    test_assert(ecs_has_id(world, inst, point));
    {
        const PointValue *pos_v = ecs_get_id(world, inst, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 5);
        test_flt(pos_v->y, 6);
    }

    ecs_entity_t leaf = ecs_lookup(world, "e.inner.leaf");

    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Outer: {point: Point3D(x: 8, y: 9, z: 10)} }", &ir_desc, NULL) == 0);

    v = ecs_get_id(world, e, point3d);
    test_flt(v[0], 8);
    test_flt(v[1], 9);
    test_flt(v[2], 10);
    test_assert(ecs_lookup(world, "e.inner.leaf") == leaf);

    inst = ecs_lookup(world, "e.inner.leaf");
    test_assert(inst != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt_3d = ecs_get_id(world, inst, point3d);
    test_assert(pt_3d != NULL);
    test_flt(pt_3d[0], 8);
    test_flt(pt_3d[1], 9);
    test_flt(pt_3d[2], 10);
    test_assert(ecs_has_id(world, inst, point));
    {
        const PointValue *pos_v = ecs_get_id(world, inst, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 8);
        test_flt(pos_v->y, 9);
    }

    inst = ecs_lookup(world, "e.inner.leaf2");
    test_assert(inst != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt_3d = ecs_get_id(world, inst, point3d);
    test_assert(pt_3d != NULL);
    test_flt(pt_3d[0], 100);
    test_flt(pt_3d[1], 9);
    test_flt(pt_3d[2], 10);
    test_assert(ecs_has_id(world, inst, point));
    {
        const PointValue *pos_v = ecs_get_id(world, inst, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 100);
        test_flt(pos_v->y, 9);
    }

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Point3D : Point {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Inner3D {"
    LINE "  prop point : template Point3D"
    LINE "  leaf { point }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  inner { Inner3D: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: Point3D(x: 5, y: 6, z: 7)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t point3d = ecs_lookup(world, "Point3D");
    ecs_entity_t leaf = ecs_lookup(world, "e.inner.leaf");
    test_assert(leaf != 0);
    const float *v = ecs_get_id(world, leaf, point3d);
    test_assert(v != NULL);
    test_flt(v[0], 5);
    test_flt(v[1], 6);
    test_flt(v[2], 7);
    test_assert(ecs_has_id(world, leaf, ecs_lookup(world, "Point")));

    ecs_fini(world);
}

void TemplateProp_pass_derived_to_child_template_base_two_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Point3D : Point {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Middle {"
    LINE "  prop point : template Point3D"
    LINE "  inner { Inner: {point: $point} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point3D"
    LINE "  middle { Middle: {point: $point} }"
    LINE "}"
    LINE "e { Outer: {point: Point3D(x: 5, y: 6, z: 7)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t point3d = ecs_lookup(world, "Point3D");

    ecs_entity_t e = ecs_lookup(world, "e.middle.inner.leaf");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const float *pt_3d = ecs_get_id(world, e, point3d);
    test_assert(pt_3d != NULL);
    test_flt(pt_3d[0], 5);
    test_flt(pt_3d[1], 6);
    test_flt(pt_3d[2], 7);
    test_assert(ecs_has_id(world, e, point));
    {
        const PointValue *pos_v = ecs_get_id(world, e, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 5);
        test_flt(pos_v->y, 6);
    }

    e = ecs_lookup(world, "e.middle.inner.leaf2");
    test_assert(e != 0);

    point = ecs_lookup(world, "Point");
    position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    pt_3d = ecs_get_id(world, e, point3d);
    test_assert(pt_3d != NULL);
    test_flt(pt_3d[0], 100);
    test_flt(pt_3d[1], 6);
    test_flt(pt_3d[2], 7);
    test_assert(ecs_has_id(world, e, point));
    {
        const PointValue *pos_v = ecs_get_id(world, e, position);
        test_assert(pos_v != NULL);
        test_flt(pos_v->x, 100);
        test_flt(pos_v->y, 6);
    }

    ecs_fini(world);
}

void TemplateProp_pass_base_to_child_template_derived_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Point3D : Point {"
    LINE "  prop z: f32 = 3"
    LINE "}"
    LINE "template Inner3D {"
    LINE "  prop point : template Point3D"
    LINE "  leaf { point }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop point : template Point"
    LINE "  inner { Inner3D: {point: $point} }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_pass_unrelated_to_child_template_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "template Inner {"
    LINE "  prop point : template Point"
    LINE "  leaf { point }"
    LINE "  leaf2 { point: {x: 100} }"
    LINE "}"
    LINE "template Other {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop other : template Other"
    LINE "  inner { Inner: {point: $other} }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
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

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

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

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

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

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

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

void TemplateProp_struct_prop_default_to_struct(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Facade(height: f32)"
    LINE "template Building {"
    LINE "  prop facade: template Facade"
    LINE "  prop floors: i32 = 2"
    LINE "  for i in 0..floors {"
    LINE "    {"
    LINE "      facade: {height: 3}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Building b {}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(facade != 0);
    test_assert(b != 0);

    int32_t count = 0;
    ecs_iter_t it = ecs_children(world, b);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            const float *h = ecs_get_id(world, it.entities[i], facade);
            test_assert(h != NULL);
            test_flt(*h, 3);
            count ++;
        }
    }
    test_int(count, 2);

    ecs_fini(world);
}

void TemplateProp_struct_prop_explicit_struct_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Facade(height: f32)"
    LINE "template Building {"
    LINE "  prop facade: template Facade = Facade"
    LINE "  prop floors: i32 = 2"
    LINE "  for i in 0..floors {"
    LINE "    {"
    LINE "      facade: {height: 3}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Building b {}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(facade != 0);
    test_assert(b != 0);

    int32_t count = 0;
    ecs_iter_t it = ecs_children(world, b);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            const float *h = ecs_get_id(world, it.entities[i], facade);
            test_assert(h != NULL);
            test_flt(*h, 3);
            count ++;
        }
    }
    test_int(count, 2);

    ecs_fini(world);
}


void TemplateProp_struct_prop_default_and_derived_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "  prop y: f32 = 2"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "struct Facade(height: f32)"
    LINE "template Brick : Facade {"
    LINE "  Point: {height, 1}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade"
    LINE "  child {"
    LINE "    facade: {height: 3}"
    LINE "  }"
    LINE "}"
    LINE "Building a {}"
    LINE "Building b(facade: Brick) {}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t brick = ecs_lookup(world, "Brick");
    ecs_entity_t a_child = ecs_lookup(world, "a.child");
    ecs_entity_t b_child = ecs_lookup(world, "b.child");
    test_assert(facade != 0);
    test_assert(brick != 0);
    test_assert(a_child != 0);
    test_assert(b_child != 0);

    const float *h = ecs_get_id(world, a_child, facade);
    test_assert(h != NULL);
    test_flt(*h, 3);
    test_assert(!ecs_has_id(world, a_child, brick));

    h = ecs_get_id(world, b_child, brick);
    test_assert(h != NULL);
    test_flt(*h, 3);

    ecs_entity_t e = ecs_lookup(world, "b.child");
    test_assert(e != 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(point != 0);
    test_assert(position != 0);

    const PointValue *pt = ecs_get_id(world, e, point);
    test_assert(pt != NULL);
    test_flt(pt->x, 3);
    test_flt(pt->y, 1);

    const PointValue *pos = ecs_get_id(world, e, position);
    test_assert(pos != NULL);
    test_flt(pos->x, 3);
    test_flt(pos->y, 1);

    ecs_fini(world);
}

void TemplateProp_interface_prop_as_tag_instantiates(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct StreetLight(on_off: bool)"
    LINE "template MyStreetLight : StreetLight {"
    LINE "  prop scale: f32 = 2"
    LINE "  Position: {$scale, $scale + 1}"
    LINE "}"
    LINE "template Road {"
    LINE "  prop street_light : template StreetLight"
    LINE "  lamp { street_light }"
    LINE "}"
    LINE "e { Road: {street_light: MyStreetLight} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t lamp = ecs_lookup(world, "e.lamp");
    ecs_entity_t light = ecs_lookup(world, "MyStreetLight");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(lamp != 0);
    test_assert(light != 0);
    test_assert(position != 0);
    test_assert(ecs_has_id(world, lamp, light));

    const PointValue *p = ecs_get_id(world, lamp, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    test_flt(p->y, 3);

    ecs_fini(world);
}

void TemplateProp_interface_prop_empty_initializer(void) {
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

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t lamp = ecs_lookup(world, "r.lamp");
    ecs_entity_t light = ecs_lookup(world, "Light");
    test_assert(lamp != 0);
    test_assert(light != 0);
    test_assert(ecs_has_id(world, lamp, light));

    const bool *v = ecs_get_id(world, lamp, light);
    test_assert(v != NULL);
    test_bool(*v, false);

    test_assert(ecs_lookup(world, "r.lamp.Bulb") != 0);

    ecs_fini(world);
}

void TemplateProp_string_prop_in_const(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_entity(world, { .name = "Text" });
    ecs_struct(world, {
        .entity = text,
        .members = {
            {"data", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop text: string = \"HELLO\""
    LINE "  const p0 = \"<text>{text}</text>\""
    LINE "  Text: {data: \"{p0}\"}"
    LINE "}"
    LINE "Sign a()"
    LINE "Sign b(text: \"OTHER\")";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "a");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    const char *const *ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<text>HELLO</text>");

    e = ecs_lookup(world, "b");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<text>OTHER</text>");

    ecs_fini(world);
}

void TemplateProp_string_prop_in_2_consts(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_entity(world, { .name = "Text" });
    ecs_struct(world, {
        .entity = text,
        .members = {
            {"data", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop text: string = \"HELLO\""
    LINE "  const p0 = \"<a>{text}</a>\""
    LINE "  const p1 = \"<b>{text}</b>\""
    LINE "  Text: {data: \"{p0}{p1}</svg>\"}"
    LINE "}"
    LINE "Sign a()"
    LINE "Sign b(text: \"OTHER\")";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "a");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    const char *const *ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>HELLO</a><b>HELLO</b></svg>");

    e = ecs_lookup(world, "b");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>OTHER</a><b>OTHER</b></svg>");

    ecs_fini(world);
}

void TemplateProp_string_prop_in_3_consts(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_entity(world, { .name = "Text" });
    ecs_struct(world, {
        .entity = text,
        .members = {
            {"data", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop l1: string = \"ONE\""
    LINE "  prop l2: string = \"TWO\""
    LINE "  prop size: f32 = 12"
    LINE "  const p0 = \"<text y='{size}'>{l1}</text>\""
    LINE "  const p1 = \"<text y='{size}'>{l2}</text>\""
    LINE "  const p2 = \"<text y='{size}'>{l1}{l2}</text>\""
    LINE "  Text: {data: \"{p0}{p1}{p2}</svg>\"}"
    LINE "}"
    LINE "Sign a()"
    LINE "Sign b(l1: \"AAA\", l2: \"BBB\")";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "a");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    const char *const *ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<text y='12.000000'>ONE</text>"
            "<text y='12.000000'>TWO</text>"
            "<text y='12.000000'>ONETWO</text></svg>");

    e = ecs_lookup(world, "b");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<text y='12.000000'>AAA</text>"
            "<text y='12.000000'>BBB</text>"
            "<text y='12.000000'>AAABBB</text></svg>");

    ecs_fini(world);
}

void TemplateProp_string_prop_in_const_w_adjacent_prop(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_entity(world, { .name = "Text" });
    ecs_struct(world, {
        .entity = text,
        .members = {
            {"data", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop text: string = \"HELLO\""
    LINE "  const p0 = \"<a>{text}</a>\""
    LINE "  Text: {data: \"{p0}{text}!\"}"
    LINE "}"
    LINE "Sign a()"
    LINE "Sign b(text: \"OTHER\")";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "a");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    const char *const *ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>HELLO</a>HELLO!");

    e = ecs_lookup(world, "b");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>OTHER</a>OTHER!");

    ecs_fini(world);
}

void TemplateProp_string_prop_in_chained_consts(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_entity(world, { .name = "Text" });
    ecs_struct(world, {
        .entity = text,
        .members = {
            {"data", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop text: string = \"HELLO\""
    LINE "  const a0 = \"<a>{text}</a>\""
    LINE "  const a1 = \"[{a0}]\""
    LINE "  const a2 = \"({a1})\""
    LINE "  Text: {data: \"{a2}\"}"
    LINE "}"
    LINE "Sign a()"
    LINE "Sign b(text: \"OTHER\")";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "a");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    const char *const *ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "([<a>HELLO</a>])");

    e = ecs_lookup(world, "b");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "([<a>OTHER</a>])");

    ecs_fini(world);
}

void TemplateProp_string_prop_in_consts_reassign(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_entity(world, { .name = "Text" });
    ecs_struct(world, {
        .entity = text,
        .members = {
            {"data", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop l1: string = \"ONE\""
    LINE "  prop l2: string = \"TWO\""
    LINE "  const p0 = \"<a>{l1}</a>\""
    LINE "  const p1 = \"<b>{l2}</b>\""
    LINE "  const p2 = \"<c>{l1}{l2}</c>\""
    LINE "  Text: {data: \"{p0}{p1}{p2}</svg>\"}"
    LINE "}"
    LINE "Sign a()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "a");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    const char *const *ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>ONE</a><b>TWO</b><c>ONETWO</c></svg>");

    test_assert(ecs_script_run_w_desc(world, NULL,
        "a { Sign: {l1: \"AAA\"} }", &ir_desc, NULL) == 0);

    e = ecs_lookup(world, "a");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>AAA</a><b>TWO</b><c>AAATWO</c></svg>");

    test_assert(ecs_script_run_w_desc(world, NULL,
        "a { Sign: {l2: \"BBB\"} }", &ir_desc, NULL) == 0);

    e = ecs_lookup(world, "a");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>AAA</a><b>BBB</b><c>AAABBB</c></svg>");

    ecs_fini(world);
}

void TemplateProp_string_prop_in_consts_deferred(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_entity(world, { .name = "Text" });
    ecs_struct(world, {
        .entity = text,
        .members = {
            {"data", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop l1: string = \"ONE\""
    LINE "  prop l2: string = \"TWO\""
    LINE "  const p0 = \"<a>{l1}</a>\""
    LINE "  const p1 = \"<b>{l2}</b>\""
    LINE "  const p2 = \"<c>{l1}{l2}</c>\""
    LINE "  Text: {data: \"{p0}{p1}{p2}</svg>\"}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t sign = ecs_lookup(world, "Sign");
    test_assert(sign != 0);

    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_entity_t c = ecs_entity(world, { .name = "c" });

    char *sign_a[] = {"ONE", "TWO"};
    char *sign_b[] = {"AAA", "BBB"};
    char *sign_c[] = {"CCCCCCCCCCCCCCCC", "TWO"};

    ecs_world_t *stage_1 = ecs_get_stage(world, 0);
    ecs_set_id(stage_1, a, sign, sizeof(sign_a), sign_a);
    ecs_set_id(stage_1, b, sign, sizeof(sign_b), sign_b);
    ecs_set_id(stage_1, c, sign, sizeof(sign_c), sign_c);
    ecs_merge(stage_1);

    ecs_entity_t e = ecs_lookup(world, "a");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    const char *const *ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>ONE</a><b>TWO</b><c>ONETWO</c></svg>");

    e = ecs_lookup(world, "b");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>AAA</a><b>BBB</b><c>AAABBB</c></svg>");

    e = ecs_lookup(world, "c");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<a>CCCCCCCCCCCCCCCC</a><b>TWO</b><c>CCCCCCCCCCCCCCCCTWO</c></svg>");

    ecs_fini(world);
}

void TemplateProp_string_prop_in_consts_nested_template(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_entity(world, { .name = "Text" });
    ecs_struct(world, {
        .entity = text,
        .members = {
            {"data", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Inner {"
    LINE "  prop text: string = \"I\""
    LINE "  const p0 = \"<i>{text}</i>\""
    LINE "  Text: {data: \"{p0}\"}"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop text: string = \"O\""
    LINE "  const q0 = \"<o>{text}</o>\""
    LINE "  kid { Inner: {text: \"{q0}\"} }"
    LINE "}"
    LINE "Outer a()"
    LINE "Outer b(text: \"Z\")";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "a.kid");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    const char *const *ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<i><o>O</o></i>");

    e = ecs_lookup(world, "b.kid");
    test_assert(e != 0);

    text = ecs_lookup(world, "Text");
    test_assert(text != 0);

    ptr = ecs_get_id(world, e, text);
    test_assert(ptr != NULL);
    test_assert(ptr[0] != NULL);
    test_str(ptr[0], "<i><o>Z</o></i>");

    ecs_fini(world);
}

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RgbValue;

typedef struct {
    int32_t x;
    int32_t y;
    RgbValue color;
} MyBlockValue;

typedef struct {
    int32_t size;
    RgbValue color;
} MyRoadValue;

#define BLOCK_CITY_HEAD \
    HEAD "struct Block(x: i32, y: i32)" \
    LINE "struct Rgb(r: u8, g: u8, b: u8)" \
    LINE "template MyBlock : Block {" \
    LINE "  prop color: Rgb = {0, 0, 0}" \
    LINE "}" \
    LINE "template City {" \
    LINE "  prop block: template Block" \
    LINE "  b { block: {x: 1, y: 2} }" \
    LINE "}"

static void test_my_block(
    ecs_world_t *world,
    const char *path,
    int32_t x,
    int32_t y,
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);
    ecs_entity_t my_block = ecs_lookup(world, "MyBlock");
    ecs_entity_t block = ecs_lookup(world, "Block");
    test_assert(my_block != 0);
    test_assert(block != 0);
    test_assert(ecs_has_id(world, e, block));

    const MyBlockValue *v = ecs_get_id(world, e, my_block);
    test_assert(v != NULL);
    test_int(v->x, x);
    test_int(v->y, y);
    test_int(v->color.r, r);
    test_int(v->color.g, g);
    test_int(v->color.b, b);
}

void TemplateProp_template_ref_call_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "City c(block: MyBlock())";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 2, 0, 0, 0);

    ecs_entity_t c = ecs_lookup(world, "c");
    ecs_entity_t city = ecs_lookup(world, "City");
    const ecs_script_template_ref_t *ref = ecs_get_id(world, c, city);
    test_assert(ref != NULL);
    test_assert(ref->type == ecs_lookup(world, "MyBlock"));

    ecs_fini(world);
}

void TemplateProp_template_ref_call_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "City c(block: MyBlock(0, 0, {50, 50, 50}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 2, 50, 50, 50);

    ecs_entity_t c = ecs_lookup(world, "c");
    ecs_entity_t city = ecs_lookup(world, "City");
    const ecs_script_template_ref_t *ref = ecs_get_id(world, c, city);
    test_assert(ref != NULL);
    test_assert(ref->type == ecs_lookup(world, "MyBlock"));
    test_assert(ref->value != NULL);
    const MyBlockValue *v = ref->value;
    test_int(v->x, 0);
    test_int(v->y, 0);
    test_int(v->color.r, 50);
    test_int(v->color.g, 50);
    test_int(v->color.b, 50);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_named(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "City c(block: MyBlock(color: {50, 50, 50}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 2, 50, 50, 50);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_named_inherited_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Block(x: i32, y: i32)"
    LINE "struct Rgb(r: u8, g: u8, b: u8)"
    LINE "template MyBlock : Block {"
    LINE "  prop color: Rgb = {0, 0, 0}"
    LINE "}"
    LINE "template City {"
    LINE "  prop block: template Block"
    LINE "  b { block: {y: 2} }"
    LINE "}"
    LINE "City c(block: MyBlock(x: 10, color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 10, 2, 1, 2, 3);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_use_site_overrides(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Block(x: i32, y: i32)"
    LINE "struct Rgb(r: u8, g: u8, b: u8)"
    LINE "template MyBlock : Block {"
    LINE "  prop color: Rgb = {0, 0, 0}"
    LINE "}"
    LINE "template City {"
    LINE "  prop block: template Block"
    LINE "  b { block: {x: 1} }"
    LINE "}"
    LINE "City c(block: MyBlock(x: 10, y: 20, color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 20, 1, 2, 3);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_tag_form(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Block(x: i32, y: i32)"
    LINE "struct Rgb(r: u8, g: u8, b: u8)"
    LINE "template MyBlock : Block {"
    LINE "  prop color: Rgb = {0, 0, 0}"
    LINE "}"
    LINE "template City {"
    LINE "  prop block: template Block"
    LINE "  b { block }"
    LINE "}"
    LINE "City c(block: MyBlock(color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 0, 0, 1, 2, 3);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_in_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Block(x: i32, y: i32)"
    LINE "struct Rgb(r: u8, g: u8, b: u8)"
    LINE "template MyBlock : Block {"
    LINE "  prop color: Rgb = {0, 0, 0}"
    LINE "}"
    LINE "template City {"
    LINE "  prop block: template Block"
    LINE "  with block(x: 5) {"
    LINE "    b {}"
    LINE "  }"
    LINE "}"
    LINE "City c(block: MyBlock(color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 5, 0, 1, 2, 3);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Block(x: i32, y: i32)"
    LINE "struct Rgb(r: u8, g: u8, b: u8)"
    LINE "template MyBlock : Block {"
    LINE "  prop color: Rgb = {0, 0, 0}"
    LINE "}"
    LINE "template City {"
    LINE "  prop block: template Block = MyBlock(color: {1, 2, 3})"
    LINE "  b { block: {x: 1, y: 2} }"
    LINE "}"
    LINE "City c()"
    LINE "City d(block: MyBlock(color: {4, 5, 6}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 2, 1, 2, 3);
    test_my_block(world, "d.b", 1, 2, 4, 5, 6);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_w_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "const shade: Rgb = {7, 8, 9}"
    LINE "City c(block: MyBlock(color: $shade))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 2, 7, 8, 9);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_w_prop_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "template Town {"
    LINE "  prop shade: u8 = 3"
    LINE "  c { City: {block: MyBlock(color: {$shade, $shade, $shade})} }"
    LINE "}"
    LINE "Town t(shade: 9)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "t.c.b", 1, 2, 9, 9, 9);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "Town t(shade: 4)", &ir_desc, NULL) == 0);

    test_my_block(world, "t.c.b", 1, 2, 4, 4, 4);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "City c(block: MyBlock(color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 2, 1, 2, 3);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "City c(block: MyBlock(color: {4, 5, 6}))", &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 2, 4, 5, 6);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "City c(block: MyBlock())", &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 2, 0, 0, 0);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_from_c_w_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr = BLOCK_CITY_HEAD;

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t city = ecs_lookup(world, "City");
    ecs_entity_t my_block = ecs_lookup(world, "MyBlock");
    test_assert(city != 0);
    test_assert(my_block != 0);

    MyBlockValue *value = ecs_ptr_new(world, my_block);
    test_assert(value != NULL);
    value->color = (RgbValue){10, 20, 30};

    ecs_script_template_ref_t ref = { .type = my_block, .value = value };
    ecs_entity_t c = ecs_entity(world, { .name = "c" });
    ecs_set_id(world, c, city, sizeof(ecs_script_template_ref_t), &ref);
    ecs_ptr_free(world, my_block, value);

    test_my_block(world, "c.b", 1, 2, 10, 20, 30);

    const ecs_script_template_ref_t *stored = ecs_get_id(world, c, city);
    test_assert(stored != NULL);
    test_assert(stored->type == my_block);
    test_assert(stored->value != NULL);
    test_assert(stored->value != value);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_too_many_args_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "City c(block: MyBlock(0, 0, {1, 2, 3}, {4, 5, 6}))";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_unknown_member_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "City c(block: MyBlock(colour: {1, 2, 3}))";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_not_derived_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "template Other {"
    LINE "  prop v: i32 = 1"
    LINE "}"
    LINE "City c(block: Other(v: 2))";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_not_template_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "City c(block: Rgb(1, 2, 3))";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_ast_to_str(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "City c(block: MyBlock(color: {1, 2, 3}))";

    ecs_script_t *s = ecs_script_parse(world, NULL, expr, &ir_desc, NULL);
    test_assert(s != NULL);

    char *str = ecs_script_ast_to_str(s, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "MyBlock(") != NULL);
    ecs_os_free(str);

    ecs_script_free(s);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_to_json(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    BLOCK_CITY_HEAD
    LINE "City c(block: MyBlock(color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t c = ecs_lookup(world, "c");
    char *json = ecs_entity_to_json(world, c, &(ecs_entity_to_json_desc_t){
        .serialize_values = true
    });
    test_assert(json != NULL);
    test_assert(strstr(json, "\"City\":{\"block\":\"MyBlock\"}") != NULL);
    ecs_os_free(json);

    ecs_fini(world);
}

#define ROAD_NETWORK_HEAD \
    HEAD "struct Road(size: i32)" \
    LINE "struct Rgb(r: u8, g: u8, b: u8)" \
    LINE "template RoadNetwork {" \
    LINE "  prop road : template Road" \
    LINE "  prop corner : template Road" \
    LINE "  prop intersection : template Road" \
    LINE "  r { road: {size: 1} }" \
    LINE "  c { corner: {size: 2} }" \
    LINE "  i { intersection: {size: 3} }" \
    LINE "}" \
    LINE "template City {" \
    LINE "  prop roads : template RoadNetwork" \
    LINE "  n { roads }" \
    LINE "}" \
    LINE "template MyRoad : Road {" \
    LINE "  prop color: Rgb = {}" \
    LINE "}"

static void test_road(
    ecs_world_t *world,
    const char *path,
    int32_t size)
{
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);
    ecs_entity_t road = ecs_lookup(world, "Road");
    ecs_entity_t my_road = ecs_lookup(world, "MyRoad");
    test_assert(road != 0);
    test_assert(my_road != 0);
    test_assert(!ecs_has_id(world, e, my_road));

    const int32_t *v = ecs_get_id(world, e, road);
    test_assert(v != NULL);
    test_int(*v, size);
}

static void test_my_road(
    ecs_world_t *world,
    const char *path,
    int32_t size,
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);
    ecs_entity_t road = ecs_lookup(world, "Road");
    ecs_entity_t my_road = ecs_lookup(world, "MyRoad");
    test_assert(road != 0);
    test_assert(my_road != 0);
    test_assert(ecs_has_id(world, e, road));

    const MyRoadValue *v = ecs_get_id(world, e, my_road);
    test_assert(v != NULL);
    test_int(v->size, size);
    test_int(v->color.r, r);
    test_int(v->color.g, g);
    test_int(v->color.b, b);
}

void TemplateProp_template_ref_nested_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ROAD_NETWORK_HEAD
    LINE "City a(roads: RoadNetwork())";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_road(world, "a.n.r", 1);
    test_road(world, "a.n.c", 2);
    test_road(world, "a.n.i", 3);

    ecs_fini(world);
}

void TemplateProp_template_ref_nested_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ROAD_NETWORK_HEAD
    LINE "City b(roads: RoadNetwork(MyRoad(color: {1, 2, 3})))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_road(world, "b.n.r", 1, 1, 2, 3);
    test_road(world, "b.n.c", 2);
    test_road(world, "b.n.i", 3);

    ecs_fini(world);
}

void TemplateProp_template_ref_nested_named(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ROAD_NETWORK_HEAD
    LINE "City c(roads: RoadNetwork(road: MyRoad(color: {1, 2, 3})))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_road(world, "c.n.r", 1, 1, 2, 3);
    test_road(world, "c.n.c", 2);
    test_road(world, "c.n.i", 3);

    ecs_fini(world);
}

void TemplateProp_template_ref_nested_multiple(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ROAD_NETWORK_HEAD
    LINE "City c(roads: RoadNetwork("
    LINE "  MyRoad(color: {1, 2, 3}),"
    LINE "  intersection: MyRoad(0, {7, 8, 9}),"
    LINE "  corner: MyRoad()))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_road(world, "c.n.r", 1, 1, 2, 3);
    test_my_road(world, "c.n.c", 2, 0, 0, 0);
    test_my_road(world, "c.n.i", 3, 7, 8, 9);

    ecs_fini(world);
}

void TemplateProp_template_ref_nested_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ROAD_NETWORK_HEAD
    LINE "City c(roads: RoadNetwork(road: MyRoad(color: {1, 2, 3})))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_road(world, "c.n.r", 1, 1, 2, 3);
    test_road(world, "c.n.c", 2);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "City c(roads: RoadNetwork(corner: MyRoad(color: {4, 5, 6})))",
        &ir_desc, NULL) == 0);

    test_road(world, "c.n.r", 1);
    test_my_road(world, "c.n.c", 2, 4, 5, 6);
    test_road(world, "c.n.i", 3);

    ecs_fini(world);
}

void TemplateProp_template_ref_nested_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Road(size: i32)"
    LINE "struct Rgb(r: u8, g: u8, b: u8)"
    LINE "template MyRoad : Road {"
    LINE "  prop color: Rgb = {}"
    LINE "}"
    LINE "template RoadNetwork {"
    LINE "  prop road : template Road = MyRoad(color: {1, 2, 3})"
    LINE "  prop corner : template Road"
    LINE "  r { road: {size: 1} }"
    LINE "  c { corner: {size: 2} }"
    LINE "}"
    LINE "template City {"
    LINE "  prop roads : template RoadNetwork = RoadNetwork(corner: MyRoad())"
    LINE "  n { roads }"
    LINE "}"
    LINE "City a()"
    LINE "City b(roads: RoadNetwork(road: MyRoad(color: {4, 5, 6})))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_road(world, "a.n.r", 1, 1, 2, 3);
    test_my_road(world, "a.n.c", 2, 0, 0, 0);
    test_my_road(world, "b.n.r", 1, 4, 5, 6);
    test_road(world, "b.n.c", 2);

    ecs_fini(world);
}

#define ANY_PROP_HEAD \
    HEAD "struct Block(x: i32, y: i32)" \
    LINE "struct Rgb(r: u8, g: u8, b: u8)" \
    LINE "struct Position(x: f32, y: f32)" \
    LINE "template MyBlock : Block {" \
    LINE "  prop color: Rgb = {0, 0, 0}" \
    LINE "}"

void TemplateProp_any_prop_template_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  b { object }"
    LINE "}"
    LINE "City c(object: MyBlock)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 0, 0, 0, 0, 0);

    ecs_entity_t c = ecs_lookup(world, "c");
    ecs_entity_t city = ecs_lookup(world, "City");
    const ecs_script_template_ref_t *ref = ecs_get_id(world, c, city);
    test_assert(ref != NULL);
    test_assert(ref->type == ecs_lookup(world, "MyBlock"));
    test_assert(ref->value == NULL);

    ecs_fini(world);
}

void TemplateProp_any_prop_template_call_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  b { object }"
    LINE "}"
    LINE "City c(object: MyBlock(x: 1, color: {4, 5, 6}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 1, 0, 4, 5, 6);

    ecs_fini(world);
}

void TemplateProp_any_prop_struct_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  b { object }"
    LINE "}"
    LINE "City c(object: Position)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t b = ecs_lookup(world, "c.b");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(b != 0);
    test_assert(position != 0);
    const PointValue *p = ecs_get_id(world, b, position);
    test_assert(p != NULL);
    test_flt(p->x, 0);
    test_flt(p->y, 0);
    test_assert(!ecs_has_id(world, b, ecs_lookup(world, "MyBlock")));

    ecs_fini(world);
}

void TemplateProp_any_prop_default_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template = MyBlock(color: {7, 8, 9})"
    LINE "  b { object }"
    LINE "}"
    LINE "City c()"
    LINE "City d(object: Position)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 0, 0, 7, 8, 9);

    ecs_entity_t d_b = ecs_lookup(world, "d.b");
    test_assert(d_b != 0);
    test_assert(ecs_has_id(world, d_b, ecs_lookup(world, "Position")));
    test_assert(!ecs_has_id(world, d_b, ecs_lookup(world, "MyBlock")));

    ecs_fini(world);
}

void TemplateProp_any_prop_default_struct(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template = Position"
    LINE "  b { object }"
    LINE "}"
    LINE "City c()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t b = ecs_lookup(world, "c.b");
    test_assert(b != 0);
    test_assert(ecs_has_id(world, b, ecs_lookup(world, "Position")));

    ecs_fini(world);
}

void TemplateProp_any_prop_in_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  with object {"
    LINE "    b {}"
    LINE "    d {}"
    LINE "  }"
    LINE "}"
    LINE "City c(object: MyBlock(color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 0, 0, 1, 2, 3);
    test_my_block(world, "c.d", 0, 0, 1, 2, 3);

    ecs_fini(world);
}

void TemplateProp_any_prop_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  b { object }"
    LINE "}"
    LINE "City c(object: MyBlock(color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 0, 0, 1, 2, 3);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "City c(object: MyBlock(color: {4, 5, 6}))", &ir_desc, NULL) == 0);

    test_my_block(world, "c.b", 0, 0, 4, 5, 6);

    ecs_fini(world);
}

void TemplateProp_any_prop_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop objects: template[] = [MyBlock(color: {1, 2, 3}), Position]"
    LINE "  for (i, o) in objects {"
    LINE "    \"b_{i}\" { o }"
    LINE "  }"
    LINE "  x { objects[0] }"
    LINE "}"
    LINE "City c()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.b_0", 0, 0, 1, 2, 3);
    test_my_block(world, "c.x", 0, 0, 1, 2, 3);

    ecs_entity_t b_1 = ecs_lookup(world, "c.b_1");
    test_assert(b_1 != 0);
    test_assert(ecs_has_id(world, b_1, ecs_lookup(world, "Position")));

    ecs_fini(world);
}

void TemplateProp_any_prop_from_c(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  b { object }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t city = ecs_lookup(world, "City");
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(city != 0);
    test_assert(position != 0);

    ecs_script_template_ref_t ref = { .type = position };
    ecs_entity_t c = ecs_entity(world, { .name = "c" });
    ecs_set_id(world, c, city, sizeof(ecs_script_template_ref_t), &ref);

    ecs_entity_t b = ecs_lookup(world, "c.b");
    test_assert(b != 0);
    test_assert(ecs_has_id(world, b, position));

    ecs_fini(world);
}

void TemplateProp_any_prop_ast_to_str(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  prop objects: template[]"
    LINE "  b { object }"
    LINE "}";

    ecs_script_t *s = ecs_script_parse(world, NULL, expr, &ir_desc, NULL);
    test_assert(s != NULL);

    char *str = ecs_script_ast_to_str(s, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "object : template") != NULL);
    test_assert(strstr(str, "objects : template[]") != NULL);
    ecs_os_free(str);

    ecs_script_free(s);

    ecs_fini(world);
}

void TemplateProp_any_prop_missing_value_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  b { object }"
    LINE "}"
    LINE "City c()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_any_prop_initializer_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  b { object: {x: 1} }"
    LINE "}"
    LINE "City c(object: MyBlock)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_any_prop_with_initializer_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  with object(x: 1) {"
    LINE "    b {}"
    LINE "  }"
    LINE "}"
    LINE "City c(object: MyBlock)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_any_prop_not_struct_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "Foo {}"
    LINE "template City {"
    LINE "  prop object: template"
    LINE "  b { object }"
    LINE "}"
    LINE "City c(object: Foo)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_any_prop_mut_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    ANY_PROP_HEAD
    LINE "template City {"
    LINE "  mut object: template"
    LINE "  b { object }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateProp_template_ref_call_positional_inherited(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Block(x: i32, y: i32)"
    LINE "struct Rgb(r: u8, g: u8, b: u8)"
    LINE "template MyBlock : Block {"
    LINE "  prop color: Rgb = {0, 0, 0}"
    LINE "}"
    LINE "template City {"
    LINE "  prop block: template Block"
    LINE "  a { block }"
    LINE "  b { block: {y: 2} }"
    LINE "}"
    LINE "City c(block: MyBlock(5, 6, {1, 2, 3}))"
    LINE "City d(block: MyBlock(7))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_block(world, "c.a", 5, 6, 1, 2, 3);
    test_my_block(world, "c.b", 5, 2, 1, 2, 3);
    test_my_block(world, "d.a", 7, 0, 0, 0, 0);
    test_my_block(world, "d.b", 7, 2, 0, 0, 0);

    ecs_fini(world);
}

void TemplateProp_template_ref_prop_call_as_template_arg(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Road(size: i32)"
    LINE "struct Rgb(r: u8, g: u8, b: u8)"
    LINE "template MyRoad : Road {"
    LINE "  prop color: Rgb = {}"
    LINE "}"
    LINE "template Layout {"
    LINE "  prop object: template"
    LINE "  n { object }"
    LINE "}"
    LINE "template City {"
    LINE "  prop road : template Road"
    LINE "  Layout a(road(5))"
    LINE "  Layout b(object: road(size: 6))"
    LINE "}"
    LINE "City c(MyRoad)"
    LINE "City d(MyRoad(color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_my_road(world, "c.a.n", 5, 0, 0, 0);
    test_my_road(world, "c.b.n", 6, 0, 0, 0);
    test_my_road(world, "d.a.n", 5, 1, 2, 3);
    test_my_road(world, "d.b.n", 6, 1, 2, 3);

    ecs_fini(world);
}

typedef struct {
    float size;
    float width;
    RgbValue color;
} TemplateRoadValue;

#define TEMPLATE_INTERFACE_HEAD \
    HEAD "struct Rgb(r: u8, g: u8, b: u8)" \
    LINE "interface {" \
    LINE "  template Road {" \
    LINE "    prop size: f32 = 3" \
    LINE "    prop width: f32 = 1" \
    LINE "  }" \
    LINE "}" \
    LINE "template Road : interface.Road {" \
    LINE "  prop color: Rgb = {30, 30, 30}" \
    LINE "}"

static void test_template_road(
    ecs_world_t *world,
    const char *path,
    float size,
    float width,
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);
    ecs_entity_t road = ecs_lookup(world, "Road");
    ecs_entity_t base = ecs_lookup(world, "interface.Road");
    test_assert(road != 0);
    test_assert(base != 0);
    test_assert(ecs_has_id(world, e, base));
    const TemplateRoadValue *v = ecs_get_id(world, e, road);
    test_assert(v != NULL);
    test_flt(v->size, size);
    test_flt(v->width, width);
    test_int(v->color.r, r);
    test_int(v->color.g, g);
    test_int(v->color.b, b);
}

static void test_base_road(
    ecs_world_t *world,
    const char *path,
    float size,
    float width)
{
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);
    ecs_entity_t road = ecs_lookup(world, "Road");
    ecs_entity_t base = ecs_lookup(world, "interface.Road");
    test_assert(road != 0);
    test_assert(base != 0);
    test_assert(!ecs_has_id(world, e, road));
    const float *v = ecs_get_id(world, e, base);
    test_assert(v != NULL);
    test_flt(v[0], size);
    test_flt(v[1], width);
}

void TemplateProp_derived_template_inherits_base_defaults(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    TEMPLATE_INTERFACE_HEAD
    LINE "a { Road }"
    LINE "b { Road: {width: 7} }"
    LINE "c { Road: {color: {1, 2, 3}} }"
    LINE "Road d(size: 9, width: 8, color: {1, 2, 3})";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_template_road(world, "a", 3, 1, 30, 30, 30);
    test_template_road(world, "b", 3, 7, 30, 30, 30);
    test_template_road(world, "c", 3, 1, 1, 2, 3);
    test_template_road(world, "d", 9, 8, 1, 2, 3);

    ecs_fini(world);
}

void TemplateProp_template_interface_prop_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    TEMPLATE_INTERFACE_HEAD
    LINE "template City {"
    LINE "  prop road : template interface.Road"
    LINE "  a { road }"
    LINE "  b { road: {width: 7} }"
    LINE "}"
    LINE "City e"
    LINE "City f(Road)"
    LINE "City g(Road(color: {1, 2, 3}))"
    LINE "City h(Road(size: 9, width: 8, color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_base_road(world, "e.a", 3, 1);
    test_base_road(world, "e.b", 3, 7);
    test_template_road(world, "f.a", 3, 1, 30, 30, 30);
    test_template_road(world, "f.b", 3, 7, 30, 30, 30);
    test_template_road(world, "g.a", 3, 1, 1, 2, 3);
    test_template_road(world, "g.b", 3, 7, 1, 2, 3);
    test_template_road(world, "h.a", 9, 8, 1, 2, 3);
    test_template_road(world, "h.b", 9, 7, 1, 2, 3);

    ecs_fini(world);
}

void TemplateProp_template_interface_prop_call_as_template_arg(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    TEMPLATE_INTERFACE_HEAD
    LINE "template Layout {"
    LINE "  prop object: template"
    LINE "  n { object }"
    LINE "}"
    LINE "template City {"
    LINE "  prop road : template interface.Road"
    LINE "  Layout a(road(5, 2))"
    LINE "  Layout b(object: road(width: 6))"
    LINE "  Layout c(road)"
    LINE "}"
    LINE "City e"
    LINE "City f(Road)"
    LINE "City g(Road(color: {1, 2, 3}))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_base_road(world, "e.a.n", 5, 2);
    test_base_road(world, "e.b.n", 3, 6);
    test_base_road(world, "e.c.n", 3, 1);
    test_template_road(world, "f.a.n", 5, 2, 30, 30, 30);
    test_template_road(world, "f.b.n", 3, 6, 30, 30, 30);
    test_template_road(world, "f.c.n", 3, 1, 30, 30, 30);
    test_template_road(world, "g.a.n", 5, 2, 1, 2, 3);
    test_template_road(world, "g.b.n", 3, 6, 1, 2, 3);
    test_template_road(world, "g.c.n", 3, 1, 1, 2, 3);

    ecs_fini(world);
}

#define EMPTY_STRUCT_HEAD \
    HEAD "struct Facade()" \
    LINE "template BrickFacade : Facade {" \
    LINE "  prop height: f32 = 1" \
    LINE "}"

void TemplateProp_empty_struct_base(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    EMPTY_STRUCT_HEAD
    LINE "e { BrickFacade }"
    LINE "f { BrickFacade: {height: 2} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t brick = ecs_lookup(world, "BrickFacade");
    test_assert(facade != 0);
    test_assert(brick != 0);
    test_assert(ecs_has_pair(world, brick, EcsIsA, facade));

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const float *v = ecs_get_id(world, e, brick);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_assert(ecs_has_id(world, e, facade));

    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(f != 0);
    v = ecs_get_id(world, f, brick);
    test_assert(v != NULL);
    test_flt(v[0], 2);

    ecs_fini(world);
}

void TemplateProp_empty_struct_interface_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    EMPTY_STRUCT_HEAD
    LINE "template Building {"
    LINE "  prop facade: template Facade"
    LINE "  ground { facade }"
    LINE "}"
    LINE "Building e"
    LINE "Building f(BrickFacade)"
    LINE "Building g(BrickFacade(height: 3))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t brick = ecs_lookup(world, "BrickFacade");
    test_assert(facade != 0);
    test_assert(brick != 0);

    ecs_entity_t e = ecs_lookup(world, "e.ground");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, facade));
    test_assert(!ecs_has_id(world, e, brick));

    ecs_entity_t f = ecs_lookup(world, "f.ground");
    test_assert(f != 0);
    test_assert(ecs_has_id(world, f, facade));
    const float *v = ecs_get_id(world, f, brick);
    test_assert(v != NULL);
    test_flt(v[0], 1);

    ecs_entity_t g = ecs_lookup(world, "g.ground");
    test_assert(g != 0);
    v = ecs_get_id(world, g, brick);
    test_assert(v != NULL);
    test_flt(v[0], 3);

    ecs_fini(world);
}

void TemplateProp_empty_struct_interface_prop_as_template_arg(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    EMPTY_STRUCT_HEAD
    LINE "template Layout {"
    LINE "  prop object: template"
    LINE "  n { object }"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade"
    LINE "  Layout a(facade)"
    LINE "}"
    LINE "Building e"
    LINE "Building f(BrickFacade(height: 3))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t brick = ecs_lookup(world, "BrickFacade");
    test_assert(facade != 0);
    test_assert(brick != 0);

    ecs_entity_t e = ecs_lookup(world, "e.a.n");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, facade));

    ecs_entity_t f = ecs_lookup(world, "f.a.n");
    test_assert(f != 0);
    const float *v = ecs_get_id(world, f, brick);
    test_assert(v != NULL);
    test_flt(v[0], 3);

    ecs_fini(world);
}

void TemplateProp_empty_struct_in_interface_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "interface {"
    LINE "  struct Block(x: i32, z: i32, size: i32)"
    LINE "  template Road {"
    LINE "    prop size: f32 = 3"
    LINE "    prop width: f32 = 1"
    LINE "  }"
    LINE "  template Intersection {"
    LINE "    prop width: f32 = 1"
    LINE "  }"
    LINE "  struct Facade()"
    LINE "  template Building {"
    LINE "    prop floors: i32 = 2"
    LINE "    prop ground_facade: template interface.Facade"
    LINE "    ground { ground_facade }"
    LINE "  }"
    LINE "}"
    LINE "template Brick : interface.Facade {"
    LINE "  prop height: f32 = 1"
    LINE "}"
    LINE "interface.Building e"
    LINE "interface.Building f(ground_facade: Brick(height: 4))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "interface.Facade");
    ecs_entity_t brick = ecs_lookup(world, "Brick");
    test_assert(facade != 0);
    test_assert(brick != 0);

    ecs_entity_t e = ecs_lookup(world, "e.ground");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, facade));

    ecs_entity_t f = ecs_lookup(world, "f.ground");
    test_assert(f != 0);
    const float *v = ecs_get_id(world, f, brick);
    test_assert(v != NULL);
    test_flt(v[0], 4);

    ecs_fini(world);
}

void TemplateProp_empty_template_base(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Facade {"
    LINE "}"
    LINE "template BrickFacade : Facade {"
    LINE "  prop height: f32 = 1"
    LINE "}"
    LINE "e { Facade }"
    LINE "f { BrickFacade: {height: 2} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t brick = ecs_lookup(world, "BrickFacade");
    test_assert(facade != 0);
    test_assert(brick != 0);
    test_assert(ecs_has_pair(world, brick, EcsIsA, facade));

    const EcsStruct *st = ecs_get(world, facade, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 0);

    const EcsType *t = ecs_get(world, facade, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, brick, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, facade));

    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(f != 0);
    const float *v = ecs_get_id(world, f, brick);
    test_assert(v != NULL);
    test_flt(v[0], 2);
    test_assert(ecs_has_id(world, f, facade));

    ecs_fini(world);
}

void TemplateProp_empty_template_interface_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Facade {"
    LINE "}"
    LINE "template BrickFacade : Facade {"
    LINE "  prop height: f32 = 1"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade"
    LINE "  ground { facade }"
    LINE "}"
    LINE "Building e"
    LINE "Building f(BrickFacade)"
    LINE "Building g(BrickFacade(height: 3))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t brick = ecs_lookup(world, "BrickFacade");
    test_assert(facade != 0);
    test_assert(brick != 0);

    ecs_entity_t e = ecs_lookup(world, "e.ground");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, facade));
    test_assert(!ecs_has_id(world, e, brick));

    ecs_entity_t f = ecs_lookup(world, "f.ground");
    test_assert(f != 0);
    test_assert(ecs_has_id(world, f, facade));
    const float *v = ecs_get_id(world, f, brick);
    test_assert(v != NULL);
    test_flt(v[0], 1);

    ecs_entity_t g = ecs_lookup(world, "g.ground");
    test_assert(g != 0);
    v = ecs_get_id(world, g, brick);
    test_assert(v != NULL);
    test_flt(v[0], 3);

    ecs_fini(world);
}

void TemplateProp_empty_template_in_interface_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "interface {"
    LINE "  struct Block(x: i32, z: i32, size: i32)"
    LINE "  template Road {"
    LINE "    prop size: f32 = 3"
    LINE "    prop width: f32 = 1"
    LINE "  }"
    LINE "  template Intersection {"
    LINE "    prop width: f32 = 1"
    LINE "  }"
    LINE "  template Facade {"
    LINE "  }"
    LINE "  template Building {"
    LINE "    prop floors: i32 = 2"
    LINE "    prop ground_facade: template interface.Facade"
    LINE "    ground { ground_facade }"
    LINE "  }"
    LINE "}"
    LINE "template Brick : interface.Facade {"
    LINE "  prop height: f32 = 1"
    LINE "}"
    LINE "interface.Building e"
    LINE "interface.Building f(ground_facade: Brick(height: 4))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "interface.Facade");
    ecs_entity_t brick = ecs_lookup(world, "Brick");
    test_assert(facade != 0);
    test_assert(brick != 0);

    ecs_entity_t e = ecs_lookup(world, "e.ground");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, facade));

    ecs_entity_t f = ecs_lookup(world, "f.ground");
    test_assert(f != 0);
    const float *v = ecs_get_id(world, f, brick);
    test_assert(v != NULL);
    test_flt(v[0], 4);

    ecs_fini(world);
}

void TemplateProp_prop_default_from_nested_struct_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Rgb(r: f32, g: f32, b: f32)"
    LINE "struct Theme(panel: Rgb, text: Rgb)"
    LINE "const theme: Theme = {panel: {1, 2, 3}, text: {4, 5, 6}}"
    LINE "template Checkbox {"
    LINE "  prop colors: Theme = $theme"
    LINE "  child { Theme: {panel: {$colors.panel.r, $colors.panel.g, $colors.panel.b}, text: {$colors.text.r, $colors.text.g, $colors.text.b}} }"
    LINE "}"
    LINE "Checkbox default_cb"
    LINE "Checkbox explicit_cb(colors: {panel: {7, 8, 9}, text: {10, 11, 12}})";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t theme = ecs_lookup(world, "Theme");
    ecs_entity_t checkbox = ecs_lookup(world, "Checkbox");
    test_assert(theme != 0);
    test_assert(checkbox != 0);

    const float *v = ecs_get_id(world, ecs_lookup(world, "default_cb"), checkbox);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 3);
    test_flt(v[3], 4);
    test_flt(v[4], 5);
    test_flt(v[5], 6);

    v = ecs_get_id(world, ecs_lookup(world, "default_cb.child"), theme);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 3);
    test_flt(v[3], 4);
    test_flt(v[4], 5);
    test_flt(v[5], 6);

    v = ecs_get_id(world, ecs_lookup(world, "explicit_cb.child"), theme);
    test_assert(v != NULL);
    test_flt(v[0], 7);
    test_flt(v[1], 8);
    test_flt(v[2], 9);
    test_flt(v[3], 10);
    test_flt(v[4], 11);
    test_flt(v[5], 12);

    ecs_fini(world);
}

void TemplateProp_prop_default_from_nested_struct_const_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Rgb(r: f32, g: f32, b: f32)"
    LINE "struct Theme(panel: Rgb, text: Rgb)"
    LINE "const theme: Theme = {panel: {1, 2, 3}, text: {4, 5, 6}}"
    LINE "template Checkbox {"
    LINE "  prop color: Rgb = $theme.panel"
    LINE "  child { Rgb: {$color.r, $color.g, $color.b} }"
    LINE "}"
    LINE "Checkbox default_cb"
    LINE "Checkbox explicit_cb(color: {7, 8, 9})";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t rgb = ecs_lookup(world, "Rgb");
    ecs_entity_t checkbox = ecs_lookup(world, "Checkbox");
    test_assert(rgb != 0);
    test_assert(checkbox != 0);

    const float *v = ecs_get_id(world, ecs_lookup(world, "default_cb"), checkbox);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 3);

    v = ecs_get_id(world, ecs_lookup(world, "default_cb.child"), rgb);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 3);

    v = ecs_get_id(world, ecs_lookup(world, "explicit_cb.child"), rgb);
    test_assert(v != NULL);
    test_flt(v[0], 7);
    test_flt(v[1], 8);
    test_flt(v[2], 9);

    ecs_fini(world);
}
