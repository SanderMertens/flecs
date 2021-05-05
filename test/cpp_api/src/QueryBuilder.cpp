#include <cpp_api.h>

void QueryBuilder_builder_assign_same_type() {
    flecs::world ecs;

    flecs::query<Position, Velocity> q = 
        ecs.query_builder<Position, Velocity>();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_assign_to_empty() {
    flecs::world ecs;

    flecs::query<> q = ecs.query_builder<Position, Velocity>();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_assign_from_empty() {
    flecs::world ecs;

    flecs::query<> q = ecs.query_builder<>()
        .term<Position>()
        .term<Velocity>();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_build() {
    flecs::world ecs;

    flecs::query<Position, Velocity> q = 
        ecs.query_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_build_to_auto() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_1_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>().build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_1_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term<Position>()
        .build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_2_types() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term<Position>()
        .term<Velocity>()
        .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_1_type_w_1_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>()
        .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_2_types_w_1_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>()
        .term<Mass>()
        .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>().add<Mass>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_pair() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<>()
        .term(Likes, Bob)
        .build();

    auto e1 = ecs.entity().add(Likes, Bob);
    ecs.entity().add(Likes, Alice);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_not() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>().oper(flecs::Not)
        .build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_or() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term<Position>().oper(flecs::Or)
        .term<Velocity>().oper(flecs::Or)
        .build();

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();
    ecs.entity().add<Mass>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1 || e == e2);
    });
    
    test_int(count, 2);
}

void QueryBuilder_add_optional() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term<Position>()
        .term<Velocity>().oper(flecs::Optional)
        .build();

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>().add<Mass>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1 || e == e2);
    });
    
    test_int(count, 2);
}

void QueryBuilder_ptr_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position, Velocity*>().build();

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>().add<Mass>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity* v) {
        count ++;
        test_assert(e == e1 || e == e2);
    });
    
    test_int(count, 2);
}

void QueryBuilder_const_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<const Position>().build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, const Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}
