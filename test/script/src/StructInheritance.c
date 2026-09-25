#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void StructInheritance_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

typedef struct {
    float x;
    float y;
} PositionBase;

typedef struct {
    PositionBase base;
    float z;
} Position3D;

typedef struct {
    int32_t x;
    int8_t y;
} PaddedBase;

typedef struct {
    PaddedBase base;
    int8_t z;
} PaddedDerived;

void StructInheritance_block_syntax(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, base, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, base, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(PositionBase));
    test_int(c->alignment, ECS_ALIGNOF(PositionBase));

    const EcsType *t = ecs_get(world, base, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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

    ecs_fini(world);
}

void StructInheritance_paren_syntax(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, base, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, base, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(PositionBase));
    test_int(c->alignment, ECS_ALIGNOF(PositionBase));

    const EcsType *t = ecs_get(world, base, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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

    ecs_fini(world);
}

void StructInheritance_paren_syntax_space_before_paren(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base (z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(2 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 2);
    test_str(m->name, "z");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 8);

    ecs_fini(world);
}

void StructInheritance_mixed_syntax(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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

    ecs_fini(world);
}

void StructInheritance_mixed_syntax_block_base(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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

    ecs_fini(world);
}

void StructInheritance_no_own_members(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(PositionBase));
    test_int(c->alignment, ECS_ALIGNOF(PositionBase));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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

    ecs_fini(world);
}

void StructInheritance_no_own_members_newline_before_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base()";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(PositionBase));
    test_int(c->alignment, ECS_ALIGNOF(PositionBase));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    ecs_fini(world);
}

void StructInheritance_chain(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(x: f32)"
    LINE "struct B : A(y: f32)"
    LINE "struct C : B(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "A");
    ecs_entity_t b = ecs_lookup(world, "B");
    ecs_entity_t c = ecs_lookup(world, "C");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(c != 0);

    const EcsStruct *st = ecs_get(world, a, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);

    const EcsComponent *comp = ecs_get(world, a, EcsComponent);
    test_assert(comp != NULL);
    test_int(comp->size, 4);
    test_int(comp->alignment, 4);

    const EcsType *t = ecs_get(world, a, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, b, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    comp = ecs_get(world, b, EcsComponent);
    test_assert(comp != NULL);
    test_int(comp->size, 8);
    test_int(comp->alignment, 4);

    t = ecs_get(world, b, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    comp = ecs_get(world, c, EcsComponent);
    test_assert(comp != NULL);
    test_int(comp->size, 12);
    test_int(comp->alignment, 4);

    t = ecs_get(world, c, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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

    ecs_fini(world);
}

void StructInheritance_chain_w_padding(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(a: i64, b: i8)"
    LINE "struct B : A(c: i8)"
    LINE "struct C : B(d: i8)"
    LINE "e { C: {a: 1, b: 2, c: 3, d: 4} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t b = ecs_lookup(world, "B");
    ecs_entity_t c = ecs_lookup(world, "C");

    const EcsStruct *st = ecs_get(world, b, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *comp = ecs_get(world, b, EcsComponent);
    test_assert(comp != NULL);
    test_int(comp->size, 24);
    test_int(comp->alignment, 8);

    const EcsType *t = ecs_get(world, b, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, b, EcsStruct);
    test_assert(st != NULL);
    test_assert(0 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "a");
    test_uint(m->type, ecs_id(ecs_i64_t));
    test_int(m->offset, 0);

    st = ecs_get(world, b, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "b");
    test_uint(m->type, ecs_id(ecs_i8_t));
    test_int(m->offset, 8);

    st = ecs_get(world, b, EcsStruct);
    test_assert(st != NULL);
    test_assert(2 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 2);
    test_str(m->name, "c");
    test_uint(m->type, ecs_id(ecs_i8_t));
    test_int(m->offset, 16);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 4);

    comp = ecs_get(world, c, EcsComponent);
    test_assert(comp != NULL);
    test_int(comp->size, 32);
    test_int(comp->alignment, 8);

    t = ecs_get(world, c, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_assert(0 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "a");
    test_uint(m->type, ecs_id(ecs_i64_t));
    test_int(m->offset, 0);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "b");
    test_uint(m->type, ecs_id(ecs_i8_t));
    test_int(m->offset, 8);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_assert(2 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 2);
    test_str(m->name, "c");
    test_uint(m->type, ecs_id(ecs_i8_t));
    test_int(m->offset, 16);

    st = ecs_get(world, c, EcsStruct);
    test_assert(st != NULL);
    test_assert(3 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 3);
    test_str(m->name, "d");
    test_uint(m->type, ecs_id(ecs_i8_t));
    test_int(m->offset, 24);

    ecs_entity_t e = ecs_lookup(world, "e");
    const char *ptr = ecs_get_id(world, e, c);
    test_assert(ptr != NULL);
    test_int(*(int64_t*)ptr, 1);
    test_int(ptr[8], 2);
    test_int(ptr[16], 3);
    test_int(ptr[24], 4);

    ecs_fini(world);
}

void StructInheritance_two_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32)"
    LINE "struct D1 : Base(y: f32)"
    LINE "struct D2 : Base(z: i32, w: i32)";

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
    test_int(c->alignment, 4);

    const EcsType *t = ecs_get(world, base, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, d1, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    c = ecs_get(world, d1, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 8);
    test_int(c->alignment, 4);

    t = ecs_get(world, d1, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, d2, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    c = ecs_get(world, d2, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 12);
    test_int(c->alignment, 4);

    t = ecs_get(world, d2, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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
    test_uint(m->type, ecs_id(ecs_i32_t));
    test_int(m->offset, 4);

    st = ecs_get(world, d2, EcsStruct);
    test_assert(st != NULL);
    test_assert(2 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 2);
    test_str(m->name, "w");
    test_uint(m->type, ecs_id(ecs_i32_t));
    test_int(m->offset, 8);

    ecs_fini(world);
}

void StructInheritance_base_w_padding(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: i32, y: i8)"
    LINE "struct Derived : Base(z: i8)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");

    const EcsStruct *st = ecs_get(world, base, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, base, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(PaddedBase));
    test_int(c->alignment, ECS_ALIGNOF(PaddedBase));

    const EcsType *t = ecs_get(world, base, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(PaddedDerived));
    test_int(c->alignment, ECS_ALIGNOF(PaddedDerived));

    t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(0 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "x");
    test_uint(m->type, ecs_id(ecs_i32_t));
    test_int(m->offset, offsetof(PaddedDerived, base.x));

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "y");
    test_uint(m->type, ecs_id(ecs_i8_t));
    test_int(m->offset, offsetof(PaddedDerived, base.y));

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(2 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 2);
    test_str(m->name, "z");
    test_uint(m->type, ecs_id(ecs_i8_t));
    test_int(m->offset, offsetof(PaddedDerived, z));

    ecs_fini(world);
}

void StructInheritance_set_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "e { Derived: {x: 10, y: 20, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(derived != 0);
    test_assert(e != 0);

    const Position3D *p = ecs_get_id(world, e, derived);
    test_assert(p != NULL);
    test_flt(p->base.x, 10);
    test_flt(p->base.y, 20);
    test_flt(p->z, 30);

    ecs_fini(world);
}

