#include <cpp.h>

void OrderedChildren_iter_no_children(void) {
    flecs::world world;

    flecs::entity parent = world.entity().add(flecs::OrderedChildren);

    int32_t count = 0;
    parent.children([&](flecs::entity) {
        count ++;
    });

    test_int(count, 0);
}

void OrderedChildren_children_1_table(void) {
    flecs::world world;

    flecs::entity parent = world.entity().add(flecs::OrderedChildren);
    flecs::entity child_a = world.entity().child_of(parent).add<Position>();
    flecs::entity child_b = world.entity().child_of(parent).add<Position>();
    flecs::entity child_c = world.entity().child_of(parent).add<Position>();

    std::vector<flecs::entity> v;
    parent.children([&](flecs::entity e) {
        v.push_back(e);
    });

    test_assert(v[0] == child_a);
    test_assert(v[1] == child_b);
    test_assert(v[2] == child_c);
}

void OrderedChildren_children_2_tables(void) {
    flecs::world world;

    flecs::entity parent = world.entity().add(flecs::OrderedChildren);
    flecs::entity child_a = world.entity().child_of(parent).add<Position>();
    flecs::entity child_b = world.entity().child_of(parent).add<Velocity>();
    flecs::entity child_c = world.entity().child_of(parent).add<Position>();

    std::vector<flecs::entity> v;
    parent.children([&](flecs::entity e) {
        v.push_back(e);
    });

    test_assert(v[0] == child_a);
    test_assert(v[1] == child_b);
    test_assert(v[2] == child_c);
}

void OrderedChildren_set_child_order(void) {
    flecs::world world;

    flecs::entity parent = world.entity().add(flecs::OrderedChildren);
    flecs::entity child_a = world.entity().child_of(parent).add<Position>();
    flecs::entity child_b = world.entity().child_of(parent).add<Position>();
    flecs::entity child_c = world.entity().child_of(parent).add<Position>();

    {
        std::vector<flecs::entity> v;
        parent.children([&](flecs::entity e) {
            v.push_back(e);
        });

        test_assert(v[0] == child_a);
        test_assert(v[1] == child_b);
        test_assert(v[2] == child_c);
    }

    flecs::entity_t entities[] = {child_c, child_a, child_b};
    parent.set_child_order(entities, 3);

    {
        std::vector<flecs::entity> v;
        parent.children([&](flecs::entity e) {
            v.push_back(e);
        });

        test_assert(v[0] == child_c);
        test_assert(v[1] == child_a);
        test_assert(v[2] == child_b);
    }
}
