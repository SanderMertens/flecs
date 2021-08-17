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
    ecs_component_desc_t module_desc = {};
    module_desc.entity.name = "TestInteropModule";
    module_desc.size = sizeof(TestInteropModule);
    module_desc.alignment = alignof(TestInteropModule);
    ecs_entity_t m = ecs_module_init(world, &module_desc);
    ecs_set_scope(world, m);

    ecs_component_desc_t desc = {};
    desc.entity.name = "Position";
    desc.entity.symbol = "Position";
    desc.size = sizeof(Position);
    desc.alignment = alignof(Position);
    ecs_component_init(world, &desc);
}

namespace test {
namespace interop {

class module : TestInteropModule {
public:
    module(flecs::world& ecs) {
        TestInteropModuleImport(ecs.c_ptr());

        ecs.module<test::interop::module>();

        ecs.component<Position>("::test::interop::module::Position");
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
    test_assert(ecs.component<flecs::Type>() == ecs_id(EcsType));
    test_assert(ecs.component<flecs::Trigger>() == ecs_id(EcsTrigger));
    test_assert(ecs.component<flecs::Observer>() == ecs_id(EcsObserver));
    test_assert(ecs.component<flecs::Query>() == ecs_id(EcsQuery));
    test_assert(ecs.component<flecs::RateFilter>() == ecs_id(EcsRateFilter));
    test_assert(ecs.component<flecs::TickSource>() == ecs_id(EcsTickSource));
    test_assert(flecs::Name == EcsName);
    test_assert(flecs::Symbol == EcsSymbol);
    test_assert(flecs::System == ecs_id(EcsSystem));
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


void World_type_id() {
    flecs::world ecs;

    auto p = ecs.component<Position>();

    test_assert(p.id() == flecs::type_id<Position>());
}

void World_different_comp_same_name() {
    flecs::world ecs;

    install_test_abort();
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
    flecs::world ecs;

    install_test_abort();
    test_expect_abort();    

    ecs.component<Position>("Position");

    // Simulate different binary
    flecs::_::cpp_type<Position>::reset();

    ecs.component<Position>("Velocity");
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

void World_staged_count() {
    flecs::world ecs;

    flecs::world stage = ecs.get_stage(0);

    ecs.staging_begin();

    test_int(stage.count<Position>(), 0);

    ecs.staging_end();

    ecs.staging_begin();

    stage.entity().add<Position>();
    stage.entity().add<Position>();
    stage.entity().add<Position>();
    stage.entity().add<Position>().add<Mass>();
    stage.entity().add<Position>().add<Mass>();
    stage.entity().add<Position>().add<Velocity>();

    test_int(stage.count<Position>(), 0);

    ecs.staging_end();

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

void World_entity_as_tag() {
    flecs::world ecs;

    auto e = ecs.entity()
        .component<Tag>();
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

    auto e = ecs.entity("Foo")
        .component<Tag>();
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

    auto e = ecs.entity()
        .component<Position>();
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

    auto e = ecs.entity("Foo")
        .component<Position>();
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
    auto e_1 = ecs_1.entity()
        .component<Position>();
    test_assert(e_1.id() != 0);

    flecs::world ecs_2;
    auto e_2 = ecs_2.entity()
        .component<Position>();
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
    auto e_1 = ecs_1.entity()
        .component<Parent>();
    test_assert(e_1.id() != 0);

    auto e_1_1 = ecs_1.entity()
        .component<Parent::Child>();
    test_assert(e_1_1.id() != 0);

    flecs::world ecs_2;
    auto e_2 = ecs_2.entity()
        .component<Parent>();
    test_assert(e_2.id() != 0);

    auto e_2_1 = ecs_2.entity()
        .component<Parent::Child>();
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
    auto e_1 = ecs_1.entity()
        .component<Parent>();
    test_assert(e_1.id() != 0);

    ecs_1.entity().add<Parent::Child>();

    flecs::world ecs_2;
    auto e_2 = ecs_2.entity()
        .component<Parent>();
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

void World_component_as_component() {
    flecs::world ecs;

    auto e = ecs.component<Position>()
        .component<PositionDerived>();
    test_assert(e.id() != 0);

    auto t = ecs.component<Position>();
    test_assert(t.id() != 0);
    test_assert(e == t);

    auto e2 = ecs.entity()
        .set<PositionDerived>({10, 20});

    test_bool(e2.has<Position>(), true);
    test_bool(e2.has<PositionDerived>(), true);

    const Position *p = e2.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_str(e.name(), "Position");
}

void World_type_as_component() {
    flecs::world ecs;

    auto e = ecs.type()
        .component<Position>();
    test_assert(e.id() != 0);

    auto t = ecs.component<Position>();
    test_assert(t.id() != 0);
    test_assert(e.id() == t);

    auto e2 = ecs.entity()
        .set<Position>({10, 20});

    test_bool(e2.has<Position>(), true);

    test_str(t.name(), "Position");
}

void World_type_w_name_as_component() {
    flecs::world ecs;

    auto e = ecs.type("Foo")
        .component<Position>();
    test_assert(e.id() != 0);

    auto t = ecs.component<Position>();
    test_assert(t.id() != 0);
    test_assert(e.id() == t);

    auto e2 = ecs.entity()
        .set<Position>({10, 20});

    test_bool(e2.has<Position>(), true);

    test_str(t.name(), "Foo");
}