void StructInheritance_set_component_partial(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "e { Derived: {y: 20} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    const Position3D *p = ecs_get_id(world, e, derived);
    test_assert(p != NULL);
    test_flt(p->base.x, 0);
    test_flt(p->base.y, 20);
    test_flt(p->z, 0);

    ecs_fini(world);
}

void StructInheritance_set_component_positional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "e { Derived: {10, 20, 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    const Position3D *p = ecs_get_id(world, e, derived);
    test_assert(p != NULL);
    test_flt(p->base.x, 10);
    test_flt(p->base.y, 20);
    test_flt(p->z, 30);

    ecs_fini(world);
}

void StructInheritance_set_base_and_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "e {"
    LINE "  Base: {1, 2}"
    LINE "  Derived: {10, 20, 30}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    const PositionBase *b = ecs_get_id(world, e, base);
    test_assert(b != NULL);
    test_flt(b->x, 1);
    test_flt(b->y, 2);

    const Position3D *p = ecs_get_id(world, e, derived);
    test_assert(p != NULL);
    test_flt(p->base.x, 10);
    test_flt(p->base.y, 20);
    test_flt(p->z, 30);

    ecs_fini(world);
}

void StructInheritance_entity_to_json(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "e { Derived: {x: 10, y: 20, z: 30} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    char *json = ecs_entity_to_json(world, e, &(ecs_entity_to_json_desc_t){
        .serialize_values = true
    });
    test_str(json, "{\"name\":\"e\", \"components\":{\"Derived\":{\"x\":10, \"y\":20, \"z\":30}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void StructInheritance_expr_run(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    Position3D value = {0};
    test_assert(ecs_expr_run(world, "{x: 1, y: 2, z: 3}",
        &(ecs_value_t){ .type = derived, .ptr = &value }, NULL) != NULL);
    test_flt(value.base.x, 1);
    test_flt(value.base.y, 2);
    test_flt(value.z, 3);

    char *str = ecs_ptr_to_expr(world, derived, &value);
    test_str(str, "{x: 1, y: 2, z: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void StructInheritance_add_member_to_base_w_derived(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    test_assert(base != 0);

    test_expect_abort();
    ecs_struct_add_member(world, base, &(ecs_member_t){
        .name = "x2",
        .type = ecs_id(ecs_f32_t)
    });
}

void StructInheritance_base_defined_in_c(void) {
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
    HEAD "struct Derived : PositionBase(z: f32)"
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
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    ecs_entity_t e = ecs_lookup(world, "e");
    const Position3D *p = ecs_get_id(world, e, derived);
    test_assert(p != NULL);
    test_flt(p->base.x, 10);
    test_flt(p->base.y, 20);
    test_flt(p->z, 30);

    ecs_fini(world);
}

void StructInheritance_derived_defined_in_c(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position3D);

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    test_assert(base != 0);

    ecs_add_pair(world, ecs_id(Position3D), EcsIsA, base);
    ecs_struct(world, {
        .entity = ecs_id(Position3D),
        .members = {
            {"z", ecs_id(ecs_f32_t)}
        }
    });

    const EcsStruct *st = ecs_get(world, ecs_id(Position3D), EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, ecs_id(Position3D), EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    const EcsType *ti = ecs_get(world, ecs_id(Position3D), EcsType);
    test_assert(ti != NULL);
    test_assert(ti->kind == EcsStructType);

    const EcsType *t = ecs_get(world, ecs_id(Position3D), EcsType);
    test_bool(t->existing, true);
    test_bool(t->partial, false);

    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Position3D: {x: 10, y: 20, z: 30} }", &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    const Position3D *p = ecs_get(world, e, Position3D);
    test_assert(p != NULL);
    test_flt(p->base.x, 10);
    test_flt(p->base.y, 20);
    test_flt(p->z, 30);

    ecs_fini(world);
}

