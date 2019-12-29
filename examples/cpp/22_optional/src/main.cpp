#include <optional.h>
#include <iostream>

struct Health {
    float value;
};

struct Stamina {
    float value;
};

struct Mana {
    float value;
};

void Regenerate(flecs::rows& rows) {
    flecs::column<Health> health(rows, 1);
    flecs::column<Stamina> stamina(rows, 2);
    flecs::column<Mana> mana(rows, 3);

    for (auto row : rows) {
        if (health.is_set()) {
            health[row].value ++;
            std::cout << rows.entity(row).name() << " process health" << std::endl;
        }

        if (stamina.is_set()) {
            stamina[row].value ++;
            std::cout << rows.entity(row).name() << " process stamina" << std::endl;
        }

        if (mana.is_set()) {
            mana[row].value ++;
            std::cout << rows.entity(row).name() << " process mana" << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    /* Register components */
    flecs::component<Health>(world, "Health");
    flecs::component<Stamina>(world, "Stamina");
    flecs::component<Mana>(world, "Mana");

    /* Create system with three optional columns */
    flecs::system<>(world).signature("?Health, ?Stamina, ?Mana").action(Regenerate);

    /* Create three entities that will all match with the Regenerate system */
    flecs::entity(world, "HealthEntity").set<Health>({0});
    flecs::entity(world, "StaminaEntity").set<Stamina>({0});
    flecs::entity(world, "ManaEntity").set<Mana>({0});

    world.set_target_fps(1);

    std::cout << "Application optional is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (world.progress()) { }
}
