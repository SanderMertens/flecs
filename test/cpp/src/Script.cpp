#include <cpp.h>

static void script_sum(const ecs_function_ctx_t *ctx, int32_t argc,
    const ecs_value_t *argv, ecs_value_t *result)
{
    test_assert(argc == 1 || argc == 2);
    int64_t a = *static_cast<const int64_t*>(argv[0].ptr);
    int64_t b = *static_cast<const int64_t*>(argv[1].ptr);
    int64_t offset = ctx->ctx ? *static_cast<int64_t*>(ctx->ctx) : 0;
    *static_cast<int64_t*>(result->ptr) = a + b + offset;
}

template <typename T>
static void script_lerp(const ecs_function_ctx_t*, int32_t argc,
    const ecs_value_t *argv, ecs_value_t *result, int32_t count)
{
    test_int(argc, 3);
    const T *a = static_cast<const T*>(argv[0].ptr);
    const T *b = static_cast<const T*>(argv[1].ptr);
    double t = *static_cast<const double*>(argv[2].ptr);
    T *r = static_cast<T*>(result->ptr);
    for (int32_t i = 0; i < count; i ++) {
        r[i] = a[i] + t * (b[i] - a[i]);
    }
}

void Script_function(void) {
    flecs::world world;
    auto f = world.function("sum")
        .return_type<int64_t>()
        .param<int64_t>("a")
        .param<int64_t>("b")
        .callback(script_sum)
        .build();
    test_assert(f != 0);
    test_int(world.script_run(nullptr, "export const result = sum(10, 20)"), 0);
    test_int(world.get_const_var<int64_t>("result"), 30);
}

void Script_function_scope_and_ctx(void) {
    flecs::world world;
    auto parent = world.entity("math");
    int64_t offset = 5;
    world.scope(parent, [&] {
        auto f = world.function("sum")
            .return_type(world.id<int64_t>())
            .param("a", world.id<int64_t>())
            .param("b", world.id<int64_t>())
            .callback(script_sum)
            .ctx(&offset)
            .build();
        test_assert(f.parent() == parent);
    });
    auto other = world.entity("other");
    auto f = world.function("sum")
        .parent(other)
        .return_type<int64_t>()
        .param<int64_t>("a")
        .param<int64_t>("b")
        .callback(script_sum)
        .build();
    test_assert(f.parent() == other);
    test_int(world.script_run(nullptr,
        "export const result = math.sum(10, 20) + other.sum(1, 2)"), 0);
    test_int(world.get_const_var<int64_t>("result"), 38);
}

void Script_method(void) {
    flecs::world world;
    auto m = world.method<int64_t>("add")
        .return_type<int64_t>()
        .param<int64_t>("a")
        .callback(script_sum)
        .build();
    test_assert(m.parent() == world.id<int64_t>());
    test_int(world.script_run(nullptr,
        "const v: i64 = 10\nexport const result = v.add(20)"), 0);
    test_int(world.get_const_var<int64_t>("result"), 30);
}

void Script_vector_function(void) {
    flecs::world world;
    auto f = world.function("lerp")
        .return_type(EcsScriptVectorType)
        .param("a", EcsScriptVectorType)
        .param("b", EcsScriptVectorType)
        .param<double>("t")
        .vector_callback(flecs::F32, script_lerp<float>)
        .vector_callback(flecs::F64, script_lerp<double>)
        .build();
    test_assert(f != 0);
    test_int(world.script_run(nullptr,
        "struct V32(x: f32, y: f32)\n"
        "struct V64(x: f64, y: f64)\n"
        "const a: V32 = {10, 20}\nconst b: V32 = {30, 40}\n"
        "const c: V64 = {20, 30}\nconst d: V64 = {40, 50}\n"
        "export const r32: V32 = lerp(a, b, 0.5)\n"
        "export const r64: V64 = lerp(c, d, 0.25)"), 0);
    ecs_value_t r32 = ecs_const_var_get(world, world.lookup("r32"));
    ecs_value_t r64 = ecs_const_var_get(world, world.lookup("r64"));
    test_assert(r32.ptr != nullptr);
    test_assert(r64.ptr != nullptr);
    test_flt(static_cast<const float*>(r32.ptr)[0], 20);
    test_flt(static_cast<const float*>(r32.ptr)[1], 30);
    test_flt(static_cast<const double*>(r64.ptr)[0], 25);
    test_flt(static_cast<const double*>(r64.ptr)[1], 35);
}

void Script_const_var(void) {
    flecs::world world;
    double value = 3.1415926;
    auto v = world.const_var("pi", value);
    test_assert(v != 0);
    value = 0;
    test_flt(world.get_const_var<double>("pi"), 3.1415926);
    test_int(world.script_run(nullptr, "export const result = pi * 2"), 0);
    test_flt(world.get_const_var<double>("result"), 6.2831852);
}

