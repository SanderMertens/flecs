#include <cpp_api.h>

typedef struct Trait {
    float value;
} Trait;

void Traits_add_component_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait<Trait, Position>();

    test_assert(entity.id() != 0);
    test_assert((entity.has_trait<Trait, Position>()));
    test_assert((!entity.has_trait<Position, Trait>()));

    test_str(entity.type().str().c_str(), "TRAIT|Trait>Position");
}

void Traits_add_tag_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait_tag<Position>(Trait);

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait_tag<Position>(Trait));
    test_assert(!entity.has_trait<Position>(Trait));
    test_str(entity.type().str().c_str(), "TRAIT|Trait>Position");
}

void Traits_add_tag_trait_to_tag() {
    flecs::world world;

    auto Tag = flecs::entity(world, "Tag");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait(Trait, Tag);

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait(Trait, Tag));
    test_str(entity.type().str().c_str(), "TRAIT|Trait>Tag");
}

void Traits_remove_component_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait<Trait, Position>();

    test_assert(entity.id() != 0);
    test_assert((entity.has_trait<Trait, Position>()));
    test_assert((!entity.has_trait<Position, Trait>()));

    test_str(entity.type().str().c_str(), "TRAIT|Trait>Position");

    entity.remove_trait<Position, Trait>();
    test_assert(!(entity.has_trait<Position, Trait>()));
}

void Traits_remove_tag_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait_tag<Position>(Trait);

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait_tag<Position>(Trait));
    test_assert(!entity.has_trait<Position>(Trait));
    test_str(entity.type().str().c_str(), "TRAIT|Trait>Position");

    entity.remove_trait<Position>(Trait);
    test_assert(!entity.has_trait<Position>(Trait));
}

void Traits_remove_tag_trait_to_tag() {
    flecs::world world;

    auto Tag = flecs::entity(world, "Tag");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .add_trait(Trait, Tag);

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait(Trait, Tag));
    test_str(entity.type().str().c_str(), "TRAIT|Trait>Tag");

    entity.remove_trait(Tag, Trait);
    test_assert(!entity.has_trait(Tag, Trait));
}

void Traits_set_component_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    auto entity = flecs::entity(world)
        .set_trait<Trait, Position>({10});

    test_assert(entity.id() != 0);
    test_assert((entity.has_trait<Trait, Position>()));
    test_assert((!entity.has_trait<Position, Trait>()));

    test_str(entity.type().str().c_str(), "TRAIT|Trait>Position");

    const Trait *t = entity.get_trait<Trait, Position>();
    test_int(t->value, 10);
}

void Traits_set_tag_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Trait = flecs::entity(world, "Trait");

    auto entity = flecs::entity(world)
        .set_trait_tag<Position>(Trait, {10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has_trait_tag<Position>(Trait));
    test_str(entity.type().str().c_str(), "TRAIT|Trait>Position");

    const Position *p = entity.get_trait_tag<Position>(Trait);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Traits_system_1_trait_instance() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    flecs::entity(world)
        .set_trait<Trait, Position>({10});

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
    flecs::component<Velocity>(world, "Velocity");
    flecs::component<Trait>(world, "Trait");

    flecs::entity(world)
        .set_trait<Trait, Position>({10})
        .set_trait<Trait, Velocity>({20});

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

void Traits_override_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    auto base = flecs::entity(world)
        .set_trait<Trait, Position>({10});

    auto instance = flecs::entity(world)
        .add_instanceof(base);

    test_assert((instance.has_trait<Trait, Position>()));
    const Trait *t = instance.get_trait<Trait, Position>();
    test_int(t->value, 10);

    const Trait *t_2 = base.get_trait<Trait, Position>();
    test_assert(t == t_2);

    instance.add_trait<Trait, Position>();
    t = instance.get_trait<Trait, Position>();
    test_int(t->value, 10);
    test_assert(t != t_2);

    instance.remove_trait<Trait, Position>();
    t = instance.get_trait<Trait, Position>();
    test_int(t->value, 10);
    test_assert(t == t_2);    
}

void Traits_override_tag_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Trait = flecs::entity(world);

    auto base = flecs::entity(world)
        .set_trait_tag<Position>(Trait, {10, 20});

    auto instance = flecs::entity(world)
        .add_instanceof(base);

    test_assert((instance.has_trait_tag<Position>(Trait)));
    const Position *t = instance.get_trait_tag<Position>(Trait);
    test_int(t->x, 10);
    test_int(t->y, 20);

    const Position *t_2 = base.get_trait_tag<Position>(Trait);
    test_assert(t == t_2);

    instance.add_trait_tag<Position>(Trait);
    t = instance.get_trait_tag<Position>(Trait);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t != t_2);

    instance.remove_trait_tag<Position>(Trait);
    t = instance.get_trait_tag<Position>(Trait);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t == t_2); 
}

void Traits_get_mut_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    auto e = flecs::entity(world);

    bool added = false;
    Trait *t = e.get_trait_mut<Trait, Position>(&added);
    test_assert(t != NULL);
    test_bool(added, true);
    t->value = 10;

    const Trait *t_2 = e.get_trait<Trait, Position>();
    test_assert(t == t_2);
    test_int(t->value, 10);
}