void StructInheritance_base_in_module(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module shapes"
    LINE "struct Base(x: f32, y: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    expr =
    HEAD "struct Derived : shapes.Base(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "shapes.Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    ecs_fini(world);
}

void StructInheritance_base_w_using(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module shapes"
    LINE "struct Base(x: f32, y: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    expr =
    HEAD "using shapes"
    LINE "struct Derived : Base(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "shapes.Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    ecs_fini(world);
}

void StructInheritance_derived_as_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "struct Outer(d: Derived, w: f32)"
    LINE "e { Outer: {d: {x: 1, y: 2, z: 3}, w: 4} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t outer = ecs_lookup(world, "Outer");
    test_assert(outer != 0);

    const EcsStruct *st = ecs_get(world, outer, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, outer, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 16);
    test_int(c->alignment, 4);

    const EcsType *t = ecs_get(world, outer, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *values = ecs_get_id(world, e, outer);
    test_assert(values != NULL);
    test_flt(values[0], 1);
    test_flt(values[1], 2);
    test_flt(values[2], 3);
    test_flt(values[3], 4);

    ecs_fini(world);
}

void StructInheritance_base_w_string_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(name: string)"
    LINE "struct Derived : Base(value: i32)"
    LINE "e { Derived: {name: \"hello\", value: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");

    const void *ptr = ecs_get_id(world, e, derived);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, derived, ptr);
    test_str(str, "{name: \"hello\", value: 10}");
    ecs_os_free(str);

    ecs_delete(world, e);

    ecs_fini(world);
}

void StructInheritance_base_w_array_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(arr: {type: f32, count: 3})"
    LINE "struct Derived : Base(w: f32)"
    LINE "e { Derived: {arr: [1, 2, 3], w: 4} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 16);
    test_int(c->alignment, 4);

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(0 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 0);
    test_str(m->name, "arr");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 0);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(1 < ecs_vec_count(&st->members));
    m = ecs_vec_get_t(&st->members, ecs_member_t, 1);
    test_str(m->name, "w");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 12);

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *values = ecs_get_id(world, e, derived);
    test_assert(values != NULL);
    test_flt(values[0], 1);
    test_flt(values[1], 2);
    test_flt(values[2], 3);
    test_flt(values[3], 4);

    ecs_fini(world);
}

