#include <script.h>

typedef struct {
    float height;
} TmplHeight;

typedef struct {
    float a;
    float b;
} TmplAB;

typedef struct {
    bool cond;
} TmplCond;

typedef struct {
    bool cond;
    float height;
} TmplCondHeight;

typedef struct {
    int32_t count;
    float height;
} TmplCountHeight;

typedef struct {
    int32_t x;
    int32_t y;
} TmplIntXY;

static
void register_position(ecs_world_t *world, ecs_entity_t position) {
    ecs_struct(world, {
        .entity = position,
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
}

static int position_set_invoked = 0;

static
void PositionSetObserver(ecs_iter_t *it) {
    position_set_invoked += it->count;
}

static
int32_t find_children_w_id(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t id,
    ecs_entity_t *out,
    int32_t out_max)
{
    int32_t count = 0;
    ecs_iter_t it = ecs_children(world, parent);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            if (!id || ecs_has_id(world, it.entities[i], id)) {
                if (out && count < out_max) {
                    out[count] = it.entities[i];
                }
                count ++;
            }
        }
    }
    return count;
}

void IncrementalUpdates_component_value_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void IncrementalUpdates_component_value_2nd_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});
    ecs_set(world, e, TmplHeight, {20});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 2);

    ecs_fini(world);
}

void IncrementalUpdates_entity_not_deleted_after_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);

    ecs_fini(world);
}

void IncrementalUpdates_entity_not_deleted_after_same_value_set(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {1});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);

    ecs_fini(world);
}

void IncrementalUpdates_user_tag_kept_after_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_add(world, child, UserTag);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has(world, child, UserTag));

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);

    ecs_fini(world);
}

void IncrementalUpdates_unchanged_statement_not_reevaluated(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    position_set_invoked = 0;
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = { EcsOnSet },
        .callback = PositionSetObserver
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop a = flecs.meta.f32: 1"
    LINE "  prop b = flecs.meta.f32: 1"
    LINE "  child_a {"
    LINE "    Position: {$a, 0}"
    LINE "  }"
    LINE "  child_b {"
    LINE "    Position: {$b, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_int(position_set_invoked, 2);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    ecs_entity_t ecs_id(TmplAB) = tree;
    ecs_set(world, e, TmplAB, {10, 1});

    test_int(position_set_invoked, 3);

    test_assert(ecs_lookup(world, "e.child_a") == child_a);
    test_assert(ecs_lookup(world, "e.child_b") == child_b);

    {
        const Position *p = ecs_get(world, child_a, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }
    {
        const Position *p = ecs_get(world, child_b, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_update_instance_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  Position: {$height, 2}"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_add(world, e, UserTag);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_has(world, e, UserTag));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 2);

    ecs_fini(world);
}

void IncrementalUpdates_update_w_const_dependency(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  const h2: $height * 2"
    LINE "  child {"
    LINE "    Position: {$h2, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
    }

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void IncrementalUpdates_update_w_const_chain(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  const h2: $height * 2"
    LINE "  const h4: $h2 * 2"
    LINE "  child {"
    LINE "    Position: {$h4, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.child") == child);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 40);

    ecs_fini(world);
}

void IncrementalUpdates_static_tag_kept_after_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Foo"
    LINE "    Position: {$height, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has_id(world, child, foo));

    ecs_fini(world);
}

void IncrementalUpdates_pair_component_value_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "Tgt {}"
    LINE "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    (Position, Tgt): {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t tgt = ecs_lookup(world, "Tgt");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.child") == child);

    const Position *p = ecs_get_id(world, child,
        ecs_pair(ecs_id(Position), tgt));
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 2);

    ecs_fini(world);
}

void IncrementalUpdates_pair_target_to_template_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: false"
    LINE "  target_child {}"
    LINE "  if $cond {"
    LINE "    child {"
    LINE "      (Rel, target_child)"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t target_child = ecs_lookup(world, "e.target_child");
    test_assert(target_child != 0);
    test_assert(ecs_lookup(world, "e.child") == 0);

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {true});

    test_assert(ecs_lookup(world, "e.target_child") == target_child);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_has_pair(world, child, rel, target_child));

    ecs_fini(world);
}

void IncrementalUpdates_conditional_entity_true_to_false(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "e.cond_child") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_conditional_entity_false_to_true(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: false"
    LINE "  if $cond {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(ecs_lookup(world, "e.cond_child") == 0);

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {true});

    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);

    ecs_fini(world);
}

void IncrementalUpdates_conditional_entity_preserved_on_unrelated_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  if $cond {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "  child {"
    LINE "    Position: {$height, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);

    ecs_add(world, cond_child, UserTag);

    ecs_entity_t ecs_id(TmplCondHeight) = tree;
    ecs_set(world, e, TmplCondHeight, {true, 10});

    test_assert(ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "e.cond_child") == cond_child);
    test_assert(ecs_has(world, cond_child, UserTag));

    ecs_fini(world);
}

void IncrementalUpdates_conditional_component_true_to_false(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    Foo"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(ecs_has_id(world, e, foo));

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_has_id(world, e, foo));

    ecs_fini(world);
}

void IncrementalUpdates_conditional_component_false_to_true(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: false"
    LINE "  if $cond {"
    LINE "    Foo"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(!ecs_has_id(world, e, foo));

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {true});

    test_assert(ecs_has_id(world, e, foo));

    ecs_fini(world);
}

void IncrementalUpdates_conditional_pair_true_to_false(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "Tgt {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    (Rel, Tgt)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt = ecs_lookup(world, "Tgt");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(ecs_has_pair(world, e, rel, tgt));

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_has_pair(world, e, rel, tgt));

    ecs_fini(world);
}

void IncrementalUpdates_conditional_if_else_switch(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    child_true {}"
    LINE "  } else {"
    LINE "    child_false {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_true = ecs_lookup(world, "e.child_true");
    test_assert(child_true != 0);
    test_assert(ecs_lookup(world, "e.child_false") == 0);

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_is_alive(world, child_true));
    test_assert(ecs_lookup(world, "e.child_true") == 0);
    ecs_entity_t child_false = ecs_lookup(world, "e.child_false");
    test_assert(child_false != 0);

    ecs_set(world, e, TmplCond, {true});

    test_assert(!ecs_is_alive(world, child_false));
    test_assert(ecs_lookup(world, "e.child_false") == 0);
    test_assert(ecs_lookup(world, "e.child_true") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_nested_conditional_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop a = flecs.meta.bool: true"
    LINE "  prop b = flecs.meta.bool: true"
    LINE "  if $a {"
    LINE "    child_a {}"
    LINE "    if $b {"
    LINE "      child_b {}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    typedef struct {
        bool a;
        bool b;
    } TmplT;

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    ecs_entity_t ecs_id(TmplT) = tree;
    ecs_set(world, e, TmplT, {true, false});

    test_assert(ecs_is_alive(world, child_a));
    test_assert(!ecs_is_alive(world, child_b));
    test_assert(ecs_lookup(world, "e.child_a") == child_a);
    test_assert(ecs_lookup(world, "e.child_b") == 0);

    ecs_set(world, e, TmplT, {false, false});

    test_assert(!ecs_is_alive(world, child_a));
    test_assert(ecs_lookup(world, "e.child_a") == 0);

    ecs_set(world, e, TmplT, {true, true});

    test_assert(ecs_lookup(world, "e.child_a") != 0);
    test_assert(ecs_lookup(world, "e.child_b") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_loop_count_increase(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(ecs_lookup(world, "e.child_2") == 0);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {4, 1});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");
    ecs_entity_t child_3 = ecs_lookup(world, "e.child_3");
    test_assert(child_2 != 0);
    test_assert(child_3 != 0);

    const Position *p = ecs_get(world, child_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 1);

    ecs_fini(world);
}

