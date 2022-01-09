#include <cpp_api.h>

void Type_type_no_name() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto type = flecs::type(world, nullptr, "Position");
    auto id = type.id();
    auto e = world.entity(id);
    test_assert(!e.has<flecs::Identifier>(flecs::Name));
}

void Type_null_args() {
    flecs::world world;

    flecs::type t();

    // Make sure code didn't crash
    test_assert(true);
}

void Type_has_type() {
    flecs::world world;

    auto type = world.type()
        .add<Position>()
        .add<Velocity>();

    test_assert(type.has<Position>());
    test_assert(type.has<Velocity>());
    test_assert(!type.has<Mass>());
}

void Type_has_entity() {
    flecs::world world;

    auto e1 = world.entity();
    auto e2 = world.entity();
    auto e3 = world.entity();

    auto type = world.type()
        .add(e1)
        .add(e2);

    test_assert(type.has(e1));
    test_assert(type.has(e2));
    test_assert(!type.has(e3));
}

void Type_has_pair_type() {
    flecs::world world;

    struct Eats {};
    struct Apples {};
    struct Pears {};
    struct Bananas {};

    auto type = world.type()
        .add<Eats, Apples>()
        .add<Eats, Pears>();

    test_assert((type.has<Eats, Apples>()));
    test_assert((type.has<Eats, Pears>()));
    test_assert((!type.has<Eats, Bananas>()));
}

void Type_has_pair_entity() {
    flecs::world world;

    auto eats = world.entity();
    auto apples = world.entity();
    auto pears = world.entity();
    auto bananas = world.entity();

    auto type = world.type()
        .add(eats, apples)
        .add(eats, pears);

    test_assert(type.has(eats, apples));
    test_assert(type.has(eats, pears));
    test_assert(!type.has(eats, bananas));
}

void Type_get() {
    flecs::world world;

    auto apples = world.entity();
    auto pears = world.entity();
    auto bananas = world.entity();

    auto type = world.type()
        .add(apples)
        .add(pears)
        .add(bananas);

    test_assert(type.has(apples));
    test_assert(type.has(pears));
    test_assert(type.has(bananas));

    test_assert(type.get(0) == apples);
    test_assert(type.get(1) == pears);
    test_assert(type.get(2) == bananas);
}

void Type_get_out_of_range() {
    install_test_abort();

    flecs::world world;

    auto apples = world.entity();
    auto pears = world.entity();

    auto type = world.type()
        .add(apples)
        .add(pears);

    test_assert(type.has(apples));
    test_assert(type.has(pears));

    test_assert(type.get(0) == apples);
    test_assert(type.get(1) == pears);

    test_expect_abort();
    type.get(2);
}

struct MyStruct {
    int Field;
};

void Type_has_from_stage() {
    flecs::world ecs;

    int32_t count = 0;

    ecs.system()
        .iter([&](flecs::iter &it){
            test_assert(it.type().has(it.world().id<MyStruct>()) == false);
            count ++;
        });

    test_int(count, 0);

    ecs.progress(1.0f);

    test_int(count, 1);
}

void Type_create_type_from_system() {
    struct SwitchType {};
    struct TagA {};
    struct TagB {};
    struct TagC {};

    flecs::world ecs;

    auto e = ecs.entity();

    auto s = ecs.system()
        .iter([&](flecs::iter& it)
    {
        auto t = ecs.type<SwitchType>()
            .add<TagA>()
            .add<TagB>()
            .add<TagC>();

        e.add_switch(t)
         .add_case<TagB>();
    });

    s.run();

    test_assert(e.has_switch<SwitchType>());
    test_assert(e.has_case<TagB>());
}

void Type_type_from_staged_iter() {
    flecs::world ecs;

    flecs::id taga_id = ecs.id<TagA>();
    flecs::id_t taga_id_t = ecs.id<TagA>();

    auto e = ecs.entity().add<TagA>();

    flecs::id result;

    int32_t count = 0;
    ecs.system<TagA>()
        .iter([&](flecs::iter& it)
        {
            test_int(it.count(), 1);
            test_assert(it.entity(0) == e);
            test_bool(it.type().has<TagA>(), true);
            test_bool(it.type().has(taga_id), true);
            test_bool(it.type().has(taga_id_t), true);
            test_bool(it.type().has<TagB>(), false);

            result = it.type().get(0);

            count ++;
        });

    test_int(count, 0);

    ecs.progress();

    test_int(count, 1);

    test_str(result.str(), "TagA");
}

void Type_type_from_iter() {
    flecs::world ecs;

    flecs::id taga_id = ecs.id<TagA>();
    flecs::id_t taga_id_t = ecs.id<TagA>();

    auto e = ecs.entity().add<TagA>();
    
    flecs::id result;

    int32_t count = 0;
    auto s = ecs.system<TagA>()
        .iter([&](flecs::iter& it)
        {
            test_int(it.count(), 1);
            test_assert(it.entity(0) == e);
            test_bool(it.type().has<TagA>(), true);
            test_bool(it.type().has(taga_id), true);
            test_bool(it.type().has(taga_id_t), true);
            test_bool(it.type().has<TagB>(), false);

            result = it.type().get(0);

            count ++;
        });

    test_int(count, 0);

    s.run();

    test_int(count, 1);

    test_str(result.str(), "TagA");
}
