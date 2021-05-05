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

void QueryBuilder_builder_build() {
    // Implement testcase
}

void QueryBuilder_1_type() {
    // Implement testcase
}

void QueryBuilder_2_types() {
    // Implement testcase
}

void QueryBuilder_add_1_type() {
    // Implement testcase
}

void QueryBuilder_add_2_types() {
    // Implement testcase
}

void QueryBuilder_add_1_type_w_1_type() {
    // Implement testcase
}

void QueryBuilder_add_2_types_w_1_type() {
    // Implement testcase
}

void QueryBuilder_add_pair() {
    // Implement testcase
}

void QueryBuilder_add_not() {
    // Implement testcase
}

void QueryBuilder_add_or() {
    // Implement testcase
}

void QueryBuilder_add_optional() {
    // Implement testcase
}

void QueryBuilder_add_in() {
    // Implement testcase
}

void QueryBuilder_add_out() {
    // Implement testcase
}

void QueryBuilder_add_inout() {
    // Implement testcase
}

void QueryBuilder_builder_build_to_auto() {
    // Implement testcase
}

void QueryBuilder_builder_assign_from_empty() {
    // Implement testcase
}
