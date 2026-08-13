#include <script.h>
#include "../../../src/addons/script/script.h"

static
void test_slots(
    const ecs_vec_t *entities,
    const ecs_entity_t *expect,
    int32_t count)
{
    test_int(ecs_vec_count(entities), count);

    int32_t i;
    for (i = 0; i < count; i ++) {
        test_uint(ecs_vec_get_t(entities, ecs_entity_t, i)[0], expect[i]);
    }
}

static
const ecs_vec_t* script_entities(
    const ecs_world_t *world,
    ecs_entity_t script)
{
    const EcsScript *ptr = ecs_get(world, script, EcsScript);
    test_assert(ptr != NULL);
    test_assert(ptr->script != NULL);
    return &flecs_script_impl(ptr->script)->entities;
}

static
const ecs_vec_t* instance_entities(
    const ecs_world_t *world,
    ecs_entity_t instance,
    ecs_entity_t template_entity)
{
    const EcsScriptTemplateRoot *ptr = ecs_get_pair(
        world, instance, EcsScriptTemplateRoot, template_entity);
    test_assert(ptr != NULL);
    return &ptr->entities;
}

void InternalRefs_script_one_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "a {}"
    });
    test_assert(s != 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    test_assert(a != 0);

    ecs_entity_t expect[] = { a };
    test_slots(script_entities(world, s), expect, 1);

    ecs_fini(world);
}

void InternalRefs_script_two_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "a {}"
        LINE "b {}"
    });
    test_assert(s != 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);

    ecs_entity_t expect[] = { a, b };
    test_slots(script_entities(world, s), expect, 2);

    ecs_fini(world);
}

void InternalRefs_script_two_entities_if(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "if false {"
        LINE "  a {}"
        LINE "}"
        LINE "b {}"
    });
    test_assert(s != 0);

    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(b != 0);

    ecs_entity_t expect[] = { 0, b };
    test_slots(script_entities(world, s), expect, 2);

    ecs_fini(world);
}

void InternalRefs_script_nested_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "parent {"
        LINE "  a {}"
        LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t parent = ecs_lookup(world, "parent");
    ecs_entity_t a = ecs_lookup(world, "parent.a");
    test_assert(parent != 0);
    test_assert(a != 0);

    ecs_entity_t expect[] = { parent, a };
    test_slots(script_entities(world, s), expect, 2);

    ecs_fini(world);
}

void InternalRefs_script_nested_two_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "parent {"
        LINE "  a {}"
        LINE "  b {}"
        LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t parent = ecs_lookup(world, "parent");
    ecs_entity_t a = ecs_lookup(world, "parent.a");
    ecs_entity_t b = ecs_lookup(world, "parent.b");
    test_assert(parent != 0);
    test_assert(a != 0);
    test_assert(b != 0);

    ecs_entity_t expect[] = { parent, a, b };
    test_slots(script_entities(world, s), expect, 3);

    ecs_fini(world);
}

void InternalRefs_script_nested_two_entities_if(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "parent {"
        LINE "  if false {"
        LINE "    a {}"
        LINE "  }"
        LINE "  b {}"
        LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t parent = ecs_lookup(world, "parent");
    ecs_entity_t b = ecs_lookup(world, "parent.b");
    test_assert(parent != 0);
    test_assert(ecs_lookup(world, "parent.a") == 0);
    test_assert(b != 0);

    ecs_entity_t expect[] = { parent, 0, b };
    test_slots(script_entities(world, s), expect, 3);

    ecs_fini(world);
}

void InternalRefs_template_one_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  a {}"
    LINE "}"
    LINE "e1 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t a = ecs_lookup(world, "e1.a");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(a != 0);

    ecs_entity_t expect[] = { a };
    test_slots(instance_entities(world, e1, foo), expect, 1);

    ecs_fini(world);
}

void InternalRefs_template_two_entities(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  a {}"
    LINE "  b {}"
    LINE "}"
    LINE "e1 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t a = ecs_lookup(world, "e1.a");
    ecs_entity_t b = ecs_lookup(world, "e1.b");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(a != 0);
    test_assert(b != 0);

    ecs_entity_t expect[] = { a, b };
    test_slots(instance_entities(world, e1, foo), expect, 2);

    ecs_fini(world);
}

