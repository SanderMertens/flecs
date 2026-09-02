#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void Scenario_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

#define SCENARIO_TYPES\
    HEAD "struct Position3(x: f32, y: f32, z: f32)"\
    LINE "struct Rotation3(x: f32, y: f32, z: f32)"\
    LINE "struct Scale3(x: f32, y: f32, z: f32)"\
    LINE "struct Position(x: f32, y: f32)"\
    LINE "struct Rgba(r: u8, g: u8, b: u8, a: u8)"\
    LINE "struct Box(x: f32, y: f32, z: f32)"\
    LINE "struct Cylinder(segments: i32, smooth: bool, length: f32)"\
    LINE "struct Cone(segments: i32, smooth: bool, length: f32)"\
    LINE "struct Sphere(segments: i32, smooth: bool, radius: f32)"\
    LINE "struct PbrMaterial(metallic: f32, roughness: f32)"\
    LINE "struct Emissive(strength: f32, color: Rgba)"\
    LINE "struct Text(text: string, size: i32)"\
    LINE "struct Rect(width: f32, height: f32)"\
    LINE "struct Border(width: f32, color: Rgba, radius: f32)"

#define ROCKET_TEMPLATES\
    LINE "const rocketHull: Rgba = {228, 232, 240, 255}"\
    LINE "const rocketShade: Rgba = {150, 158, 174, 255}"\
    LINE "const rocketDark: Rgba = {48, 54, 68, 255}"\
    LINE "const rocketTrim: Rgba = {238, 138, 52, 255}"\
    LINE "const emberGlow: Rgba = {255, 150, 70, 255}"\
    LINE "const rkSkirtH: f32 = 0.62"\
    LINE "const rkTankH: f32 = 1.02"\
    LINE "const rkBayH: f32 = 0.78"\
    LINE "const rkRadius: f32 = 0.42"\
    LINE ""\
    LINE "@tree Parent"\
    LINE "template RocketPort {"\
    LINE "    prop lit: bool = false"\
    LINE "    frame {"\
    LINE "        Position3: {0, 0, 0}"\
    LINE "        Box: {0.12, 0.12, 0.05}"\
    LINE "        Rgba: rocketShade"\
    LINE "    }"\
    LINE "    if lit {"\
    LINE "        glow {"\
    LINE "            Position3: {0, 0, 0.03}"\
    LINE "            Rgba: emberGlow"\
    LINE "            Emissive: {strength: 2, color: emberGlow}"\
    LINE "        }"\
    LINE "    }"\
    LINE "}"\
    LINE ""\
    LINE "@tree Parent"\
    LINE "template RocketBay {"\
    LINE "    prop lit: bool = false"\
    LINE "    hull {"\
    LINE "        Position3: {0, 0, 0}"\
    LINE "        Cylinder: {segments: 20, smooth: true, length: rkBayH - 0.03}"\
    LINE "        Scale3: {(rkRadius + 0.05) * 2, 1, (rkRadius + 0.05) * 2}"\
    LINE "        Rgba: rocketShade"\
    LINE "        PbrMaterial: {metallic: 0.66, roughness: 0.36}"\
    LINE "    }"\
    LINE "    for i in 0..4 {"\
    LINE "        const a = 1.5708 * i"\
    LINE "        \"port_{i}\" {"\
    LINE "            Position3: {a * (rkRadius + 0.06), 0.06, (rkRadius + 0.06) - a}"\
    LINE "            Rotation3: {0, 0 - a, 0}"\
    LINE "            RocketPort: {lit: lit}"\
    LINE "        }"\
    LINE "    }"\
    LINE "}"\
    LINE ""\
    LINE "@tree Parent"\
    LINE "template Rocket {"\
    LINE "    prop engines: i32 = 0"\
    LINE "    prop fuel: i32 = 0"\
    LINE "    prop cargo: i32 = 0"\
    LINE "    prop loaded: bool = false"\
    LINE "    prop valid: bool = false"\
    LINE "    const engineY: f32 = match engines > 0 {"\
    LINE "        true: rkSkirtH"\
    LINE "        false: 0"\
    LINE "    }"\
    LINE "    const tankY: f32 = engineY + fuel * rkTankH"\
    LINE "    const bayY: f32 = tankY + cargo * rkBayH"\
    LINE "    const finCount: i32 = match engines > 0 {"\
    LINE "        true: 4"\
    LINE "        false: 0"\
    LINE "    }"\
    LINE "    if engines > 0 {"\
    LINE "        skirt {"\
    LINE "            Position3: {0, 0.44, 0}"\
    LINE "            Cylinder: {segments: 20, smooth: true, length: 0.36}"\
    LINE "            Scale3: {rkRadius * 2, 1, rkRadius * 2}"\
    LINE "            Rgba: rocketDark"\
    LINE "            PbrMaterial: {metallic: 0.86, roughness: 0.28}"\
    LINE "        }"\
    LINE "    }"\
    LINE "    for i in 0..finCount {"\
    LINE "        const a = 1.5708 * i + 0.7854"\
    LINE "        \"fin_{i}\" {"\
    LINE "            Position3: {a * (rkRadius + 0.02), 0.4, (rkRadius + 0.02) - a}"\
    LINE "            Box: {0.2, 0.56, 0.4}"\
    LINE "            Rgba: rocketTrim"\
    LINE "        }"\
    LINE "    }"\
    LINE "    for i in 0..fuel {"\
    LINE "        const y = engineY + i * rkTankH"\
    LINE "        \"tank_{i}\" {"\
    LINE "            Position3: {0, y + rkTankH * 0.5, 0}"\
    LINE "            Cylinder: {segments: 20, smooth: true, length: rkTankH - 0.03}"\
    LINE "            Scale3: {rkRadius * 2, 1, rkRadius * 2}"\
    LINE "            Rgba: rocketHull"\
    LINE "        }"\
    LINE "        \"tank_stripe_{i}\" {"\
    LINE "            Position3: {0, y + rkTankH * 0.5, rkRadius + 0.01}"\
    LINE "            Box: {0.18, rkTankH * 0.58, 0.06}"\
    LINE "            Rgba: rocketTrim"\
    LINE "        }"\
    LINE "    }"\
    LINE "    for i in 0..cargo {"\
    LINE "        const y = tankY + i * rkBayH"\
    LINE "        \"bay_{i}\" {"\
    LINE "            Position3: {0, y + rkBayH * 0.5, 0}"\
    LINE "            Rotation3: {0, 0.5 * i, 0}"\
    LINE "            RocketBay: {lit: loaded}"\
    LINE "        }"\
    LINE "    }"\
    LINE "    if valid {"\
    LINE "        nose_cone {"\
    LINE "            Position3: {0, bayY + 0.57, 0}"\
    LINE "            Cone: {segments: 20, smooth: true, length: 0.88}"\
    LINE "            Rgba: rocketHull"\
    LINE "        }"\
    LINE "    }"\
    LINE "}"

