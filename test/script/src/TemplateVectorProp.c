#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void TemplateVectorProp_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

typedef struct {
    float x;
    float y;
} PositionValue;

void TemplateVectorProp_declaration_w_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[0]: {height: 10} }"
    LINE "  b { facade[1]: {height: 20} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.a");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 10);
    test_flt(p->y, 1);

    e = ecs_lookup(world, "e.b");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 20);
    test_flt(p->y, 2);

    ecs_entity_t building = ecs_lookup(world, "Building");
    test_assert(building != 0);
    const EcsStruct *st = ecs_get(world, building, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "facade");
    test_assert(ecs_has(world, m->type, EcsVector));

    ecs_fini(world);
}

void TemplateVectorProp_declaration_wo_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[]"
    LINE "  Position: {facade.count(), 0}"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 0);
    test_flt(p->y, 0);

    ecs_fini(world);
}

void TemplateVectorProp_count(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  Position: {facade.count(), 0}"
    LINE "}"
    LINE "Building e()"
    LINE "f { Building: {facade: [BrickFacade]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    test_flt(p->y, 0);

    e = ecs_lookup(world, "f");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 0);

    ecs_fini(world);
}

void TemplateVectorProp_index_instantiate_w_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  prop floors: i32 = 3"
    LINE "  for i in 0..floors {"
    LINE "    \"floor_{i}\" {"
    LINE "      facade[i % facade.count()]: {height: i * 3}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.floor_0");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 0);
    test_flt(p->y, 1);

    e = ecs_lookup(world, "e.floor_1");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 3);
    test_flt(p->y, 2);

    e = ecs_lookup(world, "e.floor_2");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 6);
    test_flt(p->y, 1);

    ecs_fini(world);
}

void TemplateVectorProp_index_tag_form(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[0] }"
    LINE "  b {"
    LINE "    facade[1]"
    LINE "  }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t brick = ecs_lookup(world, "BrickFacade");
    ecs_entity_t glass = ecs_lookup(world, "GlassFacade");
    test_assert(brick != 0);
    test_assert(glass != 0);

    ecs_entity_t a = ecs_lookup(world, "e.a");
    ecs_entity_t b = ecs_lookup(world, "e.b");
    test_assert(a != 0);
    test_assert(b != 0);

    const float *av = ecs_get_id(world, a, brick);
    test_assert(av != NULL);
    test_flt(*av, 0);
    test_assert(!ecs_has_id(world, a, glass));

    const float *bv = ecs_get_id(world, b, glass);
    test_assert(bv != NULL);
    test_flt(*bv, 0);
    test_assert(!ecs_has_id(world, b, brick));

    ecs_fini(world);
}

void TemplateVectorProp_pass_element_to_interface_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Wall {"
    LINE "  prop facade: template Facade"
    LINE "  side { facade: {height: 7} }"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  wall { Wall: {facade: facade[1]} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.wall.side");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 7);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateVectorProp_iterate_w_for(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  for (i, f) in facade {"
    LINE "    \"side_{i}\" { f: {height: 5} }"
    LINE "  }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.side_0");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 5);
    test_flt(p->y, 1);

    e = ecs_lookup(world, "e.side_1");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 5);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateVectorProp_index_by_rng(void) {
    ecs_world_t *world = ecs_init();

#ifdef FLECS_SCRIPT_MATH
    ECS_IMPORT(world, FlecsScriptMath);

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  const rng: flecs.script.math.Rng = {seed: 7}"
    LINE "  for i in 0..4 {"
    LINE "    \"floor_{i}\" {"
    LINE "      facade[rng.u(facade.count())]: {height: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Building e()";
#else
    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  for i in 0..4 {"
    LINE "    \"floor_{i}\" {"
    LINE "      facade[(i * 7 + 3) % facade.count()]: {height: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Building e()";
#endif

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    int32_t i;
    for (i = 0; i < 4; i ++) {
        char name[32];
        ecs_os_snprintf(name, sizeof(name), "e.floor_%d", i);
        ecs_entity_t e = ecs_lookup(world, name);
        test_assert(e != 0);
        const PositionValue *p = ecs_get_id(world, e, position);
        test_assert(p != NULL);
        test_flt(p->x, 2);
        test_assert(p->y == 1 || p->y == 2);
    }

    ecs_fini(world);
}

void TemplateVectorProp_override_kind_syntax(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[0]: {height: 1} }"
    LINE "  b { facade[1]: {height: 2} }"
    LINE "}"
    LINE "Building e(facade: [GlassFacade, BrickFacade])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.a");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    e = ecs_lookup(world, "e.b");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 2);
    test_flt(p->y, 1);

    ecs_fini(world);
}

