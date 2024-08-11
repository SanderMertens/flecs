#include <query.h>

void Plan_reordered_plan_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, ChildOf($this, $p, $gp, $ggp), Bar($ggp)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  selfupid    $[this]           (Foo)"
    LINE " 2. [ 1,  3]  and         $[this]           (ChildOf, $p)"
    LINE " 3. [ 2,  4]  and         $p                (ChildOf, $gp)"
    LINE " 4. [ 3,  5]  and         $gp               (ChildOf, $ggp)"
    LINE " 5. [ 4,  6]  selfupid    $ggp              (Bar)"
    LINE " 6. [ 5,  7]  setvars     "
    LINE " 7. [ 6,  8]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_reordered_plan_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($ggp), ChildOf($this, $p, $gp, $ggp), Bar($this)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  selfupid    $[ggp]            (Foo)"
    LINE " 2. [ 1,  3]  each        $ggp              ($[ggp])"
    LINE " 3. [ 2,  4]  and         $[gp]             (ChildOf, $ggp)"
    LINE " 4. [ 3,  5]  each        $gp               ($[gp])"
    LINE " 5. [ 4,  6]  and         $[p]              (ChildOf, $gp)"
    LINE " 6. [ 5,  7]  each        $p                ($[p])"
    LINE " 7. [ 6,  8]  and         $[this]           (ChildOf, $p)"
    LINE " 8. [ 7,  9]  selfupid    $[this]           (Bar)"
    LINE " 9. [ 8, 10]  setvars     "
    LINE "10. [ 9, 11]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_reordered_plan_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "?Foo, Bar"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  andid       $[this]           (Bar)"
    LINE " 2. [ 1,  4]  option      "
    LINE " 3. [ 2,  4]   and        $[this]           (Foo)"
    LINE " 4. [ 2,  5]  end         $[this]           (Foo)"
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_reordered_plan_4(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);
    ECS_TAG(world, World);

    ecs_query_t *r = ecs_query(world, {
        .expr = "?Foo, ?Bar, Hello"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  andid       $[this]           (Hello)"
    LINE " 2. [ 1,  4]  option      "
    LINE " 3. [ 2,  4]   and        $[this]           (Foo)"
    LINE " 4. [ 2,  5]  end         $[this]           (Foo)"
    LINE " 5. [ 4,  7]  option      "
    LINE " 6. [ 5,  7]   andid      $[this]           (Bar)"
    LINE " 7. [ 5,  8]  end         $[this]           (Bar)"
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_reordered_plan_5(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_query_t *r = ecs_query(world, {
        .expr = "?Foo, Hello, ?Bar"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  andid       $[this]           (Hello)"
    LINE " 2. [ 1,  4]  option      "
    LINE " 3. [ 2,  4]   and        $[this]           (Foo)"
    LINE " 4. [ 2,  5]  end         $[this]           (Foo)"
    LINE " 5. [ 4,  7]  option      "
    LINE " 6. [ 5,  7]   andid      $[this]           (Bar)"
    LINE " 7. [ 5,  8]  end         $[this]           (Bar)"
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_reordered_plan_6(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);
    ECS_TAG(world, World);

    ecs_query_t *r = ecs_query(world, {
        .expr = "?Foo, Hello, ?Bar, World"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  triv        {1,3}"
    LINE " 2. [ 1,  4]  option      "
    LINE " 3. [ 2,  4]   and        $[this]           (Foo)"
    LINE " 4. [ 2,  5]  end         $[this]           (Foo)"
    LINE " 5. [ 4,  7]  option      "
    LINE " 6. [ 5,  7]   andid      $[this]           (Bar)"
    LINE " 7. [ 5,  8]  end         $[this]           (Bar)"
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_reordered_plan_7(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);
    ECS_TAG(world, World);

    ecs_query_t *r = ecs_query(world, {
        .expr = "?Foo, Hello, ?Bar($this, $foo), World($foo)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  andid       $[this]           (Hello)"
    LINE " 2. [ 1,  4]  option      "
    LINE " 3. [ 2,  4]   and        $[this]           (Foo)"
    LINE " 4. [ 2,  5]  end         $[this]           (Foo)"
    LINE " 5. [ 4,  7]  option      "
    LINE " 6. [ 5,  7]   and        $[this]           (Bar, $foo)"
    LINE " 7. [ 5,  8]  end         $[this]           (Bar, $foo)"
    LINE " 8. [ 7, 10]  ifvar       $foo"
    LINE " 9. [ 8, 10]   andid      $foo              (World)"
    LINE "10. [ 8, 11]  end         $foo              (World)"
    LINE "11. [10, 12]  setvars     "
    LINE "12. [11, 13]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_1_trivial_plan(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    char *plan = ecs_query_plan(r);
    test_str(NULL, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_2_trivial_plan(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self), Bar(self)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    char *plan = ecs_query_plan(r);
    test_str(NULL, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_1_trivial_plan_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(self)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    char *plan = ecs_query_plan(r);
    test_str(NULL, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_2_trivial_plan_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(self), Velocity(self)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    char *plan = ecs_query_plan(r);
    test_str(NULL, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_3_trivial_plan_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "p" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self), Bar(self), ChildOf(self, p)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    char *plan = ecs_query_plan(r);
    test_str(NULL, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_3_trivial_plan_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "p" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self), Bar(self), ChildOf(self, *)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  triv        {0,1}"
    LINE " 2. [ 1,  3]  and         $[this]           (ChildOf, $*'1)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_3_trivial_plan_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "p" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self), Bar(self), ChildOf(self, _)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      " 
    LINE " 1. [ 0,  2]  triv        {0,1}"
    LINE " 2. [ 1,  3]  andany      $[this]           (ChildOf, $_'1)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_3_trivial_plan_w_pair_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "p" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(self), Velocity(self), ChildOf(self, p)"
    });
    
    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    char *plan = ecs_query_plan(r);
    test_str(NULL, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_3_trivial_plan_w_wildcard_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "p" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(self), Velocity(self), ChildOf(self, *)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  triv        {0,1}"
    LINE " 2. [ 1,  3]  and         $[this]           (ChildOf, $*'1)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_3_trivial_plan_w_any_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "p" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(self), Velocity(self), ChildOf(self, _)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      " 
    LINE " 1. [ 0,  2]  triv        {0,1}"
    LINE " 2. [ 1,  3]  andany      $[this]           (ChildOf, $_'1)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_1_trivial_component_w_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "p" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "[none] Position(self)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    char *plan = ecs_query_plan(r);
    test_str(NULL, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_2_trivial_component_w_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "p" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "[none] Position(self), [none] Velocity(self)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    char *plan = ecs_query_plan(r);
    test_str(NULL, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_2_trivial_plan_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self), ChildOf(self, *)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setids      "
    LINE " 1. [ 0,  2]  andid       $[this]           (Foo)"
    LINE " 2. [ 1,  3]  and         $[this]           (ChildOf, $*'1)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_this_before_fixed_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, Bar(e)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Bar)"
    LINE " 3. [ 2,  4]  selfupid    $[this]           (Foo)"
    LINE " 4. [ 3,  5]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_fixed_src_before_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Bar(e), Foo"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Bar)"
    LINE " 3. [ 2,  4]  selfupid    $[this]           (Foo)"
    LINE " 4. [ 3,  5]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_var_before_fixed_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($var), Bar(e)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Bar)"
    LINE " 3. [ 2,  4]  selfupid    $[var]            (Foo)"
    LINE " 4. [ 3,  5]  each        $var              ($[var])"
    LINE " 5. [ 4,  6]  setvars     "
    LINE " 6. [ 5,  7]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_fixed_src_before_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Bar(e), Foo($var)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Bar)"
    LINE " 3. [ 2,  4]  selfupid    $[var]            (Foo)"
    LINE " 4. [ 3,  5]  each        $var              ($[var])"
    LINE " 5. [ 4,  6]  setvars     "
    LINE " 6. [ 5,  7]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_this_before_fixed_src_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, !Bar(e)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  4]  not         "
    LINE " 3. [ 2,  4]   selfupid   e                 (Bar)"
    LINE " 4. [ 2,  5]  end         e                 (Bar)"
    LINE " 5. [ 4,  6]  selfupid    $[this]           (Foo)"
    LINE " 6. [ 5,  7]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_this_before_fixed_src_w_first_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, $this(e)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  with        e                 ($this)"
    LINE " 3. [ 2,  4]  selfupid    $this             (Foo)"
    LINE " 4. [ 3,  5]  setthis                       ($this)"
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_this_before_fixed_src_w_first_var_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, !$this(e)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    $[this]           (Foo)"
    LINE " 3. [ 2,  4]  each        $this             ($[this])"
    LINE " 4. [ 3,  6]  not         "
    LINE " 5. [ 4,  6]   with       e                 ($this)"
    LINE " 6. [ 4,  7]  end         e                 ($this)"
    LINE " 7. [ 6,  8]  setthis                       ($this)"
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_this_before_fixed_src_w_second_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, Bar(e, $this)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfup      e                 (Bar, $this)"
    LINE " 3. [ 2,  4]  selfupid    $this             (Foo)"
    LINE " 4. [ 3,  5]  setthis                       ($this)"
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_this_before_fixed_src_w_second_var_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, !Bar(e, $this)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    $[this]           (Foo)"
    LINE " 3. [ 2,  4]  each        $this             ($[this])"
    LINE " 4. [ 3,  6]  not         "
    LINE " 5. [ 4,  6]   selfup     e                 (Bar, $this)"
    LINE " 6. [ 4,  7]  end         e                 (Bar, $this)"
    LINE " 7. [ 6,  8]  setthis                       ($this)"
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_1_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(e)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_1_fixed_1_this_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(self)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  andid       $[this]           (Velocity)"
    LINE " 4. [ 3,  5]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_this_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(e), Mass(self), Rotation(self)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  triv        {2,3}"
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_this_self_interleaved(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Mass(self), Velocity(e), Rotation(self)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  triv        {1,3}"
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_this_self_2_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Mass(self), Rotation(self), Position(e), Velocity(e)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  triv        {0,1}"
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_1_fixed_1_this_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(up)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  upid        $[this]           (Velocity)"
    LINE " 4. [ 3,  5]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_this_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(e), Mass(up), Rotation(up)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  upid        $[this]           (Mass)"
    LINE " 5. [ 4,  6]  upid        $[this]           (Rotation)"
    LINE " 6. [ 5,  7]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_this_up_interleaved(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Mass(up), Velocity(e), Rotation(up)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  upid        $[this]           (Mass)"
    LINE " 5. [ 4,  6]  upid        $[this]           (Rotation)"
    LINE " 6. [ 5,  7]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_this_up_2_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Mass(up), Rotation(up), Position(e), Velocity(e)"
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  upid        $[this]           (Mass)"
    LINE " 5. [ 4,  6]  upid        $[this]           (Rotation)"
    LINE " 6. [ 5,  7]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_1_fixed_1_this_self_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(self)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  cache       "
    LINE " 4. [ 3,  5]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_this_self_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(e), Mass(self), Rotation(self)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  cache       "
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_this_self_interleaved_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Mass(self), Velocity(e), Rotation(self)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  cache       "
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_this_self_2_fixed_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Mass(self), Rotation(self), Position(e), Velocity(e)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  cache       "
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_1_fixed_1_this_up_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(up)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  cache       "
    LINE " 4. [ 3,  5]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_this_up_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(e), Mass(up), Rotation(up)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  cache       "
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_this_up_interleaved_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Mass(up), Velocity(e), Rotation(up)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  cache       "
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_this_up_2_fixed_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Mass(up), Rotation(up), Position(e), Velocity(e)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  cache       "
    LINE " 5. [ 4,  6]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_1_fixed_1_var_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity($var|self)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  andid       $[var]            (Velocity)"
    LINE " 4. [ 3,  5]  each        $var              ($[var])"
    LINE " 5. [ 4,  6]  setvars     "
    LINE " 6. [ 5,  7]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_var_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(e), Mass($var|self), Rotation($var|self)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  andid       $[var]            (Mass)"
    LINE " 5. [ 4,  6]  andid       $[var]            (Rotation)"
    LINE " 6. [ 5,  7]  each        $var              ($[var])"
    LINE " 7. [ 6,  8]  setvars     "
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_var_self_interleaved(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Mass($var|self), Velocity(e), Rotation($var|self)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  andid       $[var]            (Mass)"
    LINE " 5. [ 4,  6]  andid       $[var]            (Rotation)"
    LINE " 6. [ 5,  7]  each        $var              ($[var])"
    LINE " 7. [ 6,  8]  setvars     "
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_var_self_2_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Mass($var|self), Rotation($var|self), Position(e), Velocity(e)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  andid       $[var]            (Mass)"
    LINE " 5. [ 4,  6]  andid       $[var]            (Rotation)"
    LINE " 6. [ 5,  7]  each        $var              ($[var])"
    LINE " 7. [ 6,  8]  setvars     "
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_1_fixed_1_var_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity($var|up)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  upid        $[var]            (Velocity)"
    LINE " 4. [ 3,  5]  each        $var              ($[var])"
    LINE " 5. [ 4,  6]  setvars     "
    LINE " 6. [ 5,  7]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_var_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Velocity(e), Mass($var|up), Rotation($var|up)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  upid        $[var]            (Mass)"
    LINE " 5. [ 4,  6]  upid        $[var]            (Rotation)"
    LINE " 6. [ 5,  7]  each        $var              ($[var])"
    LINE " 7. [ 6,  8]  setvars     "
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_fixed_2_var_up_interleaved(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e), Mass($var|up), Velocity(e), Rotation($var|up)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  upid        $[var]            (Mass)"
    LINE " 5. [ 4,  6]  upid        $[var]            (Rotation)"
    LINE " 6. [ 5,  7]  each        $var              ($[var])"
    LINE " 7. [ 6,  8]  setvars     "
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_populate_2_var_up_2_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Mass($var|up), Rotation($var|up), Position(e), Velocity(e)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  selfupid    e                 (Position)"
    LINE " 3. [ 2,  4]  selfupid    e                 (Velocity)"
    LINE " 4. [ 3,  5]  upid        $[var]            (Mass)"
    LINE " 5. [ 4,  6]  upid        $[var]            (Rotation)"
    LINE " 6. [ 5,  7]  each        $var              ($[var])"
    LINE " 7. [ 6,  8]  setvars     "
    LINE " 8. [ 7,  9]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_cache_2_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position || Velocity",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  xcache      "
    LINE " 1. [ 0,  2]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_cache_2_or_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position || Velocity, !Mass",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  xcache      "
    LINE " 1. [ 0,  2]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_1_plan_any_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  ids         $[_'1]            (RelA)"
    LINE " 1. [ 0,  2]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);
    ecs_query_fini(r);
    ecs_fini(world);
}

void Plan_1_plan_not_any_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "!RelA(_)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  2]  not         "
    LINE " 1. [ 0,  2]   ids        $[_'1]            (RelA)"
    LINE " 2. [ 0,  3]  end         $[_'1]            (RelA)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);
    
    ecs_fini(world);
}

void Plan_1_plan_optional_any_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "?RelA(_)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  2]  option      "
    LINE " 1. [ 0,  2]   ids        $[_'1]            (RelA)"
    LINE " 2. [ 0,  3]  end         $[_'1]            (RelA)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);
    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_pair_first_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "(*, Tgt)",
        .cache_kind = EcsQueryCacheDefault
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  and         $[this]           ($*'1, Tgt)"
    LINE " 1. [ 0,  2]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);
    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_pair_first_wildcard_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "(*, Tgt)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  xcache      "
    LINE " 1. [ 0,  2]  yield       "
    LINE "";    
    char *plan = ecs_query_plan(r);
    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_pair_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *r = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = EcsQueryCacheDefault
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  and         $[this]           (Rel, $*'1)"
    LINE " 1. [ 0,  2]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);
    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_pair_second_wildcard_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *r = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  xcache      "
    LINE " 1. [ 0,  2]  yield       "
    LINE "";
    char *plan = ecs_query_plan(r);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Plan_0_src_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(#0)"
    });

    test_assert(q != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  yield       "
    LINE "";
    char *plan = ecs_query_plan(q);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Plan_0_src_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(#0)"
    });

    test_assert(q != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  yield       "
    LINE "";
    char *plan = ecs_query_plan(q);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Plan_0_src_w_sparse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(#0)"
    });

    test_assert(q != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  yield       "
    LINE "";
    char *plan = ecs_query_plan(q);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Plan_0_src_w_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(#0)"
    });

    test_assert(q != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  yield       "
    LINE "";
    char *plan = ecs_query_plan(q);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Plan_0_src_w_union(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Movement);

    ecs_add_id(world, ecs_id(Movement), EcsUnion);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement(#0, *)"
    });

    test_assert(q != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  yield       "
    LINE "";
    char *plan = ecs_query_plan(q);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Plan_0_src_w_sparse_and_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(#0), Velocity"
    });

    test_assert(q != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  andid       $[this]           (Velocity)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(q);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Plan_0_src_w_toggle_and_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(#0), Velocity"
    });

    test_assert(q != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  andid       $[this]           (Velocity)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(q);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Plan_0_src_w_union_and_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Movement);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Movement), EcsUnion);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement(#0, *), Velocity"
    });

    test_assert(q != NULL);

    ecs_log_enable_colors(false);

    const char *expect = 
    HEAD " 0. [-1,  1]  setfix      "
    LINE " 1. [ 0,  2]  setids      "
    LINE " 2. [ 1,  3]  andid       $[this]           (Velocity)"
    LINE " 3. [ 2,  4]  yield       "
    LINE "";
    char *plan = ecs_query_plan(q);

    test_str(expect, plan);
    ecs_os_free(plan);

    ecs_query_fini(q);

    ecs_fini(world);
}