#define BUNNY_TEMPLATE\
    LINE "template Bunny {"\
    LINE "    prop speed: f32 = 0"\
    LINE "    prop dead: bool = false"\
    LINE "    const ear_angle = (speed - 12) * 0.05"\
    LINE "    body {"\
    LINE "        Position3: {-0.05, 0.55, 0}"\
    LINE "        Box: {1, 0.75, 0.6}"\
    LINE "        Rgba: {240, 240, 240, 255}"\
    LINE "    }"\
    LINE "    ear_left {"\
    LINE "        Position3: {0.38, 1.54, 0.14}"\
    LINE "        Rotation3: {0, 0, ear_angle}"\
    LINE "        Box: {0.14, 0.7, 0.14}"\
    LINE "    }"\
    LINE "    ear_right {"\
    LINE "        Position3: {0.38, 1.54, -0.14}"\
    LINE "        Rotation3: {0, 0, ear_angle}"\
    LINE "        Box: {0.14, 0.7, 0.14}"\
    LINE "    }"\
    LINE "    eye_left {"\
    LINE "        Position3: {0.68, 1.18, 0.26}"\
    LINE "        if dead {"\
    LINE "            bar_a {"\
    LINE "                Rotation3: {0, 0, 3.1415926 / 4}"\
    LINE "                Box: {0.16, 0.05, 0.06}"\
    LINE "            }"\
    LINE "            bar_b {"\
    LINE "                Rotation3: {0, 0, -3.1415926 / 4}"\
    LINE "                Box: {0.16, 0.05, 0.06}"\
    LINE "            }"\
    LINE "        } else {"\
    LINE "            Box: {0.08, 0.1, 0.06}"\
    LINE "            Rgba: {40, 40, 40, 255}"\
    LINE "        }"\
    LINE "    }"\
    LINE "}"