void InternalRefs_template_two_entities_if(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  if false {"
    LINE "    a {}"
    LINE "  }"
    LINE "  b {}"
    LINE "}"
    LINE "e1 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t b = ecs_lookup(world, "e1.b");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(ecs_lookup(world, "e1.a") == 0);
    test_assert(b != 0);

    ecs_entity_t expect[] = { 0, b };
    test_slots(instance_entities(world, e1, foo), expect, 2);

    ecs_fini(world);
}

void InternalRefs_template_nested_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  parent {"
    LINE "    a {}"
    LINE "  }"
    LINE "}"
    LINE "e1 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t parent = ecs_lookup(world, "e1.parent");
    ecs_entity_t a = ecs_lookup(world, "e1.parent.a");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(parent != 0);
    test_assert(a != 0);

    ecs_entity_t expect[] = { parent, a };
    test_slots(instance_entities(world, e1, foo), expect, 2);

    ecs_fini(world);
}

void InternalRefs_template_nested_two_entities(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  parent {"
    LINE "    a {}"
    LINE "    b {}"
    LINE "  }"
    LINE "}"
    LINE "e1 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t parent = ecs_lookup(world, "e1.parent");
    ecs_entity_t a = ecs_lookup(world, "e1.parent.a");
    ecs_entity_t b = ecs_lookup(world, "e1.parent.b");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(parent != 0);
    test_assert(a != 0);
    test_assert(b != 0);

    ecs_entity_t expect[] = { parent, a, b };
    test_slots(instance_entities(world, e1, foo), expect, 3);

    ecs_fini(world);
}

void InternalRefs_template_nested_two_entities_if(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  parent {"
    LINE "    if false {"
    LINE "      a {}"
    LINE "    }"
    LINE "    b {}"
    LINE "  }"
    LINE "}"
    LINE "e1 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t parent = ecs_lookup(world, "e1.parent");
    ecs_entity_t b = ecs_lookup(world, "e1.parent.b");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(parent != 0);
    test_assert(ecs_lookup(world, "e1.parent.a") == 0);
    test_assert(b != 0);

    ecs_entity_t expect[] = { parent, 0, b };
    test_slots(instance_entities(world, e1, foo), expect, 3);

    ecs_fini(world);
}

void InternalRefs_2_instances_one_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  a {}"
    LINE "}"
    LINE "e1 { Foo: {} }"
    LINE "e2 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t a1 = ecs_lookup(world, "e1.a");
    ecs_entity_t a2 = ecs_lookup(world, "e2.a");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(a1 != 0);
    test_assert(a2 != 0);
    test_assert(a1 != a2);

    ecs_entity_t expect_1[] = { a1 };
    ecs_entity_t expect_2[] = { a2 };
    test_slots(instance_entities(world, e1, foo), expect_1, 1);
    test_slots(instance_entities(world, e2, foo), expect_2, 1);

    ecs_fini(world);
}

void InternalRefs_2_instances_two_entities(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  a {}"
    LINE "  b {}"
    LINE "}"
    LINE "e1 { Foo: {} }"
    LINE "e2 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t a1 = ecs_lookup(world, "e1.a");
    ecs_entity_t b1 = ecs_lookup(world, "e1.b");
    ecs_entity_t a2 = ecs_lookup(world, "e2.a");
    ecs_entity_t b2 = ecs_lookup(world, "e2.b");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(a1 != 0);
    test_assert(b1 != 0);
    test_assert(a2 != 0);
    test_assert(b2 != 0);

    ecs_entity_t expect_1[] = { a1, b1 };
    ecs_entity_t expect_2[] = { a2, b2 };
    test_slots(instance_entities(world, e1, foo), expect_1, 2);
    test_slots(instance_entities(world, e2, foo), expect_2, 2);

    ecs_fini(world);
}

void InternalRefs_2_instances_two_entities_if(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  if false {"
    LINE "    a {}"
    LINE "  }"
    LINE "  b {}"
    LINE "}"
    LINE "e1 { Foo: {} }"
    LINE "e2 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t b1 = ecs_lookup(world, "e1.b");
    ecs_entity_t b2 = ecs_lookup(world, "e2.b");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(ecs_lookup(world, "e1.a") == 0);
    test_assert(ecs_lookup(world, "e2.a") == 0);
    test_assert(b1 != 0);
    test_assert(b2 != 0);

    ecs_entity_t expect_1[] = { 0, b1 };
    ecs_entity_t expect_2[] = { 0, b2 };
    test_slots(instance_entities(world, e1, foo), expect_1, 2);
    test_slots(instance_entities(world, e2, foo), expect_2, 2);

    ecs_fini(world);
}

void InternalRefs_2_instances_nested_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  parent {"
    LINE "    a {}"
    LINE "  }"
    LINE "}"
    LINE "e1 { Foo: {} }"
    LINE "e2 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t parent_1 = ecs_lookup(world, "e1.parent");
    ecs_entity_t a1 = ecs_lookup(world, "e1.parent.a");
    ecs_entity_t parent_2 = ecs_lookup(world, "e2.parent");
    ecs_entity_t a2 = ecs_lookup(world, "e2.parent.a");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(parent_1 != 0);
    test_assert(a1 != 0);
    test_assert(parent_2 != 0);
    test_assert(a2 != 0);

    ecs_entity_t expect_1[] = { parent_1, a1 };
    ecs_entity_t expect_2[] = { parent_2, a2 };
    test_slots(instance_entities(world, e1, foo), expect_1, 2);
    test_slots(instance_entities(world, e2, foo), expect_2, 2);

    ecs_fini(world);
}

void InternalRefs_2_instances_nested_two_entities(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  parent {"
    LINE "    a {}"
    LINE "    b {}"
    LINE "  }"
    LINE "}"
    LINE "e1 { Foo: {} }"
    LINE "e2 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t parent_1 = ecs_lookup(world, "e1.parent");
    ecs_entity_t a1 = ecs_lookup(world, "e1.parent.a");
    ecs_entity_t b1 = ecs_lookup(world, "e1.parent.b");
    ecs_entity_t parent_2 = ecs_lookup(world, "e2.parent");
    ecs_entity_t a2 = ecs_lookup(world, "e2.parent.a");
    ecs_entity_t b2 = ecs_lookup(world, "e2.parent.b");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(parent_1 != 0);
    test_assert(a1 != 0);
    test_assert(b1 != 0);
    test_assert(parent_2 != 0);
    test_assert(a2 != 0);
    test_assert(b2 != 0);

    ecs_entity_t expect_1[] = { parent_1, a1, b1 };
    ecs_entity_t expect_2[] = { parent_2, a2, b2 };
    test_slots(instance_entities(world, e1, foo), expect_1, 3);
    test_slots(instance_entities(world, e2, foo), expect_2, 3);

    ecs_fini(world);
}

void InternalRefs_2_instances_nested_two_entities_if(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  parent {"
    LINE "    if false {"
    LINE "      a {}"
    LINE "    }"
    LINE "    b {}"
    LINE "  }"
    LINE "}"
    LINE "e1 { Foo: {} }"
    LINE "e2 { Foo: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t parent_1 = ecs_lookup(world, "e1.parent");
    ecs_entity_t b1 = ecs_lookup(world, "e1.parent.b");
    ecs_entity_t parent_2 = ecs_lookup(world, "e2.parent");
    ecs_entity_t b2 = ecs_lookup(world, "e2.parent.b");
    test_assert(foo != 0);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(parent_1 != 0);
    test_assert(ecs_lookup(world, "e1.parent.a") == 0);
    test_assert(b1 != 0);
    test_assert(parent_2 != 0);
    test_assert(ecs_lookup(world, "e2.parent.a") == 0);
    test_assert(b2 != 0);

    ecs_entity_t expect_1[] = { parent_1, 0, b1 };
    ecs_entity_t expect_2[] = { parent_2, 0, b2 };
    test_slots(instance_entities(world, e1, foo), expect_1, 3);
    test_slots(instance_entities(world, e2, foo), expect_2, 3);

    ecs_fini(world);
}