void IncrementalUpdates_loop_count_decrease(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 4"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");
    ecs_entity_t child_3 = ecs_lookup(world, "e.child_3");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(child_2 != 0);
    test_assert(child_3 != 0);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {2, 1});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(!ecs_is_alive(world, child_2));
    test_assert(!ecs_is_alive(world, child_3));
    test_assert(ecs_lookup(world, "e.child_2") == 0);
    test_assert(ecs_lookup(world, "e.child_3") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_loop_count_to_zero(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {0, 1});

    test_assert(!ecs_is_alive(world, child_0));
    test_assert(!ecs_is_alive(world, child_1));

    ecs_set(world, e, TmplCountHeight, {2, 1});

    test_assert(ecs_lookup(world, "e.child_0") != 0);
    test_assert(ecs_lookup(world, "e.child_1") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_loop_entities_preserved_on_count_increase(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    test_assert(child_0 != 0);
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_1 != 0);

    ecs_add(world, child_0, UserTag);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {4, 1});

    test_assert(ecs_is_alive(world, child_0));
    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_has(world, child_0, UserTag));

    test_assert(ecs_lookup(world, "e.child_1") != 0);
    test_assert(ecs_lookup(world, "e.child_2") != 0);
    test_assert(ecs_lookup(world, "e.child_3") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_loop_value_update_entities_preserved(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(child_2 != 0);

    ecs_add(world, child_1, UserTag);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {3, 10});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(ecs_lookup(world, "e.child_2") == child_2);
    test_assert(ecs_has(world, child_1, UserTag));

    {
        const Position *p = ecs_get(world, child_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 10);
    }
    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 10);
    }
    {
        const Position *p = ecs_get(world, child_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_conditional_in_loop(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 4"
    LINE "  prop height = flecs.meta.f32: 2"
    LINE "  for i in 0..8 {"
    LINE "    if $i < $count {"
    LINE "      \"child_$i\" {"
    LINE "        Position: {$i, $height}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");
    ecs_entity_t child_3 = ecs_lookup(world, "e.child_3");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(child_2 != 0);
    test_assert(child_3 != 0);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {2, 2});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(!ecs_is_alive(world, child_2));
    test_assert(!ecs_is_alive(world, child_3));
    test_assert(ecs_lookup(world, "e.child_2") == 0);
    test_assert(ecs_lookup(world, "e.child_3") == 0);

    ecs_set(world, e, TmplCountHeight, {4, 2});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") != 0);
    test_assert(ecs_lookup(world, "e.child_2") != 0);
    test_assert(ecs_lookup(world, "e.child_3") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_loop_in_conditional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    for i in 0..2 {"
    LINE "      \"child_$i\" {"
    LINE "        Position: {$i, 0}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_is_alive(world, child_0));
    test_assert(!ecs_is_alive(world, child_1));
    test_assert(ecs_lookup(world, "e.child_0") == 0);
    test_assert(ecs_lookup(world, "e.child_1") == 0);

    ecs_set(world, e, TmplCond, {true});

    test_assert(ecs_lookup(world, "e.child_0") != 0);
    test_assert(ecs_lookup(world, "e.child_1") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_with_value_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  with Position($height, 2) {"
    LINE "    child_a {}"
    LINE "    child_b {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    {
        const Position *p = ecs_get(world, child_a, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.child_a") == child_a);
    test_assert(ecs_lookup(world, "e.child_b") == child_b);

    {
        const Position *p = ecs_get(world, child_a, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 2);
    }
    {
        const Position *p = ecs_get(world, child_b, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void IncrementalUpdates_nested_child_preserved(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    grandchild {"
    LINE "      Position: {$height, 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t grandchild = ecs_lookup(world, "e.child.grandchild");
    test_assert(child != 0);
    test_assert(grandchild != 0);

    ecs_add(world, grandchild, UserTag);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_lookup(world, "e.child.grandchild") == grandchild);
    test_assert(ecs_has(world, grandchild, UserTag));

    const Position *p = ecs_get(world, grandchild, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);

    ecs_fini(world);
}

void IncrementalUpdates_multiple_instances_independent(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e1()"
    LINE "Tree e2()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t child_1 = ecs_lookup(world, "e1.child");
    ecs_entity_t child_2 = ecs_lookup(world, "e2.child");
    test_assert(child_1 != 0);
    test_assert(child_2 != 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e1, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e1.child") == child_1);
    test_assert(ecs_lookup(world, "e2.child") == child_2);

    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }
    {
        const Position *p = ecs_get(world, child_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_set(world, e2, TmplHeight, {20});

    test_assert(ecs_lookup(world, "e2.child") == child_2);

    {
        const Position *p = ecs_get(world, child_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void IncrementalUpdates_update_from_default_values(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 5"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_id(world, e, tree);
    test_assert(ecs_lookup(world, "e.child") == 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, TmplHeight, {20});

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void IncrementalUpdates_deferred_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_defer_begin(world);
    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});
    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);

    ecs_fini(world);
}

void IncrementalUpdates_update_after_child_deleted_by_app(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_delete(world, child);
    test_assert(ecs_lookup(world, "e.child") == 0);

    test_expect_abort();

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});
}

void IncrementalUpdates_instance_deleted_other_instance_updated(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e1()"
    LINE "Tree e2()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t child_2 = ecs_lookup(world, "e2.child");

    ecs_delete(world, e1);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e2, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e2.child") == child_2);

    const Position *p = ecs_get(world, child_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);

    ecs_fini(world);
}

void IncrementalUpdates_nested_template_children_preserved(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Inner {"
    LINE "  prop val = flecs.meta.f32: 1"
    LINE "  inner_child {"
    LINE "    Position: {$val, 2}"
    LINE "  }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop val = flecs.meta.f32: 1"
    LINE "  mid {"
    LINE "    Inner: {val: $val}"
    LINE "  }"
    LINE "}"
    LINE "Outer e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t outer = ecs_lookup(world, "Outer");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t mid = ecs_lookup(world, "e.mid");
    ecs_entity_t inner_child = ecs_lookup(world, "e.mid.inner_child");
    test_assert(mid != 0);
    test_assert(inner_child != 0);

    ecs_add(world, inner_child, UserTag);

    {
        const Position *p = ecs_get(world, inner_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_entity_t ecs_id(TmplHeight) = outer;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.mid") == mid);
    test_assert(ecs_lookup(world, "e.mid.inner_child") == inner_child);
    test_assert(ecs_has(world, inner_child, UserTag));

    {
        const Position *p = ecs_get(world, inner_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_entity_rename_w_prop(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop offset = flecs.meta.i32: 0"
    LINE "  for i in $offset..($offset + 1) {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, 0}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child_0");
    test_assert(child != 0);

    ecs_add(world, child, UserTag);

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {5});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child_0") == 0);
    test_assert(ecs_lookup(world, "e.child_5") == child);
    test_assert(ecs_has(world, child, UserTag));

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);

    ecs_fini(world);
}

void IncrementalUpdates_anonymous_child_preserved(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "Marker {}"
    LINE "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  _ {"
    LINE "    Marker"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t marker = ecs_lookup(world, "Marker");
    ecs_entity_t e = ecs_lookup(world, "e");

    ecs_entity_t anon = 0;
    {
        ecs_iter_t it = ecs_children(world, e);
        while (ecs_children_next(&it)) {
            int32_t i;
            for (i = 0; i < it.count; i ++) {
                if (ecs_has_id(world, it.entities[i], marker)) {
                    anon = it.entities[i];
                }
            }
        }
    }
    test_assert(anon != 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, anon));
    test_assert(ecs_has_id(world, anon, marker));

    const Position *p = ecs_get(world, anon, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);

    {
        int32_t marker_count = 0;
        ecs_iter_t it = ecs_children(world, e);
        while (ecs_children_next(&it)) {
            int32_t i;
            for (i = 0; i < it.count; i ++) {
                if (ecs_has_id(world, it.entities[i], marker)) {
                    marker_count ++;
                }
            }
        }
        test_int(marker_count, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_entities_not_deleted_after_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    grandchild {}"
    LINE "    Position: {$height, 0}"
    LINE "  }"
    LINE "  if $cond {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "  for i in 0..2 {"
    LINE "    \"loop_child_$i\" {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t grandchild = ecs_lookup(world, "e.child.grandchild");
    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    ecs_entity_t loop_child_0 = ecs_lookup(world, "e.loop_child_0");
    ecs_entity_t loop_child_1 = ecs_lookup(world, "e.loop_child_1");
    test_assert(child != 0);
    test_assert(grandchild != 0);
    test_assert(cond_child != 0);
    test_assert(loop_child_0 != 0);
    test_assert(loop_child_1 != 0);

    ecs_entity_t ecs_id(TmplCondHeight) = tree;
    ecs_set(world, e, TmplCondHeight, {true, 10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_is_alive(world, grandchild));
    test_assert(ecs_is_alive(world, cond_child));
    test_assert(ecs_is_alive(world, loop_child_0));
    test_assert(ecs_is_alive(world, loop_child_1));

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_lookup(world, "e.child.grandchild") == grandchild);
    test_assert(ecs_lookup(world, "e.cond_child") == cond_child);
    test_assert(ecs_lookup(world, "e.loop_child_0") == loop_child_0);
    test_assert(ecs_lookup(world, "e.loop_child_1") == loop_child_1);

    ecs_fini(world);
}

void IncrementalUpdates_loop_entities_not_deleted_after_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {3, 20});

    test_assert(ecs_is_alive(world, child_0));
    test_assert(ecs_is_alive(world, child_1));
    test_assert(ecs_is_alive(world, child_2));
    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(ecs_lookup(world, "e.child_2") == child_2);

    ecs_fini(world);
}

void IncrementalUpdates_conditional_entities_not_deleted_after_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  if $cond {"
    LINE "    cond_child {"
    LINE "      Position: {$height, 0}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);

    ecs_entity_t ecs_id(TmplCondHeight) = tree;
    ecs_set(world, e, TmplCondHeight, {true, 10});

    test_assert(ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "e.cond_child") == cond_child);

    const Position *p = ecs_get(world, cond_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);

    ecs_fini(world);
}

void IncrementalUpdates_update_w_2_props_both_changed(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop a = flecs.meta.f32: 1"
    LINE "  prop b = flecs.meta.f32: 2"
    LINE "  child {"
    LINE "    Position: {$a, $b}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplAB) = tree;
    ecs_set(world, e, TmplAB, {10, 20});

    test_assert(ecs_lookup(world, "e.child") == child);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void IncrementalUpdates_two_templates_on_instance(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop a = flecs.meta.f32: 1"
    LINE "  child_a {"
    LINE "    Position: {$a, 0}"
    LINE "  }"
    LINE "}"
    LINE "template Bar {"
    LINE "  prop b = flecs.meta.f32: 1"
    LINE "  child_b {"
    LINE "    Position: {$b, 0}"
    LINE "  }"
    LINE "}"
    LINE "e {"
    LINE "  Foo: {}"
    LINE "  Bar: {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    typedef struct { float height; } TmplFooHeight;
    typedef struct { float height; } TmplBarHeight;
    ecs_entity_t ecs_id(TmplFooHeight) = foo;
    ecs_entity_t ecs_id(TmplBarHeight) = bar;

    ecs_set(world, e, TmplFooHeight, {10});

    test_assert(ecs_lookup(world, "e.child_a") == child_a);
    test_assert(ecs_lookup(world, "e.child_b") == child_b);

    {
        const Position *p = ecs_get(world, child_a, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }
    {
        const Position *p = ecs_get(world, child_b, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_set(world, e, TmplBarHeight, {20});

    test_assert(ecs_lookup(world, "e.child_a") == child_a);
    test_assert(ecs_lookup(world, "e.child_b") == child_b);

    {
        const Position *p = ecs_get(world, child_b, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void IncrementalUpdates_pair_w_entity_prop_target(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "TgtB {}"
    LINE "template Tree {"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  child {"
    LINE "    (Rel, $x)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_has_pair(world, child, rel, tgt_a));

    ecs_entity_t ecs_id(ecs_entity_t) = tree;
    ecs_set(world, e, ecs_entity_t, {tgt_b});

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has_pair(world, child, rel, tgt_b));
    test_assert(!ecs_has_pair(world, child, rel, tgt_a));

    ecs_fini(world);
}

void IncrementalUpdates_pair_scope_w_entity_prop_target(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "TgtB {}"
    LINE "template Tree {"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  (Rel, $x) {"
    LINE "    child {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_has_pair(world, child, rel, tgt_a));

    ecs_entity_t ecs_id(ecs_entity_t) = tree;
    ecs_set(world, e, ecs_entity_t, {tgt_b});

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has_pair(world, child, rel, tgt_b));
    test_assert(!ecs_has_pair(world, child, rel, tgt_a));

    ecs_fini(world);
}

void IncrementalUpdates_composite_prop_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop pos = Position: {10, 20}"
    LINE "  child {"
    LINE "    Position: $pos"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    typedef struct { Position pos; } T;
    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {{30, 40}});

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void IncrementalUpdates_with_prop_value_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop pos = Position: {10, 20}"
    LINE "  with $pos {"
    LINE "    child_a {}"
    LINE "    child_b {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    typedef struct { Position pos; } T;
    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {{30, 40}});

    test_assert(ecs_lookup(world, "e.child_a") == child_a);
    test_assert(ecs_lookup(world, "e.child_b") == child_b);

    {
        const Position *p = ecs_get(world, child_a, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Position *p = ecs_get(world, child_b, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void IncrementalUpdates_else_if_chain(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop v = flecs.meta.i32: 1"
    LINE "  if $v == 1 {"
    LINE "    child_a {}"
    LINE "  } else if $v == 2 {"
    LINE "    child_b {}"
    LINE "  } else {"
    LINE "    child_c {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    test_assert(child_a != 0);
    test_assert(ecs_lookup(world, "e.child_b") == 0);
    test_assert(ecs_lookup(world, "e.child_c") == 0);

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {2});

    test_assert(!ecs_is_alive(world, child_a));
    test_assert(ecs_lookup(world, "e.child_a") == 0);
    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    test_assert(child_b != 0);
    test_assert(ecs_lookup(world, "e.child_c") == 0);

    ecs_set(world, e, int32_t, {3});

    test_assert(!ecs_is_alive(world, child_b));
    test_assert(ecs_lookup(world, "e.child_a") == 0);
    test_assert(ecs_lookup(world, "e.child_b") == 0);
    ecs_entity_t child_c = ecs_lookup(world, "e.child_c");
    test_assert(child_c != 0);

    ecs_set(world, e, int32_t, {1});

    test_assert(!ecs_is_alive(world, child_c));
    test_assert(ecs_lookup(world, "e.child_a") != 0);
    test_assert(ecs_lookup(world, "e.child_b") == 0);
    test_assert(ecs_lookup(world, "e.child_c") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_loop_negative_range(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 1"
    LINE "  for i in -1..$count {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, 0}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_min_1 = ecs_lookup(world, "e.child_-1");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    test_assert(child_min_1 != 0);
    test_assert(child_0 != 0);
    test_assert(ecs_lookup(world, "e.child_1") == 0);

    ecs_add(world, child_min_1, UserTag);

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {2});

    test_assert(ecs_lookup(world, "e.child_-1") == child_min_1);
    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") != 0);
    test_assert(ecs_has(world, child_min_1, UserTag));

    ecs_fini(world);
}

void IncrementalUpdates_nested_for_loop_value_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop h = flecs.meta.f32: 1"
    LINE "  for i in 0..2 {"
    LINE "    for j in 0..2 {"
    LINE "      \"child_{$i}_{$j}\" {"
    LINE "        Position: {$j, $h}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_00 = ecs_lookup(world, "e.child_0_0");
    ecs_entity_t child_01 = ecs_lookup(world, "e.child_0_1");
    ecs_entity_t child_10 = ecs_lookup(world, "e.child_1_0");
    ecs_entity_t child_11 = ecs_lookup(world, "e.child_1_1");
    test_assert(child_00 != 0);
    test_assert(child_01 != 0);
    test_assert(child_10 != 0);
    test_assert(child_11 != 0);

    ecs_add(world, child_11, UserTag);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.child_0_0") == child_00);
    test_assert(ecs_lookup(world, "e.child_0_1") == child_01);
    test_assert(ecs_lookup(world, "e.child_1_0") == child_10);
    test_assert(ecs_lookup(world, "e.child_1_1") == child_11);
    test_assert(ecs_has(world, child_11, UserTag));

    {
        const Position *p = ecs_get(world, child_01, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 10);
    }
    {
        const Position *p = ecs_get(world, child_10, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_prefab_child_in_template(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop h = flecs.meta.f32: 1"
    LINE "  prefab Base {"
    LINE "    Position: {$h, 0}"
    LINE "  }"
    LINE "  inst {"
    LINE "    (IsA, Base)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t base = ecs_lookup(world, "e.Base");
    ecs_entity_t inst = ecs_lookup(world, "e.inst");
    test_assert(base != 0);
    test_assert(inst != 0);
    test_assert(ecs_has_pair(world, inst, EcsIsA, base));

    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.Base") == base);
    test_assert(ecs_lookup(world, "e.inst") == inst);
    test_assert(ecs_has_pair(world, inst, EcsIsA, base));

    {
        const Position *p = ecs_get(world, base, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }
    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_tree_parent_children_preserved(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "@tree Parent"
    LINE "template Tree {"
    LINE "  prop h = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$h, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup_child(world, e, "child");
    test_assert(child != 0);

    {
        const EcsParent *p = ecs_get(world, child, EcsParent);
        test_assert(p != NULL);
        test_uint(p->value, e);
    }

    ecs_add(world, child, UserTag);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup_child(world, e, "child") == child);
    test_assert(ecs_has(world, child, UserTag));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_bulk_create_then_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop h = flecs.meta.f32: 0"
    LINE "  child {"
    LINE "    Position: {$h, 0}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    TmplHeight values[] = {{10}, {20}, {30}};
    void *data[] = {values};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {tree},
        .data = data
    });

    ecs_entity_t e0 = entities[0];
    ecs_entity_t e1 = entities[1];
    ecs_entity_t e2 = entities[2];

    ecs_entity_t child_0 = ecs_lookup_child(world, e0, "child");
    ecs_entity_t child_1 = ecs_lookup_child(world, e1, "child");
    ecs_entity_t child_2 = ecs_lookup_child(world, e2, "child");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(child_2 != 0);

    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e1, TmplHeight, {40});

    test_assert(ecs_lookup_child(world, e0, "child") == child_0);
    test_assert(ecs_lookup_child(world, e1, "child") == child_1);
    test_assert(ecs_lookup_child(world, e2, "child") == child_2);

    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 40);
    }
    {
        const Position *p = ecs_get(world, child_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_string_prop_conditional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop s = flecs.meta.string: \"a\""
    LINE "  if $s == \"b\" {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "  child {}"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_lookup(world, "e.cond_child") == 0);

    const ecs_type_info_t *ti = ecs_get_type_info(world, tree);
    test_assert(ti != NULL);

    {
        void *ptr = ecs_ensure_id(world, e, tree, (size_t)ti->size);
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, tree, ptr);
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_set_string(&cur, "b"), 0);
        test_int(ecs_meta_pop(&cur), 0);
        ecs_modified_id(world, e, tree);
    }

    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);
    test_assert(ecs_lookup(world, "e.child") == child);

    {
        void *ptr = ecs_ensure_id(world, e, tree, (size_t)ti->size);
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, tree, ptr);
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_set_string(&cur, "a"), 0);
        test_int(ecs_meta_pop(&cur), 0);
        ecs_modified_id(world, e, tree);
    }

    test_assert(!ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "e.cond_child") == 0);
    test_assert(ecs_lookup(world, "e.child") == child);

    ecs_fini(world);
}

void IncrementalUpdates_const_in_conditional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  if $cond {"
    LINE "    const h2: $height * 2"
    LINE "    cond_child {"
    LINE "      Position: {$h2, 0}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);

    {
        const Position *p = ecs_get(world, cond_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
    }

    ecs_entity_t ecs_id(TmplCondHeight) = tree;
    ecs_set(world, e, TmplCondHeight, {true, 10});

    test_assert(ecs_lookup(world, "e.cond_child") == cond_child);

    {
        const Position *p = ecs_get(world, cond_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_set(world, e, TmplCondHeight, {false, 10});

    test_assert(!ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "e.cond_child") == 0);

    ecs_set(world, e, TmplCondHeight, {true, 20});

    cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);

    {
        const Position *p = ecs_get(world, cond_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 40);
    }

    ecs_fini(world);
}

void IncrementalUpdates_const_in_loop(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..2 {"
    LINE "    const t: $i + $height"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$t, 0}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);

    ecs_add(world, child_0, UserTag);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(ecs_has(world, child_0, UserTag));

    {
        const Position *p = ecs_get(world, child_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }
    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 11);
    }

    ecs_fini(world);
}

void IncrementalUpdates_fn_call_dependency(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "fn double(a: f32) -> f32 { a * 2 }"
    LINE "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {double($height), 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
    }

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void IncrementalUpdates_fn_call_in_condition(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn check(a: i32) -> i32 { a }"
    LINE "template Tree {"
    LINE "  prop v = flecs.meta.i32: 1"
    LINE "  if check($v) == 1 {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {2});

    test_assert(!ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "e.cond_child") == 0);

    ecs_set(world, e, int32_t, {1});

    test_assert(ecs_lookup(world, "e.cond_child") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_all_primitive_props(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    position_set_invoked = 0;
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = { EcsOnSet },
        .callback = PositionSetObserver
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop p0 = flecs.meta.u8: 1"
    LINE "  prop p1 = flecs.meta.u16: 2"
    LINE "  prop p2 = flecs.meta.u32: 3"
    LINE "  prop p3 = flecs.meta.u64: 4"
    LINE "  prop p4 = flecs.meta.i8: 5"
    LINE "  prop p5 = flecs.meta.i16: 6"
    LINE "  prop p6 = flecs.meta.i64: 7"
    LINE "  prop p7 = flecs.meta.f32: 8"
    LINE "  prop p8 = flecs.meta.f64: 9"
    LINE "  child_0 { Position: {$p0, 0} }"
    LINE "  child_1 { Position: {$p1, 0} }"
    LINE "  child_2 { Position: {$p2, 0} }"
    LINE "  child_3 { Position: {$p3, 0} }"
    LINE "  child_4 { Position: {$p4, 0} }"
    LINE "  child_5 { Position: {$p5, 0} }"
    LINE "  child_6 { Position: {$p6, 0} }"
    LINE "  child_7 { Position: {$p7, 0} }"
    LINE "  child_8 { Position: {$p8, 0} }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_int(position_set_invoked, 9);

    typedef struct {
        uint8_t p0;
        uint16_t p1;
        uint32_t p2;
        uint64_t p3;
        int8_t p4;
        int16_t p5;
        int64_t p6;
        float p7;
        double p8;
    } T;

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");

    const ecs_type_info_t *ti = ecs_get_type_info(world, tree);
    test_assert(ti != NULL);
    test_int(ti->size, ECS_SIZEOF(T));

    ecs_entity_t children[9];
    int32_t i;
    for (i = 0; i < 9; i ++) {
        char name[32];
        ecs_os_snprintf(name, 32, "e.child_%d", i);
        children[i] = ecs_lookup(world, name);
        test_assert(children[i] != 0);

        const Position *p = ecs_get(world, children[i], Position);
        test_assert(p != NULL);
        test_int(p->x, i + 1);
    }

    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {1, 2, 3, 40, 5, 6, 7, 8, 9});

    test_int(position_set_invoked, 10);

    for (i = 0; i < 9; i ++) {
        char name[32];
        ecs_os_snprintf(name, 32, "e.child_%d", i);
        test_assert(ecs_lookup(world, name) == children[i]);

        const Position *p = ecs_get(world, children[i], Position);
        test_assert(p != NULL);
        if (i == 3) {
            test_int(p->x, 40);
        } else {
            test_int(p->x, i + 1);
        }
    }

    ecs_set(world, e, T, {10, 2, 3, 40, 5, 6, 7, 8, 90});

    test_int(position_set_invoked, 12);

    {
        const Position *p = ecs_get(world, children[0], Position);
        test_int(p->x, 10);
    }
    {
        const Position *p = ecs_get(world, children[8], Position);
        test_int(p->x, 90);
    }

    ecs_fini(world);
}

void IncrementalUpdates_array_prop_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    ecs_array(world, {
        .entity = ecs_entity(world, { .name = "FloatArr" }),
        .type = ecs_id(ecs_f32_t),
        .count = 3
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop arr = FloatArr: [1, 2, 3]"
    LINE "  child {"
    LINE "    Position: {$arr[0], $arr[1]}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    typedef struct { float v[3]; } T;
    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {{10, 20, 30}});

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void IncrementalUpdates_vector_prop_update(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec_type = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "FloatVec" }),
        .type = ecs_id(ecs_f32_t)
    });

    ecs_entity_t holder = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Holder" }),
        .members = {{ .name = "vec", .type = vec_type }}
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop v = FloatVec: [1, 2, 3]"
    LINE "  child {"
    LINE "    Holder: {vec: $v}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const ecs_vec_t *ptr = ecs_get_id(world, child, holder);
        test_assert(ptr != NULL);
        test_int(ecs_vec_count(ptr), 3);
        test_int(*(float*)ecs_vec_get_t(ptr, float, 0), 1);
        test_int(*(float*)ecs_vec_get_t(ptr, float, 2), 3);
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, tree);
    test_assert(ti != NULL);

    {
        void *ptr = ecs_ensure_id(world, e, tree, (size_t)ti->size);
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, tree, ptr);
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_member(&cur, "v"), 0);
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_set_float(&cur, 10), 0);
        test_int(ecs_meta_next(&cur), 0);
        test_int(ecs_meta_set_float(&cur, 20), 0);
        test_int(ecs_meta_pop(&cur), 0);
        test_int(ecs_meta_pop(&cur), 0);
        ecs_modified_id(world, e, tree);
    }

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const ecs_vec_t *ptr = ecs_get_id(world, child, holder);
        test_assert(ptr != NULL);
        test_int(ecs_vec_count(ptr), 3);
        test_int(*(float*)ecs_vec_get_t(ptr, float, 0), 10);
        test_int(*(float*)ecs_vec_get_t(ptr, float, 1), 20);
        test_int(*(float*)ecs_vec_get_t(ptr, float, 2), 3);
    }

    ecs_fini(world);
}

typedef struct {
    int32_t x, y;
} IuOpaqueStruct;

static
void* IuOpaqueStruct_member(void *ptr, const char *member) {
    IuOpaqueStruct *data = ptr;
    if (!strcmp(member, "x")) {
        return &data->x;
    } else if (!strcmp(member, "y")) {
        return &data->y;
    }
    return NULL;
}

void IncrementalUpdates_opaque_prop_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IuOpaqueStruct);

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(IuOpaqueStruct),
        .type.as_type = s,
        .type.ensure_member = IuOpaqueStruct_member
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop o = IuOpaqueStruct: {10, 20}"
    LINE "  child {"
    LINE "    IuOpaqueStruct: $o"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const IuOpaqueStruct *p = ecs_get(world, child, IuOpaqueStruct);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    typedef struct { IuOpaqueStruct o; } T;
    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {{30, 40}});

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const IuOpaqueStruct *p = ecs_get(world, child, IuOpaqueStruct);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void IncrementalUpdates_conditional_component_on_child(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  child {"
    LINE "    if $cond {"
    LINE "      Foo"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_has_id(world, child, foo));

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(!ecs_has_id(world, child, foo));

    ecs_set(world, e, TmplCond, {true});

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has_id(world, child, foo));

    ecs_fini(world);
}