#define GAUGE_TEMPLATE\
    LINE "const GaugeSlot = 28"\
    LINE "const GaugeBarY = 13"\
    LINE "const GaugeBarH = 12"\
    LINE "const GaugeWell: Rgba = {13, 14, 17, 255}"\
    LINE "template StatusGauge {"\
    LINE "    prop text = \"\""\
    LINE "    prop bar: f32 = 0"\
    LINE "    prop barX: f32 = 28"\
    LINE "    prop frac: f32 = 0"\
    LINE "    prop color: Rgba = {124, 226, 148, 255}"\
    LINE "    if text != \"\" {"\
    LINE "        label {"\
    LINE "            Position: {GaugeSlot, 11}"\
    LINE "            Text: {text: text, size: 15}"\
    LINE "            Rgba: color"\
    LINE "        }"\
    LINE "    }"\
    LINE "    if bar > 0 {"\
    LINE "        bg {"\
    LINE "            Position: {barX, GaugeBarY}"\
    LINE "            Rect: {bar, GaugeBarH}"\
    LINE "            Border: {radius: 2}"\
    LINE "            Rgba: GaugeWell"\
    LINE "        }"\
    LINE "        fill {"\
    LINE "            Position: {barX, GaugeBarY}"\
    LINE "            Rect: {3 + (bar - 3) * frac, GaugeBarH}"\
    LINE "            Rgba: color"\
    LINE "        }"\
    LINE "    }"\
    LINE "}"

typedef struct { float x, y; } Vec2;
typedef struct { uint8_t r, g, b, a; } RgbaValue;
typedef struct { int32_t segments; bool smooth; float length; } ShapeValue;
typedef struct { float metallic, roughness; } PbrValue;
typedef struct { float strength; RgbaValue color; } EmissiveValue;
typedef struct { char *text; int32_t size; } TextValue;
typedef struct { float width, height; } RectValue;

static ecs_world_t* scenario_world(const char *code) {
    ecs_world_t *world = ecs_init();
    test_assert(ecs_script_run_w_desc(world, NULL, code, &ir_desc, NULL) == 0);
    return world;
}

static const void* scenario_get(ecs_world_t *world, const char *path,
    const char *component)
{
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);
    ecs_entity_t c = ecs_lookup(world, component);
    test_assert(c != 0);
    return ecs_get_id(world, e, c);
}

static ecs_entity_t scenario_instantiate(ecs_world_t *world, const char *name,
    const char *tmpl_name, const char **members, const double *values,
    int32_t count)
{
    ecs_entity_t tmpl = ecs_lookup(world, tmpl_name);
    test_assert(tmpl != 0);
    const ecs_type_info_t *ti = ecs_get_type_info(world, tmpl);
    test_assert(ti != NULL);
    void *data = ecs_os_calloc(ti->size);
    ecs_ptr_init(world, tmpl, data);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, tmpl, data);
    test_assert(ecs_meta_push(&cur) == 0);
    for (int32_t i = 0; i < count; i ++) {
        test_assert(ecs_meta_member(&cur, members[i]) == 0);
        test_assert(ecs_meta_set_float(&cur, values[i]) == 0);
    }
    test_assert(ecs_meta_pop(&cur) == 0);
    ecs_entity_t e = ecs_entity(world, { .name = name });
    ecs_set_id(world, e, tmpl, (size_t)ti->size, data);
    ecs_ptr_fini(world, tmpl, data);
    ecs_os_free(data);
    return e;
}

static void scenario_set_prop(ecs_world_t *world, ecs_entity_t e,
    const char *tmpl_name, const char *member, double value)
{
    ecs_entity_t tmpl = ecs_lookup(world, tmpl_name);
    test_assert(tmpl != 0);
    const ecs_type_info_t *ti = ecs_get_type_info(world, tmpl);
    void *data = ecs_os_calloc(ti->size);
    ecs_ptr_init(world, tmpl, data);
    const void *cur_data = ecs_get_id(world, e, tmpl);
    test_assert(cur_data != NULL);
    ecs_ptr_copy(world, tmpl, data, cur_data);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, tmpl, data);
    test_assert(ecs_meta_push(&cur) == 0);
    test_assert(ecs_meta_member(&cur, member) == 0);
    test_assert(ecs_meta_set_float(&cur, value) == 0);
    test_assert(ecs_meta_pop(&cur) == 0);
    ecs_set_id(world, e, tmpl, (size_t)ti->size, data);
    ecs_ptr_fini(world, tmpl, data);
    ecs_os_free(data);
}

