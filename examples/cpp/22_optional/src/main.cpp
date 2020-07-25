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

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    /* Register components */
    flecs::component<Health>(world, "Health");
    flecs::component<Stamina>(world, "Stamina");
    flecs::component<Mana>(world, "Mana");

    /* Create system with three optional columns. Pointer template arguments are
     * converted to components with the optional operator. 
     *
     * This is an extreme example where all components are optional, which means
     * that this system will evaluate all entities. */
    flecs::system<Health*, Stamina*, Mana*>(world)
        .each([](flecs::entity e, Health* h, Stamina *s, Mana *m) {
            if (h) {
                h->value ++;
                std::cout << e.name() << " process health" << std::endl;
            }

            if (s) {
                s->value ++;
                std::cout << e.name() << " process stamina" << std::endl;
            }

            if (m) {
                m->value ++;
                std::cout << e.name() << " process mana" << std::endl;
            }
        });

    /* Create three entities that will all match with the Regenerate system */
    flecs::entity(world, "HealthEntity").set<Health>({0});
    flecs::entity(world, "StaminaEntity").set<Stamina>({0});
    flecs::entity(world, "ManaEntity").set<Mana>({0});

    world.set_target_fps(1);

    std::cout << "Application optional is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (world.progress()) { }
}