void IncrementalUpdates_match_expr_dependency(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop v = flecs.meta.i32: 1"
    LINE "  const size = flecs.meta.f32: match $v {"
    LINE "    1: 10"
    LINE "    2: 20"
    LINE "  }"
    LINE "  child {"
    LINE "    Position: {$size, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {2});

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void IncrementalUpdates_new_expr_in_const_incremental(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "Foo {}"
    LINE "Rel {}"
    LINE "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  const helper: new { Foo }"
    LINE "  child {"
    LINE "    (Rel, $helper)"
    LINE "    Position: {$height, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t helper = ecs_get_target(world, child, rel, 0);
    test_assert(helper != 0);
    test_assert(ecs_has_id(world, helper, foo));
    test_assert(ecs_has_pair(world, helper, EcsChildOf, e));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(!ecs_is_alive(world, child));
    test_assert(!ecs_is_alive(world, helper));

    ecs_entity_t new_child = ecs_lookup(world, "e.child");
    test_assert(new_child != 0);
    test_assert(new_child != child);

    ecs_entity_t new_helper = ecs_get_target(world, new_child, rel, 0);
    test_assert(new_helper != 0);
    test_assert(new_helper != helper);
    test_assert(ecs_has_id(world, new_helper, foo));
    test_assert(ecs_has_pair(world, new_helper, EcsChildOf, e));

    {
        const Position *p = ecs_get(world, new_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_new_expr_incremental(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ref_comp = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "RefComp" }),
        .members = {
            {"e", ecs_id(ecs_entity_t)},
            {"x", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    RefComp: {e: new {}, x: $height}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    typedef struct { ecs_entity_t e; float x; } RefComp;

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t first_ref;
    {
        const RefComp *rc = ecs_get_id(world, child, ref_comp);
        test_assert(rc != NULL);
        test_int(rc->x, 1);
        test_assert(rc->e != 0);
        test_assert(ecs_is_alive(world, rc->e));
        first_ref = rc->e;
    }

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(!ecs_is_alive(world, child));
    test_assert(!ecs_is_alive(world, first_ref));

    ecs_entity_t new_child = ecs_lookup(world, "e.child");
    test_assert(new_child != 0);
    test_assert(new_child != child);

    {
        const RefComp *rc = ecs_get_id(world, new_child, ref_comp);
        test_assert(rc != NULL);
        test_int(rc->x, 10);
        test_assert(rc->e != 0);
        test_assert(rc->e != first_ref);
        test_assert(ecs_is_alive(world, rc->e));
    }

    ecs_fini(world);
}

void IncrementalUpdates_nested_template_3_levels(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Leaf {"
    LINE "  prop val = flecs.meta.f32: 1"
    LINE "  leaf_child {"
    LINE "    Position: {$val, 0}"
    LINE "  }"
    LINE "}"
    LINE "template Mid {"
    LINE "  prop val = flecs.meta.f32: 1"
    LINE "  mid_child {"
    LINE "    Leaf: {val: $val}"
    LINE "  }"
    LINE "}"
    LINE "template Outer {"
    LINE "  prop val = flecs.meta.f32: 1"
    LINE "  outer_child {"
    LINE "    Mid: {val: $val}"
    LINE "  }"
    LINE "}"
    LINE "Outer e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t outer = ecs_lookup(world, "Outer");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t outer_child = ecs_lookup(world, "e.outer_child");
    ecs_entity_t mid_child = ecs_lookup(world, "e.outer_child.mid_child");
    ecs_entity_t leaf_child = ecs_lookup(world,
        "e.outer_child.mid_child.leaf_child");
    test_assert(outer_child != 0);
    test_assert(mid_child != 0);
    test_assert(leaf_child != 0);

    ecs_add(world, leaf_child, UserTag);

    {
        const Position *p = ecs_get(world, leaf_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_entity_t ecs_id(TmplHeight) = outer;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_lookup(world, "e.outer_child") == outer_child);
    test_assert(ecs_lookup(world, "e.outer_child.mid_child") == mid_child);
    test_assert(ecs_lookup(world,
        "e.outer_child.mid_child.leaf_child") == leaf_child);
    test_assert(ecs_has(world, leaf_child, UserTag));

    {
        const Position *p = ecs_get(world, leaf_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_template_w_64_props_incremental(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_strbuf_appendlit(&buf, "template Tree {\n");
    int i;
    for (i = 0; i < 64; i ++) {
        ecs_strbuf_append(&buf, "prop p%d = flecs.meta.f32: %d\n", i, i + 1);
    }
    ecs_strbuf_appendlit(&buf, 
        "child {\n"
        "Position: {$p0, $p63}\n"
        "}\n"
        "}\n"
        "Tree e()\n");
    char *expr = ecs_strbuf_get(&buf);

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    ecs_os_free(expr);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 64);
    }

    float values[64];
    for (i = 0; i < 64; i ++) {
        values[i] = (float)(i + 1);
    }
    values[0] = 100;
    ecs_set_id(world, e, tree, sizeof(values), values);

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 100);
        test_int(p->y, 64);
    }

    ecs_fini(world);
}

void IncrementalUpdates_template_w_65_props_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_strbuf_appendlit(&buf, "template Tree {\n");
    int i;
    for (i = 0; i < 65; i ++) {
        ecs_strbuf_append(&buf, "prop p%d = flecs.meta.f32: %d\n", i, i + 1);
    }
    ecs_strbuf_appendlit(&buf,
        "child {\n"
        "Position: {$p0, $p64}\n"
        "}\n"
        "}\n"
        "Tree e()\n");
    char *expr = ecs_strbuf_get(&buf);

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_os_free(expr);

    ecs_fini(world);
}

void IncrementalUpdates_loop_child_deleted_by_app(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);

    ecs_delete(world, child_1);
    test_assert(ecs_lookup(world, "e.child_1") == 0);

    test_expect_abort();

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {2, 10});
}

void IncrementalUpdates_pair_w_entity_prop_target_in_loop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "TgtB {}"
    LINE "template Tree {"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      (Rel, $x)"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(ecs_has_pair(world, child_0, rel, tgt_a));
    test_assert(ecs_has_pair(world, child_1, rel, tgt_a));

    typedef struct { ecs_entity_t x; int32_t count; } T;
    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {tgt_b, 2});

    test_assert(!ecs_is_alive(world, child_0));
    test_assert(!ecs_is_alive(world, child_1));

    ecs_entity_t new_child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t new_child_1 = ecs_lookup(world, "e.child_1");
    test_assert(new_child_0 != 0);
    test_assert(new_child_1 != 0);
    test_assert(new_child_0 != child_0);
    test_assert(new_child_1 != child_1);
    test_assert(ecs_has_pair(world, new_child_0, rel, tgt_b));
    test_assert(ecs_has_pair(world, new_child_1, rel, tgt_b));
    test_assert(!ecs_has_pair(world, new_child_0, rel, tgt_a));
    test_assert(!ecs_has_pair(world, new_child_1, rel, tgt_a));

    ecs_fini(world);
}

void IncrementalUpdates_pair_component_w_entity_prop_target(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "TgtA {}"
    LINE "TgtB {}"
    LINE "template Tree {"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  child {"
    LINE "    (Position, $x): {5, 6}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get_id(world, child,
            ecs_pair(ecs_id(Position), tgt_a));
        test_assert(p != NULL);
        test_int(p->x, 5);
        test_int(p->y, 6);
    }

    ecs_entity_t ecs_id(ecs_entity_t) = tree;
    ecs_set(world, e, ecs_entity_t, {tgt_b});

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(!ecs_has_pair(world, child, ecs_id(Position), tgt_a));

    {
        const Position *p = ecs_get_id(world, child,
            ecs_pair(ecs_id(Position), tgt_b));
        test_assert(p != NULL);
        test_int(p->x, 5);
        test_int(p->y, 6);
    }

    ecs_fini(world);
}

void IncrementalUpdates_pair_w_entity_prop_first(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "RelA {}"
    LINE "RelB {}"
    LINE "Tgt {}"
    LINE "template Tree {"
    LINE "  prop r = flecs.meta.entity: RelA"
    LINE "  child {"
    LINE "    ($r, Tgt)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel_a = ecs_lookup(world, "RelA");
    ecs_entity_t rel_b = ecs_lookup(world, "RelB");
    ecs_entity_t tgt = ecs_lookup(world, "Tgt");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_has_pair(world, child, rel_a, tgt));

    ecs_entity_t ecs_id(ecs_entity_t) = tree;
    ecs_set(world, e, ecs_entity_t, {rel_b});

    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has_pair(world, child, rel_b, tgt));
    test_assert(!ecs_has_pair(world, child, rel_a, tgt));

    ecs_fini(world);
}