static void scenario_set_prop_str(ecs_world_t *world, ecs_entity_t e,
    const char *tmpl_name, const char *member, const char *value)
{
    ecs_entity_t tmpl = ecs_lookup(world, tmpl_name);
    test_assert(tmpl != 0);
    const ecs_type_info_t *ti = ecs_get_type_info(world, tmpl);
    void *data = ecs_os_calloc(ti->size);
    ecs_ptr_init(world, tmpl, data);
    const void *cur_data = ecs_get_id(world, e, tmpl);
    test_assert(cur_data != NULL);
    ecs_ptr_copy(world, tmpl, data, cur_data);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, tmpl, data);
    test_assert(ecs_meta_push(&cur) == 0);
    test_assert(ecs_meta_member(&cur, member) == 0);
    test_assert(ecs_meta_set_string(&cur, value) == 0);
    test_assert(ecs_meta_pop(&cur) == 0);
    ecs_set_id(world, e, tmpl, (size_t)ti->size, data);
    ecs_ptr_fini(world, tmpl, data);
    ecs_os_free(data);
}

static int32_t scenario_child_count(ecs_world_t *world, ecs_entity_t e) {
    int32_t count = 0;
    ecs_iter_t it = ecs_children(world, e);
    while (ecs_children_next(&it)) {
        count += it.count;
    }
    return count;
}

static const char *rocket_members[] = {
    "engines", "fuel", "cargo", "loaded", "valid"
};

void Scenario_rocket_instantiate(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES ROCKET_TEMPLATES);

    double values[] = { 6, 2, 2, 1, 1 };
    ecs_entity_t r = scenario_instantiate(
        world, "r", "Rocket", rocket_members, values, 5);
    test_assert(r != 0);

    test_int(scenario_child_count(world, r), 1 + 4 + 4 + 2 + 1);

    const Vec3 *p = scenario_get(world, "r.skirt", "Position3");
    test_assert(p != NULL);
    test_flt(p->y, 0.44);

    const ShapeValue *cyl = scenario_get(world, "r.tank_1", "Cylinder");
    test_assert(cyl != NULL);
    test_int(cyl->segments, 20);
    test_bool(cyl->smooth, true);
    test_flt(cyl->length, 1.02 - 0.03);

    p = scenario_get(world, "r.tank_1", "Position3");
    test_assert(p != NULL);
    test_flt(p->y, 0.62 + 1.02 + 1.02 * 0.5);

    p = scenario_get(world, "r.bay_1", "Position3");
    test_assert(p != NULL);
    test_flt(p->y, 0.62 + 2 * 1.02 + 0.78 + 0.78 * 0.5);

    const Vec3 *rot = scenario_get(world, "r.bay_1", "Rotation3");
    test_assert(rot != NULL);
    test_flt(rot->y, 0.5);

    ecs_entity_t bay = ecs_lookup(world, "r.bay_1");
    test_int(scenario_child_count(world, bay), 1 + 4);

    const Vec3 *port = scenario_get(world, "r.bay_1.port_1", "Position3");
    test_assert(port != NULL);
    test_flt(port->x, 1.5708 * (0.42 + 0.06));
    test_flt(port->z, (0.42 + 0.06) - 1.5708);

    ecs_entity_t port_e = ecs_lookup(world, "r.bay_1.port_1");
    test_int(scenario_child_count(world, port_e), 2);
    const EmissiveValue *em = scenario_get(
        world, "r.bay_1.port_1.glow", "Emissive");
    test_assert(em != NULL);
    test_flt(em->strength, 2);
    test_int(em->color.r, 255);

    const RgbaValue *fin = scenario_get(world, "r.fin_3", "Rgba");
    test_assert(fin != NULL);
    test_int(fin->r, 238);

    p = scenario_get(world, "r.nose_cone", "Position3");
    test_assert(p != NULL);
    test_flt(p->y, 0.62 + 2 * 1.02 + 2 * 0.78 + 0.57);

    ecs_fini(world);
}

