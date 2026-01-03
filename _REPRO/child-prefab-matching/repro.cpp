#include <iostream>
#include <cstdint>

#include "../../distr/flecs.h"

int main(int argc, char* argv[]) {
    flecs::world world;

    struct HitPoints {
        float value;
    };
    world.component<HitPoints>("HitPoints")
        .member<float>("value")
        .set<HitPoints>({ float(10.0) });

    flecs::entity enemy = world.prefab("Enemy");

    if (world.script_run_file("Troll.flecs") != 0) {
        std::cerr << "Error running script" << std::endl;
        return 1;
    }

    flecs::entity Troll = world.lookup("Troll");
    flecs::entity instance = world.entity().is_a(Troll);

    auto q = world.query_builder()
        .with(flecs::IsA, enemy);

    // The original pattern as in https://github.com/VoxInteractive/Surwave/blob/4aa069c7fb4ccb292ffe0cd4690ec6023ccb1be6/Game/cpp/systems/enemy_death.h#L44
    q.each([](flecs::entity e) {
        std::cout << e.target(flecs::IsA).name() << std::endl; // prints Troll
    });

    // Recommended pattern (faster) - https://discord.com/channels/633826290415435777/1455553733978099763/1455641967605059854
    q.each([](flecs::iter& it, size_t row) {
        std::cout << it.pair(0).second().name() << std::endl; // prints Troll
    });

    auto repro_system = world.system<const HitPoints>("Repro")
        .with(flecs::IsA, enemy)
        .kind(0)
        .run([](flecs::iter& it) {

        while (it.next()) {
            for (auto entity_index : it) {
                flecs::entity entity = it.entity(static_cast<std::int32_t>(entity_index));

                // The original pattern
                const flecs::entity prefab_entity_via_entity_target = entity.target(flecs::IsA);

                // Recommended pattern
                const flecs::entity prefab_entity_via_it_pair = it.pair(1).second();

                std::cout << prefab_entity_via_entity_target.name() << std::endl; // prints Troll
                std::cout << prefab_entity_via_it_pair.name() << std::endl; // !!! prints Enemy
            }
        }
    });

    repro_system.run();

    return 0;
}
