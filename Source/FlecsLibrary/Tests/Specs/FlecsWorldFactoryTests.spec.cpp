
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "flecs.h"

#include "Bake/FlecsTestUtils.h"
#include "Bake/FlecsTestTypes.h"

class MyModule {
public:
    MyModule(flecs::world& ecs) {
        ecs.module<MyModule>();
        ecs.component<Position>();
    }
};

BEGIN_DEFINE_SPEC(FFlecsWorldFactoryTestsSpec,
                  "FlecsLibrary.WorldFactory",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

void WorldFactory_entity(void) {
    flecs::world ecs;

    auto e = ecs.entity();
    test_assert(e.id() != 0);
}

void WorldFactory_entity_w_name(void) {
    flecs::world ecs;

    auto e = ecs.entity("MyName");
    test_assert(e.id() != 0);
    test_str(e.name().c_str(), "MyName");
}

void WorldFactory_entity_w_id(void) {
    flecs::world ecs;

    auto e = ecs.entity(100);
    test_assert(e.id() == 100);
}

void WorldFactory_prefab(void) {
    flecs::world ecs;

    auto e = ecs.prefab();
    test_assert(e.id() != 0);
    test_assert(e.has(flecs::Prefab));
}

void WorldFactory_prefab_w_name(void) {
    flecs::world ecs;

    auto e = ecs.prefab("MyName");
    test_assert(e.id() != 0);
    test_assert(e.has(flecs::Prefab));
    test_str(e.name().c_str(), "MyName");
}

void WorldFactory_system(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto s = ecs.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    test_assert(s.id() != 0);

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.progress();

    const Position *p = e.try_get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void WorldFactory_system_w_name(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto s = ecs.system<Position, const Velocity>("MySystem")
        .each([](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    test_assert(s.id() != 0);
    test_str(s.name().c_str(), "MySystem");

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.progress();

    const Position *p = e.try_get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void WorldFactory_system_w_expr(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto s = ecs.system<>("MySystem")
        .expr("Position, [in] Velocity")
        .run([](flecs::iter it) {
            while (it.next()) {
                flecs::field<Position> p(it, 0);
                flecs::field<const Velocity> v(it, 1);

                for (auto i : it) {
                    p[i].x += v[i].x;
                    p[i].y += v[i].y;
                }
            }
        });

    test_assert(s.id() != 0);
    test_str(s.name().c_str(), "MySystem");

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.progress();

    const Position *p = e.try_get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void WorldFactory_query(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto q = ecs.query<Position, const Velocity>();

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    q.each([](flecs::entity e, Position& p, const Velocity& v) {
        p.x += v.x;
        p.y += v.y;
    });

    const Position *p = e.try_get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void WorldFactory_query_w_expr(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto q = ecs.query_builder<>().expr("Position, [in] Velocity").build();

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    q.run([](flecs::iter it) {
        while (it.next()) {
            flecs::field<Position> p(it, 0);
            flecs::field<const Velocity> v(it, 1);

            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }
        }
    });

    const Position *p = e.try_get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void WorldFactory_module(void) {
    flecs::world ecs;

    ecs.import<MyModule>();

    auto p = ecs.lookup("MyModule::Position");
    test_assert(p.id() != 0);
}

END_DEFINE_SPEC(FFlecsWorldFactoryTestsSpec);

/*"id": "WorldFactory",
"testcases": [
"entity",
"entity_w_name",
"entity_w_id",
"prefab",
"prefab_w_name",
"system",
"system_w_name",
"system_w_expr",
"query",
"query_w_expr",
"module"
]*/

void FFlecsWorldFactoryTestsSpec::Define()
{
	It("WorldFactory_entity", [this]() { WorldFactory_entity(); });
    It("WorldFactory_entity_w_name", [this]() { WorldFactory_entity_w_name(); });
    It("WorldFactory_entity_w_id", [this]() { WorldFactory_entity_w_id(); });
    It("WorldFactory_prefab", [this]() { WorldFactory_prefab(); });
    It("WorldFactory_prefab_w_name", [this]() { WorldFactory_prefab_w_name(); });
    It("WorldFactory_system", [this]() { WorldFactory_system(); });
    It("WorldFactory_system_w_name", [this]() { WorldFactory_system_w_name(); });
    It("WorldFactory_system_w_expr", [this]() { WorldFactory_system_w_expr(); });
    It("WorldFactory_query", [this]() { WorldFactory_query(); });
    It("WorldFactory_query_w_expr", [this]() { WorldFactory_query_w_expr(); });
    It("WorldFactory_module", [this]() { WorldFactory_module(); });
}

#endif // WITH_AUTOMATION_TESTS
