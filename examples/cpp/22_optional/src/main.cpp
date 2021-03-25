#include <optional.h>
#include <iostream>

struct Health {
    double value;
};

struct Stamina {
    double value;
};

struct Mana {
    double value;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* Create system with three optional columns. Pointer template arguments are
     * converted to components with the optional operator. 
     *
     * This is an extreme example where all components are optional, which means
     * that this system will evaluate all entities. */
    ecs.system<Health*, Stamina*, Mana*>()
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
    ecs.entity("HealthEntity").set<Health>({0});
    ecs.entity("StaminaEntity").set<Stamina>({0});
    ecs.entity("ManaEntity").set<Mana>({0});

    ecs.set_target_fps(1);

    std::cout << "Application optional is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (ecs.progress()) { }
}