void TemplateVectorProp_override_component_syntax(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[0]: {height: 1} }"
    LINE "}"
    LINE "e { Building: {facade: [WoodFacade]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.a");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 3);

    ecs_fini(world);
}

void TemplateVectorProp_native_set_id(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade[0]: {height: 4} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t building = ecs_lookup(world, "Building");
    ecs_entity_t glass = ecs_lookup(world, "GlassFacade");
    test_assert(building != 0);
    test_assert(glass != 0);

    const ecs_type_info_t *ti = ecs_get_type_info(world, building);
    test_assert(ti != NULL);
    test_int(ti->size, ECS_SIZEOF(ecs_vec_t));

    ecs_vec_t vec;
    ecs_vec_init_t(NULL, &vec, ecs_script_template_ref_t, 1);
    ecs_vec_append_t(NULL, &vec, ecs_script_template_ref_t)[0] =
        (ecs_script_template_ref_t){ .type = glass };

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set_id(world, e, building, ECS_SIZEOF(ecs_vec_t), &vec);

    ecs_vec_fini_t(NULL, &vec, ecs_script_template_ref_t);

    ecs_entity_t inst = ecs_lookup(world, "e.a");
    test_assert(inst != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, inst, position);
    test_assert(p != NULL);
    test_flt(p->x, 4);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateVectorProp_expr_run_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade[0]: {height: 8} }"
    LINE "  b { facade[1]: {height: 9} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t building = ecs_lookup(world, "Building");
    test_assert(building != 0);

    const ecs_type_info_t *ti = ecs_get_type_info(world, building);
    test_assert(ti != NULL);

    void *ptr = ecs_os_calloc(ti->size);
    ecs_value_t value = { .type = building, .ptr = ptr };
    test_assert(ecs_expr_run(world,
        "{facade: [BrickFacade, GlassFacade]}", &value, NULL) != NULL);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set_id(world, e, building, (size_t)ti->size, ptr);

    if (ti->hooks.dtor) {
        ti->hooks.dtor(ptr, 1, ti);
    }
    ecs_os_free(ptr);

    ecs_entity_t inst = ecs_lookup(world, "e.a");
    test_assert(inst != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, inst, position);
    test_assert(p != NULL);
    test_flt(p->x, 8);
    test_flt(p->y, 1);

    inst = ecs_lookup(world, "e.b");
    test_assert(inst != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, inst, position);
    test_assert(p != NULL);
    test_flt(p->x, 9);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateVectorProp_index_out_of_range_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade[2]: {height: 1} }"
    LINE "}"
    LINE "Building e()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateVectorProp_index_empty_vector_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[]"
    LINE "  a { facade[0]: {height: 1} }"
    LINE "}"
    LINE "Building e()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateVectorProp_default_not_derived_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Unrelated {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, Unrelated]"
    LINE "  a { facade[0]: {height: 1} }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateVectorProp_value_not_derived_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Unrelated {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade[0]: {height: 1} }"
    LINE "}"
    LINE "e { Building: {facade: [Unrelated]} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateVectorProp_value_not_derived_from_c_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Unrelated {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade[0]: {height: 1} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t building = ecs_lookup(world, "Building");
    ecs_entity_t unrelated = ecs_lookup(world, "Unrelated");
    test_assert(building != 0);
    test_assert(unrelated != 0);

    ecs_vec_t vec;
    ecs_vec_init_t(NULL, &vec, ecs_script_template_ref_t, 1);
    ecs_vec_append_t(NULL, &vec, ecs_script_template_ref_t)[0] =
        (ecs_script_template_ref_t){ .type = unrelated };

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_log_set_level(-4);
    ecs_set_id(world, e, building, ECS_SIZEOF(ecs_vec_t), &vec);
    ecs_log_set_level(-1);

    ecs_vec_fini_t(NULL, &vec, ecs_script_template_ref_t);

    test_assert(ecs_lookup(world, "e.a") == 0);

    ecs_fini(world);
}