void StructInheritance_duplicate_member_paren(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(x: f32)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_duplicate_member_block(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(x: f32)";

    ecs_log_set_level(-4);
    ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL);
    ecs_log_set_level(-1);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(PositionBase));
    test_int(c->alignment, ECS_ALIGNOF(PositionBase));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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

    ecs_fini(world);
}

void StructInheritance_base_not_a_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Base" });

    const char *expr =
    HEAD "struct Derived : Base(x: f32)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_self_inherit(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A : A(x: f32)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_inheritance_cycle(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct S1(x: f32)"
    LINE "struct S2 : S1(y: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, "struct S1 : S2(z: f32)", &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_entity_t s1 = ecs_lookup(world, "S1");
    ecs_entity_t s2 = ecs_lookup(world, "S2");
    test_assert(!ecs_has_pair(world, s1, EcsIsA, s2));
    test_assert(ecs_has_pair(world, s2, EcsIsA, s1));

    ecs_fini(world);
}

void StructInheritance_base_is_prefab(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "prefab Base"
    LINE "struct Derived : Base(x: f32)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_base_is_enum(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "enum Color { Red, Green }"
    LINE "struct Derived : Color(x: f32)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_base_is_primitive(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Derived : f32(x: f32)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_base_unresolved(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Derived : Base(x: f32)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    test_assert(ecs_lookup(world, "Derived") == 0);

    ecs_fini(world);
}

void StructInheritance_base_declared_after_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Derived : Base(z: f32)"
    LINE "struct Base(x: f32, y: f32)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_run_script_twice(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);
    test_assert(ecs_script_run_w_desc(world, NULL,
        "e { Derived: {x: 10, y: 20, z: 30} }", &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");

    const EcsStruct *st = ecs_get(world, base, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    const EcsComponent *c = ecs_get(world, base, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(PositionBase));
    test_int(c->alignment, ECS_ALIGNOF(PositionBase));

    const EcsType *t = ecs_get(world, base, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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
    const Position3D *p = ecs_get_id(world, e, derived);
    test_assert(p != NULL);
    test_flt(p->base.x, 10);
    test_flt(p->base.y, 20);
    test_flt(p->z, 30);

    ecs_fini(world);
}

void StructInheritance_managed_script_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "e { Derived: {x: 10, y: 20, z: 30} }";

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled,
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    test_assert(ecs_script_update(world, s, 0, expr) == 0);

    derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

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
    const Position3D *p = ecs_get_id(world, e, derived);
    test_assert(p != NULL);
    test_flt(p->base.x, 10);
    test_flt(p->base.y, 20);
    test_flt(p->z, 30);

    ecs_fini(world);
}

void StructInheritance_managed_script_update_add_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "";

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled,
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32, w: f32)"
    LINE "e { Derived: {x: 10, y: 20, z: 30, w: 40} }";

    test_assert(ecs_script_update(world, s, 0, expr) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 4);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 16);
    test_int(c->alignment, 4);

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_assert(3 < ecs_vec_count(&st->members));
    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 3);
    test_str(m->name, "w");
    test_uint(m->type, ecs_id(ecs_f32_t));
    test_int(m->offset, 12);

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *values = ecs_get_id(world, e, derived);
    test_assert(values != NULL);
    test_flt(values[0], 10);
    test_flt(values[1], 20);
    test_flt(values[2], 30);
    test_flt(values[3], 40);

    ecs_fini(world);
}

void StructInheritance_query_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "e1 { Derived: {1, 2, 3} }"
    LINE "e2 { Derived: {4, 5, 6} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_query_t *q = ecs_query(world, { .expr = "Derived" });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    Position3D *p = ecs_field_w_size(&it, sizeof(Position3D), 0);
    test_flt(p[0].base.x, 1);
    test_flt(p[0].z, 3);
    test_flt(p[1].base.y, 5);
    test_flt(p[1].z, 6);
    test_uint(it.entities[0], ecs_lookup(world, "e1"));
    test_uint(it.entities[1], ecs_lookup(world, "e2"));
    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);
    (void)derived;

    ecs_fini(world);
}

void StructInheritance_derived_in_nested_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "shapes {"
    LINE "  struct Base(x: f32, y: f32)"
    LINE "  struct Derived : Base(z: f32)"
    LINE "}"
    LINE "e { shapes.Derived: {1, 2, 3} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "shapes.Base");
    ecs_entity_t derived = ecs_lookup(world, "shapes.Derived");
    test_assert(base != 0);
    test_assert(derived != 0);
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 3);

    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(Position3D));
    test_int(c->alignment, ECS_ALIGNOF(Position3D));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);

    ecs_entity_t e = ecs_lookup(world, "e");
    const Position3D *p = ecs_get_id(world, e, derived);
    test_assert(p != NULL);
    test_flt(p->base.x, 1);
    test_flt(p->z, 3);

    ecs_fini(world);
}