void IncrementalUpdates_with_pair_w_entity_prop_target(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "TgtB {}"
    LINE "template Tree {"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  with (Rel, $x) {"
    LINE "    child_a {}"
    LINE "    child_b {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    test_assert(child_a != 0);
    test_assert(child_b != 0);
    test_assert(ecs_has_pair(world, child_a, rel, tgt_a));
    test_assert(ecs_has_pair(world, child_b, rel, tgt_a));

    ecs_entity_t ecs_id(ecs_entity_t) = tree;
    ecs_set(world, e, ecs_entity_t, {tgt_b});

    test_assert(ecs_lookup(world, "e.child_a") == child_a);
    test_assert(ecs_lookup(world, "e.child_b") == child_b);
    test_assert(ecs_has_pair(world, child_a, rel, tgt_b));
    test_assert(ecs_has_pair(world, child_b, rel, tgt_b));
    test_assert(!ecs_has_pair(world, child_a, rel, tgt_a));
    test_assert(!ecs_has_pair(world, child_b, rel, tgt_a));

    ecs_fini(world);
}

void IncrementalUpdates_nested_conditional_stale_inner_control(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop outer = flecs.meta.bool: true"
    LINE "  prop inner = flecs.meta.bool: true"
    LINE "  if $outer {"
    LINE "    if $inner {"
    LINE "      child_a {}"
    LINE "    } else {"
    LINE "      child_b {}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    test_assert(child_a != 0);
    test_assert(ecs_lookup(world, "e.child_b") == 0);

    typedef struct { bool outer; bool inner; } T;

    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {false, true});

    test_assert(!ecs_is_alive(world, child_a));
    test_assert(ecs_lookup(world, "e.child_a") == 0);
    test_assert(ecs_lookup(world, "e.child_b") == 0);

    ecs_set(world, e, T, {false, false});

    test_assert(ecs_lookup(world, "e.child_a") == 0);
    test_assert(ecs_lookup(world, "e.child_b") == 0);

    ecs_set(world, e, T, {true, false});

    test_assert(ecs_lookup(world, "e.child_a") == 0);
    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    test_assert(child_b != 0);

    ecs_fini(world);
}

void IncrementalUpdates_loop_child_w_grandchild_value_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      grandchild {"
    LINE "        Position: {$i, $height}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t gc_0 = ecs_lookup(world, "e.child_0.grandchild");
    ecs_entity_t gc_1 = ecs_lookup(world, "e.child_1.grandchild");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(gc_0 != 0);
    test_assert(gc_1 != 0);

    ecs_add(world, gc_1, UserTag);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {2, 10});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(ecs_lookup(world, "e.child_0.grandchild") == gc_0);
    test_assert(ecs_lookup(world, "e.child_1.grandchild") == gc_1);
    test_assert(ecs_has(world, gc_1, UserTag));

    {
        const Position *p = ecs_get(world, gc_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 10);
    }
    {
        const Position *p = ecs_get(world, gc_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_loop_child_w_grandchild_count_decrease(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      grandchild {"
    LINE "        Position: {$i, $height}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");
    ecs_entity_t gc_0 = ecs_lookup(world, "e.child_0.grandchild");
    ecs_entity_t gc_1 = ecs_lookup(world, "e.child_1.grandchild");
    ecs_entity_t gc_2 = ecs_lookup(world, "e.child_2.grandchild");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(child_2 != 0);
    test_assert(gc_0 != 0);
    test_assert(gc_1 != 0);
    test_assert(gc_2 != 0);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {1, 1});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_0.grandchild") == gc_0);
    test_assert(!ecs_is_alive(world, child_1));
    test_assert(!ecs_is_alive(world, child_2));
    test_assert(!ecs_is_alive(world, gc_1));
    test_assert(!ecs_is_alive(world, gc_2));

    ecs_fini(world);
}

void IncrementalUpdates_child_name_from_string_prop(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, UserTag);

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop suffix = flecs.meta.string: \"a\""
    LINE "  \"child_$suffix\" {}"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child_a");
    test_assert(child != 0);

    ecs_add(world, child, UserTag);

    const ecs_type_info_t *ti = ecs_get_type_info(world, tree);
    test_assert(ti != NULL);

    {
        void *ptr = ecs_ensure_id(world, e, tree, (size_t)ti->size);
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, tree, ptr);
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_set_string(&cur, "b"), 0);
        test_int(ecs_meta_pop(&cur), 0);
        ecs_modified_id(world, e, tree);
    }

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child_a") == 0);
    test_assert(ecs_lookup(world, "e.child_b") == child);
    test_assert(ecs_has(world, child, UserTag));

    ecs_fini(world);
}

void IncrementalUpdates_string_prop_component_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t string_holder = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "StringHolder" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop s = flecs.meta.string: \"hello\""
    LINE "  child {"
    LINE "    StringHolder: {$s}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    typedef struct { char *value; } StringHolder;

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const StringHolder *sh = ecs_get_id(world, child, string_holder);
        test_assert(sh != NULL);
        test_str(sh->value, "hello");
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, tree);
    test_assert(ti != NULL);

    {
        void *ptr = ecs_ensure_id(world, e, tree, (size_t)ti->size);
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, tree, ptr);
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_set_string(&cur, "world"), 0);
        test_int(ecs_meta_pop(&cur), 0);
        ecs_modified_id(world, e, tree);
    }

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const StringHolder *sh = ecs_get_id(world, child, string_holder);
        test_assert(sh != NULL);
        test_str(sh->value, "world");
    }

    {
        void *ptr = ecs_ensure_id(world, e, tree, (size_t)ti->size);
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, tree, ptr);
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_set_string(&cur, "world"), 0);
        test_int(ecs_meta_pop(&cur), 0);
        ecs_modified_id(world, e, tree);
    }

    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const StringHolder *sh = ecs_get_id(world, child, string_holder);
        test_assert(sh != NULL);
        test_str(sh->value, "world");
    }

    ecs_fini(world);
}

void IncrementalUpdates_same_value_no_reevaluation(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    position_set_invoked = 0;
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = { EcsOnSet },
        .callback = PositionSetObserver
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_int(position_set_invoked, 1);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {1});

    test_int(position_set_invoked, 1);
    test_assert(ecs_lookup(world, "e.child") == child);

    ecs_set(world, e, TmplHeight, {10});

    test_int(position_set_invoked, 2);
    test_assert(ecs_lookup(world, "e.child") == child);

    ecs_fini(world);
}

void IncrementalUpdates_remove_template_then_set(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_remove_id(world, e, tree);
    test_assert(!ecs_has_id(world, e, tree));

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    ecs_entity_t new_child = ecs_lookup(world, "e.child");
    test_assert(new_child != 0);

    const Position *p = ecs_get(world, new_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 2);

    ecs_fini(world);
}

void IncrementalUpdates_conditional_change_deferred(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);

    ecs_defer_begin(world);
    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "e.cond_child") == 0);

    ecs_defer_begin(world);
    ecs_set(world, e, TmplCond, {true});
    ecs_defer_end(world);

    test_assert(ecs_lookup(world, "e.cond_child") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_with_in_loop_value_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    with Position($i, $height) {"
    LINE "      \"child_$i\" {}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);

    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 1);
    }

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {2, 10});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);

    {
        const Position *p = ecs_get(world, child_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 10);
    }
    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_loop_bounds_from_const(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 1"
    LINE "  const n: $count * 2"
    LINE "  for i in 0..$n {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, 0}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(ecs_lookup(world, "e.child_2") == 0);

    typedef struct { int32_t count; } T;

    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {2});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");
    ecs_entity_t child_3 = ecs_lookup(world, "e.child_3");
    test_assert(child_2 != 0);
    test_assert(child_3 != 0);

    {
        const Position *p = ecs_get(world, child_3, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
    }

    ecs_set(world, e, T, {1});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(!ecs_is_alive(world, child_2));
    test_assert(!ecs_is_alive(world, child_3));

    ecs_fini(world);
}

void IncrementalUpdates_conditional_grandchild_flip_then_unrelated_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  if $cond {"
    LINE "    cond_child {"
    LINE "      grandchild {}"
    LINE "    }"
    LINE "  }"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    ecs_entity_t grandchild = ecs_lookup(world, "e.cond_child.grandchild");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(cond_child != 0);
    test_assert(grandchild != 0);
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplCondHeight) = tree;
    ecs_set(world, e, TmplCondHeight, {false, 1});

    test_assert(!ecs_is_alive(world, cond_child));
    test_assert(!ecs_is_alive(world, grandchild));
    test_assert(ecs_lookup(world, "e.child") == child);

    ecs_set(world, e, TmplCondHeight, {false, 10});

    test_assert(ecs_lookup(world, "e.child") == child);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 2);

    ecs_fini(world);
}

void IncrementalUpdates_conditional_component_in_loop_true_to_false(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  for i in 0..2 {"
    LINE "    \"child_$i\" {"
    LINE "      if $cond {"
    LINE "        Foo"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(ecs_has_id(world, child_0, foo));
    test_assert(ecs_has_id(world, child_1, foo));

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(!ecs_has_id(world, child_0, foo));
    test_assert(!ecs_has_id(world, child_1, foo));

    ecs_set(world, e, TmplCond, {true});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(ecs_has_id(world, child_0, foo));
    test_assert(ecs_has_id(world, child_1, foo));

    ecs_fini(world);
}

void IncrementalUpdates_loop_range_shift_rename_collision(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, UserTag);

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop offset = flecs.meta.i32: 0"
    LINE "  for i in $offset..($offset + 2) {"
    LINE "    \"child_$i\" {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);

    ecs_add(world, child_0, UserTag);

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {1});

    test_assert(ecs_is_alive(world, child_0));
    test_assert(ecs_is_alive(world, child_1));
    test_assert(ecs_lookup(world, "e.child_0") == 0);
    test_assert(ecs_lookup(world, "e.child_1") == child_0);
    test_assert(ecs_lookup(world, "e.child_2") == child_1);
    test_assert(ecs_has(world, child_0, UserTag));

    ecs_fini(world);
}

void IncrementalUpdates_with_pair_w_entity_prop_target_around_loop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "TgtB {}"
    LINE "template Tree {"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  with (Rel, $x) {"
    LINE "    for i in 0..2 {"
    LINE "      \"child_$i\" {}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(ecs_has_pair(world, child_0, rel, tgt_a));
    test_assert(ecs_has_pair(world, child_1, rel, tgt_a));

    ecs_entity_t ecs_id(ecs_entity_t) = tree;
    ecs_set(world, e, ecs_entity_t, {tgt_b});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(ecs_has_pair(world, child_0, rel, tgt_b));
    test_assert(ecs_has_pair(world, child_1, rel, tgt_b));
    test_assert(!ecs_has_pair(world, child_0, rel, tgt_a));
    test_assert(!ecs_has_pair(world, child_1, rel, tgt_a));

    ecs_fini(world);
}

void IncrementalUpdates_deferred_update_instance_deleted_before_flush(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_defer_begin(world);
    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});
    ecs_delete(world, e);
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_deferred_string_prop_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop s = flecs.meta.string: \"a\""
    LINE "  if $s == \"b\" {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "  child {}"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_lookup(world, "e.cond_child") == 0);

    const ecs_type_info_t *ti = ecs_get_type_info(world, tree);
    test_assert(ti != NULL);

    ecs_defer_begin(world);
    {
        void *ptr = ecs_ensure_id(world, e, tree, (size_t)ti->size);
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, tree, ptr);
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_set_string(&cur, "b"), 0);
        test_int(ecs_meta_pop(&cur), 0);
        ecs_modified_id(world, e, tree);
    }
    ecs_defer_end(world);

    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);
    test_assert(ecs_lookup(world, "e.child") == child);

    ecs_defer_begin(world);
    {
        void *ptr = ecs_ensure_id(world, e, tree, (size_t)ti->size);
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, tree, ptr);
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_set_string(&cur, "a"), 0);
        test_int(ecs_meta_pop(&cur), 0);
        ecs_modified_id(world, e, tree);
    }
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "e.cond_child") == 0);
    test_assert(ecs_lookup(world, "e.child") == child);

    ecs_fini(world);
}

void IncrementalUpdates_update_after_eval_error_recovers(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "TgtB {}"
    LINE "template Tree {"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  child {"
    LINE "    (Rel, $x)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_has_pair(world, child, rel, tgt_a));

    ecs_log_set_level(-4);
    ecs_entity_t ecs_id(ecs_entity_t) = tree;
    ecs_set(world, e, ecs_entity_t, {0});
    ecs_log_set_level(-1);

    ecs_set(world, e, ecs_entity_t, {tgt_b});

    child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_has_pair(world, child, rel, tgt_b));
    test_assert(!ecs_has_pair(world, child, rel, tgt_a));

    ecs_fini(world);
}

void IncrementalUpdates_zero_props_template_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, UserTag);

    const char *expr =
    HEAD "template Tree {"
    LINE "  child {}"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_add(world, child, UserTag);

    char dummy = 0;
    ecs_set_id(world, e, tree, 1, &dummy);

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has(world, child, UserTag));

    ecs_fini(world);
}

typedef struct {
    bool cond;
    ecs_entity_t x;
} TmplCondEntity;

typedef struct {
    int32_t start;
} TmplStart;

typedef struct {
    float x;
    ecs_entity_t t;
} TmplValueEntity;

