#include <cpp.h>

void Paths_name(void) {
    flecs::world world;

    auto e = flecs::entity(world, "foo");
    test_str(e.name().c_str(), "foo");

    auto e_world = world.lookup("foo");
    test_assert(e.id() == e_world.id());

    e_world = world.lookup("::foo");
    test_assert(e.id() == e_world.id());
}

void Paths_path_depth_1(void) {
    flecs::world world;

    auto e = flecs::entity(world, "foo::bar");
    test_str(e.name().c_str(), "bar");
    test_str(e.path().c_str(), "::foo::bar");

    auto e_world = world.lookup("bar");
    test_assert(0 == e_world.id());

    e_world = world.lookup("foo::bar");
    test_assert(e.id() == e_world.id());  

    e_world = world.lookup("::foo::bar");
    test_assert(e.id() == e_world.id());        
}

void Paths_path_depth_2(void) {
    flecs::world world;

    auto e = flecs::entity(world, "foo::bar::hello");
    test_str(e.name().c_str(), "hello");
    test_str(e.path().c_str(), "::foo::bar::hello");

    auto e_world = world.lookup("hello");
    test_assert(0 == e_world.id());

    e_world = world.lookup("foo::bar::hello");
    test_assert(e.id() == e_world.id());  

    e_world = world.lookup("::foo::bar::hello");
    test_assert(e.id() == e_world.id());  
}

void Paths_entity_lookup_name(void) {
    flecs::world world;

    auto parent = flecs::entity(world, "foo");
    test_str(parent.name().c_str(), "foo");
    test_str(parent.path().c_str(), "::foo");    

    auto e = flecs::entity(world, "foo::bar");
    test_str(e.name().c_str(), "bar");
    test_str(e.path().c_str(), "::foo::bar");

    auto parent_e = parent.lookup("bar");
    test_assert(e.id() == parent_e.id());

    parent_e = parent.lookup("::foo::bar");
    test_assert(e.id() == parent_e.id());
}

void Paths_entity_lookup_depth_1(void) {
    flecs::world world;

    auto parent = flecs::entity(world, "foo");
    test_str(parent.name().c_str(), "foo");
    test_str(parent.path().c_str(), "::foo");    

    auto e = flecs::entity(world, "foo::bar::hello");
    test_str(e.name().c_str(), "hello");
    test_str(e.path().c_str(), "::foo::bar::hello");

    auto parent_e = parent.lookup("bar::hello");
    test_assert(e.id() == parent_e.id());

    parent_e = parent.lookup("::foo::bar::hello");
    test_assert(e.id() == parent_e.id());
}

void Paths_entity_lookup_depth_2(void) {
    flecs::world world;

    auto parent = flecs::entity(world, "foo");
    test_str(parent.name().c_str(), "foo");
    test_str(parent.path().c_str(), "::foo");    

    auto e = flecs::entity(world, "foo::bar::hello::world");
    test_str(e.name().c_str(), "world");
    test_str(e.path().c_str(), "::foo::bar::hello::world");

    auto parent_e = parent.lookup("bar::hello::world");
    test_assert(e.id() == parent_e.id());

    parent_e = parent.lookup("::foo::bar::hello::world");
    test_assert(e.id() == parent_e.id());
}

void Paths_entity_lookup_from_0(void) {
    install_test_abort();
    flecs::world world;

    flecs::entity foo = world.entity("foo");
    test_assert(world.lookup("foo") == foo);

    flecs::entity dummy;

    test_expect_abort();
    dummy.lookup("foo");
}

void Paths_entity_lookup_from_0_w_world(void) {
    install_test_abort();
    flecs::world world;

    flecs::entity foo = world.entity("foo");
    test_assert(world.lookup("foo") == foo);

    flecs::entity dummy = world.entity(0);

    test_expect_abort();
    dummy.lookup("foo");
}

void Paths_alias_component(void) {
    flecs::world ecs;

    auto e = ecs.use<Position>("MyPosition");
    auto a = ecs.lookup("MyPosition");
    auto c = ecs.lookup("Position");

    test_assert(e.id() == a.id());
    test_assert(e.id() == c.id());
}

namespace test {
    struct Foo {
        float x;
        float y;
    };
}

void Paths_alias_scoped_component(void) {
    flecs::world ecs;

    auto e = ecs.use<test::Foo>();
    auto a = ecs.lookup("Foo");
    auto c = ecs.lookup("test::Foo");
    
    test_assert(e.id() == a.id());
    test_assert(e.id() == c.id());
}

void Paths_alias_scoped_component_w_name(void) {
    flecs::world ecs;

    auto e = ecs.use<test::Foo>("FooAlias");
    auto a = ecs.lookup("FooAlias");
    auto f = ecs.lookup("Foo");
    auto c = ecs.lookup("test::Foo");
    
    test_assert(e.id() == a.id());
    test_assert(e.id() == c.id());
    test_assert(f.id() == 0);
}

void Paths_alias_entity(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    ecs.use(e, "FooAlias");

    auto a = ecs.lookup("FooAlias");

    test_assert(e.id() == a.id());
}

void Paths_alias_entity_by_name(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    ecs.use(e, "FooAlias");

    auto l = ecs.lookup("FooAlias");

    test_assert(e.id() == l.id());
}

void Paths_alias_entity_by_scoped_name(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo::Bar");

    auto a = ecs.use("Foo::Bar", "FooAlias");

    auto l = ecs.lookup("FooAlias");

    test_assert(e.id() == a.id());
    test_assert(e.id() == l.id());
}

void Paths_alias_entity_empty(void) {
    flecs::world ecs;

    auto parent = ecs.entity("parent");
    auto child = ecs.entity("child").child_of(parent);

    auto e = ecs.lookup("child");

    test_assert(e.id() == 0);

    ecs.use(child); // alias being nullptr

    e = ecs.lookup("child");

    test_assert(e.id() != 0);

    ecs.use(child, "FooAlias");

    e = ecs.lookup("child");

    test_assert(e.id() == 0);

    e = ecs.lookup("FooAlias");

    test_assert(e.id() != 0);
}