void Script_vars_scope(void) {
    flecs::world world;
    auto parent = world.entity("config");
    world.scope(parent, [&] {
        test_assert(world.const_var("constant", 10).parent() == parent);
        test_assert(world.mut_var("mutable", 20.0f).parent() == parent);
    });
    test_assert(world.const_var("constant2", 30, parent).parent() == parent);
    test_assert(world.mut_var("mutable2", 40.0f, parent).parent() == parent);
    test_int(world.get_const_var<int>("config::constant"), 10);
    test_int(world.get_const_var<int>("config::constant2"), 30);
    test_flt(world.get_mut_var<float>("config::mutable"), 20);
    test_int(world.set_mut_var("::config::mutable", 25.0), 0);
    test_int(world.set_mut_var("config::mutable2", 45.0f), 0);
    test_flt(world.get_mut_var<float>("config::mutable"), 25);
    test_flt(world.get_mut_var<float>("config::mutable2"), 45);
}

void Script_mut_var_reactive(void) {
    flecs::world world;
    auto v = world.mut_var("difficulty", 1.0f);
    test_assert(v != 0);
    auto s = world.script().code(
        "struct Level(value: f32)\ne { Level: {difficulty} }").run();
    test_assert(s.get<flecs::Script>().error == nullptr);
    auto e = world.lookup("e");
    auto level = world.lookup("Level");
    test_flt(*static_cast<const float*>(e.get(level)), 1);
    test_int(world.set_mut_var("difficulty", 2.0f), 0);
    test_flt(*static_cast<const float*>(e.get(level)), 2);
    test_flt(world.get_mut_var<float>("difficulty"), 2);
}

void Script_set_mut_var_invalid(void) {
    flecs::world world;
    ecs_log_set_level(-4);
    test_int(world.set_mut_var("missing", 1), -1);
    world.entity("ordinary");
    test_int(world.set_mut_var("ordinary", 1), -1);
    world.const_var("constant", 1);
    test_int(world.set_mut_var("constant", 2), -1);
    test_int(world.get_const_var<int>("constant"), 1);
}

void Script_parse_eval(void) {
    flecs::world world;
    auto script = world.script_parse("test", "e {}");
    test_assert(static_cast<bool>(script));
    test_assert(world.lookup("e") == 0);
    test_int(script.eval(), 0);
    auto e = world.lookup("e");
    test_assert(e != 0);
    e.destruct();
    test_int(script.eval(), 0);
    test_assert(world.lookup("e") != 0);
}

void Script_parse_error(void) {
    flecs::world world;
    ecs_script_eval_result_t result = {};
    auto script = world.script_parse("test", "e {", nullptr, &result);
    test_assert(!script);
    test_assert(result.error != nullptr);
    ecs_os_free(result.error);
    test_int(script.eval(), -1);
}

void Script_eval_error(void) {
    flecs::world world;
    auto script = world.script_parse("test", "e { Missing }");
    test_assert(static_cast<bool>(script));
    ecs_script_eval_result_t result = {};
    test_int(script.eval(nullptr, &result), -1);
    test_assert(result.error != nullptr);
    ecs_os_free(result.error);
}

void Script_parse_move(void) {
    flecs::world world;
    static_assert(!std::is_copy_constructible_v<flecs::parsed_script>);
    static_assert(!std::is_copy_assignable_v<flecs::parsed_script>);
    auto first = world.script_parse("first", "e {}");
    flecs::parsed_script second(FLECS_MOV(first));
    test_assert(!first);
    test_assert(static_cast<bool>(second));
    auto third = world.script_parse("third", "other {}");
    third = FLECS_MOV(second);
    test_assert(!second);
    test_int(third.eval(), 0);
    test_assert(world.lookup("e") != 0);
    test_assert(world.lookup("other") == 0);
    flecs::parsed_script empty;
    test_assert(!empty);
    test_int(empty.eval(), -1);
}

void Script_update(void) {
    flecs::world world;
    auto s = world.script().code("old {}").run();
    test_assert(s.get<flecs::Script>().error == nullptr);
    test_assert(world.lookup("old") != 0);
    test_int(world.script_update(s, "updated {}"), 0);
    test_assert(world.lookup("old") == 0);
    test_assert(world.lookup("updated") != 0);
    test_assert(s.get<flecs::Script>().error == nullptr);
    ecs_log_set_level(-4);
    test_int(world.script_update(s, "updated { Missing }"), -1);
    test_assert(s.get<flecs::Script>().error != nullptr);
    test_int(world.script_update(s, "recovered {}"), 0);
    test_assert(s.get<flecs::Script>().error == nullptr);
    test_assert(world.lookup("recovered") != 0);
}