void StructInheritance_derived_member_access_in_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "const v: Derived = {1, 2, 3}"
    LINE "const sum = $v.x + $v.y + $v.z"
    LINE "e { Base: {x: $sum, y: $v.z} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *p = ecs_get_id(world, e, base);
    test_assert(p != NULL);
    test_flt(p->x, 6);
    test_flt(p->y, 3);

    ecs_fini(world);
}

void StructInheritance_multiple_bases(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct A(x: f32)"
    LINE "struct B(y: f32)"
    LINE "struct Derived : A, B(z: f32)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "const d: Derived = {1, 2, 3}"
    LINE "const b: Base = $d"
    LINE "e { Base: $b }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *p = ecs_get_id(world, e, base);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_const_member_access(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "const d: Derived = {1, 2, 3}"
    LINE "const b: Base = $d"
    LINE "e { Base: {$b.x + $d.z, $b.y * 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *p = ecs_get_id(world, e, base);
    test_assert(p != NULL);
    test_flt(p->x, 4);
    test_flt(p->y, 20);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_const_chain(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "struct Derived2 : Derived(w: f32)"
    LINE "const d: Derived2 = {1, 2, 3, 4}"
    LINE "const b: Base = $d"
    LINE "const d1: Derived = $d"
    LINE "e { Base: $b }"
    LINE "f { Derived: $d1 }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    const PositionBase *p = ecs_get_id(world, e, base);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    const Position3D *p3 = ecs_get_id(world, f, derived);
    test_assert(p3 != NULL);
    test_flt(p3->base.x, 1);
    test_flt(p3->base.y, 2);
    test_flt(p3->z, 3);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_const_w_string(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(name: string, value: i32)"
    LINE "struct Derived : Base(extra: string)"
    LINE "const d: Derived = {\"hello\", 10, \"world\"}"
    LINE "const b: Base = $d"
    LINE "e { Base: $b }"
    LINE "f { Base: $d }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    char *str = ecs_ptr_to_expr(world, base, ecs_get_id(world, e, base));
    test_str(str, "{name: \"hello\", value: 10}");
    ecs_os_free(str);

    str = ecs_ptr_to_expr(world, base, ecs_get_id(world, f, base));
    test_str(str, "{name: \"hello\", value: 10}");
    ecs_os_free(str);

    ecs_delete(world, e);
    ecs_delete(world, f);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "template Foo {"
    LINE "  prop d: Derived = {1, 2, 3}"
    LINE "  prop b: Base = $d"
    LINE "}"
    LINE "e { Foo: {} }"
    LINE "f { Foo: {b: {10, 20}} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    const EcsStruct *st = ecs_get(world, foo, EcsStruct);
    test_int(ecs_vec_count(&st->members), 2);
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type,
        ecs_lookup(world, "Base"));

    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, foo);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 3);
    test_flt(v[3], 1);
    test_flt(v[4], 2);

    ecs_entity_t f = ecs_lookup(world, "f");
    v = ecs_get_id(world, f, foo);
    test_assert(v != NULL);
    test_flt(v[3], 10);
    test_flt(v[4], 20);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_prop_at_instantiate(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "template Foo {"
    LINE "  prop b: Base = {0, 0}"
    LINE "  Base: $b"
    LINE "}"
    LINE "const d: Derived = {1, 2, 3}"
    LINE "e { Foo: {b: $d} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *p = ecs_get_id(world, e, base);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "template Foo {"
    LINE "  prop d: Derived = {1, 2, 3}"
    LINE "  mut b: Base = $d"
    LINE "  Base: $b"
    LINE "}"
    LINE "e { Foo: {} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t foo_mut = ecs_lookup(world, "Foo.mut");
    test_assert(foo_mut != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *m = ecs_get_id(world, e, foo_mut);
    test_assert(m != NULL);
    test_flt(m->x, 1);
    test_flt(m->y, 2);

    const PositionBase *p = ecs_get_id(world, e, base);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "const d: Derived = {1, 2, 3}"
    LINE "e { Base: $d }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(!ecs_has_id(world, e, derived));

    const PositionBase *p = ecs_get_id(world, e, base);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_component_in_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "template Foo {"
    LINE "  prop d: Derived = {1, 2, 3}"
    LINE "  Base: $d"
    LINE "  child { Base: $d }"
    LINE "}"
    LINE "e { Foo: {d: {4, 5, 6}} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");

    const PositionBase *p = ecs_get_id(world, e, base);
    test_assert(p != NULL);
    test_flt(p->x, 4);
    test_flt(p->y, 5);

    p = ecs_get_id(world, child, base);
    test_assert(p != NULL);
    test_flt(p->x, 4);
    test_flt(p->y, 5);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_nested_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "struct Outer(b: Base, w: f32)"
    LINE "const d: Derived = {1, 2, 3}"
    LINE "e { Outer: {b: $d, w: 4} }"
    LINE "f { Outer: {$d, 5} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t outer = ecs_lookup(world, "Outer");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    const float *v = ecs_get_id(world, e, outer);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 4);

    v = ecs_get_id(world, f, outer);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 5);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_nested_member_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "struct Outer(b: Base, w: f32)"
    LINE "const d: Derived = {1, 2, 3}"
    LINE "const o: Outer = {b: $d, w: 4}"
    LINE "e { Outer: $o }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t outer = ecs_lookup(world, "Outer");
    ecs_entity_t e = ecs_lookup(world, "e");
    const float *v = ecs_get_id(world, e, outer);
    test_assert(v != NULL);
    test_flt(v[0], 1);
    test_flt(v[1], 2);
    test_flt(v[2], 4);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "const d: Derived = {1, 2, 3}"
    LINE "with Base($d) {"
    LINE "  e {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t e = ecs_lookup(world, "e");
    const PositionBase *p = ecs_get_id(world, e, base);
    test_assert(p != NULL);
    test_flt(p->x, 1);
    test_flt(p->y, 2);

    ecs_fini(world);
}

