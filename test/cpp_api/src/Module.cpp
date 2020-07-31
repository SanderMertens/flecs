#include <cpp_api.h>

namespace ns {
class SimpleModule {
public:
    SimpleModule(flecs::world& world) {
        flecs::module<ns::SimpleModule>(world, "ns::SimpleModule");
        flecs::component<Position>(world, "Position");
    }
};
}

void Module_import() {
    flecs::world world;
    flecs::import<ns::SimpleModule>(world);

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