void IncrementalUpdates_conditional_pair_w_prop_target_true_to_false(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  if $cond {"
    LINE "    (Rel, $x)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(ecs_has_pair(world, e, rel, tgt_a));

    ecs_entity_t ecs_id(TmplCondEntity) = tree;
    ecs_set(world, e, TmplCondEntity, {false, tgt_a});

    test_assert(!ecs_has_pair(world, e, rel, tgt_a));

    ecs_fini(world);
}

void IncrementalUpdates_conditional_pair_w_prop_first_two_instances(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "RelA {}"
    LINE "RelB {}"
    LINE "Tgt {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop r = flecs.meta.entity: RelA"
    LINE "  if $cond {"
    LINE "    ($r, Tgt)"
    LINE "  }"
    LINE "}"
    LINE "Tree a()"
    LINE "Tree b()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel_a = ecs_lookup(world, "RelA");
    ecs_entity_t rel_b = ecs_lookup(world, "RelB");
    ecs_entity_t tgt = ecs_lookup(world, "Tgt");
    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(ecs_has_pair(world, a, rel_a, tgt));
    test_assert(ecs_has_pair(world, b, rel_a, tgt));

    ecs_entity_t ecs_id(TmplCondEntity) = tree;
    ecs_set(world, b, TmplCondEntity, {true, rel_b});

    test_assert(ecs_has_pair(world, b, rel_b, tgt));
    test_assert(!ecs_has_pair(world, b, rel_a, tgt));

    ecs_set(world, a, TmplCondEntity, {false, rel_a});

    test_assert(!ecs_has_pair(world, a, rel_a, tgt));
    test_assert(!ecs_has_pair(world, a, rel_b, tgt));

    ecs_fini(world);
}

void IncrementalUpdates_loop_pair_target_after_range_shift(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "tgt_0 {}"
    LINE "tgt_1 {}"
    LINE "tgt_2 {}"
    LINE "template Tree {"
    LINE "  prop start = flecs.meta.i32: 0"
    LINE "  for i in $start..($start + 2) {"
    LINE "    \"child_$i\" {"
    LINE "      (Rel, tgt_$i)"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_0 = ecs_lookup(world, "tgt_0");
    ecs_entity_t tgt_1 = ecs_lookup(world, "tgt_1");
    ecs_entity_t tgt_2 = ecs_lookup(world, "tgt_2");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    test_assert(child_0 != 0);
    test_assert(ecs_has_pair(world, child_0, rel, tgt_0));

    ecs_entity_t ecs_id(TmplStart) = tree;
    ecs_set(world, e, TmplStart, {1});

    test_assert(ecs_lookup(world, "e.child_0") == 0);
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");
    test_assert(child_1 != 0);
    test_assert(child_2 != 0);

    test_assert(ecs_has_pair(world, child_1, rel, tgt_1));
    test_assert(!ecs_has_pair(world, child_1, rel, tgt_0));
    test_assert(ecs_has_pair(world, child_2, rel, tgt_2));
    test_assert(!ecs_has_pair(world, child_2, rel, tgt_0));
    test_assert(!ecs_has_pair(world, child_2, rel, tgt_1));

    ecs_fini(world);
}

void IncrementalUpdates_zero_props_reset_restores_removed_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  Foo"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(ecs_has_id(world, e, foo));

    ecs_remove_id(world, e, foo);
    test_assert(!ecs_has_id(world, e, foo));

    char dummy = 0;
    ecs_set_id(world, e, tree, 1, &dummy);

    test_assert(ecs_has_id(world, e, foo));

    ecs_fini(world);
}

void IncrementalUpdates_partial_eval_failure_then_revert(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "Rel {}"
    LINE "Tgt {}"
    LINE "template Tree {"
    LINE "  prop x = flecs.meta.f32: 1"
    LINE "  prop t = flecs.meta.entity: Tgt"
    LINE "  child {"
    LINE "    Position: {$x, 2}"
    LINE "    (Rel, $t)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t tgt = ecs_lookup(world, "Tgt");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_log_set_level(-4);
    ecs_entity_t ecs_id(TmplValueEntity) = tree;
    ecs_set(world, e, TmplValueEntity, {10, 0});
    ecs_log_set_level(-1);

    ecs_set(world, e, TmplValueEntity, {1, tgt});

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_clone_instance(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t clone = ecs_clone(world, 0, e, true);
    test_assert(clone != 0);

    ecs_fini(world);
}

void IncrementalUpdates_prop_type_deleted_then_instance_deleted(void) {
    test_quarantine("21 Jun 2026");

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Str {"
    LINE "  value = flecs.meta.string"
    LINE "}"
    LINE "template Tree {"
    LINE "  prop s = Str: {value: \"hello\"}"
    LINE "  child {}"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t str = ecs_lookup(world, "Str");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(str != 0);
    test_assert(e != 0);

    ecs_log_set_level(-4);
    ecs_delete(world, str);
    ecs_log_set_level(-1);

    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void IncrementalUpdates_loop_pair_on_instance_count_decrease(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "tgt_0 {}"
    LINE "tgt_1 {}"
    LINE "tgt_2 {}"
    LINE "template Tree {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  for i in 0..$count {"
    LINE "    (Rel, tgt_$i)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    typedef struct { int32_t count; } T;

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_0 = ecs_lookup(world, "tgt_0");
    ecs_entity_t tgt_1 = ecs_lookup(world, "tgt_1");
    ecs_entity_t tgt_2 = ecs_lookup(world, "tgt_2");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, rel, tgt_0));
    test_assert(ecs_has_pair(world, e, rel, tgt_1));
    test_assert(ecs_has_pair(world, e, rel, tgt_2));

    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {1});

    test_assert(ecs_has_pair(world, e, rel, tgt_0));
    test_assert(!ecs_has_pair(world, e, rel, tgt_1));
    test_assert(!ecs_has_pair(world, e, rel, tgt_2));

    ecs_set(world, e, T, {3});

    test_assert(ecs_has_pair(world, e, rel, tgt_0));
    test_assert(ecs_has_pair(world, e, rel, tgt_1));
    test_assert(ecs_has_pair(world, e, rel, tgt_2));

    ecs_fini(world);
}

void IncrementalUpdates_loop_pair_on_instance_range_shift(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "tgt_0 {}"
    LINE "tgt_1 {}"
    LINE "tgt_2 {}"
    LINE "template Tree {"
    LINE "  prop start = flecs.meta.i32: 0"
    LINE "  for i in $start..($start + 2) {"
    LINE "    (Rel, tgt_$i)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_0 = ecs_lookup(world, "tgt_0");
    ecs_entity_t tgt_1 = ecs_lookup(world, "tgt_1");
    ecs_entity_t tgt_2 = ecs_lookup(world, "tgt_2");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, rel, tgt_0));
    test_assert(ecs_has_pair(world, e, rel, tgt_1));
    test_assert(!ecs_has_pair(world, e, rel, tgt_2));

    ecs_entity_t ecs_id(TmplStart) = tree;
    ecs_set(world, e, TmplStart, {1});

    test_assert(!ecs_has_pair(world, e, rel, tgt_0));
    test_assert(ecs_has_pair(world, e, rel, tgt_1));
    test_assert(ecs_has_pair(world, e, rel, tgt_2));

    ecs_fini(world);
}

void IncrementalUpdates_default_component_in_conditional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "DefaultChildComponent Foo(Position)"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  if $cond {"
    LINE "    Foo holder {"
    LINE "      child = $height, 20"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.holder.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 20);
    }

    ecs_add(world, child, UserTag);

    ecs_entity_t ecs_id(TmplCondHeight) = tree;
    ecs_set(world, e, TmplCondHeight, {true, 10});

    test_assert(ecs_lookup(world, "e.holder.child") == child);
    test_assert(ecs_has(world, child, UserTag));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_set(world, e, TmplCondHeight, {false, 10});

    test_assert(!ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.holder") == 0);

    ecs_set(world, e, TmplCondHeight, {true, 5});

    ecs_entity_t new_child = ecs_lookup(world, "e.holder.child");
    test_assert(new_child != 0);

    {
        const Position *p = ecs_get(world, new_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 5);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void IncrementalUpdates_default_component_in_loop(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "DefaultChildComponent Foo(Position)"
    LINE "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    Foo \"holder_$i\" {"
    LINE "      child = $height, 20"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.holder_0.child");
    ecs_entity_t child_1 = ecs_lookup(world, "e.holder_1.child");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);

    ecs_add(world, child_0, UserTag);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {2, 10});

    test_assert(ecs_lookup(world, "e.holder_0.child") == child_0);
    test_assert(ecs_lookup(world, "e.holder_1.child") == child_1);
    test_assert(ecs_has(world, child_0, UserTag));

    {
        const Position *p = ecs_get(world, child_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }
    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, TmplCountHeight, {1, 10});

    test_assert(ecs_is_alive(world, child_0));
    test_assert(!ecs_is_alive(world, child_1));
    test_assert(ecs_lookup(world, "e.holder_1") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_new_expr_in_loop_count_decrease(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ref_comp = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "RefComp" }),
        .members = {
            {"e", ecs_id(ecs_entity_t)},
            {"x", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      RefComp: {e: new {}, x: $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    typedef struct { ecs_entity_t e; float x; } RefComp;

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);

    ecs_entity_t helper_0, helper_1;
    {
        const RefComp *rc = ecs_get_id(world, child_0, ref_comp);
        test_assert(rc != NULL);
        test_int(rc->x, 1);
        helper_0 = rc->e;
        test_assert(helper_0 != 0);
        test_assert(ecs_is_alive(world, helper_0));
    }
    {
        const RefComp *rc = ecs_get_id(world, child_1, ref_comp);
        test_assert(rc != NULL);
        helper_1 = rc->e;
        test_assert(helper_1 != 0);
        test_assert(ecs_is_alive(world, helper_1));
        test_assert(helper_1 != helper_0);
    }

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {2, 10});

    test_assert(!ecs_is_alive(world, child_0));
    test_assert(!ecs_is_alive(world, child_1));
    test_assert(!ecs_is_alive(world, helper_0));
    test_assert(!ecs_is_alive(world, helper_1));

    ecs_entity_t new_child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t new_child_1 = ecs_lookup(world, "e.child_1");
    test_assert(new_child_0 != 0);
    test_assert(new_child_1 != 0);
    test_assert(new_child_0 != child_0);
    test_assert(new_child_1 != child_1);

    ecs_entity_t new_helper_0, new_helper_1;
    {
        const RefComp *rc = ecs_get_id(world, new_child_0, ref_comp);
        test_assert(rc != NULL);
        test_int(rc->x, 10);
        new_helper_0 = rc->e;
        test_assert(new_helper_0 != 0);
        test_assert(ecs_is_alive(world, new_helper_0));
    }
    {
        const RefComp *rc = ecs_get_id(world, new_child_1, ref_comp);
        test_assert(rc != NULL);
        test_int(rc->x, 10);
        new_helper_1 = rc->e;
        test_assert(new_helper_1 != 0);
        test_assert(ecs_is_alive(world, new_helper_1));
    }

    ecs_set(world, e, TmplCountHeight, {1, 10});

    test_assert(!ecs_is_alive(world, new_child_0));
    test_assert(!ecs_is_alive(world, new_child_1));
    test_assert(!ecs_is_alive(world, new_helper_0));
    test_assert(!ecs_is_alive(world, new_helper_1));

    ecs_entity_t final_child_0 = ecs_lookup(world, "e.child_0");
    test_assert(final_child_0 != 0);
    test_assert(ecs_lookup(world, "e.child_1") == 0);

    {
        const RefComp *rc = ecs_get_id(world, final_child_0, ref_comp);
        test_assert(rc != NULL);
        test_int(rc->x, 10);
        test_assert(rc->e != 0);
        test_assert(ecs_is_alive(world, rc->e));
    }

    ecs_fini(world);
}

void IncrementalUpdates_new_expr_in_conditional_flip(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ref_comp = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "RefComp" }),
        .members = {
            {"e", ecs_id(ecs_entity_t)},
            {"x", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    holder {"
    LINE "      RefComp: {e: new {}, x: 1}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    typedef struct { ecs_entity_t e; float x; } RefComp;

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t holder = ecs_lookup(world, "e.holder");
    test_assert(holder != 0);

    ecs_entity_t helper;
    {
        const RefComp *rc = ecs_get_id(world, holder, ref_comp);
        test_assert(rc != NULL);
        helper = rc->e;
        test_assert(helper != 0);
        test_assert(ecs_is_alive(world, helper));
    }

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_is_alive(world, holder));
    test_assert(!ecs_is_alive(world, helper));

    ecs_set(world, e, TmplCond, {true});

    ecs_entity_t new_holder = ecs_lookup(world, "e.holder");
    test_assert(new_holder != 0);

    {
        const RefComp *rc = ecs_get_id(world, new_holder, ref_comp);
        test_assert(rc != NULL);
        test_assert(rc->e != 0);
        test_assert(ecs_is_alive(world, rc->e));
    }

    ecs_fini(world);
}

void IncrementalUpdates_with_loop_var_pair_range_shift(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "tgt_0 {}"
    LINE "tgt_1 {}"
    LINE "tgt_2 {}"
    LINE "template Tree {"
    LINE "  prop start = flecs.meta.i32: 0"
    LINE "  for i in $start..($start + 2) {"
    LINE "    with (Rel, tgt_$i) {"
    LINE "      \"child_$i\" {}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_0 = ecs_lookup(world, "tgt_0");
    ecs_entity_t tgt_1 = ecs_lookup(world, "tgt_1");
    ecs_entity_t tgt_2 = ecs_lookup(world, "tgt_2");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(ecs_has_pair(world, child_0, rel, tgt_0));
    test_assert(ecs_has_pair(world, child_1, rel, tgt_1));

    ecs_entity_t ecs_id(TmplStart) = tree;
    ecs_set(world, e, TmplStart, {1});

    test_assert(ecs_lookup(world, "e.child_0") == 0);
    ecs_entity_t new_child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t new_child_2 = ecs_lookup(world, "e.child_2");
    test_assert(new_child_1 != 0);
    test_assert(new_child_2 != 0);

    test_assert(ecs_has_pair(world, new_child_1, rel, tgt_1));
    test_assert(!ecs_has_pair(world, new_child_1, rel, tgt_0));
    test_assert(ecs_has_pair(world, new_child_2, rel, tgt_2));
    test_assert(!ecs_has_pair(world, new_child_2, rel, tgt_1));

    ecs_fini(world);
}

void IncrementalUpdates_loop_expr_bounds_to_zero(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 1"
    LINE "  for i in 0..($count * 2) {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, 0}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(ecs_lookup(world, "e.child_2") == 0);

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {0});

    test_assert(!ecs_is_alive(world, child_0));
    test_assert(!ecs_is_alive(world, child_1));
    test_assert(ecs_lookup(world, "e.child_0") == 0);
    test_assert(ecs_lookup(world, "e.child_1") == 0);

    ecs_set(world, e, int32_t, {1});

    test_assert(ecs_lookup(world, "e.child_0") != 0);
    test_assert(ecs_lookup(world, "e.child_1") != 0);
    test_assert(ecs_lookup(world, "e.child_2") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_loop_same_entity_each_iteration(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    child {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 1);
    }

    ecs_add(world, child, UserTag);

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {4, 1});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has(world, child, UserTag));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 1);
    }

    ecs_set(world, e, TmplCountHeight, {2, 10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has(world, child, UserTag));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 10);
    }

    ecs_set(world, e, TmplCountHeight, {0, 10});

    test_assert(!ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == 0);

    ecs_set(world, e, TmplCountHeight, {2, 10});

    child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_loop_component_only_no_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    Position: {$i, $height}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 1);
    }

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {3, 5});

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 5);
    }

    ecs_set(world, e, TmplCountHeight, {1, 7});

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 7);
    }

    ecs_set(world, e, TmplCountHeight, {0, 7});

    test_assert(!ecs_has(world, e, Position));

    ecs_set(world, e, TmplCountHeight, {2, 7});

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 7);
    }

    ecs_fini(world);
}