void Scenario_rocket_partial_loaded(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES ROCKET_TEMPLATES);

    double values[] = { 6, 2, 2, 1, 1 };
    ecs_entity_t r = scenario_instantiate(
        world, "r", "Rocket", rocket_members, values, 5);

    ecs_entity_t bay = ecs_lookup(world, "r.bay_0");
    ecs_entity_t port = ecs_lookup(world, "r.bay_0.port_2");
    ecs_entity_t frame = ecs_lookup(world, "r.bay_0.port_2.frame");
    ecs_entity_t tank = ecs_lookup(world, "r.tank_0");
    test_assert(bay != 0 && port != 0 && frame != 0 && tank != 0);
    test_assert(ecs_lookup(world, "r.bay_0.port_2.glow") != 0);

    scenario_set_prop(world, r, "Rocket", "loaded", 0);

    test_assert(ecs_lookup(world, "r.bay_0") == bay);
    test_assert(ecs_lookup(world, "r.bay_0.port_2") == port);
    test_assert(ecs_lookup(world, "r.bay_0.port_2.frame") == frame);
    test_assert(ecs_lookup(world, "r.tank_0") == tank);
    test_assert(ecs_lookup(world, "r.bay_0.port_2.glow") == 0);
    test_assert(ecs_lookup(world, "r.bay_1.port_0.glow") == 0);
    test_int(scenario_child_count(world, r), 1 + 4 + 4 + 2 + 1);

    scenario_set_prop(world, r, "Rocket", "loaded", 1);

    test_assert(ecs_lookup(world, "r.bay_0.port_2") == port);
    test_assert(ecs_lookup(world, "r.bay_0.port_2.frame") == frame);
    ecs_entity_t glow = ecs_lookup(world, "r.bay_0.port_2.glow");
    test_assert(glow != 0);
    const EmissiveValue *em = ecs_get_id(world, glow,
        ecs_lookup(world, "Emissive"));
    test_assert(em != NULL);
    test_flt(em->strength, 2);

    ecs_fini(world);
}

void Scenario_rocket_partial_fuel(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES ROCKET_TEMPLATES);

    double values[] = { 6, 2, 2, 1, 1 };
    ecs_entity_t r = scenario_instantiate(
        world, "r", "Rocket", rocket_members, values, 5);

    ecs_entity_t skirt = ecs_lookup(world, "r.skirt");
    ecs_entity_t tank0 = ecs_lookup(world, "r.tank_0");
    ecs_entity_t bay0 = ecs_lookup(world, "r.bay_0");
    test_assert(skirt != 0 && tank0 != 0 && bay0 != 0);

    const Vec3 *p = scenario_get(world, "r.bay_0", "Position3");
    test_flt(p->y, 0.62 + 2 * 1.02 + 0.78 * 0.5);

    scenario_set_prop(world, r, "Rocket", "fuel", 3);

    test_assert(ecs_lookup(world, "r.skirt") == skirt);
    test_assert(ecs_lookup(world, "r.tank_0") == tank0);
    test_assert(ecs_lookup(world, "r.tank_2") != 0);
    test_assert(ecs_lookup(world, "r.tank_stripe_2") != 0);
    test_assert(ecs_lookup(world, "r.bay_0") == bay0);
    test_int(scenario_child_count(world, r), 1 + 4 + 6 + 2 + 1);

    p = scenario_get(world, "r.bay_0", "Position3");
    test_flt(p->y, 0.62 + 3 * 1.02 + 0.78 * 0.5);
    p = scenario_get(world, "r.nose_cone", "Position3");
    test_flt(p->y, 0.62 + 3 * 1.02 + 2 * 0.78 + 0.57);

    scenario_set_prop(world, r, "Rocket", "fuel", 1);

    test_assert(ecs_lookup(world, "r.tank_0") == tank0);
    test_assert(ecs_lookup(world, "r.tank_1") == 0);
    test_assert(ecs_lookup(world, "r.tank_2") == 0);
    test_assert(ecs_lookup(world, "r.tank_stripe_1") == 0);
    test_int(scenario_child_count(world, r), 1 + 4 + 2 + 2 + 1);

    p = scenario_get(world, "r.bay_0", "Position3");
    test_flt(p->y, 0.62 + 1 * 1.02 + 0.78 * 0.5);

    ecs_fini(world);
}

