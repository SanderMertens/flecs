#include <cpp_api.h>

namespace ns {
class NestedModule {
public:
    NestedModule(flecs::world& world) {
        flecs::module<ns::NestedModule>(world, "ns::NestedModule");
        flecs::component<Velocity>(world, "Velocity");
    }
};

class SimpleModule {
public:
    SimpleModule(flecs::world& world) {
        flecs::module<ns::SimpleModule>(world, "ns::SimpleModule");
        flecs::import<ns::NestedModule>(world);
        flecs::component<Position>(world, "Position");
    }
};

class NestedTypeModule {
public:
    struct NestedType { };

    NestedTypeModule(flecs::world& world) {
        world.module<NestedTypeModule>();
        world.component<NestedType>();
    }
};
}

void Module_import() {
    flecs::world world;
    auto m = flecs::import<ns::SimpleModule>(world);
    test_assert(m.id() != 0);
    test_str(m.path().c_str(), "::ns::SimpleModule");
    test_assert(m.has(flecs::Module));

    auto e = flecs::entity(world)
        .add<Position>();
    test_assert(e.id() != 0);
    test_assert(e.has<Position>());
}

void Module_lookup_from_scope() {
    flecs::world world;
    flecs::import<ns::SimpleModule>(world);

    auto ns_entity = world.lookup("ns");
    test_assert(ns_entity.id() != 0);

    auto module_entity = world.lookup("ns::SimpleModule");
    test_assert(module_entity.id() != 0);

    auto position_entity = world.lookup("ns::SimpleModule::Position");
    test_assert(position_entity.id() != 0);

    auto nested_module = ns_entity.lookup("SimpleModule");
    test_assert(module_entity.id() == nested_module.id());

    auto module_position = module_entity.lookup("Position");
    test_assert(position_entity.id() == module_position.id());

    auto ns_position = ns_entity.lookup("SimpleModule::Position");
    test_assert(position_entity.id() == ns_position.id());    
}

void Module_nested_module() {
    flecs::world world;
    flecs::import<ns::SimpleModule>(world);

    auto velocity = world.lookup("ns::NestedModule::Velocity");
    test_assert(velocity.id() != 0);

    test_str(velocity.path().c_str(), "::ns::NestedModule::Velocity");
}

void Module_nested_type_module() {
    flecs::world world;
    world.import<ns::NestedTypeModule>();

    auto ns_entity = world.lookup("ns");
    test_assert(ns_entity.id() != 0);

    auto module_entity = world.lookup("ns::NestedTypeModule");
    test_assert(module_entity.id() != 0);

    auto type_entity = world.lookup("ns::NestedTypeModule::NestedType");
    test_assert(type_entity.id() != 0);

    int32_t childof_count = 0;
    type_entity.each(flecs::ChildOf, [&](flecs::entity) {
        childof_count ++;
    });

    test_int(childof_count, 1);
}
