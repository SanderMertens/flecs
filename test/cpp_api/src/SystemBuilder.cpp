#include <cpp_api.h>

void SystemBuilder_builder_assign_same_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;

    flecs::system<Position, Velocity> s = 
        ecs.system<Position, Velocity>()
            .each([&](flecs::entity e, Position& p, Velocity& v) {
                count ++;
                test_assert(e == e1);
            });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_builder_assign_from_empty() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;

    flecs::system<> s = ecs.system<>()
        .term<Position>()
        .term<Velocity>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_builder_build_to_auto() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;

    auto s = ecs.system<Position, Velocity>()
        .each([&](flecs::entity e, Position& p, Velocity& v) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_builder_build_n_statements() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();  

    int32_t count = 0;

    auto qb = ecs.system<>();
    qb.term<Position>();
    qb.term<Velocity>();
    auto s = qb.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });

    s.run();
    
    test_int(count, 1);
}

void SystemBuilder_1_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_1_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .term<Position>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_2_types() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .term<Position>()
        .term<Velocity>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_1_type_w_1_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .term<Velocity>()
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_2_types_w_1_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>().add<Mass>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .term<Velocity>()
        .term<Mass>()
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_pair() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto e1 = ecs.entity().add(Likes, Bob);
    ecs.entity().add(Likes, Alice);

    int32_t count = 0;

    auto s = ecs.system<>()
        .term(Likes, Bob)
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_not() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .term<Velocity>().oper(flecs::Not)
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_or() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();
    ecs.entity().add<Mass>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .term<Position>().oper(flecs::Or)
        .term<Velocity>().oper(flecs::Or)
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1 || e == e2);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 2);
}

void SystemBuilder_add_optional() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>().add<Mass>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .term<Position>()
        .term<Velocity>().oper(flecs::Optional)
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1 || e == e2);
        });
        
    test_int(count, 0);
    s.run();
    test_int(count, 2);
}

void SystemBuilder_ptr_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>().add<Mass>();

    int32_t count = 0;

    auto s = ecs.system<Position, Velocity*>()
        .each([&](flecs::entity e, Position& p, Velocity* v) {
            count ++;
            test_assert(e == e1 || e == e2);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 2);
}

void SystemBuilder_const_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<const Position>()
        .each([&](flecs::entity e, const Position& p) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_string_term() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .term("Position")
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    s.run();
    
    test_int(count, 1);
}

void SystemBuilder_singleton_term() {
    flecs::world ecs;

    struct Entity {
        flecs::entity_view value;
    };

    struct Singleton {
        int32_t value;
    };

    ecs.set<Singleton>({10});

    int32_t count = 0;

    auto s = ecs.system<Entity>()
        .term<Singleton>().singleton()
        .iter([&](flecs::iter& it, Entity *e) {
            auto s = it.term<const Singleton>(2);
            test_assert(!s.is_owned());
            test_int(s->value, 10);

            for (auto i : it) {
                test_assert(it.entity(i) == e[i].value);
                count ++;
            }
        });

    auto 
    e = ecs.entity(); e.set<Entity>({e});
    e = ecs.entity(); e.set<Entity>({e});
    e = ecs.entity(); e.set<Entity>({e});

    s.run();
    
    test_int(count, 3);
}