void Scenario_rocket_partial_engines(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES ROCKET_TEMPLATES);

    double values[] = { 6, 2, 2, 1, 1 };
    ecs_entity_t r = scenario_instantiate(
        world, "r", "Rocket", rocket_members, values, 5);

    test_assert(ecs_lookup(world, "r.skirt") != 0);
    test_assert(ecs_lookup(world, "r.fin_3") != 0);
    ecs_entity_t tank0 = ecs_lookup(world, "r.tank_0");

    scenario_set_prop(world, r, "Rocket", "engines", 0);

    test_assert(ecs_lookup(world, "r.skirt") == 0);
    test_assert(ecs_lookup(world, "r.fin_0") == 0);
    test_assert(ecs_lookup(world, "r.fin_3") == 0);
    test_assert(ecs_lookup(world, "r.tank_0") == tank0);
    test_int(scenario_child_count(world, r), 2 * 2 + 2 + 1);

    const Vec3 *p = scenario_get(world, "r.tank_0", "Position3");
    test_flt(p->y, 1.02 * 0.5);

    scenario_set_prop(world, r, "Rocket", "engines", 2);

    test_assert(ecs_lookup(world, "r.skirt") != 0);
    test_assert(ecs_lookup(world, "r.fin_3") != 0);
    p = scenario_get(world, "r.tank_0", "Position3");
    test_flt(p->y, 0.62 + 1.02 * 0.5);

    ecs_fini(world);
}

void Scenario_rocket_partial_valid_toggle_twice(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES ROCKET_TEMPLATES);

    double values[] = { 6, 2, 2, 1, 1 };
    ecs_entity_t r = scenario_instantiate(
        world, "r", "Rocket", rocket_members, values, 5);

    ecs_entity_t nose = ecs_lookup(world, "r.nose_cone");
    test_assert(nose != 0);

    scenario_set_prop(world, r, "Rocket", "valid", 0);
    test_assert(ecs_lookup(world, "r.nose_cone") == 0);
    test_assert(!ecs_is_alive(world, nose));

    scenario_set_prop(world, r, "Rocket", "valid", 1);
    nose = ecs_lookup(world, "r.nose_cone");
    test_assert(nose != 0);
    const ShapeValue *cone = scenario_get(world, "r.nose_cone", "Cone");
    test_assert(cone != NULL);
    test_flt(cone->length, 0.88);

    scenario_set_prop(world, r, "Rocket", "valid", 0);
    test_assert(ecs_lookup(world, "r.nose_cone") == 0);
    test_int(scenario_child_count(world, r), 1 + 4 + 4 + 2);

    ecs_fini(world);
}

void Scenario_rocket_many_instances_independent(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES ROCKET_TEMPLATES);

    ecs_entity_t rockets[8];
    for (int i = 0; i < 8; i ++) {
        char name[16];
        ecs_os_snprintf(name, 16, "r%d", i);
        double values[] = { 6, i % 3, 1, i & 1, 1 };
        rockets[i] = scenario_instantiate(
            world, name, "Rocket", rocket_members, values, 5);
        test_assert(rockets[i] != 0);
    }

    for (int i = 0; i < 8; i ++) {
        test_int(scenario_child_count(world, rockets[i]),
            1 + 4 + 2 * (i % 3) + 1 + 1);
        char path[32];
        ecs_os_snprintf(path, 32, "r%d.bay_0.port_0.glow", i);
        test_bool(ecs_lookup(world, path) != 0, (i & 1) != 0);
    }

    scenario_set_prop(world, rockets[3], "Rocket", "fuel", 2);
    for (int i = 0; i < 8; i ++) {
        int32_t fuel = i == 3 ? 2 : (i % 3);
        test_int(scenario_child_count(world, rockets[i]),
            1 + 4 + 2 * fuel + 1 + 1);
    }

    ecs_fini(world);
}

void Scenario_bunny_partial_value(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES BUNNY_TEMPLATE);

    const char *members[] = { "speed" };
    double values[] = { 5 };
    ecs_entity_t b = scenario_instantiate(
        world, "b", "Bunny", members, values, 1);
    test_assert(b != 0);
    test_int(scenario_child_count(world, b), 4);

    ecs_entity_t ear = ecs_lookup(world, "b.ear_left");
    ecs_entity_t body = ecs_lookup(world, "b.body");
    test_assert(ear != 0 && body != 0);

    const Vec3 *rot = scenario_get(world, "b.ear_left", "Rotation3");
    test_flt(rot->z, -0.35);

    scenario_set_prop(world, b, "Bunny", "speed", 20);

    test_assert(ecs_lookup(world, "b.ear_left") == ear);
    test_assert(ecs_lookup(world, "b.body") == body);
    rot = scenario_get(world, "b.ear_left", "Rotation3");
    test_flt(rot->z, 0.4);
    rot = scenario_get(world, "b.ear_right", "Rotation3");
    test_flt(rot->z, 0.4);
    const Vec3 *p = scenario_get(world, "b.body", "Position3");
    test_flt(p->y, 0.55);

    scenario_set_prop(world, b, "Bunny", "speed", 100);
    rot = scenario_get(world, "b.ear_left", "Rotation3");
    test_flt(rot->z, 4.4);

    ecs_fini(world);
}

