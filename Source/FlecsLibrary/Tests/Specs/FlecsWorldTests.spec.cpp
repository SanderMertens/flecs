
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "flecs.h"

#include "Bake/FlecsTestUtils.h"
#include "Bake/FlecsTestTypes.h"

void World_multi_world_empty(void) {
    flecs::world *w1 = new flecs::world();
    delete w1;
    flecs::world *w2 = new flecs::world();
    delete w2;

    test_assert(true);
}

class FooModule {
public:
    FooModule(flecs::world& world) { 
        world.module<FooModule>();
    }
};

typedef struct TestInteropModule {
    int dummy;
} TestInteropModule;

static
void TestInteropModuleImport(ecs_world_t *world) {
    ECS_MODULE(world, TestInteropModule);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
}

static
void TestInteropModule2Import(ecs_world_t *world) {
    ECS_MODULE(world, TestInteropModule2);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
}

namespace test {
namespace interop {

class module : TestInteropModule {
public:
    struct Velocity : ::Velocity { };

    module(flecs::world& ecs) {
        TestInteropModuleImport(ecs);

        ecs.module<test::interop::module>();
        ecs.component<Position>("::test::interop::module::Position");
        ecs.component<Velocity>("::test::interop::module::Velocity");
    }
};

class module2 {
public:
    struct Velocity : ::Velocity { };

    module2(flecs::world& ecs) {
        TestInteropModule2Import(ecs);

        ecs.module<test::interop::module2>();
        ecs.component<Position>();
        ecs.component<Velocity>();
    }
};

}
}

namespace ns {
    struct FooComp {
        int value;
    };

    struct namespace_module {
        namespace_module(flecs::world& ecs) {
            ecs.module<namespace_module>();

            ecs.component<FooComp>();

            import_count ++;

            ecs.system<FooComp>()
                .kind(flecs::OnUpdate)
                .each([](flecs::entity entity, FooComp &sc) {
                    namespace_module::system_invoke_count ++;
                });
        }

        static int import_count;
        static int system_invoke_count;
    };

    int namespace_module::import_count = 0;
    int namespace_module::system_invoke_count = 0;
}

struct nested_component_module {
    struct Foo {
        struct Bar { };
    };

    nested_component_module(flecs::world& ecs) {
        ecs.component<Foo>();
        ecs.component<Foo::Bar>();
    }
};

namespace A {
    struct Comp {
        float x;
        float y;
    };
}

struct FlecsTestModule { };

template <typename T>
struct FlecsTestTmp { int32_t v; };
struct FlecsTestTest { };

struct FlecsTestParentScope { };

template <typename T>
struct FlecsTestTemplateType { };

namespace ns {
    template <typename T>
    struct FlecsTestTemplateType { };
    struct foo { };
}

namespace foo {
    template <typename T>
    struct foo { };
    struct bar { };
}

struct module_w_template_component {
    struct Foo { };
    struct Bar { };

    template <typename T, typename U>
    struct TypeWithArgs { };

    module_w_template_component(flecs::world &world) {
        world.module<module_w_template_component>();
        world.component<TypeWithArgs<Foo, Bar>>();
    };
};

struct FlecsTestParent {
    struct Child { };
};

struct PositionDerived : Position {
    PositionDerived() { }
    PositionDerived(float x, float y) : Position{x, y} { }
};

struct Outer
{
    struct Inner { };
};

struct TestFlecsScope { };

struct TestFlecsFromScope { };

namespace Nested { 
    struct TestFlecsFromScope { };
}


BEGIN_DEFINE_SPEC(FFlecsWorldTestsSpec,
                  "FlecsLibrary.World",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);


void World_builtin_components(void) {
    flecs::world ecs;

    test_assert(ecs.component<flecs::Component>() == ecs_id(EcsComponent));
    test_assert(ecs.component<flecs::Identifier>() == ecs_id(EcsIdentifier));
    test_assert(ecs.component<flecs::Poly>() == ecs_id(EcsPoly));
    test_assert(ecs.component<flecs::RateFilter>() == ecs_id(EcsRateFilter));
    test_assert(ecs.component<flecs::TickSource>() == ecs_id(EcsTickSource));
    test_assert(flecs::Name == EcsName);
    test_assert(flecs::Symbol == EcsSymbol);
    test_assert(flecs::System == EcsSystem);
    test_assert(flecs::Observer == EcsObserver);
    test_assert(flecs::Query == EcsQuery);
}

void World_multi_world_component(void) {
    flecs::world w1;
    RegisterTestTypeComponents(w1);
    flecs::world w2;
    RegisterTestTypeComponents(w2);

    auto p_1 = w1.component<Position>();
    auto v_1 = w1.component<Velocity>();
    auto v_2 = w2.component<Velocity>();
    auto m_2 = w2.component<Mass>();
    w2.component<Mass>();

    test_assert(p_1 != v_2);
    test_assert(v_1 == v_2);

    flecs::entity w1_e = w1.entity()
        .set(Position{10, 20})
        .set(Velocity{1, 2})
        .set(Mass{100});

    flecs::entity w2_e = w2.entity()
        .set(Position{10, 20})
        .set(Velocity{1, 2})
        .set(Mass{100});

    {
        const Position *p = w1_e.try_get<Position>();
        test_assert(p != nullptr);
        test_int(p->x, 10);
        test_int(p->y, 20);

        const Velocity *v = w1_e.try_get<Velocity>();
        test_assert(v != nullptr);
        test_int(v->x, 1);
        test_int(v->y, 2);

        const Mass *m = w1_e.try_get<Mass>();
        test_assert(m != nullptr);
        test_int(m->value, 100);
    }

    {
        const Position *p = w2_e.try_get<Position>();
        test_assert(p != nullptr);
        test_int(p->x, 10);
        test_int(p->y, 20);

        const Velocity *v = w2_e.try_get<Velocity>();
        test_assert(v != nullptr);
        test_int(v->x, 1);
        test_int(v->y, 2);

        const Mass *m = w2_e.try_get<Mass>();
        test_assert(m != nullptr);
        test_int(m->value, 100);
    }
}

void World_multi_world_component_namespace(void) {
    flecs::world *w = new flecs::world();
    auto c = w->component<A::Comp>();
    auto id_1 = c.id();
    delete w;

    w = new flecs::world();
    c = w->component<A::Comp>();
    auto id_2 = c.id();

    test_assert(id_1 == id_2);

    delete w;
}

void World_multi_world_module(void) {
    flecs::world world1;
    ns::namespace_module::system_invoke_count = 0;
    
    world1.import<ns::namespace_module>();

    flecs::world world2;
    world2.import<ns::namespace_module>();

    world1.entity().add<ns::FooComp>();
    world2.entity().add<ns::FooComp>();

    world1.progress();
    test_int(ns::namespace_module::system_invoke_count, 1);

    world2.progress();
    test_int(ns::namespace_module::system_invoke_count, 2);
}

void World_multi_world_recycled_component_different_generation(void) {
    flecs::entity c;
    {
        flecs::world ecs;
        for (int i = 0; i < FLECS_HI_COMPONENT_ID; i ++) {
            ecs_new_low_id(ecs);
        }

        ecs.entity().destruct();
        c = ecs.component<Position>();
    }
    {
        flecs::world ecs;
        for (int i = 0; i < FLECS_HI_COMPONENT_ID; i ++) {
            ecs_new_low_id(ecs);
        }

        ecs.entity().destruct();
        test_assert((c == ecs.component<Position>()));
    }
}

void World_type_id(void) {
    flecs::world ecs;

    auto p = ecs.component<Position>();

    test_assert(p.id() == ecs.id<Position>());
}

void World_reregister_after_reset(void) {
    flecs::world ecs;

    auto p1 = ecs.component<Position>("Position");

    // Simulate different binary
    flecs::_::type<Position>::reset();

    auto p2 = ecs.component<Position>("Position");

    test_assert(p1.id() == p2.id());
}

void World_implicit_reregister_after_reset(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    ecs.entity().add<Position>();

    flecs::entity_t p_id_1 = ecs.id<Position>();

    // Simulate different binary
    flecs::_::type<Position>::reset();

    ecs.entity().add<Position>();

    flecs::entity_t p_id_2 = ecs.id<Position>();

    test_assert(p_id_1 == p_id_2);
}

void World_reregister_after_reset_w_namespace(void) {
    flecs::world ecs;

    ecs.component<ns::FooComp>();

    flecs::entity_t p_id_1 = ecs.id<ns::FooComp>();

    // Simulate different binary
    flecs::_::type<ns::FooComp>::reset();

    ecs.component<ns::FooComp>();

    flecs::entity_t p_id_2 = ecs.id<ns::FooComp>();

    test_assert(p_id_1 == p_id_2);
}

