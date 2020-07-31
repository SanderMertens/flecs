#include <cpp_api.h>

void Paths_name() {
    flecs::world world;

    auto e = flecs::entity(world, "foo");
    test_str(e.name().c_str(), "foo");

    auto e_world = world.lookup("foo");
    test_assert(e.id() == e_world.id());

    e_world = world.lookup("::foo");
    test_assert(e.id() == e_world.id());
}

void Paths_path_depth_1() {
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

void Paths_path_depth_2() {
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

void Paths_entity_lookup_name() {
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

void Paths_entity_lookup_depth_1() {
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

void Paths_entity_lookup_depth_2() {
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