void TemplateVectorProp_template_type_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Building {"
    LINE "  prop points: template Point[] = [Point(x: 5), Point]"
    LINE "  a { points[0] }"
    LINE "  b { points[1] }"
    LINE "}"
    LINE "Building e";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t point = ecs_lookup(world, "Point");
    test_assert(point != 0);

    const float *a = ecs_get_id(world, ecs_lookup(world, "e.a"), point);
    test_assert(a != NULL);
    test_flt(a[0], 5);

    const float *b = ecs_get_id(world, ecs_lookup(world, "e.b"), point);
    test_assert(b != NULL);
    test_flt(b[0], 1);

    ecs_fini(world);
}

void TemplateVectorProp_bare_vector_prop_as_component_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade: {height: 1} }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateVectorProp_reactive_prop_change(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade[0]: {height: 6} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t inst = ecs_lookup(world, "e.a");
    test_assert(inst != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, inst, position);
    test_assert(p != NULL);
    test_flt(p->x, 6);
    test_flt(p->y, 1);

    ecs_entity_t building = ecs_lookup(world, "Building");
    ecs_entity_t glass = ecs_lookup(world, "GlassFacade");
    test_assert(building != 0);
    test_assert(glass != 0);

    ecs_vec_t vec;
    ecs_vec_init_t(NULL, &vec, ecs_entity_t, 1);
    ecs_vec_append_t(NULL, &vec, ecs_entity_t)[0] = glass;

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_set_id(world, e, building, ECS_SIZEOF(ecs_vec_t), &vec);
    ecs_vec_fini_t(NULL, &vec, ecs_entity_t);

    inst = ecs_lookup(world, "e.a");
    test_assert(inst != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, inst, position);
    test_assert(p != NULL);
    test_flt(p->x, 6);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateVectorProp_reactive_prop_change_from_script(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade[0]: {height: 6} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.a");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 6);
    test_flt(p->y, 1);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Building: {facade: [WoodFacade]} }", &ir_desc, NULL) == 0);

    e = ecs_lookup(world, "e.a");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 6);
    test_flt(p->y, 3);

    ecs_fini(world);
}

void TemplateVectorProp_nested_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Wing {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  side { facade[0]: {height: 11} }"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [GlassFacade, WoodFacade]"
    LINE "  left { Wing: {facade: [facade[0]]} }"
    LINE "  right { Wing: {facade: [facade[1]]} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.left.side");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 11);
    test_flt(p->y, 2);

    e = ecs_lookup(world, "e.right.side");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 11);
    test_flt(p->y, 3);

    ecs_fini(world);
}

void TemplateVectorProp_template_inheritance(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "struct Base(scale: f32)"
    LINE "template BaseBuilding : Base {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "}"
    LINE "template Tower : BaseBuilding {"
    LINE "  a { facade[0]: {height: scale} }"
    LINE "}"
    LINE "e { Tower: {scale: 12} }"
    LINE "f { Tower: {scale: 13, facade: [GlassFacade]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.a");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 12);
    test_flt(p->y, 1);

    e = ecs_lookup(world, "f.a");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 13);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateVectorProp_index_in_interpolated_string(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  \"named_{facade[1]}\" {}"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "e.named_GlassFacade") != 0);

    ecs_fini(world);
}

void TemplateVectorProp_collection_value_still_parses(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Vec" }),
        .type = ecs_id(ecs_i32_t)
    });
    test_assert(vec != 0);

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[1]: {height: 3} }"
    LINE "  Vec: [10, 20, 30]"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t inst = ecs_lookup(world, "e.a");
    test_assert(inst != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, inst, position);
    test_assert(p != NULL);
    test_flt(p->x, 3);
    test_flt(p->y, 2);

    ecs_entity_t e = ecs_lookup(world, "e");
    const ecs_vec_t *v = ecs_get_id(world, e, vec);
    test_assert(v != NULL);
    test_int(ecs_vec_count(v), 3);
    test_int(ecs_vec_get_t(v, int32_t, 0)[0], 10);
    test_int(ecs_vec_get_t(v, int32_t, 2)[0], 30);

    ecs_fini(world);
}

