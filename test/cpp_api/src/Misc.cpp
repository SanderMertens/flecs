#include <cpp_api.h>

void Misc_setup() {
    ecs_os_set_api_defaults();
}

void Misc_string_compare_w_char_ptr() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "foo";
    const char *ptr_3 = "bar";

    flecs::string str = flecs::string(ecs_os_strdup(ptr_1));

    test_assert(str == ptr_1);
    test_assert(str == ptr_2);
    test_assert(str != ptr_3);
}

void Misc_string_view_compare_w_char_ptr() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "foo";
    const char *ptr_3 = "bar";

    flecs::string str = flecs::string_view(ptr_1);

    test_assert(str == ptr_1);
    test_assert(str == ptr_2);
    test_assert(str != ptr_3);   
}

void Misc_string_compare_w_char_ptr_length_diff() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "foo";
    const char *ptr_3 = "barrr";
    const char *ptr_4 = "ba";

    flecs::string str = flecs::string_view(ptr_1);

    test_assert(str == ptr_1);
    test_assert(str == ptr_2);
    test_assert(str != ptr_3);
    test_assert(str != ptr_4);
}

void Misc_string_compare_w_string() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "bar";

    flecs::string str_1 = flecs::string(ecs_os_strdup(ptr_1));
    flecs::string str_2 = flecs::string(ecs_os_strdup(ptr_2));

    flecs::string str_3 = flecs::string(ecs_os_strdup(ptr_1));
    flecs::string str_4 = flecs::string(ecs_os_strdup(ptr_2));

    test_assert(str_1 == str_1);
    test_assert(str_1 == str_3);

    test_assert(str_2 == str_2);
    test_assert(str_2 == str_4);

    test_assert(str_1 != str_2);
    test_assert(str_2 != str_1);
}

void Misc_string_view_compare_w_string() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "bar";

    flecs::string str_1 = flecs::string_view(ptr_1);
    flecs::string str_2 = flecs::string_view(ptr_2);

    flecs::string str_3 = flecs::string(ecs_os_strdup(ptr_1));
    flecs::string str_4 = flecs::string(ecs_os_strdup(ptr_2));

    test_assert(str_1 == str_1);
    test_assert(str_1 == str_3);

    test_assert(str_2 == str_2);
    test_assert(str_2 == str_4);

    test_assert(str_1 != str_2);
    test_assert(str_2 != str_1);
}

void Misc_string_compare_nullptr() {
    const char *ptr_1 = "foo";

    flecs::string str = flecs::string_view(ptr_1);

    test_assert(str != nullptr);
}

void Misc_nullptr_string_compare() {
    const char *ptr = "foo";

    flecs::string str_1 = flecs::string_view(nullptr);
    flecs::string str_2 = flecs::string_view(ptr);

    test_assert(str_1 == "");
    test_assert(str_1 != ptr);
    test_assert(str_1 != str_2);
}

void Misc_nullptr_string_compare_nullptr() {
    const char *ptr_1 = "foo";

    flecs::string str = flecs::string_view(nullptr);

    test_assert(str == "");
    test_assert(str != ptr_1);
}

static ECS_COMPONENT_DECLARE(Velocity);
static ECS_DECLARE(TagB);
static ECS_DECLARE(E2);
static int sys_invoked_count = 0;
static int obs_invoked_count = 0;
static int trig_invoked_count = 0;

void Sys(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    test_assert(p != NULL);
    test_assert(v != NULL);

    sys_invoked_count += it->count;
}

void Obs(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    test_assert(p != NULL);
    test_assert(v != NULL);

    obs_invoked_count += it->count;
}

void Trig(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    test_assert(p != NULL);

    trig_invoked_count += it->count;
}

void Misc_c_macros() {
    flecs::world world;

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ECS_TAG(world, TagA);
    ECS_TAG_DEFINE(world, TagB);

    ECS_ENTITY(world, E1, 0);
    ECS_ENTITY_DEFINE(world, E2, 0);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, Position, Velocity);
    ECS_OBSERVER(world, Obs, EcsOnAdd, Position, Velocity);
    ECS_OBSERVER(world, Trig, EcsOnAdd, Position);

    test_assert(ecs_id(Position) != 0);
    test_assert(ecs_id(Velocity) != 0);

    test_assert(TagA != 0);
    test_assert(TagB != 0);

    test_assert(E1 != 0);
    test_assert(E2 != 0);

    ecs_add(world, E1, TagA);
    ecs_add(world, E1, TagB);

    test_assert( ecs_has(world, E1, TagA));
    test_assert( ecs_has(world, E1, TagB));

    ecs_add(world, E1, Position);
    test_int(obs_invoked_count, 0);
    test_int(trig_invoked_count, 1);
    ecs_add(world, E1, Velocity);
    test_int(obs_invoked_count, 1);

    test_assert( ecs_has(world, E1, Position));
    test_assert( ecs_has(world, E1, Velocity));
    
    ecs_progress(world, 0);

    test_int(sys_invoked_count, 1);
}

void Misc_app_run() {
    flecs::world ecs;

    bool system_ran = false;
    ecs.system().iter([&](flecs::iter& it) {
        system_ran = true;
        it.world().quit();
    });

    test_int(ecs.app().run(), 0);
    test_bool(system_ran, true);
}

void Misc_app_run_target_fps() {
    flecs::world ecs;

    int32_t count = 0;
    ecs.system().iter([&](flecs::iter& it) {
        count ++;
        it.world().quit();
    });

    ecs.set_target_fps(10);

    ecs.app().run();

    test_int(count, 1);
    test_int(ecs.get_target_fps(), 10);
}