void World_reregister_namespace(void) {
    flecs::world ecs;

    ecs.component<ns::FooComp>();

    flecs::entity_t p_id_1 = ecs.id<ns::FooComp>();

    ecs.component<ns::FooComp>();

    flecs::entity_t p_id_2 = ecs.id<ns::FooComp>();

    test_assert(p_id_1 == p_id_2);
}

void World_reregister_after_delete(void) {
    flecs::world ecs;

    auto c = ecs.component<Position>();
    test_str(c.name(), "Position");
    test_str(c.path(), "::Position");
    test_str(c.symbol(), "Position");

    c.destruct();

    test_assert(!c.is_alive());

    auto d = ecs.component<Position>();
    test_assert(!c.is_alive());
    test_assert(d.is_alive());

    test_str(d.name(), "Position");
    test_str(d.path(), "::Position");
    test_str(d.symbol(), "Position");
}

void World_register_component_w_reset_in_multithreaded(void) {
    flecs::world ecs;

    ecs.set_threads(2);

    flecs::entity pos = ecs.component<Position>();
    flecs::entity e = ecs.entity();

    flecs::_::type<Position>::reset();

    ecs.readonly_begin();
    e.set<Position>({10, 20});
    ecs.readonly_end();

    test_assert(e.has<Position>());
    test_assert(e.has(pos));
    const Position *p = e.try_get<Position>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void World_register_short_template(void) {
    flecs::world ecs;

    auto c = ecs.component<FlecsTestTmp<FlecsTestTest>>();
    flecs::string name = c.name();
    test_assert(c != 0);
    test_str(name, "FlecsTestTmp<FlecsTestTest>");

    const EcsComponent *ptr = c.try_get<flecs::Component>();
    test_assert(ptr != NULL);
    test_int(ptr->size, 4);
    test_int(ptr->alignment, 4);
}

void World_reimport(void) {
    flecs::world ecs;

    auto m1 = ecs.import<FooModule>();

    auto m2 = ecs.import<FooModule>();
    
    test_assert(m1.id() == m2.id());
}

void World_reimport_module_after_reset(void) {
    flecs::world ecs;

    auto m1 = ecs.import<FooModule>();

    // Simulate different binary
    flecs::_::type<FooModule>::reset();

    auto m2 = ecs.import<FooModule>();
    
    test_assert(m1.id() == m2.id());
}

void World_reimport_module_new_world(void) {
    flecs::entity e1;
    {
        flecs::world ecs;

        e1 = ecs.import<FooModule>();
    }

    {
        flecs::world ecs;

        auto e2 = ecs.import<FooModule>();
        
        test_assert(e1.id() == e2.id());
    }
}

void World_reimport_namespaced_module(void) {
    flecs::world ecs;

    ns::namespace_module::import_count = 0;

    test_int(ns::namespace_module::import_count, 0);

    // Import first time, should call module constructor.
    ecs.import<ns::namespace_module>();

    test_int(ns::namespace_module::import_count, 1);

    // Import second time, should not call constructor. 
    ecs.import<ns::namespace_module>();

    test_int(ns::namespace_module::import_count, 1);
}

void World_c_interop_module(void) {
    flecs::world ecs;

    ecs.import<test::interop::module>();

    auto e_pos = ecs.lookup("test::interop::module::Position");
    test_assert(e_pos.id() != 0);
}

void World_c_interop_after_reset(void) {
    flecs::world ecs;

    ecs.import<test::interop::module>();

    auto e_pos = ecs.lookup("test::interop::module::Position");
    test_assert(e_pos.id() != 0);

    flecs::_::type<test::interop::module>::reset();

    ecs.import<test::interop::module>();
}

void World_c_interop_module_no_names(void) {
    flecs::world ecs;

    ecs.import<test::interop::module2>();

    auto e_pos = ecs.lookup("test::interop::module2::Position");
    test_assert(e_pos.id() != 0);
}

void World_implicit_register_w_new_world(void) {
    {
        flecs::world ecs;
        ecs.component<Position>();

        auto e = ecs.entity().set<Position>({10, 20});
        test_assert(e.has<Position>());
        auto *p = e.try_get<Position>();
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        /* Recreate world, does not reset static state */
        flecs::world ecs;
        ecs.component<Position>();

        auto e = ecs.entity().set<Position>({10, 20});
        test_assert(e.has<Position>());
        auto *p = e.try_get<Position>();
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }    
}

void World_implicit_register_after_reset_register_w_custom_name(void) {
    flecs::world ecs;

    flecs::entity c = ecs.component<Position>("MyPosition");
    test_str(c.name(), "MyPosition");

    flecs::_::type<Position>().reset(); // Simulate working across boundary

    auto e = ecs.entity().add<Position>();
    test_assert(e.has<Position>());
    test_assert(e.has(c));
}

void World_register_after_reset_register_w_custom_name(void) {
    flecs::world ecs;

    flecs::entity c1 = ecs.component<Position>("MyPosition");
    test_str(c1.name(), "MyPosition");

    flecs::_::type<Position>().reset(); // Simulate working across boundary

    flecs::entity c2 = ecs.component<Position>();
    test_str(c2.name(), "MyPosition");
}

void World_register_builtin_after_reset(void) {
    flecs::world ecs;

    auto c1 = ecs.component<flecs::Component>();
    test_assert(c1 == ecs_id(EcsComponent));

    flecs::_::type<flecs::Component>().reset(); // Simulate working across boundary

    auto c2 = ecs.component<flecs::Component>();
    test_assert(c2 == ecs_id(EcsComponent));
    test_assert(c1 == c2);
}

void World_register_meta_after_reset(void) {
    flecs::world ecs;

    auto c1 = ecs.component<Position>();

    flecs::_::type<Position>().reset(); // Simulate working across boundary

    auto c2 = ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    test_assert(c1 == c2);
}

void World_count(void) {
    flecs::world ecs;
    ecs.component<Position>();
    ecs.component<Velocity>();
    ecs.component<Mass>();

    test_int(ecs.count<Position>(), 0);

    ecs.entity().add<Position>();
    ecs.entity().add<Position>();
    ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Mass>();
    ecs.entity().add<Position>().add<Mass>();
    ecs.entity().add<Position>().add<Velocity>();

    test_int(ecs.count<Position>(), 6);
}

void World_count_id(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto ent = ecs.entity();

    test_int(ecs.count(ent), 0);

    ecs.entity().add(ent);
    ecs.entity().add(ent);
    ecs.entity().add(ent);
    ecs.entity().add(ent).add<Mass>();
    ecs.entity().add(ent).add<Mass>();
    ecs.entity().add(ent).add<Velocity>();

    test_int(ecs.count(ent), 6);
}


void World_count_pair(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto parent = ecs.entity();

    test_int(ecs.count(flecs::ChildOf, parent), 0);

    ecs.entity().add(flecs::ChildOf, parent);
    ecs.entity().add(flecs::ChildOf, parent);
    ecs.entity().add(flecs::ChildOf, parent);
    ecs.entity().add(flecs::ChildOf, parent).add<Mass>();
    ecs.entity().add(flecs::ChildOf, parent).add<Mass>();
    ecs.entity().add(flecs::ChildOf, parent).add<Velocity>();

    test_int(ecs.count(flecs::ChildOf, parent), 6);
}

void World_count_pair_type_id(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    struct Rel { };
    ecs.component<Rel>();

    auto parent = ecs.entity();

    test_int(ecs.count<Rel>(parent), 0);

    ecs.entity().add<Rel>(parent);
    ecs.entity().add<Rel>(parent);
    ecs.entity().add<Rel>(parent);
    ecs.entity().add<Rel>(parent).add<Mass>();
    ecs.entity().add<Rel>(parent).add<Mass>();
    ecs.entity().add<Rel>(parent).add<Velocity>();

    test_int(ecs.count<Rel>(parent), 6);
}

void World_count_pair_id(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    struct Rel { };

    auto rel = ecs.entity();
    auto parent = ecs.entity();

    test_int(ecs.count(rel, parent), 0);

    ecs.entity().add(rel, parent);
    ecs.entity().add(rel, parent);
    ecs.entity().add(rel, parent);
    ecs.entity().add(rel, parent).add<Mass>();
    ecs.entity().add(rel, parent).add<Mass>();
    ecs.entity().add(rel, parent).add<Velocity>();

    test_int(ecs.count(rel, parent), 6);
}

void World_staged_count(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    flecs::world stage = ecs.get_stage(0);

    ecs.readonly_begin();

    test_int(stage.count<Position>(), 0);

    ecs.readonly_end();

    ecs.readonly_begin();

    stage.entity().add<Position>();
    stage.entity().add<Position>();
    stage.entity().add<Position>();
    stage.entity().add<Position>().add<Mass>();
    stage.entity().add<Position>().add<Mass>();
    stage.entity().add<Position>().add<Velocity>();

    test_int(stage.count<Position>(), 0);

    ecs.readonly_end();

    test_int(stage.count<Position>(), 6);
}

void World_async_stage_add(void) {
    flecs::world ecs;

    ecs.component<Position>();

    auto e = ecs.entity();

    flecs::world async = ecs.async_stage();
    e.mut(async).add<Position>();
    test_assert(!e.has<Position>());
    async.merge();
    test_assert(e.has<Position>());
}

void World_with_tag(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto Tag = ecs.entity();

    ecs.with(Tag, [&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get any contents from the with.
    auto self = ecs.component<Self>();
    test_assert(!self.has(Tag));    

    auto q = ecs.query_builder<>().with(Tag).build();

    int32_t count = 0;

    q.each([&](flecs::entity e) {
        test_assert(e.has(Tag));

        test_bool(e.get([&](const Self& s) {
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    count ++;
}

void World_with_tag_type(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    struct Tag { };
    ecs.component<Tag>();

    ecs.with<Tag>([&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get any contents from the with.
    auto self = ecs.component<Self>();
    test_assert(!self.has<Tag>());

    auto q = ecs.query_builder<>().with<Tag>().build();

    int32_t count = 0;

    q.each([&](flecs::entity e) {
        test_assert(e.has<Tag>());

        test_bool(e.get([&](const Self& s) {
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    count ++;
}

void World_with_relation(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();

    ecs.with(Likes, Bob, [&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get any contents from the with.
    auto self = ecs.component<Self>();
    test_assert(!self.has(Likes, Bob));

    auto q = ecs.query_builder<>().with(Likes, Bob).build();

    int32_t count = 0;

    q.each([&](flecs::entity e) {
        test_assert(e.has(Likes, Bob));

        test_bool(e.get([&](const Self& s) {
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    count ++;
}

void World_with_relation_type(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    struct Likes { };
    ecs.component<Likes>();
    
    auto Bob = ecs.entity();

    ecs.with<Likes>(Bob, [&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get any contents from the with.
    auto self = ecs.component<Self>();
    test_assert(!self.has<Likes>(Bob));

    auto q = ecs.query_builder<>().with<Likes>(Bob).build();

    int32_t count = 0;

    q.each([&](flecs::entity e) {
        test_assert(e.has<Likes>(Bob));

        test_bool(e.get([&](const Self& s) {
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    count ++;
}

void World_with_relation_object_type(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    struct Likes { };
    struct Bob { };
    ecs.component<Likes>();
    ecs.component<Bob>();

    ecs.with<Likes, Bob>([&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get any contents from the with.
    auto self = ecs.component<Self>();
    test_assert(!(self.has<Likes, Bob>()));

    auto q = ecs.query_builder<>().with<Likes, Bob>().build();

    int32_t count = 0;

    q.each([&](flecs::entity e) {
        test_assert((e.has<Likes, Bob>()));

        test_bool(e.get([&](const Self& s) {
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    count ++;
}

void World_with_scope(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto parent = ecs.entity("P");

    ecs.scope(parent, [&]{
        auto e1 = ecs.entity("C1"); e1.set<Self>({e1});
        auto e2 = ecs.entity("C2"); e2.set<Self>({e2});
        auto e3 = ecs.entity("C3"); e3.set<Self>({e3});

        // Ensure relative lookups work
        test_assert(ecs.lookup("C1") == e1);
        test_assert(ecs.lookup("C2") == e2);
        test_assert(ecs.lookup("C3") == e3);

        test_assert(parent.lookup("C1") == e1);
        test_assert(parent.lookup("C2") == e2);
        test_assert(parent.lookup("C3") == e3);

        test_assert(ecs.lookup("::P::C1") == e1);
        test_assert(ecs.lookup("::P::C2") == e2);
        test_assert(ecs.lookup("::P::C3") == e3);
    });

    test_assert(parent.lookup("C1") != 0);
    test_assert(parent.lookup("C2") != 0);
    test_assert(parent.lookup("C3") != 0);

    test_assert(ecs.lookup("P::C1") == parent.lookup("C1"));
    test_assert(ecs.lookup("P::C2") == parent.lookup("C2"));
    test_assert(ecs.lookup("P::C3") == parent.lookup("C3"));

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not become a child of the parent.
    auto self = ecs.component<Self>();
    test_assert(!self.has(flecs::ChildOf, parent));

    int count = 0;
    auto q = ecs.query_builder<>().with(flecs::ChildOf, parent).build();

    q.each([&](flecs::entity e) {
        test_assert(e.has(flecs::ChildOf, parent));

        test_bool(e.get([&](const Self& s){
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    test_int(count, 3);
}

void World_with_scope_type(void) {
    flecs::world ecs;
    ecs.component<FlecsTestParentScope>();

    ecs.scope<FlecsTestParentScope>([&]{
        ecs.entity("Child");
    });

    auto parent = ecs.lookup("FlecsTestParentScope");
    test_assert(parent != 0);

    auto child = ecs.lookup("FlecsTestParentScope::Child");
    test_assert(child != 0);
    test_assert(child == parent.lookup("Child"));
}

void World_with_scope_type_staged(void) {
    flecs::world ecs;
    ecs.component<FlecsTestParentScope>();

    flecs::entity e;
    flecs::world stage = ecs.get_stage(0);
    
    ecs.readonly_begin();
    stage.scope<FlecsTestParentScope>([&]{
        e = stage.entity("Child");
    });
    ecs.readonly_end();

    test_assert( e.has(flecs::ChildOf, ecs.id<FlecsTestParentScope>()) );

    auto parent = ecs.lookup("FlecsTestParentScope");
    test_assert(parent != 0);

    auto child = ecs.lookup("FlecsTestParentScope::Child");
    test_assert(child != 0);
    test_assert(child == parent.lookup("Child"));
}

void World_with_scope_no_lambda(void) {
    flecs::world ecs;

    auto parent = ecs.entity("Parent");
    auto child = ecs.scope(parent).entity("Child");

    test_assert(child.has(flecs::ChildOf, parent));
    test_assert(ecs.get_scope() == 0);
}

void World_with_scope_type_no_lambda(void) {
    flecs::world ecs;
    ecs.component<FlecsTestParentScope>();

    auto child = ecs.scope<FlecsTestParentScope>().entity("Child");

    test_assert(child.has(flecs::ChildOf, ecs.id<FlecsTestParentScope>()));
    test_assert(ecs.get_scope() == 0);
}

void World_with_tag_nested(void) {
    flecs::world ecs;

    auto Tier1 = ecs.entity();
    
    ecs.with(Tier1, [&]{
        ecs.entity("Tier2").with([&]{
            ecs.entity("Tier3");
        });
    });

    auto Tier2 = ecs.lookup("Tier2");
    test_assert(Tier2 != 0);

    auto Tier3 = ecs.lookup("Tier3");
    test_assert(Tier3 != 0);

    test_assert(Tier2.has(Tier1));
    test_assert(Tier3.has(Tier2));
}

void World_with_scope_nested(void) {
    flecs::world ecs;

    auto parent = ecs.entity("P");

    ecs.scope(parent, [&]{
        auto child = ecs.entity("C").scope([&]{
            auto gchild = ecs.entity("GC");
            test_assert(gchild == ecs.lookup("GC"));
            test_assert(gchild == ecs.lookup("::P::C::GC"));
        });

        // Ensure relative lookups work
        test_assert(ecs.lookup("C") == child);
        test_assert(ecs.lookup("::P::C") == child);
        test_assert(ecs.lookup("::P::C::GC") != 0);
    });

    test_assert(0 == ecs.lookup("C"));
    test_assert(0 == ecs.lookup("GC"));
    test_assert(0 == ecs.lookup("C::GC"));

    auto child = ecs.lookup("P::C");
    test_assert(0 != child);
    test_assert(child.has(flecs::ChildOf, parent));

    auto gchild = ecs.lookup("P::C::GC");
    test_assert(0 != gchild);
    test_assert(gchild.has(flecs::ChildOf, child));
}

void World_recursive_lookup(void) {
    flecs::world ecs;

    auto A = ecs.entity("A");
    auto B = ecs.entity("B");

    auto P = ecs.entity("P");
    P.scope([&]{
        auto CA = ecs.entity("A");
        test_assert(CA != A);

        test_assert(CA == ecs.lookup("A"));
        test_assert(CA == ecs.lookup("P::A"));
        test_assert(CA == ecs.lookup("::P::A"));
        test_assert(A == ecs.lookup("::A"));

        test_assert(B == ecs.lookup("B"));
        test_assert(B == ecs.lookup("::B"));
    });
}

void World_type_w_tag_name(void) {
    flecs::world ecs;

    auto c = ecs.component<Tag>();
    test_assert(c != flecs::entity());
    test_str(c.path().c_str(), "::Tag");
    test_assert(c != flecs::PairIsTag);
}

void World_entity_w_tag_name(void) {
    flecs::world ecs;

    auto c = ecs.entity("Tag");
    test_assert(c != flecs::entity());
    test_str(c.path().c_str(), "::Tag");
    test_assert(c != flecs::PairIsTag);
}

void World_template_component_name(void) {
    flecs::world ecs;

    auto c = ecs.component<FlecsTestTemplateType<Position>>();
    test_str(c.name().c_str(), "FlecsTestTemplateType<Position>");
    test_str(c.path().c_str(), "::FlecsTestTemplateType<Position>");
}

void World_template_component_w_namespace_name(void) {
    flecs::world ecs;

    auto c = ecs.component<ns::FlecsTestTemplateType<Position>>();
    test_str(c.name().c_str(), "FlecsTestTemplateType<Position>");
    test_str(c.path().c_str(), "::ns::FlecsTestTemplateType<Position>");
}

void World_template_component_w_namespace_name_and_namespaced_arg(void) {
    flecs::world ecs;

    auto c = ecs.component<ns::FlecsTestTemplateType<ns::foo>>();
    test_str(c.name().c_str(), "FlecsTestTemplateType<ns::foo>");
    test_str(c.path().c_str(), "::ns::FlecsTestTemplateType<ns::foo>");
}

void World_template_component_from_module_2_args(void) {
    flecs::world ecs;

    auto m = ecs.import<module_w_template_component>();
    test_assert(m == ecs.lookup("module_w_template_component"));

    auto tid = ecs.id<module_w_template_component::TypeWithArgs<
        module_w_template_component::Foo,
        module_w_template_component::Bar>>();
    test_assert(tid != 0);

    auto mid = m.lookup("TypeWithArgs<module_w_template_component::Foo, module_w_template_component::Bar>");
    if (mid == 0) {
        mid = m.lookup("TypeWithArgs<module_w_template_component::Foo,module_w_template_component::Bar>");
    }
    test_assert(mid != 0);
    test_assert(tid == mid);
}

void World_entity_as_tag(void) {
    flecs::world ecs;

    auto e = ecs.entity<Tag>();
    test_assert(e.id() != 0);

    auto t = ecs.component<Tag>();
    test_assert(t.id() != 0);
    test_assert(e == t);

    auto e2 = ecs.entity()
        .add<Tag>();

    test_bool(e2.has<Tag>(), true);
    test_bool(e2.has(e), true);

    test_str(e.name(), "Tag");
}

void World_entity_w_name_as_tag(void) {
    flecs::world ecs;

    auto e = ecs.entity<Tag>("Foo");
    test_assert(e.id() != 0);

    auto t = ecs.component<Tag>();
    test_assert(t.id() != 0);
    test_assert(e == t);

    auto e2 = ecs.entity()
        .add<Tag>();

    test_bool(e2.has<Tag>(), true);
    test_bool(e2.has(e), true);

    test_str(e.name(), "Foo");
}

void World_entity_as_component(void) {
    flecs::world ecs;

    auto e = ecs.entity<Position>();
    test_assert(e.id() != 0);

    auto t = ecs.component<Position>();
    test_assert(t.id() != 0);
    test_assert(e == t);

    auto e2 = ecs.entity()
        .set<Position>({10, 20});

    test_bool(e2.has<Position>(), true);
    test_bool(e2.has(e), true);

    test_str(e.name(), "Position");
}

void World_entity_w_name_as_component(void) {
    flecs::world ecs;

    auto e = ecs.entity<Position>("Foo");
    test_assert(e.id() != 0);

    auto t = ecs.component<Position>();
    test_assert(t.id() != 0);
    test_assert(e == t);

    auto e2 = ecs.entity()
        .set<Position>({10, 20});

    test_bool(e2.has<Position>(), true);
    test_bool(e2.has(e), true);

    test_str(e.name(), "Foo");
}

void World_entity_as_component_2_worlds(void) {
    flecs::world ecs_1;
    auto e_1 = ecs_1.entity<Position>();
    test_assert(e_1.id() != 0);

    flecs::world ecs_2;
    auto e_2 = ecs_2.entity<Position>();
    test_assert(e_2.id() != 0);

    test_assert(e_1 == e_2);
    test_assert(e_1 == ecs_1.component<Position>());
    test_assert(e_2 == ecs_2.component<Position>());
}

void World_entity_as_namespaced_component_2_worlds(void) {
    flecs::world ecs_1;
    auto e_1 = ecs_1.entity<FlecsTestParent>();
    test_assert(e_1.id() != 0);

    auto e_1_1 = ecs_1.entity<FlecsTestParent::Child>();
    test_assert(e_1_1.id() != 0);

    flecs::world ecs_2;
    auto e_2 = ecs_2.entity<FlecsTestParent>();
    test_assert(e_2.id() != 0);

    auto e_2_1 = ecs_2.entity<FlecsTestParent::Child>();
    test_assert(e_2_1.id() != 0);

    test_assert(e_1 == e_2);
    test_assert(e_1 == ecs_1.component<FlecsTestParent>());
    test_assert(e_2 == ecs_2.component<FlecsTestParent>());

    test_assert(e_1_1 == e_2_1);
    test_assert(e_1_1 == ecs_1.component<FlecsTestParent::Child>());
    test_assert(e_2_1 == ecs_2.component<FlecsTestParent::Child>());
}

void World_delete_with_id(void) {
    flecs::world ecs;

    flecs::id tag = ecs.entity();
    auto e_1 = ecs.entity().add(tag);
    auto e_2 = ecs.entity().add(tag);
    auto e_3 = ecs.entity().add(tag);

    ecs.delete_with(tag);

    test_assert(!e_1.is_alive());
    test_assert(!e_2.is_alive());
    test_assert(!e_3.is_alive());
}

void World_delete_with_type(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto e_1 = ecs.entity().add<Tag>();
    auto e_2 = ecs.entity().add<Tag>();
    auto e_3 = ecs.entity().add<Tag>();

    ecs.delete_with<Tag>();

    test_assert(!e_1.is_alive());
    test_assert(!e_2.is_alive());
    test_assert(!e_3.is_alive());
}

void World_delete_with_pair(void) {
    flecs::world ecs;

    flecs::id rel = ecs.entity();
    flecs::id obj = ecs.entity();
    auto e_1 = ecs.entity().add(rel, obj);
    auto e_2 = ecs.entity().add(rel, obj);
    auto e_3 = ecs.entity().add(rel, obj);

    ecs.delete_with(rel, obj);

    test_assert(!e_1.is_alive());
    test_assert(!e_2.is_alive());
    test_assert(!e_3.is_alive());
}

void World_delete_with_pair_type(void) {
    flecs::world ecs;

    struct Rel { };
    struct Obj { };
    ecs.component<Rel>();
    ecs.component<Obj>();

    auto e_1 = ecs.entity().add<Rel, Obj>();
    auto e_2 = ecs.entity().add<Rel, Obj>();
    auto e_3 = ecs.entity().add<Rel, Obj>();

    ecs.delete_with<Rel, Obj>();

    test_assert(!e_1.is_alive());
    test_assert(!e_2.is_alive());
    test_assert(!e_3.is_alive());
}

void World_delete_with_implicit(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    ecs.delete_with<Tag>();

    test_assert(true);
}

void World_delete_with_pair_implicit(void) {
    flecs::world ecs;

    struct Rel { };
    struct Obj { };
    ecs.component<Rel>();
    ecs.component<Obj>();

    ecs.delete_with<Rel, Obj>();

    test_assert(true);
}

void World_remove_all_id(void) {
    flecs::world ecs;

    flecs::id tag_a = ecs.entity();
    flecs::id tag_b = ecs.entity();
    auto e_1 = ecs.entity().add(tag_a);
    auto e_2 = ecs.entity().add(tag_a);
    auto e_3 = ecs.entity().add(tag_a).add(tag_b);

    ecs.remove_all(tag_a);

    test_assert(e_1.is_alive());
    test_assert(e_2.is_alive());
    test_assert(e_3.is_alive());

    test_assert(!e_1.has(tag_a));
    test_assert(!e_2.has(tag_a));
    test_assert(!e_3.has(tag_a));
    
    test_assert(e_3.has(tag_b));
}

void World_remove_all_type(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto e_1 = ecs.entity().add<Position>();
    auto e_2 = ecs.entity().add<Position>();
    auto e_3 = ecs.entity().add<Position>().add<Velocity>();

    ecs.remove_all<Position>();

    test_assert(e_1.is_alive());
    test_assert(e_2.is_alive());
    test_assert(e_3.is_alive());

    test_assert(!e_1.has<Position>());
    test_assert(!e_2.has<Position>());
    test_assert(!e_3.has<Position>());
    
    test_assert(e_3.has<Velocity>());
}

void World_remove_all_pair(void) {
    flecs::world ecs;

    flecs::id rel = ecs.entity();
    flecs::id obj_a = ecs.entity();
    flecs::id obj_b = ecs.entity();
    auto e_1 = ecs.entity().add(rel, obj_a);
    auto e_2 = ecs.entity().add(rel, obj_a);
    auto e_3 = ecs.entity().add(rel, obj_a).add(rel, obj_b);

    ecs.remove_all(rel, obj_a);

    test_assert(e_1.is_alive());
    test_assert(e_2.is_alive());
    test_assert(e_3.is_alive());

    test_assert(!e_1.has(rel, obj_a));
    test_assert(!e_2.has(rel, obj_a));
    test_assert(!e_3.has(rel, obj_a));
    
    test_assert(e_3.has(rel, obj_b));
}

void World_remove_all_pair_type(void) {
    flecs::world ecs;

    struct Rel { };
    struct ObjA { };
    struct ObjB { };
    ecs.component<Rel>();
    ecs.component<ObjA>();
    ecs.component<ObjB>();

    auto e_1 = ecs.entity().add<Rel, ObjA>();
    auto e_2 = ecs.entity().add<Rel, ObjA>();
    auto e_3 = ecs.entity().add<Rel, ObjA>().add<Rel, ObjB>();

    ecs.remove_all<Rel, ObjA>();

    test_assert(e_1.is_alive());
    test_assert(e_2.is_alive());
    test_assert(e_3.is_alive());

    test_assert((!e_1.has<Rel, ObjA>()));
    test_assert((!e_2.has<Rel, ObjA>()));
    test_assert((!e_3.has<Rel, ObjA>()));
    
    test_assert((!e_1.has<Rel, ObjB>()));
    test_assert((!e_2.has<Rel, ObjB>()));
    test_assert((e_3.has<Rel, ObjB>()));
}

void World_remove_all_implicit(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    ecs.remove_all<Tag>();

    test_assert(true);
}

void World_remove_all_pair_implicit(void) {
    flecs::world ecs;

    struct Rel { };
    struct Obj { };
    ecs.component<Rel>();
    ecs.component<Obj>();

    ecs.remove_all<Rel, Obj>();

    test_assert(true);
}

void World_get_scope(void) {
    flecs::world ecs;

    auto e = ecs.entity("scope");

    ecs.set_scope(e);

    auto s = ecs.get_scope();
    test_assert(s == e);
    test_str(s.name(), "scope");
}

void World_get_scope_type(void) {
    flecs::world ecs;
    ecs.component<FlecsTestParentScope>();
    
    ecs.set_scope<FlecsTestParentScope>();

    auto s = ecs.get_scope();
    test_assert(s == ecs.id<FlecsTestParentScope>());
    test_str(s.name(), "FlecsTestParentScope");
}

void World_register_namespace_after_component(void) {
    flecs::world ecs;
    auto inn = ecs.component<Outer::Inner>();
    auto out = ecs.component<Outer>();

    test_str(inn.path().c_str(), "::Outer::Inner");
    test_str(out.path().c_str(), "::Outer");

    const char *inn_sym = ecs_get_symbol(ecs, inn);
    const char *out_sym = ecs_get_symbol(ecs, out);

    test_str(inn_sym, "Outer.Inner");
    test_str(out_sym, "Outer");
}

void World_is_alive(void) {
    flecs::world ecs;

    auto e = ecs.entity();

    test_bool(ecs.is_alive(e), true);
    test_bool(ecs.is_alive(1000), false);

    e.destruct();

    test_bool(ecs.is_alive(e), false);
}

void World_is_valid(void) {
    flecs::world ecs;

    auto e = ecs.entity();

    test_bool(ecs.is_valid(e), true);
    test_bool(ecs.is_valid(1000), false);
    test_bool(ecs.is_valid(0), false);

    e.destruct();

    test_bool(ecs.is_valid(e), false);

    ecs.make_alive(1000);

    test_bool(ecs.is_valid(1000), true);
}

void World_exists(void) {
    flecs::world ecs;

    auto e = ecs.entity();

    test_bool(ecs.exists(e), true);
    test_bool(ecs.exists(1000), false);
}

void World_get_alive(void) {
    flecs::world ecs;

    auto e_1 = ecs.entity();
    auto e_no_gen = flecs::strip_generation(e_1);
    test_assert(e_1 == e_no_gen);
    e_1.destruct();

    auto e_2 = ecs.entity();
    test_assert(e_1 != e_2);
    test_assert(e_no_gen == flecs::strip_generation(e_2));

    test_assert(ecs.get_alive(e_no_gen) == e_2);
}

void World_make_alive(void) {
    flecs::world ecs;

    auto e_1 = ecs.entity();
    e_1.destruct();
    test_assert(!e_1.is_alive());

    auto e_2 = ecs.entity();
    test_assert(e_1 != e_2);
    test_assert(e_1 == flecs::strip_generation(e_2));
    e_2.destruct();
    test_assert(!e_2.is_alive());

    auto e_3 = ecs.make_alive(e_2);
    test_assert(e_2 == e_3);
    test_assert(e_3.is_alive());
}

void World_reset_all(void) {

}

void World_get_tick(void) {
    flecs::world ecs;

    test_int(ecs.get_info()->frame_count_total, 0);

    ecs.progress();

    test_int(ecs.get_info()->frame_count_total, 1);

    ecs.progress();

    test_int(ecs.get_info()->frame_count_total, 2);
}

void World_register_from_scope(void) {
    flecs::world ecs;
    ecs.component<TestFlecsScope>();
    
    ecs.set_scope<TestFlecsScope>();
    auto c = ecs.component<TestFlecsFromScope>();
    ecs.set_scope(0);

    test_assert(c.has(flecs::ChildOf, ecs.id<TestFlecsScope>()));
}

void World_register_nested_from_scope(void) {
    flecs::world ecs;
    ecs.component<TestFlecsScope>();
    
    ecs.set_scope<TestFlecsScope>();
    auto c = ecs.component<Nested::TestFlecsFromScope>();
    ecs.set_scope(0);

    test_assert(c.has(flecs::ChildOf, ecs.id<TestFlecsScope>()));
}

void World_register_w_root_name(void) {
    flecs::world ecs;

    auto c = ecs.component<TestFlecsScope>("::Root");

    test_assert(!c.has(flecs::ChildOf, flecs::Wildcard));
    test_str(c.path().c_str(), "::Root");
}

void World_register_nested_w_root_name(void) {
    flecs::world ecs;

    auto c = ecs.component<Nested::TestFlecsFromScope>("::Root");

    test_assert(!c.has(flecs::ChildOf, flecs::Wildcard));
    test_str(c.path().c_str(), "::Root");
}

void World_set_lookup_path(void) {
    flecs::world ecs;

    auto parent = ecs.entity("Parent");
    auto child = ecs.scope(parent).entity("Child");

    test_assert(ecs.lookup("Parent") == parent);
    test_assert(ecs.lookup("Child") == 0);
    test_assert(ecs.lookup("Parent::Child") == child);

    flecs::entity_t lookup_path[] = { parent, 0 };
    flecs::entity_t *old_path = ecs.set_lookup_path(lookup_path);

    test_assert(ecs.lookup("Parent") == parent);
    test_assert(ecs.lookup("Child") == child);
    test_assert(ecs.lookup("Parent::Child") == child);

    ecs.set_lookup_path(old_path);
}

void World_run_post_frame(void) {
    flecs::world ecs;

    int ctx = 10;

    ecs.system()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                it.world().run_post_frame([](flecs::world_t *w, void *ctx) {
                    int *i = static_cast<int*>(ctx);
                    test_int(*i, 10);
                    i[0] ++;
                }, &ctx);
            }
        });
    test_int(ctx, 10);

    ecs.progress();

    test_int(ctx, 11);
}

void World_component_w_low_id(void) {
    flecs::world ecs;

    flecs::entity p = ecs.component<Position>();

    test_assert(p.id() < FLECS_HI_COMPONENT_ID);
}

void World_reregister_after_reset_w_hooks_and_in_use(void) {
    flecs::world ecs;
    Pod::ctor_invoked = 0;

    ecs.component<Pod>();

    ecs.entity().add<Pod>();
    test_int(1, Pod::ctor_invoked);

    flecs::_::type<Pod>().reset();

    ecs.component<Pod>();

    ecs.entity().add<Pod>();
    test_int(2, Pod::ctor_invoked);
}

void World_reregister_after_reset_w_hooks_and_in_use_implicit(void) {
    flecs::world ecs;

    Pod::ctor_invoked = 0;

    ecs.component<Pod>();

    ecs.entity().add<Pod>();
    test_int(1, Pod::ctor_invoked);

    flecs::_::type<Pod>().reset();

    ecs.entity().add<Pod>();
    test_int(2, Pod::ctor_invoked);
}

void World_get_ref() {
    flecs::world ecs;

    struct Space { int v; };
    ecs.component<Space>();
    ecs.set<Space>({12});

    auto space = ecs.get_ref<Space>();
    test_int(12, space->v);
}

void World_reset_world(void) {
    flecs::world ecs;
    flecs::entity e = ecs.entity();

    test_assert(ecs.exists(e));
    ecs.reset();
    test_assert(!ecs.exists(e));
}

void World_id_from_pair_type(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    flecs::id id = ecs.id<flecs::pair<Position, Velocity>>();
    test_assert(id.is_pair());
    test_assert(id.first() == ecs.id<Position>());
    test_assert(id.second() == ecs.id<Velocity>());
}

void World_scope_w_name(void) {
    flecs::world ecs;

    flecs::entity parent = ecs.entity("parent");
    flecs::entity child = ecs.scope("parent").entity();

    test_assert(child.has(flecs::ChildOf, parent));
}

void World_set_get_context(void) {
    flecs::world ecs;
    ecs.set_binding_ctx(nullptr);
    ecs.set_ctx(nullptr);

    int ctx;
    ecs.set_ctx(&ctx);
    test_assert(ecs.get_ctx() == &ctx);
    test_assert(ecs.get_binding_ctx() == nullptr);
}

void World_set_get_binding_context(void) {
    flecs::world ecs;
    ecs.set_binding_ctx(nullptr);
    ecs.set_ctx(nullptr);

    int ctx;
    ecs.set_binding_ctx(&ctx);
    test_assert(ecs.get_binding_ctx() == &ctx);
    test_assert(ecs.get_ctx() == nullptr);
}

static void ctx_free(void *ctx) {
    static_cast<int*>(ctx)[0] = 10;
}

void World_set_get_context_w_free(void) {
    int ctx = 0;

    {
        flecs::world ecs;
        ecs.set_binding_ctx(nullptr);
        ecs.set_ctx(nullptr);

        ecs.set_ctx(&ctx, ctx_free);
        test_assert(ecs.get_ctx() == &ctx);
        test_assert(ecs.get_binding_ctx() == nullptr);
        test_int(ctx, 0);
    }

    test_int(ctx, 10);
}

void World_set_get_binding_context_w_free(void) {
    int ctx = 0;

    {
        flecs::world ecs;
        ecs.set_binding_ctx(nullptr);
        ecs.set_ctx(nullptr);

        ecs.set_binding_ctx(&ctx, ctx_free);
        test_assert(ecs.get_binding_ctx() == &ctx);
        test_assert(ecs.get_ctx() == nullptr);
        test_int(ctx, 0);
    }

    test_int(ctx, 10);
}

void World_make_pair(void) {
    flecs::world ecs;

    flecs::entity r = ecs.entity();
    flecs::entity t = ecs.entity();
    flecs::id id = ecs.pair(r, t);

    test_assert(id.is_pair());
    test_assert(id.first() == r);
    test_assert(id.second() == t);
}

void World_delta_time(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    float dt = 0;

    ecs.entity().add<Tag>();

    ecs.system<Tag>()
        .each([&](flecs::entity e, Tag) {
            dt = e.world().delta_time();
        });

    ecs.progress(2);

    test_int(dt, 2);
}

void World_register_nested_component_in_module(void) {
    flecs::world ecs;

    ecs.import<nested_component_module>();

    test_assert(ecs.id<nested_component_module::Foo>() != 0);
    test_assert(ecs.id<nested_component_module::Foo::Bar>() != 0);

    flecs::entity foo = ecs.component<nested_component_module::Foo>();
    flecs::entity bar = ecs.component<nested_component_module::Foo::Bar>();

    test_str(foo.path().c_str(), "::nested_component_module::Foo");
    test_str(bar.path().c_str(), "::nested_component_module::Foo::Bar");
}

static inline void *atfini_ctx = nullptr;
static inline int atfini_invoked = 0;
static void atfini_callback(flecs::world_t *world, void *ctx) {
    test_assert(world != nullptr);
    atfini_ctx = ctx;
    atfini_invoked ++;
}

void World_atfini(void) {
    atfini_invoked = 0;
    {
        flecs::world ecs;
        ecs.atfini(atfini_callback);
    }
    test_int(atfini_invoked, 1);
    test_assert(atfini_ctx == nullptr);
}

void World_atfini_w_ctx(void) {
    atfini_invoked = 0;
    int ctx;
    {
        flecs::world ecs;
        ecs.atfini(atfini_callback, &ctx);
    }
    test_int(atfini_invoked, 1);
    test_assert(atfini_ctx == &ctx);
}

void World_get_mut_T(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    Position *p = world.try_get_mut<Position>();

    test_assert(p == nullptr);

    world.set<Position>({10, 20});
    p = world.try_get_mut<Position>();

    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void World_get_mut_R_T(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    struct Tgt { };
    world.component<Tgt>();

    Position *p = world.try_get_mut<Position, Tgt>();
    test_assert(p == nullptr);

    world.set<Position, Tgt>({10, 20});
    
    p = world.try_get_mut<Position, Tgt>();
    test_assert(p != nullptr);

    test_int(p->x, 10);
    test_int(p->y, 20);
}

void World_world_mini(void) {
    int32_t count = 0;
    {
        flecs::world world(ecs_mini());
        world.make_owner();

        world.atfini([](flecs::world_t* world, void *ctx) {
            int32_t *data = static_cast<int32_t*>(ctx);
            data[0] ++;
        }, &count);

        test_assert(world.lookup("flecs.system") == 0);
        test_assert(world.lookup("flecs.pipeline") == 0);
        test_assert(world.lookup("flecs.timer") == 0);
        test_assert(world.lookup("flecs.meta") == 0);
    }
    test_int(count, 1);
}

void World_copy_world(void) {
    flecs::world world_1;
    flecs::world world_2 = world_1;

    test_assert(world_1.c_ptr() == world_2.c_ptr());
}

void World_fini_reentrancy(void) {
    {
        struct A {
            int a;
        };

        flecs::world world;

        // declare on remove hook for component A:
        world.component<A>().on_remove([&world](flecs::entity e, A &) {  //
            // This code runs on world destroy, since we did not remove this component manually before the world was destroyed.

            // before we make a copy of the world, the refcount has to be 1 since this is the special case where
            // we will be copying a world object precisely when the world is being destroyed.
            // see world::~world() code and notes.
            const ecs_header_t *hdr = (const ecs_header_t*) world.c_ptr();
            test_assert(hdr->refcount == 1);

            // obtain the entity's world. This increments the world's hdr refcount
            flecs::world world_copy = e.world();

            test_assert(hdr->refcount == 2);
            // here world_copy object wrapping c world is destroyed
            // therefore, world destroy will be called again wreaking havoc.
        });

        world.entity().add<A>();

        // world will be destroyed here, and hook above will be called.
    }
    test_assert(true); // if this runs, ecs_fini did not abort and we're good.

}

void World_fini_copy_move_assign(void) {
    // helper component to tag worlds:
    struct ID {
        int id;
    };

    auto fini_handler = [](ecs_world_t *world, void *ctx) {
        bool *finished = (bool *) ctx;
        *finished = true;
    };

    auto get_world_id = [](const flecs::world &world) { return world.try_get<ID>()->id; };

    bool finished_1 = false;
    bool finished_2 = false;
    bool finished_3 = false;

    // Create three worlds '1', '2' and '3', that initially start in 'A', 'B' and 'C' respectively:
    flecs::world world_A;
    world_A.component<ID>();
    
    world_A.set<ID>({1});
    world_A.atfini(fini_handler, &finished_1);

    flecs::world world_B;
    world_B.component<ID>();
    
    world_B.set<ID>(ID{2});
    world_B.atfini(fini_handler, &finished_2);

    flecs::world world_C;
    world_C.component<ID>();
    
    world_C.set<ID>(ID{3});
    world_C.atfini(fini_handler, &finished_3);

    // now overwrite one with another using copy-assign
    world_A = world_B;  // (copy assign). Overwrite world_A with world_B
    // world with ID '1' contained in world_A should have been destroyed:
    test_assert(finished_1 == true);

    // now world_A and world_B point to the same world '2'
    test_assert(world_A.c_ptr() == world_B.c_ptr());
    test_assert(get_world_id(world_A) == 2);
    test_assert(get_world_id(world_B) == 2);

    // test move-assign properly releases existing world '3' in C:
    world_C = FLECS_MOV(world_B);        // move-assign, overwrite what is in C.
    test_assert(world_B.c_ptr() == nullptr);  // B is now empty
    test_assert(finished_2 == false);         // still '2' is alive (in 'A')
    test_assert(finished_3 == true);          // world '3' destroyed after being overwritten
    test_assert(get_world_id(world_C) == 2);  // world '2' is now in 'C'

    world_A.release();
    test_assert(world_A.c_ptr() == nullptr);  // A is now empty
    test_assert(finished_2 == false);         // '2' is still alive in 'C', though.

    world_C.release();
    test_assert(finished_2 == true);  // '2' is now finished as well.
}

void World_exclusive_access_self_mutate(void) {
    flecs::world ecs;
    ecs.component<Position>();

    ecs.exclusive_access_begin();

    flecs::entity e = ecs.entity();

    e.add<Position>();
    test_assert(e.has<Position>());

    ecs.exclusive_access_end();
}

void World_id_if_registered(void) {
    {
        flecs::world world;

        test_assert(world.id_if_registered<Position>() == 0);
        test_assert(world.id_if_registered<Position>() == 0);

        auto c = world.component<Position>();

        test_assert(world.id_if_registered<Position>() == c);
    }

    {
        flecs::world world;

        test_assert(world.id_if_registered<Position>() == 0);
        test_assert(world.id_if_registered<Position>() == 0);

        auto c = world.component<Position>();

        test_assert(world.id_if_registered<Position>() == c);
    }
}

void World_get_type_info_t(void) {
    flecs::world world;

    flecs::entity c = world.component<Position>();

    const flecs::type_info_t *ti = world.type_info(c);
    test_assert(ti != nullptr);
    test_int(ti->size, sizeof(Position));
    test_int(ti->alignment, alignof(Position));
    test_assert(ti->component == c);
}

void World_get_type_info_T(void) {
    flecs::world world;
    world.component<Position>();

    const flecs::type_info_t *ti = world.type_info<Position>();
    test_assert(ti != nullptr);
    test_int(ti->size, sizeof(Position));
    test_int(ti->alignment, alignof(Position));
    test_assert(ti->component == world.component<Position>());
}

void World_get_type_info_r_t(void) {
    flecs::world world;

    flecs::entity c = world.component<Position>();
    flecs::entity tgt = world.entity();

    const flecs::type_info_t *ti = world.type_info(c, tgt);
    test_assert(ti != nullptr);
    test_int(ti->size, sizeof(Position));
    test_int(ti->alignment, alignof(Position));
    test_assert(ti->component == c);
}

void World_get_type_info_R_t(void) {
    flecs::world world;

    flecs::entity c = world.component<Position>();
    flecs::entity tgt = world.entity();

    const flecs::type_info_t *ti = world.type_info<Position>(tgt);
    test_assert(ti != nullptr);
    test_int(ti->size, sizeof(Position));
    test_int(ti->alignment, alignof(Position));
    test_assert(ti->component == c);
}

void World_get_type_info_R_T(void) {
    flecs::world world;
    world.component<Tgt>();
    world.component<Position>();

    flecs::entity c = world.component<Position>();

    const flecs::type_info_t *ti = world.type_info<Position, Tgt>();
    test_assert(ti != nullptr);
    test_int(ti->size, sizeof(Position));
    test_int(ti->alignment, alignof(Position));
    test_assert(ti->component == c);
}

void World_get_type_info_t_tag(void) {
    flecs::world world;

    flecs::entity c = world.component<Tag>();

    const flecs::type_info_t *ti = world.type_info(c);
    test_assert(ti == nullptr);
}

void World_get_type_info_T_tag(void) {
    flecs::world world;
    world.component<Tag>();

    const flecs::type_info_t *ti = world.type_info<Tag>();
    test_assert(ti == nullptr);
}

void World_get_type_info_r_t_tag(void) {
    flecs::world world;
    world.component<Tag>();
    
    flecs::entity c = world.component<Tag>();
    flecs::entity tgt = world.entity();

    const flecs::type_info_t *ti = world.type_info(c, tgt);
    test_assert(ti == nullptr);
}

void World_get_type_info_R_t_tag(void) {
    flecs::world world;
    world.component<Tag>();

    flecs::entity tgt = world.entity();

    const flecs::type_info_t *ti = world.type_info<Tag>(tgt);
    test_assert(ti == nullptr);
}

void World_get_type_info_R_T_tag(void) {
    flecs::world world;
    world.component<Tag>();
    world.component<Tgt>();

    const flecs::type_info_t *ti = world.type_info<Tag, Tgt>();
    test_assert(ti == nullptr);
}

END_DEFINE_SPEC(FFlecsWorldTestsSpec);

/* "id": "World",
            "testcases": [
                "builtin_components",
                "multi_world_empty",
                "multi_world_component",
                "multi_world_component_namespace",
                "multi_world_module",
                "multi_world_recycled_component_different_generation",
                "type_id",
                "reregister_after_reset",
                "reregister_after_reset_w_namespace",
                "reregister_namespace",
                "reregister_after_delete",
                "implicit_reregister_after_reset",
                "register_short_template",
                "reimport",
                "reimport_module_after_reset",
                "reimport_module_new_world",
                "reimport_namespaced_module",
                "c_interop_module",
                "c_interop_after_reset",
                "c_interop_module_no_names",
                "implicit_register_w_new_world",
                "implicit_register_after_reset_register_w_custom_name",
                "register_after_reset_register_w_custom_name",
                "register_builtin_after_reset",
                "register_meta_after_reset",
                "reregister_after_reset_w_hooks_and_in_use",
                "reregister_after_reset_w_hooks_and_in_use_implicit",
                "register_component_w_reset_in_multithreaded",
                "register_nested_component_in_module",
                "count",
                "count_id",
                "count_pair",
                "count_pair_type_id",
                "count_pair_id",
                "staged_count",
                "async_stage_add",
                "with_tag",
                "with_tag_type",
                "with_relation",
                "with_relation_type",
                "with_relation_object_type",
                "with_tag_nested",
                "with_scope",
                "with_scope_nested",
                "with_scope_type",
                "with_scope_type_staged",
                "with_scope_no_lambda",
                "with_scope_type_no_lambda",
                "recursive_lookup",
                "type_w_tag_name",
                "entity_w_tag_name",
                "template_component_name",
                "template_component_w_namespace_name",
                "template_component_w_same_namespace_name",
                "template_component_w_namespace_name_and_namespaced_arg",
                "template_component_w_same_namespace_name_and_namespaced_arg",
                "template_component_from_module_2_args",
                "entity_as_tag",
                "entity_w_name_as_tag",
                "entity_as_component",
                "entity_w_name_as_component",
                "entity_as_component_2_worlds",
                "entity_as_namespaced_component_2_worlds",
                "delete_with_id",
                "delete_with_type",
                "delete_with_pair",
                "delete_with_pair_type",
                "delete_with_implicit",
                "delete_with_pair_implicit",
                "remove_all_id",
                "remove_all_type",
                "remove_all_pair",
                "remove_all_pair_type",
                "remove_all_implicit",
                "remove_all_pair_implicit",
                "get_scope",
                "get_scope_type",
                "register_namespace_after_component",
                "is_alive",
                "is_valid",
                "exists",
                "get_alive",
                "make_alive",
                "get_tick",
                "register_from_scope",
                "register_nested_from_scope",
                "register_w_root_name",
                "register_nested_w_root_name",
                "set_lookup_path",
                "run_post_frame",
                "component_w_low_id",
                "get_set_log_level",
                "reset_world",
                "id_from_pair_type",
                "scope_w_name",
                "set_get_context",
                "set_get_binding_context",
                "set_get_context_w_free",
                "set_get_binding_context_w_free",
                "make_pair",
                "delta_time",
                "atfini",
                "atfini_w_ctx",
                "get_mut_T",
                "get_mut_R_T",
                "world_mini",
                "copy_world",
                "fini_reentrancy",
                "fini_copy_move_assign",
                "exclusive_access_self_mutate",
                "id_if_registered",
                "get_type_info_t",
                "get_type_info_T",
                "get_type_info_r_t",
                "get_type_info_R_t",
                "get_type_info_R_T",
                "get_type_info_t_tag",
                "get_type_info_T_tag",
                "get_type_info_r_t_tag",
                "get_type_info_R_t_tag",
                "get_type_info_R_T_tag"
            ]*/

void FFlecsWorldTestsSpec::Define()
{
	It("multi_world_empty", [this]() { World_multi_world_empty(); });
    It("multi_world_component", [this]() { World_multi_world_component(); });
    It("multi_world_component_namespace", [this]() { World_multi_world_component_namespace(); });
    It("multi_world_module", [this]() { World_multi_world_module(); });
    It("multi_world_recycled_component_different_generation", [this]() { World_multi_world_recycled_component_different_generation(); });
    It("type_id", [this]() { World_type_id(); });
    It("reregister_after_reset", [this]() { World_reregister_after_reset(); });
    It("reregister_after_reset_w_namespace", [this]() { World_reregister_after_reset_w_namespace(); });
    It("reregister_namespace", [this]() { World_reregister_namespace(); });
    It("reregister_after_delete", [this]() { World_reregister_after_delete(); });
    It("implicit_reregister_after_reset", [this]() { World_implicit_reregister_after_reset(); });
    It("register_short_template", [this]() { World_register_short_template(); });
    It("reimport", [this]() { World_reimport(); });
    It("reimport_module_after_reset", [this]() { World_reimport_module_after_reset(); });
    It("reimport_module_new_world", [this]() { World_reimport_module_new_world(); });
    It("reimport_namespaced_module", [this]() { World_reimport_namespaced_module(); });
    It("c_interop_module", [this]() { World_c_interop_module(); });
    It("c_interop_after_reset", [this]() { World_c_interop_after_reset(); });
    It("c_interop_module_no_names", [this]() { World_c_interop_module_no_names(); });
    It("implicit_register_w_new_world", [this]() { World_implicit_register_w_new_world(); });
    It("implicit_register_after_reset_register_w_custom_name", [this]() { World_implicit_register_after_reset_register_w_custom_name(); });
    It("register_after_reset_register_w_custom_name", [this]() { World_register_after_reset_register_w_custom_name(); });
    It("register_builtin_after_reset", [this]() { World_register_builtin_after_reset(); });
    It("register_meta_after_reset", [this]() { World_register_meta_after_reset(); });
    It("reregister_after_reset_w_hooks_and_in_use", [this]() { World_reregister_after_reset_w_hooks_and_in_use(); });
    It("reregister_after_reset_w_hooks_and_in_use_implicit", [this]() { World_reregister_after_reset_w_hooks_and_in_use_implicit(); });
    It("register_component_w_reset_in_multithreaded", [this]() { World_register_component_w_reset_in_multithreaded(); });
    It("register_nested_component_in_module", [this]() { World_register_nested_component_in_module(); });
    It("count", [this]() { World_count(); });
    It("count_id", [this]() { World_count_id(); });
    It("count_pair", [this]() { World_count_pair(); });
    It("count_pair_type_id", [this]() { World_count_pair_type_id(); });
    It("count_pair_id", [this]() { World_count_pair_id(); });
    It("staged_count", [this]() { World_staged_count(); });
    It("async_stage_add", [this]() { World_async_stage_add(); });
    It("with_tag", [this]() { World_with_tag(); });
    It("with_tag_type", [this]() { World_with_tag_type(); });
    It("with_relation", [this]() { World_with_relation(); });
    It("with_relation_type", [this]() { World_with_relation_type(); });
    It("with_relation_object_type", [this]() { World_with_relation_object_type(); });
    It("with_tag_nested", [this]() { World_with_tag_nested(); });
    It("with_scope", [this]() { World_with_scope(); });
    It("with_scope_nested", [this]() { World_with_scope_nested(); });
    It("with_scope_type", [this]() { World_with_scope_type(); });
    It("with_scope_type_staged", [this]() { World_with_scope_type_staged(); });
    It("with_scope_no_lambda", [this]() { World_with_scope_no_lambda(); });
    It("with_scope_type_no_lambda", [this]() { World_with_scope_type_no_lambda(); });
    It("recursive_lookup", [this]() { World_recursive_lookup(); });
    It("type_w_tag_name", [this]() { World_type_w_tag_name(); });
    It("entity_w_tag_name", [this]() { World_entity_w_tag_name(); });
    It("template_component_name", [this]() { World_template_component_name(); });
    It("template_component_w_namespace_name", [this]() { World_template_component_w_namespace_name(); });
    It("template_component_w_namespace_name_and_namespaced_arg", [this]() { World_template_component_w_namespace_name_and_namespaced_arg(); });
    It("template_component_from_module_2_args", [this]() { World_template_component_from_module_2_args(); });
    It("entity_as_tag", [this]() { World_entity_as_tag(); });
    It("entity_w_name_as_tag", [this]() { World_entity_w_name_as_tag(); });
    It("entity_as_component", [this]() { World_entity_as_component(); });
    It("entity_w_name_as_component", [this]() { World_entity_w_name_as_component(); });
    It("entity_as_component_2_worlds", [this]() { World_entity_as_component_2_worlds(); });
    It("entity_as_namespaced_component_2_worlds", [this]() { World_entity_as_namespaced_component_2_worlds(); });
    It("delete_with_id", [this]() { World_delete_with_id(); });
    It("delete_with_type", [this]() { World_delete_with_type(); });
    It("delete_with_pair", [this]() { World_delete_with_pair(); });
    It("delete_with_pair_type", [this]() { World_delete_with_pair_type(); });
    It("delete_with_implicit", [this]() { World_delete_with_implicit(); });
    It("delete_with_pair_implicit", [this]() { World_delete_with_pair_implicit(); });
    It("remove_all_id", [this]() { World_remove_all_id(); });
    It("remove_all_type", [this]() { World_remove_all_type(); });
    It("remove_all_pair", [this]() { World_remove_all_pair(); });
    It("remove_all_pair_type", [this]() { World_remove_all_pair_type(); });
    It("remove_all_implicit", [this]() { World_remove_all_implicit(); });
    It("remove_all_pair_implicit", [this]() { World_remove_all_pair_implicit(); });
    It("get_scope", [this]() { World_get_scope(); });
    It("get_scope_type", [this]() { World_get_scope_type(); });
    It("register_namespace_after_component", [this]() { World_register_namespace_after_component(); });
    It("is_alive", [this]() { World_is_alive(); });
    It("is_valid", [this]() { World_is_valid(); });
    It("exists", [this]() { World_exists(); });
    It("get_alive", [this]() { World_get_alive(); });
    It("make_alive", [this]() { World_make_alive(); });
    It("reset_all", [this]() { World_reset_all(); });
    It("get_tick", [this]() { World_get_tick(); });
    It("register_from_scope", [this]() { World_register_from_scope(); });
    It("register_nested_from_scope", [this]() { World_register_nested_from_scope(); });
    It("register_w_root_name", [this]() { World_register_w_root_name(); });
    It("register_nested_w_root_name", [this]() { World_register_nested_w_root_name(); });
    It("set_lookup_path", [this]() { World_set_lookup_path(); });
    It("run_post_frame", [this]() { World_run_post_frame(); });
    It("component_w_low_id", [this]() { World_component_w_low_id(); });
    It("reset_world", [this]() { World_reset_world(); });
    It("id_from_pair_type", [this]() { World_id_from_pair_type(); });
    It("scope_w_name", [this]() { World_scope_w_name(); });
    It("set_get_context", [this]() { World_set_get_context(); });
    It("set_get_binding_context", [this]() { World_set_get_binding_context(); });
    It("set_get_context_w_free", [this]() { World_set_get_context_w_free(); });
    It("set_get_binding_context_w_free", [this]() { World_set_get_binding_context_w_free(); });
    It("make_pair", [this]() { World_make_pair(); });
    It("delta_time", [this]() { World_delta_time(); });
    It("atfini", [this]() { World_atfini(); });
    It("atfini_w_ctx", [this]() { World_atfini_w_ctx(); });
    It("get_mut_T", [this]() { World_get_mut_T(); });
    It("get_mut_R_T", [this]() { World_get_mut_R_T(); });
    It("world_mini", [this]() { World_world_mini(); });
    It("copy_world", [this]() { World_copy_world(); });
    It("fini_reentrancy", [this]() { World_fini_reentrancy(); });
    It("fini_copy_move_assign", [this]() { World_fini_copy_move_assign(); });
    It("exclusive_access_self_mutate", [this]() { World_exclusive_access_self_mutate(); });
    It("id_if_registered", [this]() { World_id_if_registered(); });
    It("get_type_info_t_entity", [this]() { World_get_type_info_t(); });
    It("get_type_info_T_type", [this]() { World_get_type_info_T(); });
    It("get_type_info_r_entity_t_entity", [this]() { World_get_type_info_r_t(); });
    It("get_type_info_R_type_t_entity", [this]() { World_get_type_info_R_t(); });
    It("get_type_info_R_type_T_type", [this]() { World_get_type_info_R_T(); });
    It("get_type_info_t_entity_tag", [this]() { World_get_type_info_t_tag(); });
    It("get_type_info_T_type_tag", [this]() { World_get_type_info_T_tag(); });
    It("get_type_info_r_entity_t_entity_tag", [this]() { World_get_type_info_r_t_tag(); });
    It("get_type_info_R_type_t_entity_tag", [this]() { World_get_type_info_R_t_tag(); });
    It("get_type_info_R_type_T_type_tag", [this]() { World_get_type_info_R_T_tag(); });
}

#endif // WITH_AUTOMATION_TESTS