void TemplateVectorProp_to_str(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "struct Facade(height: f32)"
        LINE "template BrickFacade : Facade {"
        LINE "  Position: {$height, 1}"
        LINE "}"
        LINE "template GlassFacade : Facade {"
        LINE "  Position: {$height, 2}"
        LINE "}"
        LINE "template WoodFacade : Facade {"
        LINE "  Position: {$height, 3}"
        LINE "}"
        LINE "template Building {"
        LINE "  prop facade: template Facade[] = [BrickFacade]"
        LINE "  a { facade[0]: {height: 1} }"
        LINE "}", &ir_desc, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "template Facade[]") != NULL);
    test_assert(strstr(str, "facade[0]") != NULL);
    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void TemplateVectorProp_index_tag_form_instantiates(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[0] }"
    LINE "  b { facade[1] }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.a");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 0);
    test_flt(p->y, 1);

    e = ecs_lookup(world, "e.b");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 0);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void TemplateVectorProp_index_empty_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "struct Facade(height: f32)"
    LINE "template BrickFacade : Facade {"
    LINE "  Position: {$height, 1}"
    LINE "}"
    LINE "template GlassFacade : Facade {"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "template WoodFacade : Facade {"
    LINE "  Position: {$height, 3}"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[0]: {} }"
    LINE "  b { facade[1]: {} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.a");
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 0);
    test_flt(p->y, 1);

    e = ecs_lookup(world, "e.b");
    test_assert(e != 0);

    position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, 0);
    test_flt(p->y, 2);

    ecs_fini(world);
}

#define FACADE_HEAD \
    HEAD "struct Position(x: f32, y: f32)" \
    LINE "struct Facade(height: f32)" \
    LINE "template BrickFacade : Facade {" \
    LINE "  prop scale: f32 = 1" \
    LINE "  Position: {$height, $scale}" \
    LINE "}" \
    LINE "template GlassFacade : Facade {" \
    LINE "  prop scale: f32 = 2" \
    LINE "  Position: {$height, $scale}" \
    LINE "}"

typedef struct {
    float height;
    float scale;
} BrickFacadeValue;

static void test_position(
    ecs_world_t *world,
    const char *path,
    float x,
    float y)
{
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);
    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, x);
    test_flt(p->y, y);
}