void Scenario_bunny_partial_branch(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES BUNNY_TEMPLATE);

    const char *members[] = { "speed" };
    double values[] = { 5 };
    ecs_entity_t b = scenario_instantiate(
        world, "b", "Bunny", members, values, 1);

    ecs_entity_t eye = ecs_lookup(world, "b.eye_left");
    test_assert(eye != 0);
    ecs_entity_t box = ecs_lookup(world, "Box");
    ecs_entity_t rgba = ecs_lookup(world, "Rgba");
    test_assert(ecs_has_id(world, eye, box));
    test_assert(ecs_has_id(world, eye, rgba));
    test_int(scenario_child_count(world, eye), 0);

    scenario_set_prop(world, b, "Bunny", "dead", 1);

    test_assert(ecs_lookup(world, "b.eye_left") == eye);
    test_assert(!ecs_has_id(world, eye, box));
    test_assert(!ecs_has_id(world, eye, rgba));
    test_int(scenario_child_count(world, eye), 2);
    const Vec3 *rot = scenario_get(world, "b.eye_left.bar_a", "Rotation3");
    test_assert(rot != NULL);
    test_flt(rot->z, 3.1415926 / 4);

    scenario_set_prop(world, b, "Bunny", "dead", 0);

    test_assert(ecs_lookup(world, "b.eye_left") == eye);
    test_assert(ecs_has_id(world, eye, box));
    test_assert(ecs_has_id(world, eye, rgba));
    test_int(scenario_child_count(world, eye), 0);
    test_assert(ecs_lookup(world, "b.eye_left.bar_a") == 0);

    ecs_fini(world);
}

void Scenario_gauge_partial_text_and_frac(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES GAUGE_TEMPLATE);

    const char *members[] = { "bar", "frac" };
    double values[] = { 80, 0.5 };
    ecs_entity_t g = scenario_instantiate(
        world, "g", "StatusGauge", members, values, 2);
    test_assert(g != 0);

    test_assert(ecs_lookup(world, "g.label") == 0);
    ecs_entity_t fill = ecs_lookup(world, "g.fill");
    ecs_entity_t bg = ecs_lookup(world, "g.bg");
    test_assert(fill != 0 && bg != 0);

    const RectValue *rect = scenario_get(world, "g.fill", "Rect");
    test_flt(rect->width, 3 + (80 - 3) * 0.5);

    scenario_set_prop(world, g, "StatusGauge", "frac", 0.25);
    test_assert(ecs_lookup(world, "g.fill") == fill);
    test_assert(ecs_lookup(world, "g.bg") == bg);
    rect = scenario_get(world, "g.fill", "Rect");
    test_flt(rect->width, 3 + (80 - 3) * 0.25);

    scenario_set_prop_str(world, g, "StatusGauge", "text", "ore");
    ecs_entity_t label = ecs_lookup(world, "g.label");
    test_assert(label != 0);
    const TextValue *text = scenario_get(world, "g.label", "Text");
    test_str(text->text, "ore");
    test_int(text->size, 15);
    test_assert(ecs_lookup(world, "g.fill") == fill);

    scenario_set_prop_str(world, g, "StatusGauge", "text", "ore 2");
    test_assert(ecs_lookup(world, "g.label") == label);
    text = scenario_get(world, "g.label", "Text");
    test_str(text->text, "ore 2");

    scenario_set_prop_str(world, g, "StatusGauge", "text", "");
    test_assert(ecs_lookup(world, "g.label") == 0);
    test_assert(ecs_lookup(world, "g.fill") == fill);

    scenario_set_prop(world, g, "StatusGauge", "bar", 0);
    test_assert(ecs_lookup(world, "g.fill") == 0);
    test_assert(ecs_lookup(world, "g.bg") == 0);
    test_int(scenario_child_count(world, g), 0);

    ecs_fini(world);
}

