#include <cpp_api.h>

void World_multi_world_empty() {
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

void World_builtin_components() {
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

void World_multi_world_component() {
    flecs::world w1;
    flecs::world w2;

    auto p_1 = w1.component<Position>();
    auto v_1 = w1.component<Velocity>();
    auto v_2 = w2.component<Velocity>();
    auto m_2 = w2.component<Mass>();

    test_assert(v_1.id() == v_2.id());
    test_assert(p_1.id() != m_2.id());
    test_assert(m_2.id() > v_2.id());

    auto m_1 = w2.component<Mass>();
    test_assert(m_1.id() == m_2.id());
}

namespace A {
    struct Comp {
        float x;
        float y;
    };
}

void World_multi_world_component_namespace() {
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

void World_multi_world_module() {
    flecs::world world1;
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

void World_multi_world_recycled_component() {
    flecs::entity c;
    {
        flecs::world ecs;
        for (int i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
            ecs_new_low_id(ecs);
        }

        ecs.entity().destruct();
        c = ecs.component<Position>();
    }
    {
        flecs::world ecs;
        test_assert((c == ecs.component<Position>()));
    }
}

void World_multi_world_recycled_component_different_generation() {
    flecs::entity c;
    {
        flecs::world ecs;
        for (int i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
            ecs_new_low_id(ecs);
        }

        ecs.entity().destruct();
        c = ecs.component<Position>();
    }
    {
        flecs::world ecs;
        for (int i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
            ecs_new_low_id(ecs);
        }

        ecs.entity().destruct();
        test_assert((c == ecs.component<Position>()));
    }
}

void World_type_id() {
    flecs::world ecs;

    auto p = ecs.component<Position>();

    test_assert(p.id() == flecs::type_id<Position>());
}

void World_different_comp_same_name() {
    install_test_abort();

    flecs::world ecs;

    test_expect_abort();

    ecs.component<Position>("Position");
    ecs.component<Velocity>("Position");
}

void World_reregister_after_reset() {
    flecs::world ecs;

    auto p1 = ecs.component<Position>("Position");

    // Simulate different binary
    flecs::_::cpp_type<Position>::reset();

    auto p2 = ecs.component<Position>("Position");

    test_assert(p1.id() == p2.id());
}

void World_implicit_reregister_after_reset() {
    flecs::world ecs;

    ecs.entity().add<Position>();

    flecs::entity_t p_id_1 = flecs::type_id<Position>();

    // Simulate different binary
    flecs::_::cpp_type<Position>::reset();

    ecs.entity().add<Position>();

    flecs::entity_t p_id_2 = flecs::type_id<Position>();

    test_assert(p_id_1 == p_id_2);
}

void World_reregister_after_reset_w_namespace() {
    flecs::world ecs;

    ecs.component<ns::FooComp>();

    flecs::entity_t p_id_1 = flecs::type_id<ns::FooComp>();

    // Simulate different binary
    flecs::_::cpp_type<ns::FooComp>::reset();

    ecs.component<ns::FooComp>();

    flecs::entity_t p_id_2 = flecs::type_id<ns::FooComp>();

    test_assert(p_id_1 == p_id_2);
}

void World_reregister_namespace() {
    flecs::world ecs;

    ecs.component<ns::FooComp>();

    flecs::entity_t p_id_1 = flecs::type_id<ns::FooComp>();

    ecs.component<ns::FooComp>();

    flecs::entity_t p_id_2 = flecs::type_id<ns::FooComp>();

    test_assert(p_id_1 == p_id_2);
}

void World_reregister_after_reset_different_name() {
    install_test_abort();

    flecs::world ecs;

    test_expect_abort();    

    ecs.component<Position>("Position");

    // Simulate different binary
    flecs::_::cpp_type<Position>::reset();

    ecs.component<Position>("Velocity");
}

template <typename T>
struct Tmp { int32_t v; };
struct Test { };

void World_register_short_template() {
    flecs::world ecs;

    auto c = ecs.component<Tmp<Test>>();
    test_assert(c != 0);
    test_str(c.name(), "Tmp<Test>");

    const EcsComponent *ptr = c.get<flecs::Component>();
    test_assert(ptr != NULL);
    test_int(ptr->size, 4);
    test_int(ptr->alignment, 4);
}

void World_reimport() {
    flecs::world ecs;

    auto m1 = ecs.import<FooModule>();

    auto m2 = ecs.import<FooModule>();
    
    test_assert(m1.id() == m2.id());
}

void World_reimport_module_after_reset() {
    flecs::world ecs;

    auto m1 = ecs.import<FooModule>();

    // Simulate different binary
    flecs::_::cpp_type<FooModule>::reset();

    auto m2 = ecs.import<FooModule>();
    
    test_assert(m1.id() == m2.id());
}

void World_reimport_module_new_world() {
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

void World_reimport_namespaced_module() {
    flecs::world ecs;

    test_int(ns::namespace_module::import_count, 0);

    // Import first time, should call module constructor.
    ecs.import<ns::namespace_module>();

    test_int(ns::namespace_module::import_count, 1);

    // Import second time, should not call constructor. 
    ecs.import<ns::namespace_module>();

    test_int(ns::namespace_module::import_count, 1);
}


void World_c_interop_module() {
    flecs::world ecs;

    ecs.import<test::interop::module>();

    auto e_pos = ecs.lookup("test::interop::module::Position");
    test_assert(e_pos.id() != 0);
}

void World_c_interop_after_reset() {
    flecs::world ecs;

    ecs.import<test::interop::module>();

    auto e_pos = ecs.lookup("test::interop::module::Position");
    test_assert(e_pos.id() != 0);

    flecs::_::cpp_type<test::interop::module>::reset();

    ecs.import<test::interop::module>();
}

void World_implicit_register_w_new_world() {
    {
        flecs::world ecs;

        auto e = ecs.entity().set<Position>({10, 20});
        test_assert(e.has<Position>());
        auto *p = e.get<Position>();
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        /* Recreate world, does not reset static state */
        flecs::world ecs;

        auto e = ecs.entity().set<Position>({10, 20});
        test_assert(e.has<Position>());
        auto *p = e.get<Position>();
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }    
}

void World_implicit_register_after_reset_register_w_custom_name() {
    flecs::world ecs;

    flecs::entity c = ecs.component<Position>("MyPosition");
    test_str(c.name(), "MyPosition");

    flecs::reset(); // Simulate working across boundary

    auto e = ecs.entity().add<Position>();
    test_assert(e.has<Position>());
    test_assert(e.has(c));
}

void World_register_after_reset_register_w_custom_name() {
    flecs::world ecs;

    flecs::entity c1 = ecs.component<Position>("MyPosition");
    test_str(c1.name(), "MyPosition");

    flecs::reset(); // Simulate working across boundary

    flecs::entity c2 = ecs.component<Position>();
    test_str(c2.name(), "MyPosition");
}

void World_register_builtin_after_reset() {
    flecs::world ecs;

    auto c1 = ecs.component<flecs::Component>();
    test_assert(c1 == ecs_id(EcsComponent));

    flecs::reset(); // Simulate working across boundary

    auto c2 = ecs.component<flecs::Component>();
    test_assert(c2 == ecs_id(EcsComponent));
    test_assert(c1 == c2);
}

void World_register_meta_after_reset() {
    flecs::world ecs;

    auto c1 = ecs.component<Position>();

    flecs::reset(); // Simulate working across boundary

    auto c2 = ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    test_assert(c1 == c2);
}

void World_count() {
    flecs::world ecs;

    test_int(ecs.count<Position>(), 0);

    ecs.entity().add<Position>();
    ecs.entity().add<Position>();
    ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Mass>();
    ecs.entity().add<Position>().add<Mass>();
    ecs.entity().add<Position>().add<Velocity>();

    test_int(ecs.count<Position>(), 6);
}

void World_count_id() {
    flecs::world ecs;

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

void World_count_pair() {
    flecs::world ecs;

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

void World_count_pair_type_id() {
    flecs::world ecs;

    struct Rel { };

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

void World_count_pair_id() {
    flecs::world ecs;

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

void World_staged_count() {
    flecs::world ecs;

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

void World_async_stage_add() {
    flecs::world ecs;

    ecs.component<Position>();

    auto e = ecs.entity();

    flecs::world async = ecs.async_stage();
    e.mut(async).add<Position>();
    test_assert(!e.has<Position>());
    async.merge();
    test_assert(e.has<Position>());
}

void World_with_tag() {
    flecs::world ecs;

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

    auto q = ecs.query_builder<>().term(Tag).build();

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

void World_with_tag_type() {
    flecs::world ecs;

    struct Tag { };

    ecs.with<Tag>([&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get any contents from the with.
    auto self = ecs.component<Self>();
    test_assert(!self.has<Tag>());

    auto q = ecs.query_builder<>().term<Tag>().build();

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

void World_with_relation() {
    flecs::world ecs;

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

    auto q = ecs.query_builder<>().term(Likes, Bob).build();

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

void World_with_relation_type() {
    flecs::world ecs;

    struct Likes { };
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

    auto q = ecs.query_builder<>().term<Likes>(Bob).build();

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

void World_with_relation_object_type() {
    flecs::world ecs;

    struct Likes { };
    struct Bob { };

    ecs.with<Likes, Bob>([&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get any contents from the with.
    auto self = ecs.component<Self>();
    test_assert(!(self.has<Likes, Bob>()));

    auto q = ecs.query_builder<>().term<Likes, Bob>().build();

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

void World_with_scope() {
    flecs::world ecs;

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
    auto q = ecs.query_builder<>().term(flecs::ChildOf, parent).build();

    q.each([&](flecs::entity e) {
        test_assert(e.has(flecs::ChildOf, parent));

        test_bool(e.get([&](const Self& s){
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    test_int(count, 3);
}

struct ParentScope { };

void World_with_scope_type() {
    flecs::world ecs;

    ecs.scope<ParentScope>([&]{
        ecs.entity("Child");
    });

    auto parent = ecs.lookup("ParentScope");
    test_assert(parent != 0);

    auto child = ecs.lookup("ParentScope::Child");
    test_assert(child != 0);
    test_assert(child == parent.lookup("Child"));
}

void World_with_scope_type_staged() {
    flecs::world ecs;

    flecs::entity e;
    flecs::world stage = ecs.get_stage(0);
    
    ecs.readonly_begin();
    stage.scope<ParentScope>([&]{
        e = stage.entity("Child");
    });
    ecs.readonly_end();

    test_assert( e.has(flecs::ChildOf, ecs.id<ParentScope>()) );

    auto parent = ecs.lookup("ParentScope");
    test_assert(parent != 0);

    auto child = ecs.lookup("ParentScope::Child");
    test_assert(child != 0);
    test_assert(child == parent.lookup("Child"));
}

void World_with_scope_no_lambda() {
    flecs::world ecs;

    auto parent = ecs.entity("Parent");
    auto child = ecs.scope(parent).entity("Child");

    test_assert(child.has(flecs::ChildOf, parent));
    test_assert(ecs.get_scope() == 0);
}

void World_with_scope_type_no_lambda() {
    flecs::world ecs;

    auto child = ecs.scope<ParentScope>().entity("Child");

    test_assert(child.has(flecs::ChildOf, ecs.id<ParentScope>()));
    test_assert(ecs.get_scope() == 0);
}

void World_with_tag_nested() {
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

void World_with_scope_nested() {
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

void World_recursive_lookup() {
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

void World_type_w_tag_name() {
    flecs::world ecs;

    auto c = ecs.component<Tag>();
    test_assert(c != flecs::entity());
    test_str(c.path().c_str(), "::Tag");
    test_assert(c != flecs::Tag);
}

void World_entity_w_tag_name() {
    flecs::world ecs;

    auto c = ecs.entity("Tag");
    test_assert(c != flecs::entity());
    test_str(c.path().c_str(), "::Tag");
    test_assert(c != flecs::Tag);
}

template <typename T>
struct TemplateType { };

void World_template_component_name() {
    flecs::world ecs;

    auto c = ecs.component<TemplateType<Position>>();
    test_str(c.name().c_str(), "TemplateType<Position>");
    test_str(c.path().c_str(), "::TemplateType<Position>");
}

namespace ns {
template <typename T>
struct TemplateType { };
struct foo { };
}

void World_template_component_w_namespace_name() {
    flecs::world ecs;

    auto c = ecs.component<ns::TemplateType<Position>>();
    test_str(c.name().c_str(), "TemplateType<Position>");
    test_str(c.path().c_str(), "::ns::TemplateType<Position>");
}

void World_template_component_w_namespace_name_and_namespaced_arg() {
    flecs::world ecs;

    auto c = ecs.component<ns::TemplateType<ns::foo>>();
    test_str(c.name().c_str(), "TemplateType<ns::foo>");
    test_str(c.path().c_str(), "::ns::TemplateType<ns::foo>");
}

namespace foo {
template <typename T>
struct foo { };
struct bar { };
}

void World_template_component_w_same_namespace_name() {
    flecs::world ecs;

    auto c = ecs.component<foo::foo<Position>>();
    test_str(c.name().c_str(), "foo<Position>");
    test_str(c.path().c_str(), "::foo::foo<Position>");
}

void World_template_component_w_same_namespace_name_and_namespaced_arg() {
    flecs::world ecs;

    auto c = ecs.component<foo::foo<foo::bar>>();
    test_str(c.name().c_str(), "foo<foo::bar>");
    test_str(c.path().c_str(), "::foo::foo<foo::bar>");
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


void World_template_component_from_module_2_args() {
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

void World_entity_as_tag() {
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

void World_entity_w_name_as_tag() {
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

void World_entity_as_component() {
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

void World_entity_w_name_as_component() {
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

void World_entity_as_component_2_worlds() {
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

struct Parent {
    struct Child { };
};

void World_entity_as_namespaced_component_2_worlds() {
    flecs::world ecs_1;
    auto e_1 = ecs_1.entity<Parent>();
    test_assert(e_1.id() != 0);

    auto e_1_1 = ecs_1.entity<Parent::Child>();
    test_assert(e_1_1.id() != 0);

    flecs::world ecs_2;
    auto e_2 = ecs_2.entity<Parent>();
    test_assert(e_2.id() != 0);

    auto e_2_1 = ecs_2.entity<Parent::Child>();
    test_assert(e_2_1.id() != 0);

    test_assert(e_1 == e_2);
    test_assert(e_1 == ecs_1.component<Parent>());
    test_assert(e_2 == ecs_2.component<Parent>());

    test_assert(e_1_1 == e_2_1);
    test_assert(e_1_1 == ecs_1.component<Parent::Child>());
    test_assert(e_2_1 == ecs_2.component<Parent::Child>());
}

void World_entity_as_component_2_worlds_implicit_namespaced() {
    flecs::world ecs_1;
    auto e_1 = ecs_1.entity<Parent>();
    test_assert(e_1.id() != 0);

    ecs_1.entity().add<Parent::Child>();

    flecs::world ecs_2;
    auto e_2 = ecs_2.entity<Parent>();
    test_assert(e_2.id() != 0);

    ecs_2.entity().add<Parent::Child>();

    test_assert(e_1 == e_2);
    test_assert(e_1 == ecs_1.component<Parent>());
    test_assert(e_2 == ecs_2.component<Parent>());

    test_assert(ecs_1.component<Parent::Child>() == 
        ecs_2.component<Parent::Child>());
}

struct PositionDerived : Position {
    PositionDerived() { }
    PositionDerived(float x, float y) : Position{x, y} { }
};

void World_delete_with_id() {
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

void World_delete_with_type() {
    flecs::world ecs;

    auto e_1 = ecs.entity().add<Tag>();
    auto e_2 = ecs.entity().add<Tag>();
    auto e_3 = ecs.entity().add<Tag>();

    ecs.delete_with<Tag>();

    test_assert(!e_1.is_alive());
    test_assert(!e_2.is_alive());
    test_assert(!e_3.is_alive());
}

void World_delete_with_pair() {
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

void World_delete_with_pair_type() {
    flecs::world ecs;

    struct Rel { };
    struct Obj { };

    auto e_1 = ecs.entity().add<Rel, Obj>();
    auto e_2 = ecs.entity().add<Rel, Obj>();
    auto e_3 = ecs.entity().add<Rel, Obj>();

    ecs.delete_with<Rel, Obj>();

    test_assert(!e_1.is_alive());
    test_assert(!e_2.is_alive());
    test_assert(!e_3.is_alive());
}

void World_delete_with_implicit() {
    flecs::world ecs;

    ecs.delete_with<Tag>();

    test_assert(true);
}

void World_delete_with_pair_implicit() {
    flecs::world ecs;

    struct Rel { };
    struct Obj { };

    ecs.delete_with<Rel, Obj>();

    test_assert(true);
}

void World_remove_all_id() {
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

void World_remove_all_type() {
    flecs::world ecs;

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

void World_remove_all_pair() {
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

void World_remove_all_pair_type() {
    flecs::world ecs;

    struct Rel { };
    struct ObjA { };
    struct ObjB { };

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

void World_remove_all_implicit() {
    flecs::world ecs;

    ecs.remove_all<Tag>();

    test_assert(true);
}

void World_remove_all_pair_implicit() {
    flecs::world ecs;

    struct Rel { };
    struct Obj { };

    ecs.remove_all<Rel, Obj>();

    test_assert(true);
}

void World_get_scope() {
    flecs::world ecs;

    auto e = ecs.entity("scope");

    ecs.set_scope(e);

    auto s = ecs.get_scope();
    test_assert(s == e);
    test_str(s.name(), "scope");
}

void World_get_scope_type() {
    flecs::world ecs;

    ecs.set_scope<ParentScope>();

    auto s = ecs.get_scope();
    test_assert(s == ecs.id<ParentScope>());
    test_str(s.name(), "ParentScope");
}

struct Outer
{
    struct Inner { };
};

void World_register_namespace_after_component() {
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

void World_is_alive() {
    flecs::world ecs;

    auto e = ecs.entity();

    test_bool(ecs.is_alive(e), true);
    test_bool(ecs.is_alive(1000), false);

    e.destruct();

    test_bool(ecs.is_alive(e), false);
}

void World_is_valid() {
    flecs::world ecs;

    auto e = ecs.entity();

    test_bool(ecs.is_valid(e), true);
    test_bool(ecs.is_valid(1000), true);
    test_bool(ecs.is_valid(0), false);

    e.destruct();

    test_bool(ecs.is_valid(e), false);
}

void World_exists() {
    flecs::world ecs;

    auto e = ecs.entity();

    test_bool(ecs.exists(e), true);
    test_bool(ecs.exists(1000), false);
}

void World_get_alive() {
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

void World_ensure() {
    flecs::world ecs;

    auto e_1 = ecs.entity();
    e_1.destruct();
    test_assert(!e_1.is_alive());

    auto e_2 = ecs.entity();
    test_assert(e_1 != e_2);
    test_assert(e_1 == flecs::strip_generation(e_2));
    e_2.destruct();
    test_assert(!e_2.is_alive());

    auto e_3 = ecs.ensure(e_2);
    test_assert(e_2 == e_3);
    test_assert(e_3.is_alive());
}

void World_reset_all() {
    flecs::entity pos, vel;

    {
        flecs::world ecs;
        pos = ecs.component<Position>();
        vel = ecs.component<Velocity>();
    }

    test_assert(flecs::type_id<Position>() == pos);
    test_assert(flecs::type_id<Velocity>() == vel);

    flecs::reset();

    test_assert(flecs::type_id<Position>() == 0);

    /* Register components in opposite order, should result in different ids */
    {
        flecs::world ecs;
        test_assert(ecs.component<Position>() != 0);
        test_assert(ecs.component<Velocity>() != 0);
    }
}

void World_get_tick() {
    flecs::world ecs;

    test_int(ecs.tick(), 0);

    ecs.progress();

    test_int(ecs.tick(), 1);

    ecs.progress();

    test_int(ecs.tick(), 2);
}

struct Scope { };

struct FromScope { };

namespace Nested { 
    struct FromScope { };
}

void World_register_from_scope() {
    flecs::world ecs;

    ecs.set_scope<Scope>();
    auto c = ecs.component<FromScope>();
    ecs.set_scope(0);

    test_assert(c.has(flecs::ChildOf, ecs.id<Scope>()));
}

void World_register_nested_from_scope() {
    flecs::world ecs;

    ecs.set_scope<Scope>();
    auto c = ecs.component<Nested::FromScope>();
    ecs.set_scope(0);

    test_assert(c.has(flecs::ChildOf, ecs.id<Scope>()));
}

void World_register_w_root_name() {
    flecs::world ecs;

    auto c = ecs.component<Scope>("::Root");

    test_assert(!c.has(flecs::ChildOf, flecs::Wildcard));
    test_str(c.path().c_str(), "::Root");
}

void World_register_nested_w_root_name() {
    flecs::world ecs;

    auto c = ecs.component<Nested::FromScope>("::Root");

    test_assert(!c.has(flecs::ChildOf, flecs::Wildcard));
    test_str(c.path().c_str(), "::Root");
}

void World_set_lookup_path() {
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

void World_run_post_frame() {
    flecs::world ecs;

    int ctx = 10;

    ecs.system()
        .iter([&](flecs::iter& it) {
            it.world().run_post_frame([](flecs::world_t *w, void *ctx) {
                int *i = static_cast<int*>(ctx);
                test_int(*i, 10);
                i[0] ++;
            }, &ctx);
        });
    test_int(ctx, 10);

    ecs.progress();

    test_int(ctx, 11);
}

void World_component_w_low_id() {
    flecs::world ecs;

    flecs::entity p = ecs.component<Position>();

    test_assert(p.id() < ECS_HI_COMPONENT_ID);
}

void World_reregister_after_reset_w_hooks_and_in_use() {
    flecs::world ecs;

    ecs.component<Pod>();

    ecs.entity().add<Pod>();
    test_int(1, Pod::ctor_invoked);

    flecs::reset();

    ecs.component<Pod>();

    ecs.entity().add<Pod>();
    test_int(2, Pod::ctor_invoked);
}

void World_reregister_after_reset_w_hooks_and_in_use_implicit() {
    flecs::world ecs;

    ecs.component<Pod>();

    ecs.entity().add<Pod>();
    test_int(1, Pod::ctor_invoked);

    flecs::reset();

    ecs.entity().add<Pod>();
    test_int(2, Pod::ctor_invoked);
}

void World_get_ref() {
    flecs::world ecs;

    struct Space { int v; };
    ecs.set<Space>({12});

    auto space = ecs.get_ref<Space>();
    test_int(12, space->v);
}