void TemplateVectorProp_template_ref_call_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade(scale: 5), GlassFacade]"
    LINE "  a { facade[0]: {height: 10} }"
    LINE "  b { facade[1]: {height: 20} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_position(world, "e.a", 10, 5);
    test_position(world, "e.b", 20, 2);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade(0, 5), GlassFacade(0, 6)]"
    LINE "  a { facade[0]: {height: 10} }"
    LINE "  b { facade[1]: {height: 20} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_position(world, "e.a", 10, 5);
    test_position(world, "e.b", 20, 6);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_instance(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Building {"
    LINE "  prop facade: template Facade[]"
    LINE "  a { facade[0]: {height: 10} }"
    LINE "  b { facade[1]: {height: 20} }"
    LINE "}"
    LINE "Building e(facade: [BrickFacade(scale: 3), GlassFacade(scale: 4)])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_position(world, "e.a", 10, 3);
    test_position(world, "e.b", 20, 4);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t building = ecs_lookup(world, "Building");
    const ecs_vec_t *vec = ecs_get_id(world, e, building);
    test_assert(vec != NULL);
    test_int(ecs_vec_count(vec), 2);
    const ecs_script_template_ref_t *refs = ecs_vec_first(vec);
    test_assert(refs[0].type == ecs_lookup(world, "BrickFacade"));
    test_assert(refs[0].value != NULL);
    test_flt(((BrickFacadeValue*)refs[0].value)->scale, 3);
    test_assert(refs[1].type == ecs_lookup(world, "GlassFacade"));
    test_assert(refs[1].value != NULL);
    test_flt(((BrickFacadeValue*)refs[1].value)->scale, 4);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_tag_form(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade(scale: 5)]"
    LINE "  a { facade[0] }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_position(world, "e.a", 0, 5);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_iterate_w_for(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade(scale: 7), GlassFacade(scale: 8)]"
    LINE "  for (i, f) in facade {"
    LINE "    \"side_{i}\" { f: {height: 5} }"
    LINE "  }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_position(world, "e.side_0", 5, 7);
    test_position(world, "e.side_1", 5, 8);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_pass_element_to_interface_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Wall {"
    LINE "  prop facade: template Facade"
    LINE "  side { facade: {height: 7} }"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade(scale: 9)]"
    LINE "  wall { Wall: {facade: facade[1]} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_position(world, "e.wall.side", 7, 9);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_index_in_interpolated_string(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade(scale: 9)]"
    LINE "  \"named_{facade[1]}\" {}"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "e.named_GlassFacade") != 0);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Building {"
    LINE "  prop facade: template Facade[]"
    LINE "  a { facade[0]: {height: 10} }"
    LINE "}"
    LINE "Building e(facade: [BrickFacade(scale: 3)])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_position(world, "e.a", 10, 3);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "Building e(facade: [BrickFacade(scale: 4)])", &ir_desc, NULL) == 0);

    test_position(world, "e.a", 10, 4);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "Building e(facade: [GlassFacade()])", &ir_desc, NULL) == 0);

    test_position(world, "e.a", 10, 2);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_from_c_w_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Building {"
    LINE "  prop facade: template Facade[]"
    LINE "  a { facade[0]: {height: 4} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t building = ecs_lookup(world, "Building");
    ecs_entity_t brick = ecs_lookup(world, "BrickFacade");
    test_assert(building != 0);
    test_assert(brick != 0);

    BrickFacadeValue *value = ecs_ptr_new(world, brick);
    test_assert(value != NULL);
    value->scale = 6;

    ecs_vec_t vec;
    ecs_vec_init_t(NULL, &vec, ecs_script_template_ref_t, 1);
    ecs_vec_append_t(NULL, &vec, ecs_script_template_ref_t)[0] =
        (ecs_script_template_ref_t){ .type = brick, .value = value };

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set_id(world, e, building, ECS_SIZEOF(ecs_vec_t), &vec);

    ecs_vec_fini_t(NULL, &vec, ecs_script_template_ref_t);
    ecs_ptr_free(world, brick, value);

    test_position(world, "e.a", 4, 6);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_nested(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Street {"
    LINE "  prop facades: template Facade[]"
    LINE "  for (i, f) in facades {"
    LINE "    \"b_{i}\" { f: {height: 3} }"
    LINE "  }"
    LINE "}"
    LINE "template City {"
    LINE "  prop street: template Street"
    LINE "  s { street }"
    LINE "}"
    LINE "City c(street: Street(facades: [BrickFacade(scale: 4), GlassFacade(scale: 5)]))"
    LINE "City d(street: Street([GlassFacade(), BrickFacade(0, 6)]))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_position(world, "c.s.b_0", 3, 4);
    test_position(world, "c.s.b_1", 3, 5);
    test_position(world, "d.s.b_0", 3, 2);
    test_position(world, "d.s.b_1", 3, 6);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_nested_ref(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "struct Street(width: f32)"
    LINE "template MyStreet : Street {"
    LINE "  prop facades: template Facade[]"
    LINE "  for (i, f) in facades {"
    LINE "    \"b_{i}\" { f: {height: $width} }"
    LINE "  }"
    LINE "}"
    LINE "template City {"
    LINE "  prop street: template Street"
    LINE "  s { street: {width: 3} }"
    LINE "}"
    LINE "City c(street: MyStreet(facades: [BrickFacade(scale: 4), GlassFacade(scale: 5)]))";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_position(world, "c.s.b_0", 3, 4);
    test_position(world, "c.s.b_1", 3, 5);

    ecs_fini(world);
}

void TemplateVectorProp_template_ref_call_not_derived_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_HEAD
    LINE "template Unrelated {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade(scale: 5), Unrelated(x: 2)]"
    LINE "  a { facade[0]: {height: 1} }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}
