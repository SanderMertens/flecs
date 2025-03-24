#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "Misc/AutomationTest.h"
#include "flecs.h"
#include "Bake/FlecsTestUtils.h"
#include "Bake/FlecsTestTypes.h"

struct Movement { };
struct Standing { };
struct Walking { };

enum Color {
    Red,
    Green,
    Blue
};

BEGIN_DEFINE_SPEC(FFlecsUnionTestsSpec,
                  "FlecsLibrary.Union",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

void Union_add_case(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto Standing = world.entity("Standing");
    auto Walking = world.entity("Walking");
    auto Movement = world.entity().add(flecs::Union);

    auto e = world.entity()
        .add(Movement, Standing);
    test_assert(e.has(Movement, Standing));

    auto table = e.table();

    e.add(Movement, Walking);
    test_assert(e.table() == table);

    test_assert(e.has(Movement, Walking));
    test_assert(!e.has(Movement, Standing));
}

void Union_get_case(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto Standing = world.entity("Standing");
    world.entity("Walking");
    auto Movement = world.entity().add(flecs::Union);

    auto e = world.entity()
        .add(Movement, Standing);
    test_assert(e.has(Movement, Standing));

    test_assert(e.target(Movement) == Standing);
}

void Union_add_case_w_type(void) {
    flecs::world world;
    world.component<Movement>().add(flecs::Union);
    world.component<Standing>();
    world.component<Walking>();

    auto e = world.entity().add<Movement, Standing>();
    test_assert((e.has<Movement, Standing>()));

    e.add<Movement, Walking>();

    test_assert((e.has<Movement, Walking>()));
    test_assert((!e.has<Movement, Standing>()));
}

void Union_add_switch_w_type(void) {
    flecs::world world;
    world.component<Movement>().add(flecs::Union);
    world.component<Standing>();
    world.component<Walking>();

    auto e = world.entity().add<Movement, Standing>();
    test_assert((e.has<Movement, Standing>()));

    e.add<Movement, Walking>();

    test_assert((e.has<Movement, Walking>()));
    test_assert((!e.has<Movement, Standing>()));
}

void Union_add_remove_switch_w_type(void) {
    flecs::world world;
    world.component<Movement>().add(flecs::Union);
    world.component<Standing>();
    world.component<Walking>();

    auto e = world.entity().add<Movement, Standing>();
    test_assert(e.has<Movement>(flecs::Wildcard));
    test_assert((e.has<Movement, Standing>()));

    auto table = e.table();

    e.add<Movement, Walking>();

    test_assert((e.has<Movement, Walking>()));
    test_assert((!e.has<Movement, Standing>()));
    test_assert(e.table() == table);

    auto c = e.target<Movement>();
    test_assert(c != 0);
    test_assert(c == world.id<Walking>());

    e.remove<Movement>(flecs::Wildcard);
    test_assert(!e.has<Movement>(flecs::Wildcard));
    test_assert((!e.has<Movement, Walking>()));
    test_assert(e.table() != table);
}

void Union_switch_enum_type(void) {
    flecs::world world;
    world.component<Color>().add(flecs::Union);

    auto e = world.entity().add(Red);
    test_assert(e.has(Red));
    test_assert(!e.has(Green));
    test_assert(!e.has(Blue));
    test_assert(e.has<Color>(flecs::Wildcard));

    auto table = e.table();

    e.add(Green);
    test_assert(!e.has(Red));
    test_assert(e.has(Green));
    test_assert(!e.has(Blue));
    test_assert(e.has<Color>(flecs::Wildcard));
    test_assert(e.table() == table);

    e.add(Blue);
    test_assert(!e.has(Red));
    test_assert(!e.has(Green));
    test_assert(e.has(Blue));
    test_assert(e.has<Color>(flecs::Wildcard));
    test_assert(e.table() == table);

    e.remove<Color>(flecs::Wildcard);
    test_assert(!e.has(Red));
    test_assert(!e.has(Green));
    test_assert(!e.has(Blue));
    test_assert(!e.has<Color>(flecs::Wildcard));
    test_assert(e.table() != table);
}

END_DEFINE_SPEC(FFlecsUnionTestsSpec);

/*"id": "Union",
"testcases": [
"add_case",
"get_case",
"add_case_w_type",
"add_switch_w_type",
"add_remove_switch_w_type",
"switch_enum_type"
]*/

void FFlecsUnionTestsSpec::Define()
{
	It("Union_add_case", [this]() { Union_add_case(); });
    It("Union_get_case", [this]() { Union_get_case(); });
    It("Union_add_case_w_type", [this]() { Union_add_case_w_type(); });
    It("Union_add_switch_w_type", [this]() { Union_add_switch_w_type(); });
    It("Union_add_remove_switch_w_type", [this]() { Union_add_remove_switch_w_type(); });
    It("Union_switch_enum_type", [this]() { Union_switch_enum_type(); });
}

#endif // WITH_AUTOMATION_TESTS