void Traits_get_mut_trait_existing() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Trait>(world, "Trait");

    auto e = flecs::entity(world)
        .set_trait<Trait, Position>({20});

    bool added = false;
    Trait *t = e.get_trait_mut<Trait, Position>(&added);
    test_assert(t != NULL);
    test_bool(added, false);
    test_int(t->value, 20);
    t->value = 10;

    const Trait *t_2 = e.get_trait<Trait, Position>();
    test_assert(t == t_2);
    test_int(t->value, 10);
}

void Traits_get_mut_trait_tag() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Trait = flecs::entity(world);

    auto e = flecs::entity(world);

    bool added = false;
    Position *p = e.get_trait_tag_mut<Position>(Trait, &added);
    test_assert(p != NULL);
    test_bool(added, true);
    p->x = 10;
    p->y = 20;

    const Position *p_2 = e.get_trait_tag<Position>(Trait);
    test_assert(p == p_2);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Traits_get_mut_trait_tag_existing() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Trait = flecs::entity(world);

    auto e = flecs::entity(world)
        .set_trait_tag<Position>(Trait, {10, 20});

    bool added = false;
    Position *p = e.get_trait_tag_mut<Position>(Trait, &added);
    test_assert(p != NULL);
    test_bool(added, false);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_2 = e.get_trait_tag<Position>(Trait);
    test_assert(p == p_2);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Traits_type_w_trait() {
    flecs::world world;

    auto Type = flecs::type(world)
        .add_trait<Trait, Position>();

    auto e = flecs::entity(world)
        .add(Type);

    test_assert((e.has_trait<Trait, Position>()));
}

void Traits_type_w_trait_tag() {
    flecs::world world;

    auto Tag = flecs::entity(world);

    auto Type = flecs::type(world)
        .add_trait<Trait>(Tag);

    auto e = flecs::entity(world)
        .add(Type);

    test_assert((e.has_trait<Trait>(Tag)));
}

void Traits_type_w_trait_tags() {
    flecs::world world;

    auto Tag = flecs::entity(world);
    auto Trait = flecs::entity(world);

    auto Type = flecs::type(world)
        .add_trait(Trait, Tag);

    auto e = flecs::entity(world)
        .add(Type);

    test_assert((e.has_trait(Trait, Tag)));
}

void Traits_type_w_tag_trait() {
    flecs::world world;

    auto Tag = flecs::entity(world);

    auto Type = flecs::type(world)
        .add_trait<Trait>(Tag);

    auto e = flecs::entity(world)
        .add(Type);

    test_assert((e.has_trait<Trait>(Tag)));
}

void Traits_override_trait_w_type() {
    flecs::world world;

    auto Prefab = flecs::prefab(world, "Prefab")
        .set_trait<Trait, Position>({10});

    auto Type = flecs::type(world)
        .add_instanceof(Prefab)
        .add_trait<Trait, Position>();

    auto e = flecs::entity(world)
        .add(Type);

    test_assert((e.has_trait<Trait, Position>()));

    const Trait *t_1 = Prefab.get_trait<Trait, Position>();
    test_assert(t_1 != nullptr);
    test_int(t_1->value, 10);

    const Trait *t_2 = e.get_trait<Trait, Position>();
    test_assert(t_2 != nullptr);

    test_assert(t_1 != t_2);
    test_int(t_2->value, 10);
}

void Traits_override_trait_w_type_tag() {
    flecs::world world;

    auto Tag = flecs::entity(world);

    auto Prefab = flecs::prefab(world, "Prefab")
        .set_trait<Trait>({10}, Tag);

    auto Type = flecs::type(world)
        .add_instanceof(Prefab)
        .add_trait<Trait>(Tag);

    auto e = flecs::entity(world)
        .add(Type);

    test_assert((e.has_trait<Trait>(Tag)));

    const Trait *t_1 = Prefab.get_trait<Trait>(Tag);
    test_assert(t_1 != nullptr);
    test_int(t_1->value, 10);

    const Trait *t_2 = e.get_trait<Trait>(Tag);
    test_assert(t_2 != nullptr);

    test_assert(t_1 != t_2);
    test_int(t_2->value, 10);    
}

void Traits_override_tag_trait_w_type() {
    flecs::world world;

    auto Trait = flecs::entity(world);

    auto Prefab = flecs::prefab(world, "Prefab")
        .set_trait_tag<Position>(Trait, {10, 20});

    auto Type = flecs::type(world)
        .add_instanceof(Prefab)
        .add_trait_tag<Position>(Trait);

    auto e = flecs::entity(world)
        .add(Type);

    test_assert((e.has_trait_tag<Position>(Trait)));

    const Position *p_1 = Prefab.get_trait_tag<Position>(Trait);
    test_assert(p_1 != nullptr);
    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Position *p_2 = e.get_trait_tag<Position>(Trait);
    test_assert(p_2 != nullptr);

    test_assert(p_1 != p_2);
    test_int(p_2->x, 10);
    test_int(p_2->y, 20);
}
