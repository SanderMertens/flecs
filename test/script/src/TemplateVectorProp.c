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

#define FACADE_DEFS\
    HEAD "struct Position(x: f32, y: f32)"\
    LINE "struct Facade(height: f32)"\
    LINE "template BrickFacade : Facade {"\
    LINE "  Position: {$height, 1}"\
    LINE "}"\
    LINE "template GlassFacade : Facade {"\
    LINE "  Position: {$height, 2}"\
    LINE "}"\
    LINE "template WoodFacade : Facade {"\
    LINE "  Position: {$height, 3}"\
    LINE "}"

static void test_facade(
    ecs_world_t *world,
    const char *path,
    float height,
    float kind)
{
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);

    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    const PositionValue *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_flt(p->x, height);
    test_flt(p->y, kind);
}

void TemplateVectorProp_declaration_w_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[0]: {height: 10} }"
    LINE "  b { facade[1]: {height: 20} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_facade(world, "e.a", 10, 1);
    test_facade(world, "e.b", 20, 2);

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
    FACADE_DEFS
    LINE "template Building {"
    LINE "  prop facade: template Facade[]"
    LINE "  Position: {facade.count(), 0}"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_facade(world, "e", 0, 0);

    ecs_fini(world);
}

void TemplateVectorProp_count(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  Position: {facade.count(), 0}"
    LINE "}"
    LINE "Building e()"
    LINE "f { Building: {facade: [BrickFacade]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_facade(world, "e", 2, 0);
    test_facade(world, "f", 1, 0);

    ecs_fini(world);
}

void TemplateVectorProp_index_instantiate_w_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
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

    test_facade(world, "e.floor_0", 0, 1);
    test_facade(world, "e.floor_1", 3, 2);
    test_facade(world, "e.floor_2", 6, 1);

    ecs_fini(world);
}

void TemplateVectorProp_index_tag_form(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
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
    FACADE_DEFS
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

    test_facade(world, "e.wall.side", 7, 2);

    ecs_fini(world);
}

void TemplateVectorProp_iterate_w_for(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  for (i, f) in facade {"
    LINE "    \"side_{i}\" { f: {height: 5} }"
    LINE "  }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_facade(world, "e.side_0", 5, 1);
    test_facade(world, "e.side_1", 5, 2);

    ecs_fini(world);
}

void TemplateVectorProp_index_by_rng(void) {
    ecs_world_t *world = ecs_init();

#ifdef FLECS_SCRIPT_MATH
    ECS_IMPORT(world, FlecsScriptMath);

    const char *expr =
    FACADE_DEFS
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
    FACADE_DEFS
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
    FACADE_DEFS
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[0]: {height: 1} }"
    LINE "  b { facade[1]: {height: 2} }"
    LINE "}"
    LINE "Building e(facade: [GlassFacade, BrickFacade])";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_facade(world, "e.a", 1, 2);
    test_facade(world, "e.b", 2, 1);

    ecs_fini(world);
}

void TemplateVectorProp_override_component_syntax(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[0]: {height: 1} }"
    LINE "}"
    LINE "e { Building: {facade: [WoodFacade]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_facade(world, "e.a", 1, 3);

    ecs_fini(world);
}

void TemplateVectorProp_native_set_id(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
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
    ecs_vec_init_t(NULL, &vec, ecs_entity_t, 1);
    ecs_vec_append_t(NULL, &vec, ecs_entity_t)[0] = glass;

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set_id(world, e, building, ECS_SIZEOF(ecs_vec_t), &vec);

    ecs_vec_fini_t(NULL, &vec, ecs_entity_t);

    test_facade(world, "e.a", 4, 2);

    ecs_fini(world);
}

void TemplateVectorProp_expr_run_props(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
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

    test_facade(world, "e.a", 8, 1);
    test_facade(world, "e.b", 9, 2);

    ecs_fini(world);
}

void TemplateVectorProp_index_out_of_range_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
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
    FACADE_DEFS
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
    FACADE_DEFS
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
    FACADE_DEFS
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
    FACADE_DEFS
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
    ecs_vec_init_t(NULL, &vec, ecs_entity_t, 1);
    ecs_vec_append_t(NULL, &vec, ecs_entity_t)[0] = unrelated;

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_log_set_level(-4);
    ecs_set_id(world, e, building, ECS_SIZEOF(ecs_vec_t), &vec);
    ecs_log_set_level(-1);

    ecs_vec_fini_t(NULL, &vec, ecs_entity_t);

    test_assert(ecs_lookup(world, "e.a") == 0);

    ecs_fini(world);
}

void TemplateVectorProp_template_type_vector_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Point {"
    LINE "  prop x: f32 = 1"
    LINE "}"
    LINE "template Building {"
    LINE "  prop points: template Point[]"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void TemplateVectorProp_bare_vector_prop_as_component_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
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
    FACADE_DEFS
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade[0]: {height: 6} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);
    test_facade(world, "e.a", 6, 1);

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

    test_facade(world, "e.a", 6, 2);

    ecs_fini(world);
}

void TemplateVectorProp_reactive_prop_change_from_script(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade]"
    LINE "  a { facade[0]: {height: 6} }"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);
    test_facade(world, "e.a", 6, 1);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Building: {facade: [WoodFacade]} }", &ir_desc, NULL) == 0);

    test_facade(world, "e.a", 6, 3);

    ecs_fini(world);
}

void TemplateVectorProp_nested_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
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

    test_facade(world, "e.left.side", 11, 2);
    test_facade(world, "e.right.side", 11, 3);

    ecs_fini(world);
}

void TemplateVectorProp_template_inheritance(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
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

    test_facade(world, "e.a", 12, 1);
    test_facade(world, "f.a", 13, 2);

    ecs_fini(world);
}

void TemplateVectorProp_index_in_interpolated_string(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    FACADE_DEFS
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
    FACADE_DEFS
    LINE "template Building {"
    LINE "  prop facade: template Facade[] = [BrickFacade, GlassFacade]"
    LINE "  a { facade[1]: {height: 3} }"
    LINE "  Vec: [10, 20, 30]"
    LINE "}"
    LINE "Building e()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_facade(world, "e.a", 3, 2);

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
        FACADE_DEFS
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
