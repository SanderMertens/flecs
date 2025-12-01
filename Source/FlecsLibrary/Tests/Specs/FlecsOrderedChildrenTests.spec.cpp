// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "Bake/FlecsTestUtils.h"

#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "flecs.h"

#include "Bake/FlecsTestTypes.h"

BEGIN_DEFINE_SPEC(FFlecsOrderedChildrenTestsSpec,
                   "FlecsLibrary.OrderedChildren",
                   EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

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
    world.component<Position>();

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
    RegisterTestTypeComponents(world);

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
    world.component<Position>();

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

END_DEFINE_SPEC(FFlecsOrderedChildrenTestsSpec);

/*{
"id": "OrderedChildren",
"testcases": [
"iter_no_children",
"children_1_table",
"children_2_tables",
"set_child_order"
]*/

void FFlecsOrderedChildrenTestsSpec::Define()
{
    It("iter_no_children", [this]() { OrderedChildren_iter_no_children(); });
    It("children_1_table", [this]() { OrderedChildren_children_1_table(); });
    It("children_2_tables", [this]() { OrderedChildren_children_2_tables(); });
    It("set_child_order", [this]() { OrderedChildren_set_child_order(); });
}




#endif // WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)