void Scenario_nested_template_reeval_keeps_children(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES ROCKET_TEMPLATES);

    const char *members[] = { "lit" };
    double values[] = { 1 };
    ecs_entity_t bay = scenario_instantiate(
        world, "bay", "RocketBay", members, values, 1);
    test_assert(bay != 0);

    ecs_entity_t hull = ecs_lookup(world, "bay.hull");
    ecs_entity_t ports[4];
    for (int i = 0; i < 4; i ++) {
        char path[32];
        ecs_os_snprintf(path, 32, "bay.port_%d", i);
        ports[i] = ecs_lookup(world, path);
        test_assert(ports[i] != 0);
        ecs_os_snprintf(path, 32, "bay.port_%d.glow", i);
        test_assert(ecs_lookup(world, path) != 0);
    }

    for (int round = 0; round < 3; round ++) {
        scenario_set_prop(world, bay, "RocketBay", "lit", 0);
        test_assert(ecs_lookup(world, "bay.hull") == hull);
        for (int i = 0; i < 4; i ++) {
            char path[32];
            ecs_os_snprintf(path, 32, "bay.port_%d", i);
            test_assert(ecs_lookup(world, path) == ports[i]);
            ecs_os_snprintf(path, 32, "bay.port_%d.glow", i);
            test_assert(ecs_lookup(world, path) == 0);
        }
        scenario_set_prop(world, bay, "RocketBay", "lit", 1);
        for (int i = 0; i < 4; i ++) {
            char path[32];
            ecs_os_snprintf(path, 32, "bay.port_%d", i);
            test_assert(ecs_lookup(world, path) == ports[i]);
            ecs_os_snprintf(path, 32, "bay.port_%d.glow", i);
            test_assert(ecs_lookup(world, path) != 0);
        }
    }

    ecs_fini(world);
}

void Scenario_template_tree_annotation_applies_to_instances(void) {
    ecs_world_t *world = scenario_world(SCENARIO_TYPES ROCKET_TEMPLATES);

    double values[] = { 2, 1, 1, 1, 1 };
    ecs_entity_t r = scenario_instantiate(
        world, "r", "Rocket", rocket_members, values, 5);

    ecs_entity_t skirt = ecs_lookup(world, "r.skirt");
    test_assert(skirt != 0);
    test_assert(ecs_get_parent(world, skirt) == r);
    test_assert(ecs_get_parent(world,
        ecs_lookup(world, "r.bay_0.port_0.frame")) ==
        ecs_lookup(world, "r.bay_0.port_0"));

    ecs_entity_t r2 = scenario_instantiate(
        world, "r2", "Rocket", rocket_members, values, 5);
    test_assert(ecs_lookup(world, "r2.skirt") != 0);
    test_assert(ecs_lookup(world, "r.skirt") == skirt);

    scenario_set_prop(world, r, "Rocket", "engines", 0);
    test_assert(ecs_lookup(world, "r.skirt") == 0);
    test_assert(ecs_lookup(world, "r2.skirt") != 0);

    ecs_fini(world);
}

void Scenario_ir_and_ast_instances_coexist(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_eval_desc_t ast_desc = { .ir = false };
    ecs_script_eval_desc_t ir_only_desc = { .ir = true };
    test_assert(ecs_script_run_w_desc(world, NULL,
        SCENARIO_TYPES BUNNY_TEMPLATE, &ast_desc, NULL) == 0);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template Bunny2 {"
        LINE "    prop speed: f32 = 0"
        LINE "    b { Bunny: {speed: speed} }"
        LINE "}", &ir_only_desc, NULL) == 0);

    const char *members[] = { "speed" };
    double values[] = { 20 };
    ecs_entity_t b = scenario_instantiate(
        world, "b", "Bunny2", members, values, 1);
    test_assert(b != 0);

    const Vec3 *rot = scenario_get(world, "b.b.ear_left", "Rotation3");
    test_assert(rot != NULL);
    test_flt(rot->z, 0.4);

    scenario_set_prop(world, b, "Bunny2", "speed", 5);
    rot = scenario_get(world, "b.b.ear_left", "Rotation3");
    test_flt(rot->z, -0.35);

    ecs_fini(world);
}