void IncrementalUpdates_if_else_in_loop(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop threshold = flecs.meta.i32: 2"
    LINE "  for i in 0..4 {"
    LINE "    if $i < $threshold {"
    LINE "      \"low_$i\" {"
    LINE "        Position: {$i, 0}"
    LINE "      }"
    LINE "    } else {"
    LINE "      \"high_$i\" {"
    LINE "        Position: {$i, 1}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t low_0 = ecs_lookup(world, "e.low_0");
    ecs_entity_t low_1 = ecs_lookup(world, "e.low_1");
    ecs_entity_t high_2 = ecs_lookup(world, "e.high_2");
    ecs_entity_t high_3 = ecs_lookup(world, "e.high_3");
    test_assert(low_0 != 0);
    test_assert(low_1 != 0);
    test_assert(high_2 != 0);
    test_assert(high_3 != 0);
    test_assert(ecs_lookup(world, "e.low_2") == 0);
    test_assert(ecs_lookup(world, "e.high_0") == 0);

    {
        const Position *p = ecs_get(world, high_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 1);
    }

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {4});

    test_assert(ecs_lookup(world, "e.low_0") == low_0);
    test_assert(ecs_lookup(world, "e.low_1") == low_1);
    ecs_entity_t low_2 = ecs_lookup(world, "e.low_2");
    ecs_entity_t low_3 = ecs_lookup(world, "e.low_3");
    test_assert(low_2 != 0);
    test_assert(low_3 != 0);
    test_assert(!ecs_is_alive(world, high_2));
    test_assert(!ecs_is_alive(world, high_3));
    test_assert(ecs_lookup(world, "e.high_2") == 0);
    test_assert(ecs_lookup(world, "e.high_3") == 0);

    {
        const Position *p = ecs_get(world, low_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 0);
    }

    ecs_set(world, e, int32_t, {0});

    test_assert(!ecs_is_alive(world, low_0));
    test_assert(!ecs_is_alive(world, low_1));
    test_assert(!ecs_is_alive(world, low_2));
    test_assert(!ecs_is_alive(world, low_3));

    ecs_entity_t high_0 = ecs_lookup(world, "e.high_0");
    test_assert(high_0 != 0);
    test_assert(ecs_lookup(world, "e.high_3") != 0);
    test_assert(ecs_lookup(world, "e.low_0") == 0);

    {
        const Position *p = ecs_get(world, high_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_loop_if_prop_eq_create_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop sel = flecs.meta.i32: 0"
    LINE "  for i in 0..4 {"
    LINE "    if $sel == 0 {"
    LINE "      \"a_$i\" {"
    LINE "        Position: {$i, 0}"
    LINE "      }"
    LINE "    }"
    LINE "    if $sel == 1 {"
    LINE "      \"b_$i\" {"
    LINE "        Position: {$i, 1}"
    LINE "      }"
    LINE "    }"
    LINE "    if $sel == 2 {"
    LINE "      \"c_$i\" {"
    LINE "        Position: {$i, 2}"
    LINE "      }"
    LINE "    }"
    LINE "    if $sel == 3 {"
    LINE "      \"d_$i\" {"
    LINE "        Position: {$i, 3}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t a_0 = ecs_lookup(world, "e.a_0");
    ecs_entity_t a_3 = ecs_lookup(world, "e.a_3");
    test_assert(a_0 != 0);
    test_assert(a_3 != 0);
    test_assert(ecs_lookup(world, "e.b_0") == 0);
    test_assert(ecs_lookup(world, "e.c_0") == 0);
    test_assert(ecs_lookup(world, "e.d_0") == 0);

    {
        const Position *p = ecs_get(world, a_3, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 0);
    }

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {2});

    test_assert(!ecs_is_alive(world, a_0));
    test_assert(!ecs_is_alive(world, a_3));
    test_assert(ecs_lookup(world, "e.a_0") == 0);
    test_assert(ecs_lookup(world, "e.b_0") == 0);
    test_assert(ecs_lookup(world, "e.d_0") == 0);

    ecs_entity_t c_0 = ecs_lookup(world, "e.c_0");
    ecs_entity_t c_3 = ecs_lookup(world, "e.c_3");
    test_assert(c_0 != 0);
    test_assert(c_3 != 0);

    {
        const Position *p = ecs_get(world, c_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void flecs_dump_backtrace(void *stream);

static void DbgVelocityAdded(ecs_iter_t *it) {
    printf("DBG Velocity OnAdd\n");
    flecs_dump_backtrace(stdout);
}

void IncrementalUpdates_loop_if_prop_eq_new_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Point);

    register_position(world, ecs_id(Position));
    register_position(world, ecs_id(Velocity));
    register_position(world, ecs_id(Point));
    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop sel = flecs.meta.i32: 0"
    LINE "  for i in 0..4 {"
    LINE "    if $sel == 0 {"
    LINE "      Position: {10, 1}"
    LINE "    }"
    LINE "    if $sel == 1 {"
    LINE "      Velocity: {11, 1}"
    LINE "    }"
    LINE "    if $sel == 2 {"
    LINE "      Mass: {12}"
    LINE "    }"
    LINE "    if $sel == 3 {"
    LINE "      Point: {13, 1}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));
    test_assert(!ecs_has(world, e, Point));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 1);
    }

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {1});

    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));
    test_assert(!ecs_has(world, e, Point));

    {
        const Velocity *v = ecs_get(world, e, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 11);
        test_int(v->y, 1);
    }

    ecs_set(world, e, int32_t, {3});

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));
    test_assert(ecs_has(world, e, Point));

    {
        const Point *p = ecs_get(world, e, Point);
        test_assert(p != NULL);
        test_int(p->x, 13);
        test_int(p->y, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_component_overlap_w_two_conditionals_in_loop_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop sel = flecs.meta.i32: 0"
    LINE "  for i in 0..4 {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, 0}"
    LINE "      if $sel == 0 {"
    LINE "        Position: {$i, 10}"
    LINE "      }"
    LINE "      if $sel == 1 {"
    LINE "        Position: {$i, 11}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Tree") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_loop_if_prop_neq_create_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop sel = flecs.meta.i32: 0"
    LINE "  for i in 0..4 {"
    LINE "    if $sel != 0 {"
    LINE "      \"a_$i\" {"
    LINE "        Position: {$i, 0}"
    LINE "      }"
    LINE "    }"
    LINE "    if $sel != 1 {"
    LINE "      \"b_$i\" {"
    LINE "        Position: {$i, 1}"
    LINE "      }"
    LINE "    }"
    LINE "    if $sel != 2 {"
    LINE "      \"c_$i\" {"
    LINE "        Position: {$i, 2}"
    LINE "      }"
    LINE "    }"
    LINE "    if $sel != 3 {"
    LINE "      \"d_$i\" {"
    LINE "        Position: {$i, 3}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");

    test_assert(ecs_lookup(world, "e.a_0") == 0);
    test_assert(ecs_lookup(world, "e.a_3") == 0);

    ecs_entity_t b_0 = ecs_lookup(world, "e.b_0");
    ecs_entity_t b_3 = ecs_lookup(world, "e.b_3");
    ecs_entity_t c_0 = ecs_lookup(world, "e.c_0");
    ecs_entity_t c_3 = ecs_lookup(world, "e.c_3");
    ecs_entity_t d_0 = ecs_lookup(world, "e.d_0");
    ecs_entity_t d_3 = ecs_lookup(world, "e.d_3");
    test_assert(b_0 != 0);
    test_assert(b_3 != 0);
    test_assert(c_0 != 0);
    test_assert(c_3 != 0);
    test_assert(d_0 != 0);
    test_assert(d_3 != 0);

    {
        const Position *p = ecs_get(world, d_3, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 3);
    }

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {1});

    ecs_entity_t a_0 = ecs_lookup(world, "e.a_0");
    ecs_entity_t a_3 = ecs_lookup(world, "e.a_3");
    test_assert(a_0 != 0);
    test_assert(a_3 != 0);

    test_assert(!ecs_is_alive(world, b_0));
    test_assert(!ecs_is_alive(world, b_3));
    test_assert(ecs_lookup(world, "e.b_0") == 0);
    test_assert(ecs_lookup(world, "e.b_3") == 0);

    test_assert(ecs_lookup(world, "e.c_0") == c_0);
    test_assert(ecs_lookup(world, "e.c_3") == c_3);
    test_assert(ecs_lookup(world, "e.d_0") == d_0);
    test_assert(ecs_lookup(world, "e.d_3") == d_3);

    {
        const Position *p = ecs_get(world, a_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 0);
    }

    ecs_fini(world);
}

void IncrementalUpdates_loop_if_prop_neq_new_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Point);
    register_position(world, ecs_id(Position));
    register_position(world, ecs_id(Velocity));
    register_position(world, ecs_id(Point));
    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop sel = flecs.meta.i32: 0"
    LINE "  for i in 0..4 {"
    LINE "    if $sel != 0 {"
    LINE "      Position: {10, 1}"
    LINE "    }"
    LINE "    if $sel != 1 {"
    LINE "      Velocity: {11, 1}"
    LINE "    }"
    LINE "    if $sel != 2 {"
    LINE "      Mass: {12}"
    LINE "    }"
    LINE "    if $sel != 3 {"
    LINE "      Point: {13, 1}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    test_assert(ecs_has(world, e, Point));

    {
        const Velocity *v = ecs_get(world, e, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 11);
        test_int(v->y, 1);
    }
    {
        const Mass *m = ecs_get(world, e, Mass);
        test_assert(m != NULL);
        test_int(m->value, 12);
    }

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {1});

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    test_assert(ecs_has(world, e, Point));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 1);
    }
    {
        const Point *p = ecs_get(world, e, Point);
        test_assert(p != NULL);
        test_int(p->x, 13);
        test_int(p->y, 1);
    }

    ecs_set(world, e, int32_t, {0});

    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    test_assert(ecs_has(world, e, Point));

    ecs_fini(world);
}