void StructInheritance_assign_derived_to_base_expr_run(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t derived = ecs_lookup(world, "Derived");

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define_id(vars, "d", derived);
    Position3D *dv = var->value.ptr;
    dv->base.x = 1;
    dv->base.y = 2;
    dv->z = 3;

    PositionBase b = {0};
    test_assert(ecs_expr_run(world, "$d",
        &(ecs_value_t){ .type = base, .ptr = &b },
        &(ecs_expr_eval_desc_t){ .vars = vars }) != NULL);
    test_flt(b.x, 1);
    test_flt(b.y, 2);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void StructInheritance_assign_base_to_derived_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "const b: Base = {1, 2}"
    LINE "const d: Derived = $b";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_assign_base_to_derived_component_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Derived : Base(z: f32)"
    LINE "const b: Base = {1, 2}"
    LINE "e { Derived: $b }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_assign_unrelated_struct_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Other(x: f32, y: f32)"
    LINE "const o: Other = {1, 2}"
    LINE "const b: Base = $o";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_assign_unrelated_struct_prop_fails(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Base(x: f32, y: f32)"
    LINE "struct Other(x: f32, y: f32)"
    LINE "template Foo {"
    LINE "  prop o: Other = {1, 2}"
    LINE "  prop b: Base = $o"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void StructInheritance_empty_base(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Facade()"
    LINE "struct Brick : Facade(height: f32)"
    LINE "e { Brick: {height: 2} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t brick = ecs_lookup(world, "Brick");
    test_assert(facade != 0);
    test_assert(brick != 0);
    test_assert(ecs_has_pair(world, brick, EcsIsA, facade));

    const EcsComponent *c = ecs_get(world, facade, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 1);
    test_int(c->alignment, 1);

    const EcsStruct *st = ecs_get(world, brick, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 1);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "height");

    c = ecs_get(world, brick, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, sizeof(float));
    test_int(c->alignment, ECS_ALIGNOF(float));

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const float *v = ecs_get_id(world, e, brick);
    test_assert(v != NULL);
    test_flt(v[0], 2);
    test_assert(ecs_has_id(world, e, facade));

    ecs_fini(world);
}

void StructInheritance_empty_base_empty_derived(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Facade()"
    LINE "struct Brick : Facade()"
    LINE "e { Brick }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_entity_t brick = ecs_lookup(world, "Brick");
    test_assert(facade != 0);
    test_assert(brick != 0);
    test_assert(ecs_has_pair(world, brick, EcsIsA, facade));

    const EcsComponent *c = ecs_get(world, brick, EcsComponent);
    test_assert(c != NULL);
    test_int(c->size, 1);
    test_int(c->alignment, 1);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, brick));

    ecs_fini(world);
}
