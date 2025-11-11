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

void OrderedChildren_prefab_children_with_is_a(void) {
    flecs::world world;

    struct Id { uint32_t value; };

    flecs::entity child_type_a = world.prefab("TypeA");
    flecs::entity child_type_b = world.prefab("TypeB");

    flecs::entity parent_prefab = world.prefab("parent")
        .add(flecs::OrderedChildren);

    world.prefab()
        .is_a(child_type_a)
        .child_of(parent_prefab)
        .set<Id>({0});

    world.prefab()
        .is_a(child_type_b)
        .child_of(parent_prefab)
        .set<Id>({1});

    world.prefab()
        .is_a(child_type_a)
        .child_of(parent_prefab)
        .set<Id>({2});

    flecs::entity instance = world.entity("instance")
        .is_a(parent_prefab)
        .add(flecs::OrderedChildren);

    std::vector<uint32_t> ids;
    instance.children([&](flecs::entity child) {
        const Id* id = child.get<Id>();
        test_assert(id != nullptr);
        ids.push_back(id->value);
    });

    test_int(ids.size(), 3);
    test_int(ids[0], 0);
    test_int(ids[1], 1);
    test_int(ids[2], 2);
}