void IncrementalUpdates_loop_if_prop_neq_app_component_preserved(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop sel = flecs.meta.i32: 0"
    LINE "  for i in 0..4 {"
    LINE "    if $sel != 0 {"
    LINE "      \"a_$i\" {"
    LINE "        Position: {$i, 0}"
    LINE "      }"
    LINE "    }"
    LINE "    if $sel != 1 {"
    LINE "      \"b_$i\" {"
    LINE "        Position: {$i, 1}"
    LINE "      }"
    LINE "    }"
    LINE "    if $sel != 2 {"
    LINE "      \"c_$i\" {"
    LINE "        Position: {$i, 2}"
    LINE "      }"
    LINE "    }"
    LINE "    if $sel != 3 {"
    LINE "      \"d_$i\" {"
    LINE "        Position: {$i, 3}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t b_0 = ecs_lookup(world, "e.b_0");
    ecs_entity_t c_1 = ecs_lookup(world, "e.c_1");
    ecs_entity_t d_3 = ecs_lookup(world, "e.d_3");
    test_assert(b_0 != 0);
    test_assert(c_1 != 0);
    test_assert(d_3 != 0);

    ecs_set(world, c_1, Velocity, {1, 2});
    ecs_set(world, d_3, Velocity, {3, 4});

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {1});

    test_assert(!ecs_is_alive(world, b_0));
    test_assert(ecs_lookup(world, "e.c_1") == c_1);
    test_assert(ecs_lookup(world, "e.d_3") == d_3);

    {
        const Velocity *v = ecs_get(world, c_1, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }
    {
        const Velocity *v = ecs_get(world, d_3, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3);
        test_int(v->y, 4);
    }
    {
        const Position *p = ecs_get(world, c_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void IncrementalUpdates_component_overlap_w_conditional_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.f32: 10"
    LINE "  Position: {$x, $x * 2}"
    LINE "  if $x > 2 {"
    LINE "    Position: {$x, $x * 3}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_entity_overlap_w_conditional_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.f32: 10"
    LINE "  child {"
    LINE "    Position: {$x, $x * 2}"
    LINE "  }"
    LINE "  if $x > 2 {"
    LINE "    child {"
    LINE "      Position: {$x, $x * 3}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_tag_overlap_w_conditional_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Tag {}"
    LINE "template Foo {"
    LINE "  prop x = flecs.meta.f32: 10"
    LINE "  Tag"
    LINE "  if $x > 2 {"
    LINE "    Tag"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_two_conditionals_same_component_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop a = flecs.meta.bool: true"
    LINE "  prop b = flecs.meta.bool: true"
    LINE "  if $a {"
    LINE "    Position: {1, 2}"
    LINE "  }"
    LINE "  if $b {"
    LINE "    Position: {3, 4}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_two_conditionals_same_entity_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop a = flecs.meta.bool: true"
    LINE "  prop b = flecs.meta.bool: true"
    LINE "  if $a {"
    LINE "    child {"
    LINE "      Position: {1, 2}"
    LINE "    }"
    LINE "  }"
    LINE "  if $b {"
    LINE "    child {"
    LINE "      Position: {3, 4}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_two_conditionals_same_tag_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Tag {}"
    LINE "template Foo {"
    LINE "  prop a = flecs.meta.bool: true"
    LINE "  prop b = flecs.meta.bool: true"
    LINE "  if $a {"
    LINE "    Tag"
    LINE "  }"
    LINE "  if $b {"
    LINE "    Tag"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_two_conditionals_same_pair_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "Tgt {}"
    LINE "template Foo {"
    LINE "  prop a = flecs.meta.bool: true"
    LINE "  prop b = flecs.meta.bool: true"
    LINE "  if $a {"
    LINE "    (Rel, Tgt)"
    LINE "  }"
    LINE "  if $b {"
    LINE "    (Rel, Tgt)"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_entity_overlap_w_loop_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  child {"
    LINE "    Position: {$count, 2}"
    LINE "  }"
    LINE "  for i in 0..$count {"
    LINE "    child {"
    LINE "      Position: {$i, 10}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_two_loops_same_entity_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop a = flecs.meta.i32: 2"
    LINE "  prop b = flecs.meta.i32: 2"
    LINE "  for i in 0..$a {"
    LINE "    child {"
    LINE "      Position: {$i, $b}"
    LINE "    }"
    LINE "  }"
    LINE "  for i in 0..$b {"
    LINE "    child {"
    LINE "      Position: {$i, 9}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_tag_overlap_w_conditional_in_loop_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Tag {}"
    LINE "template Foo {"
    LINE "  prop sel = flecs.meta.i32: 0"
    LINE "  for i in 0..2 {"
    LINE "    \"child_$i\" {"
    LINE "      Tag"
    LINE "      if $sel == $i {"
    LINE "        Tag"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_component_overlap_w_conditional_in_loop_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop x = flecs.meta.f32: 10"
    LINE "  Position: {$x, 1}"
    LINE "  for i in 0..2 {"
    LINE "    if $cond {"
    LINE "      Position: {$x, 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_if_else_same_entity_both_branches(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    child {"
    LINE "      Position: {1, 2}"
    LINE "    }"
    LINE "  } else {"
    LINE "    child {"
    LINE "      Position: {3, 4}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_add(world, child, UserTag);

    ecs_entity_t ecs_id(TmplCond) = foo;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_is_alive(world, child));
    ecs_entity_t new_child = ecs_lookup(world, "e.child");
    test_assert(new_child != child);
    test_assert(!ecs_has(world, new_child, UserTag));

    {
        const Position *p = ecs_get(world, new_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 4);
    }

    ecs_fini(world);
}

void IncrementalUpdates_if_else_same_component_both_branches(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    Position: {1, 2}"
    LINE "  } else {"
    LINE "    Position: {3, 4}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_entity_t ecs_id(TmplCond) = foo;
    ecs_set(world, e, TmplCond, {false});

    test_assert(ecs_has(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 4);
    }

    ecs_fini(world);
}

void IncrementalUpdates_entity_overlap_w_conditional_in_loop_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop sel = flecs.meta.i32: 0"
    LINE "  for i in 0..2 {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, 1}"
    LINE "    }"
    LINE "    if $sel == $i {"
    LINE "      \"child_$i\" {"
    LINE "        Position: {$i, 2}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_var_pair_overlap_w_static_pair_same_scope_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "TgtB {}"
    LINE "template Foo {"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  child {"
    LINE "    (Rel, TgtA)"
    LINE "    (Rel, $x)"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_interpolated_name_overlap_w_expr_same_scope_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.i32: 1"
    LINE "  prop y = flecs.meta.i32: 0"
    LINE "  \"e_$x\" {"
    LINE "    Position: {$x, 1}"
    LINE "  }"
    LINE "  \"e_{$y + 1}\" {"
    LINE "    Position: {$y, 2}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_interpolated_names_overlap_same_scope_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.i32: 1"
    LINE "  prop y = flecs.meta.i32: 2"
    LINE "  \"e_$x\" {"
    LINE "    Position: {$x, 1}"
    LINE "  }"
    LINE "  \"e_$y\" {"
    LINE "    Position: {$y, 2}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_var_pair_overlap_w_static_pair_in_loop_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "template Foo {"
    LINE "  prop x = flecs.meta.entity: TgtA"
    LINE "  (Rel, $x)"
    LINE "  for i in 0..2 {"
    LINE "    (Rel, TgtA)"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_interpolated_name_overlap_w_expr_in_conditional_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.i32: 1"
    LINE "  prop y = flecs.meta.i32: 0"
    LINE "  \"e_$x\" {"
    LINE "    Position: {$x, 1}"
    LINE "  }"
    LINE "  if $x > 0 {"
    LINE "    \"e_{$y + 1}\" {"
    LINE "      Position: {$y, 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_interpolated_name_overlap_w_loop_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.i32: 1"
    LINE "  \"e_$x\" {"
    LINE "    Position: {$x, 1}"
    LINE "  }"
    LINE "  for i in 0..2 {"
    LINE "    \"e_$i\" {"
    LINE "      Position: {$i, 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_interpolated_name_expr_rename(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.i32: 1"
    LINE "  \"e_{$x + 1}\" {"
    LINE "    Position: {$x, 1}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.e_2");
    test_assert(e != 0);
    test_assert(child != 0);
    test_assert(ecs_lookup(world, "e.e_3") == 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 1);
    }

    ecs_entity_t ecs_id(int32_t) = foo;
    ecs_set(world, e, int32_t, {2});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.e_3") == child);
    test_assert(ecs_lookup(world, "e.e_2") == 0);
    test_str(ecs_get_name(world, child), "e_3");

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_interpolated_name_expr_rename_w_app_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.i32: 1"
    LINE "  \"e_{$x + 1}\" {"
    LINE "    Position: {$x, 1}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.e_2");
    test_assert(e != 0);
    test_assert(child != 0);

    ecs_entity_t ecs_id(int32_t) = foo;
    ecs_set(world, e, int32_t, {2});

    test_assert(ecs_lookup(world, "e.e_3") == child);

    ecs_add(world, child, UserTag);

    ecs_set(world, e, int32_t, {3});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.e_4") == child);
    test_assert(ecs_lookup(world, "e.e_3") == 0);
    test_str(ecs_get_name(world, child), "e_4");
    test_assert(ecs_has(world, child, UserTag));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_pair_overlap_w_conditional_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  (Rel, TgtA)"
    LINE "  if $cond {"
    LINE "    (Rel, TgtA)"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_var_pair_overlap_w_conditional_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "TgtA {}"
    LINE "TgtB {}"
    LINE "template Foo {"
    LINE "  prop a = flecs.meta.entity: TgtA"
    LINE "  prop b = flecs.meta.entity: TgtB"
    LINE "  (Position, $a): {10, 20}"
    LINE "  if $a != $b {"
    LINE "    (Position, $b): {20, 30}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_var_pair_overlap_w_static_pair_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop a = flecs.meta.entity: TgtA"
    LINE "  (Rel, $a)"
    LINE "  if $cond {"
    LINE "    (Rel, TgtA)"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_var_pair_no_overlap_in_conditional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    register_position(world, ecs_id(Position));
    register_position(world, ecs_id(Velocity));

    typedef struct {
        bool cond;
        ecs_entity_t a;
        ecs_entity_t b;
    } TmplT;

    const char *expr =
    HEAD "TgtA {}"
    LINE "TgtB {}"
    LINE "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop a = flecs.meta.entity: TgtA"
    LINE "  prop b = flecs.meta.entity: TgtB"
    LINE "  (Position, $a): {10, 20}"
    LINE "  if $cond {"
    LINE "    (Velocity, $b): {1, 2}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        const Position *p = ecs_get_pair(world, e, Position, tgt_a);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *v = ecs_get_pair(world, e, Velocity, tgt_b);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_entity_t ecs_id(TmplT) = foo;
    ecs_set(world, e, TmplT, {false, tgt_a, tgt_b});

    test_assert(!ecs_has_pair(world, e, ecs_id(Velocity), tgt_b));

    {
        const Position *p = ecs_get_pair(world, e, Position, tgt_a);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void IncrementalUpdates_interpolated_name_overlap_w_conditional_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop a = flecs.meta.i32: 1"
    LINE "  prop b = flecs.meta.i32: 2"
    LINE "  \"e_$a\" {"
    LINE "    Position: {$a, 1}"
    LINE "  }"
    LINE "  if $a != $b {"
    LINE "    \"e_$b\" {"
    LINE "      Position: {$b, 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_interpolated_name_no_overlap_in_conditional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    typedef struct {
        int32_t x;
        bool cond;
    } TmplT;

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.i32: 1"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  \"e_$x\" {"
    LINE "    Position: {$x, 1}"
    LINE "  }"
    LINE "  if $cond {"
    LINE "    \"f_$x\" {"
    LINE "      Position: {$x, 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t e_1 = ecs_lookup(world, "e.e_1");
    ecs_entity_t f_1 = ecs_lookup(world, "e.f_1");
    test_assert(e != 0);
    test_assert(e_1 != 0);
    test_assert(f_1 != 0);

    ecs_entity_t ecs_id(TmplT) = foo;
    ecs_set(world, e, TmplT, {1, false});

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_lookup(world, "e.e_1") == e_1);
    test_assert(ecs_has(world, e_1, Position));

    test_assert(!ecs_is_alive(world, f_1));
    test_assert(ecs_lookup(world, "e.f_1") == 0);

    {
        const Position *p = ecs_get(world, e_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_entity_overlap_w_else_error(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  child {"
    LINE "    Position: {1, 2}"
    LINE "  }"
    LINE "  if $cond {"
    LINE "    other {}"
    LINE "  } else {"
    LINE "    child {"
    LINE "      Position: {3, 4}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_partial_assign_in_conditional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.f32: 10"
    LINE "  Position: {$x, 20}"
    LINE "  if $x > 2 {"
    LINE "    Position: {y: 30}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_partial_assign_in_conditional_initial_false(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.f32: 2"
    LINE "  Position: {$x, 20}"
    LINE "  if $x > 2 {"
    LINE "    Position: {y: 30}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_partial_assign_standalone_in_conditional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    Position: {y: 30}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 30);
    }

    ecs_entity_t ecs_id(TmplCond) = foo;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_has(world, e, Position));

    ecs_set(world, e, TmplCond, {true});

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 30);
    }

    ecs_fini(world);
}

void IncrementalUpdates_partial_assign_named_members_in_conditional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x = flecs.meta.f32: 10"
    LINE "  Position: {10, 20}"
    LINE "  if $x > 2 {"
    LINE "    Position: {x: 1, y: 2}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "e") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_if_else_same_tag_both_branches(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Tag {}"
    LINE "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    Tag"
    LINE "  } else {"
    LINE "    Tag"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tag = ecs_lookup(world, "Tag");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(tag != 0);
    test_assert(ecs_has_id(world, e, tag));

    ecs_entity_t ecs_id(TmplCond) = foo;
    ecs_set(world, e, TmplCond, {false});

    test_assert(ecs_has_id(world, e, tag));

    ecs_set(world, e, TmplCond, {true});

    test_assert(ecs_has_id(world, e, tag));

    ecs_fini(world);
}

void IncrementalUpdates_if_else_same_pair_both_branches(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "Tgt {}"
    LINE "template Foo {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    (Position, Tgt): {1, 2}"
    LINE "  } else {"
    LINE "    (Position, Tgt): {3, 4}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tgt = ecs_lookup(world, "Tgt");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        const Position *p = ecs_get_pair(world, e, Position, tgt);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_entity_t ecs_id(TmplCond) = foo;
    ecs_set(world, e, TmplCond, {false});

    test_assert(ecs_has_pair(world, e, ecs_id(Position), tgt));

    {
        const Position *p = ecs_get_pair(world, e, Position, tgt);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 4);
    }

    ecs_fini(world);
}

void IncrementalUpdates_else_if_chain_same_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop v = flecs.meta.i32: 1"
    LINE "  if $v == 1 {"
    LINE "    Position: {1, 1}"
    LINE "  } else if $v == 2 {"
    LINE "    Position: {2, 2}"
    LINE "  } else {"
    LINE "    Position: {3, 3}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 1);
    }

    ecs_entity_t ecs_id(int32_t) = foo;
    ecs_set(world, e, int32_t, {2});

    test_assert(ecs_has(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 2);
    }

    ecs_set(world, e, int32_t, {5});

    test_assert(ecs_has(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 3);
    }

    ecs_set(world, e, int32_t, {1});

    test_assert(ecs_has(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_else_if_chain_same_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop v = flecs.meta.i32: 1"
    LINE "  if $v == 1 {"
    LINE "    child {"
    LINE "      Position: {1, 1}"
    LINE "    }"
    LINE "  } else if $v == 2 {"
    LINE "    child {"
    LINE "      Position: {2, 2}"
    LINE "    }"
    LINE "  } else {"
    LINE "    child {"
    LINE "      Position: {3, 3}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 1);
    }

    ecs_add(world, child, UserTag);

    ecs_entity_t ecs_id(int32_t) = foo;
    ecs_set(world, e, int32_t, {2});

    test_assert(!ecs_is_alive(world, child));
    ecs_entity_t child_1 = ecs_lookup(world, "e.child");
    test_assert(child_1 != child);
    test_assert(!ecs_has(world, child_1, UserTag));

    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 2);
    }

    ecs_set(world, e, int32_t, {5});

    test_assert(!ecs_is_alive(world, child));
    test_assert(!ecs_is_alive(world, child_1));
    ecs_entity_t child_2 = ecs_lookup(world, "e.child");
    test_assert(child_2 != child);
    test_assert(child_2 != child_1);
    test_assert(!ecs_has(world, child_2, UserTag));

    {
        const Position *p = ecs_get(world, child_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 3);
    }

    ecs_set(world, e, int32_t, {1});

    test_assert(!ecs_is_alive(world, child));
    test_assert(!ecs_is_alive(world, child_1));
    test_assert(!ecs_is_alive(world, child_2));
    ecs_entity_t child_3 = ecs_lookup(world, "e.child");
    test_assert(child_3 != child);
    test_assert(child_3 != child_1);
    test_assert(child_3 != child_2);
    test_assert(!ecs_has(world, child_3, UserTag));

    {
        const Position *p = ecs_get(world, child_3, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 1);
    }

    ecs_fini(world);
}

void IncrementalUpdates_overlap_outside_template_allowed(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "const cond: true"
    LINE "e {"
    LINE "  Position: {1, 2}"
    LINE "}"
    LINE "if $cond {"
    LINE "  e {"
    LINE "    Position: {3, 4}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 4);
    }

    ecs_fini(world);
}

