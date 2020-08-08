#include <cpp_api.h>

typedef struct Trait {
    float value;
} Trait;

void Traits_add_component_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait<Position, Trait>();

    test_assert(entity.id() != 0);
    test_assert((entity.has_trait<Position, Trait>()));
    test_assert((!entity.has_trait<Trait, Position>()));

    test_str(entity.type().str().c_str(), "TRAIT|Position<Trait");
}

void Traits_add_tag_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait<Position>(Trait);

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait<Position>(Trait));
    test_str(entity.type().str().c_str(), "TRAIT|Position<Trait");
}

void Traits_add_tag_trait_to_tag() {
    flecs::world world;

    auto Tag = flecs::entity(world, "Tag");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait(Tag, Trait);

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait(Tag, Trait));
    test_str(entity.type().str().c_str(), "TRAIT|Tag<Trait");
}

void Traits_remove_component_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait<Position, Trait>();

    test_assert(entity.id() != 0);
    test_assert((entity.has_trait<Position, Trait>()));
    test_assert((!entity.has_trait<Trait, Position>()));

    test_str(entity.type().str().c_str(), "TRAIT|Position<Trait");

    entity.remove_trait<Position, Trait>();
    test_assert(!(entity.has_trait<Position, Trait>()));
}

void Traits_remove_tag_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait<Position>(Trait);

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait<Position>(Trait));
    test_str(entity.type().str().c_str(), "TRAIT|Position<Trait");

    entity.remove_trait<Position>(Trait);
    test_assert(!entity.has_trait<Position>(Trait));
}

void Traits_remove_tag_trait_to_tag() {
    flecs::world world;

    auto Tag = flecs::entity(world, "Tag");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait(Tag, Trait);

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait(Tag, Trait));
    test_str(entity.type().str().c_str(), "TRAIT|Tag<Trait");

    entity.remove_trait(Tag, Trait);
    test_assert(!entity.has_trait(Tag, Trait));
}

void Traits_set_component_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    auto entity = flecs::entity(world)
        .set_trait<Position, Trait>({10});

    test_assert(entity.id() != 0);
    test_assert((entity.has_trait<Position, Trait>()));
    test_assert((!entity.has_trait<Trait, Position>()));

    test_str(entity.type().str().c_str(), "TRAIT|Position<Trait");

    const Trait *t = entity.get_trait<Position, Trait>();
    test_int(t->value, 10);
}

void Traits_set_tag_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .set_trait<Position>(Trait, {10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait<Position>(Trait));
    test_str(entity.type().str().c_str(), "TRAIT|Position<Trait");

    const Position *p = entity.get_trait<Position>(Trait);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Traits_system_1_trait_instance() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    flecs::entity(world)
        .set_trait<Position, Trait>({10});

    int invoke_count = 0;
    int entity_count = 0;
    int trait_value = 0;

    flecs::system<>(world, nullptr, "TRAIT | Trait")
        .action([&](flecs::iter it) {
            flecs::column<Trait> tr(it, 1);
            invoke_count ++;            
            for (auto i : it) {
                entity_count ++;
                trait_value = tr[i].value;
            }
        });

    world.progress();

    test_int(invoke_count, 1);
    test_int(entity_count, 1);
    test_int(trait_value, 10);
}

void Traits_system_2_trait_instances() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Position>(world, "Velocity");
    flecs::component<Trait>(world, "Trait");

    flecs::entity(world)
        .set_trait<Position, Trait>({10})
        .set_trait<Velocity, Trait>({20});

    int invoke_count = 0;
    int entity_count = 0;
    int trait_value = 0;

    flecs::system<>(world, nullptr, "TRAIT | Trait")
        .action([&](flecs::iter it) {
            flecs::column<Trait> tr(it, 1);
            invoke_count ++;            
            for (auto i : it) {
                entity_count ++;
                trait_value += tr[i].value;
            }
        });

    world.progress();

    test_int(invoke_count, 2);
    test_int(entity_count, 2);
    test_int(trait_value, 30);
}