void IncrementalUpdates_code_instance_set_template(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {5});

    test_assert(ecs_has_id(world, e, tree));

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 5);
        test_int(p->y, 2);
    }

    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void IncrementalUpdates_code_instance_set_template_anonymous(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_new(world);
    test_assert(ecs_get_name(world, e) == NULL);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {5});

    ecs_entity_t child = ecs_lookup_child(world, e, "child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 5);
        test_int(p->y, 2);
    }

    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup_child(world, e, "child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void IncrementalUpdates_code_instance_set_template_w_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t e = ecs_entity(world, { .name = "e", .parent = parent });

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {5});

    ecs_entity_t child = ecs_lookup(world, "parent.e.child");
    test_assert(child != 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 5);
        test_int(p->y, 2);
    }

    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "parent.e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void IncrementalUpdates_code_instance_conditional_flip(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {true});

    ecs_entity_t cond_child = ecs_lookup(world, "e.cond_child");
    test_assert(cond_child != 0);

    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "e.cond_child") == 0);

    ecs_set(world, e, TmplCond, {true});

    test_assert(ecs_lookup(world, "e.cond_child") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_code_instance_loop_count_change(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {2, 1});

    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(ecs_lookup(world, "e.child_2") == 0);

    ecs_set(world, e, TmplCountHeight, {4, 1});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(ecs_lookup(world, "e.child_2") != 0);
    test_assert(ecs_lookup(world, "e.child_3") != 0);

    ecs_set(world, e, TmplCountHeight, {1, 1});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(!ecs_is_alive(world, child_1));
    test_assert(ecs_lookup(world, "e.child_1") == 0);
    test_assert(ecs_lookup(world, "e.child_2") == 0);
    test_assert(ecs_lookup(world, "e.child_3") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_anonymous_entity_in_conditional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Marker {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  if $cond {"
    LINE "    _ {"
    LINE "      Marker"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t marker = ecs_lookup(world, "Marker");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t anon = 0;
    test_int(find_children_w_id(world, e, marker, &anon, 1), 1);
    test_assert(anon != 0);

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_is_alive(world, anon));
    test_int(find_children_w_id(world, e, marker, NULL, 0), 0);

    ecs_set(world, e, TmplCond, {true});

    test_int(find_children_w_id(world, e, marker, &anon, 1), 1);
    test_assert(anon != 0);

    ecs_fini(world);
}

void IncrementalUpdates_anonymous_entities_in_loop(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "Marker {}"
    LINE "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    _ {"
    LINE "      Marker"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t marker = ecs_lookup(world, "Marker");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t anons[4];
    test_int(find_children_w_id(world, e, marker, anons, 4), 2);
    ecs_entity_t anon_0 = anons[0];
    ecs_entity_t anon_1 = anons[1];

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {4, 1});

    test_int(find_children_w_id(world, e, marker, anons, 4), 4);
    test_assert(ecs_is_alive(world, anon_0));
    test_assert(ecs_is_alive(world, anon_1));

    ecs_set(world, e, TmplCountHeight, {2, 5});

    test_int(find_children_w_id(world, e, marker, anons, 4), 2);
    test_assert(ecs_is_alive(world, anon_0));
    test_assert(ecs_is_alive(world, anon_1));

    {
        const Position *p = ecs_get(world, anon_0, Position);
        test_assert(p != NULL);
        test_int(p->y, 5);
    }
    {
        const Position *p = ecs_get(world, anon_1, Position);
        test_assert(p != NULL);
        test_int(p->y, 5);
    }

    ecs_fini(world);
}

void IncrementalUpdates_anonymous_empty_child_not_duplicated(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  _ {}"
    LINE "  child {"
    LINE "    Position: {$height, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    ecs_entity_t children[4];
    test_int(find_children_w_id(world, e, 0, children, 4), 2);

    ecs_entity_t anon = children[0] == child ? children[1] : children[0];
    test_assert(anon != 0);
    test_assert(ecs_get_name(world, anon) == NULL);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_int(find_children_w_id(world, e, 0, children, 4), 2);
    test_assert(ecs_is_alive(world, anon));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_empty_child_preserved_after_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, UserTag);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {}"
    LINE "  other {"
    LINE "    Position: {$height, 0}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t other = ecs_lookup(world, "e.other");
    test_assert(e != 0);
    test_assert(child != 0);
    test_assert(other != 0);
    test_assert(!ecs_has(world, child, Position));

    ecs_add(world, child, UserTag);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "e.child") == child);
    test_assert(ecs_has(world, child, UserTag));
    test_assert(!ecs_has(world, child, Position));

    {
        const Position *p = ecs_get(world, other, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_loop_empty_children_count_change(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {4});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(ecs_lookup(world, "e.child_1") == child_1);
    test_assert(ecs_lookup(world, "e.child_2") != 0);
    test_assert(ecs_lookup(world, "e.child_3") != 0);

    ecs_set(world, e, int32_t, {1});

    test_assert(ecs_lookup(world, "e.child_0") == child_0);
    test_assert(!ecs_is_alive(world, child_1));
    test_assert(ecs_lookup(world, "e.child_1") == 0);
    test_assert(ecs_lookup(world, "e.child_2") == 0);
    test_assert(ecs_lookup(world, "e.child_3") == 0);

    ecs_fini(world);
}

void IncrementalUpdates_ordered_children_preserved_after_value_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  a {"
    LINE "    Position: {$height, 0}"
    LINE "  }"
    LINE "  b {}"
    LINE "  c {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_id(world, e, EcsOrderedChildren);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {1});

    ecs_entity_t a = ecs_lookup(world, "e.a");
    ecs_entity_t b = ecs_lookup(world, "e.b");
    ecs_entity_t c = ecs_lookup(world, "e.c");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(c != 0);

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 3);
        test_uint(children.ids[0], a);
        test_uint(children.ids[1], b);
        test_uint(children.ids[2], c);
    }

    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_assert(ecs_is_alive(world, c));

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 3);
        test_uint(children.ids[0], a);
        test_uint(children.ids[1], b);
        test_uint(children.ids[2], c);
    }

    {
        const Position *p = ecs_get(world, a, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void IncrementalUpdates_ordered_children_conditional_flip(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  a {}"
    LINE "  if $cond {"
    LINE "    b {}"
    LINE "  }"
    LINE "  c {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_id(world, e, EcsOrderedChildren);

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {true});

    ecs_entity_t a = ecs_lookup(world, "e.a");
    ecs_entity_t b = ecs_lookup(world, "e.b");
    ecs_entity_t c = ecs_lookup(world, "e.c");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(c != 0);

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 3);
        test_uint(children.ids[0], a);
        test_uint(children.ids[1], b);
        test_uint(children.ids[2], c);
    }

    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_is_alive(world, b));

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 2);
        test_uint(children.ids[0], a);
        test_uint(children.ids[1], c);
    }

    ecs_set(world, e, TmplCond, {true});

    ecs_entity_t b_new = ecs_lookup(world, "e.b");
    test_assert(b_new != 0);

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 3);
        test_uint(children.ids[0], a);
        test_uint(children.ids[1], b_new);
        test_uint(children.ids[2], c);
    }

    ecs_fini(world);
}

void IncrementalUpdates_ordered_children_loop_count_change(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  first {}"
    LINE "  for i in 0..$count {"
    LINE "    \"child_$i\" {}"
    LINE "  }"
    LINE "  last {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_id(world, e, EcsOrderedChildren);

    ecs_entity_t ecs_id(int32_t) = tree;
    ecs_set(world, e, int32_t, {2});

    ecs_entity_t first = ecs_lookup(world, "e.first");
    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t last = ecs_lookup(world, "e.last");
    test_assert(first != 0);
    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(last != 0);

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 4);
        test_uint(children.ids[0], first);
        test_uint(children.ids[1], child_0);
        test_uint(children.ids[2], child_1);
        test_uint(children.ids[3], last);
    }

    ecs_set(world, e, int32_t, {4});

    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");
    ecs_entity_t child_3 = ecs_lookup(world, "e.child_3");
    test_assert(child_2 != 0);
    test_assert(child_3 != 0);

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 6);
        test_uint(children.ids[0], first);
        test_uint(children.ids[1], child_0);
        test_uint(children.ids[2], child_1);
        test_uint(children.ids[3], child_2);
        test_uint(children.ids[4], child_3);
        test_uint(children.ids[5], last);
    }

    ecs_set(world, e, int32_t, {1});

    test_assert(!ecs_is_alive(world, child_1));
    test_assert(!ecs_is_alive(world, child_2));
    test_assert(!ecs_is_alive(world, child_3));

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 3);
        test_uint(children.ids[0], first);
        test_uint(children.ids[1], child_0);
        test_uint(children.ids[2], last);
    }

    ecs_fini(world);
}

void IncrementalUpdates_tree_annotation_order_preserved_w_conditional(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@tree Parent"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  a {}"
    LINE "  if $cond {"
    LINE "    b {}"
    LINE "  }"
    LINE "  c {}"
    LINE "}"
    LINE "Tree e {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t a = ecs_lookup_child(world, e, "a");
    ecs_entity_t b = ecs_lookup_child(world, e, "b");
    ecs_entity_t c = ecs_lookup_child(world, e, "c");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(c != 0);

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 3);
        test_uint(children.ids[0], a);
        test_uint(children.ids[1], b);
        test_uint(children.ids[2], c);
    }

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(!ecs_is_alive(world, b));

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 2);
        test_uint(children.ids[0], a);
        test_uint(children.ids[1], c);
    }

    ecs_set(world, e, TmplCond, {true});

    ecs_entity_t b_new = ecs_lookup_child(world, e, "b");
    test_assert(b_new != 0);

    {
        ecs_entities_t children = ecs_get_ordered_children(world, e);
        test_int(children.count, 3);
        test_uint(children.ids[0], a);
        test_uint(children.ids[1], b_new);
        test_uint(children.ids[2], c);
    }

    ecs_fini(world);
}

void IncrementalUpdates_instance_in_parent_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "parent {"
    LINE "  Tree e()"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t parent = ecs_lookup(world, "parent");
    ecs_entity_t e = ecs_lookup(world, "parent.e");
    ecs_entity_t child = ecs_lookup(world, "parent.e.child");
    test_assert(parent != 0);
    test_assert(e != 0);
    test_assert(child != 0);
    test_assert(ecs_lookup(world, "e") == 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "parent.e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void IncrementalUpdates_instance_in_nested_parent_scope_conditional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  if $cond {"
    LINE "    cond_child {}"
    LINE "  }"
    LINE "  child {"
    LINE "    Position: {$height, 0}"
    LINE "  }"
    LINE "}"
    LINE "p1 {"
    LINE "  p2 {"
    LINE "    Tree e()"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "p1.p2.e");
    ecs_entity_t cond_child = ecs_lookup(world, "p1.p2.e.cond_child");
    ecs_entity_t child = ecs_lookup(world, "p1.p2.e.child");
    test_assert(e != 0);
    test_assert(cond_child != 0);
    test_assert(child != 0);

    ecs_entity_t ecs_id(TmplCondHeight) = tree;
    ecs_set(world, e, TmplCondHeight, {false, 10});

    test_assert(!ecs_is_alive(world, cond_child));
    test_assert(ecs_lookup(world, "p1.p2.e.cond_child") == 0);
    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "p1.p2.e.child") == child);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 0);
    }

    ecs_set(world, e, TmplCondHeight, {true, 10});

    test_assert(ecs_lookup(world, "p1.p2.e.cond_child") != 0);

    ecs_fini(world);
}

void IncrementalUpdates_instance_reparented_then_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  child {"
    LINE "    Position: {$height, 2}"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    ecs_entity_t parent = ecs_entity(world, { .name = "p" });
    ecs_add_pair(world, e, EcsChildOf, parent);

    test_assert(ecs_lookup(world, "p.e") == e);
    test_assert(ecs_lookup(world, "p.e.child") == child);

    ecs_entity_t ecs_id(TmplHeight) = tree;
    ecs_set(world, e, TmplHeight, {10});

    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "p.e.child") == child);
    test_assert(ecs_lookup(world, "e.child") == 0);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void IncrementalUpdates_conditional_pair_diff_target_preserved_on_false(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "TgtB {}"
    LINE "template Tree {"
    LINE "  prop cond = flecs.meta.bool: true"
    LINE "  (Rel, TgtA)"
    LINE "  if $cond {"
    LINE "    (Rel, TgtB)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_pair(world, e, rel, tgt_a));
    test_assert(ecs_has_pair(world, e, rel, tgt_b));

    ecs_entity_t ecs_id(TmplCond) = tree;
    ecs_set(world, e, TmplCond, {false});

    test_assert(ecs_has_pair(world, e, rel, tgt_a));
    test_assert(!ecs_has_pair(world, e, rel, tgt_b));

    ecs_set(world, e, TmplCond, {true});

    test_assert(ecs_has_pair(world, e, rel, tgt_a));
    test_assert(ecs_has_pair(world, e, rel, tgt_b));

    ecs_fini(world);
}

void IncrementalUpdates_loop_pair_to_zero_other_relation_preserved(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "Other {}"
    LINE "OtherTgt {}"
    LINE "tgt_0 {}"
    LINE "tgt_1 {}"
    LINE "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  (Other, OtherTgt)"
    LINE "  for i in 0..$count {"
    LINE "    (Rel, tgt_$i)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    typedef struct { int32_t count; } T;

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t other = ecs_lookup(world, "Other");
    ecs_entity_t other_tgt = ecs_lookup(world, "OtherTgt");
    ecs_entity_t tgt_0 = ecs_lookup(world, "tgt_0");
    ecs_entity_t tgt_1 = ecs_lookup(world, "tgt_1");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, rel, tgt_0));
    test_assert(ecs_has_pair(world, e, rel, tgt_1));
    test_assert(ecs_has_pair(world, e, other, other_tgt));

    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {0});

    test_assert(!ecs_has_pair(world, e, rel, tgt_0));
    test_assert(!ecs_has_pair(world, e, rel, tgt_1));
    test_assert(ecs_has_pair(world, e, other, other_tgt));

    ecs_set(world, e, T, {2});

    test_assert(ecs_has_pair(world, e, rel, tgt_0));
    test_assert(ecs_has_pair(world, e, rel, tgt_1));
    test_assert(ecs_has_pair(world, e, other, other_tgt));

    ecs_fini(world);
}

void IncrementalUpdates_loop_pair_diff_target_preserved_on_zero(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel {}"
    LINE "TgtA {}"
    LINE "TgtB {}"
    LINE "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  (Rel, TgtA)"
    LINE "  for i in 0..$count {"
    LINE "    (Rel, TgtB)"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    typedef struct { int32_t count; } T;

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt_a = ecs_lookup(world, "TgtA");
    ecs_entity_t tgt_b = ecs_lookup(world, "TgtB");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_pair(world, e, rel, tgt_a));
    test_assert(ecs_has_pair(world, e, rel, tgt_b));

    ecs_entity_t ecs_id(T) = tree;
    ecs_set(world, e, T, {0});

    test_assert(ecs_has_pair(world, e, rel, tgt_a));
    test_assert(!ecs_has_pair(world, e, rel, tgt_b));

    ecs_set(world, e, T, {2});

    test_assert(ecs_has_pair(world, e, rel, tgt_a));
    test_assert(ecs_has_pair(world, e, rel, tgt_b));

    ecs_fini(world);
}

void IncrementalUpdates_anonymous_entities_in_loop_count_to_zero(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    register_position(world, ecs_id(Position));

    const char *expr =
    HEAD "Marker {}"
    LINE "template Tree {"
    LINE "  prop count = flecs.meta.i32: 2"
    LINE "  prop height = flecs.meta.f32: 1"
    LINE "  for i in 0..$count {"
    LINE "    _ {"
    LINE "      Marker"
    LINE "      Position: {$i, $height}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Tree e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t marker = ecs_lookup(world, "Marker");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t anons[4];
    test_int(find_children_w_id(world, e, marker, anons, 4), 2);
    ecs_entity_t anon_0 = anons[0];
    ecs_entity_t anon_1 = anons[1];

    ecs_entity_t ecs_id(TmplCountHeight) = tree;
    ecs_set(world, e, TmplCountHeight, {0, 1});

    test_int(find_children_w_id(world, e, marker, anons, 4), 0);
    test_assert(!ecs_is_alive(world, anon_0));
    test_assert(!ecs_is_alive(world, anon_1));

    ecs_set(world, e, TmplCountHeight, {2, 1});

    test_int(find_children_w_id(world, e, marker, anons, 4), 2);

    ecs_fini(world);
}
