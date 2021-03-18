#include <cpp_api.h>

typedef struct Pair {
    float value;
} Pair;

void Traits_add_component_trait() {
    flecs::world world;

    auto entity = world.entity()
        .add<Pair, Position>();

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");
}

void Traits_add_tag_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Pair = world.entity("Pair");

    auto entity = world.entity()
        .add_object<Position>(Pair);

    test_assert(entity.id() != 0);
    test_assert(entity.has_object<Position>(Pair));
    test_assert(!entity.has<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");
}

void Traits_add_tag_trait_to_tag() {
    flecs::world world;

    auto Tag = world.entity("Tag");
    auto Pair = world.entity("Pair");

    auto entity = world.entity()
        .add(Pair, Tag);

    test_assert(entity.id() != 0);
    test_assert(entity.has(Pair, Tag));
    test_str(entity.type().str().c_str(), "(Pair,Tag)");
}

void Traits_remove_component_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Pair>(world, "Pair");

    auto entity = world.entity()
        .add<Pair, Position>();

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");

    entity.remove_trait<Position, Pair>();
    test_assert(!(entity.has<Position, Pair>()));
}

void Traits_remove_tag_trait() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Pair = world.entity("Pair");

    auto entity = world.entity()
        .add_object<Position>(Pair);

    test_assert(entity.id() != 0);
    test_assert(entity.has_object<Position>(Pair));
    test_assert(!entity.has<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");

    entity.remove_trait<Position>(Pair);
    test_assert(!entity.has<Position>(Pair));
}

void Traits_remove_tag_trait_to_tag() {
    flecs::world world;

    auto Tag = world.entity("Tag");
    auto Pair = world.entity("Pair");

    auto entity = world.entity()
        .add(Pair, Tag);

    test_assert(entity.id() != 0);
    test_assert(entity.has(Pair, Tag));
    test_str(entity.type().str().c_str(), "(Pair,Tag)");

    entity.remove_trait(Tag, Pair);
    test_assert(!entity.has(Tag, Pair));
}

void Traits_set_component_trait() {
    flecs::world world;

    auto entity = world.entity()
        .set<Pair, Position>({10});

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");

    const Pair *t = entity.get<Pair, Position>();
    test_int(t->value, 10);
}

void Traits_set_tag_trait() {
    flecs::world world;

    auto Pair = world.entity("Pair");

    auto entity = world.entity()
        .set_object<Position>(Pair, {10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has_object<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");

    const Position *p = entity.get_object<Position>(Pair);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Traits_system_1_trait_instance() {
    flecs::world world;

    world.entity()
        .set<Pair, Position>({10});

    int invoke_count = 0;
    int entity_count = 0;
    int trait_value = 0;

    flecs::system<>(world, nullptr, "PAIR | Pair")
        .iter([&](flecs::iter it) {
            flecs::column<Pair> tr(it, 1);
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

    world.entity()
        .set<Pair, Position>({10})
        .set<Pair, Velocity>({20});

    int invoke_count = 0;
    int entity_count = 0;
    int trait_value = 0;

    flecs::system<>(world, nullptr, "PAIR | Pair")
        .iter([&](flecs::iter it) {
            flecs::column<Pair> tr(it, 1);
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

    auto base = world.entity()
        .set<Pair, Position>({10});

    auto instance = world.entity()
        .add_instanceof(base);

    test_assert((instance.has<Pair, Position>()));
    const Pair *t = instance.get<Pair, Position>();
    test_int(t->value, 10);

    const Pair *t_2 = base.get<Pair, Position>();
    test_assert(t == t_2);

    instance.add<Pair, Position>();
    t = instance.get<Pair, Position>();
    test_int(t->value, 10);
    test_assert(t != t_2);

    instance.remove_trait<Pair, Position>();
    t = instance.get<Pair, Position>();
    test_int(t->value, 10);
    test_assert(t == t_2);    
}

void Traits_override_tag_trait() {
    flecs::world world;

    auto Pair = world.entity();

    auto base = world.entity()
        .set_object<Position>(Pair, {10, 20});

    auto instance = world.entity()
        .add_instanceof(base);

    test_assert((instance.has_object<Position>(Pair)));
    const Position *t = instance.get_object<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);

    const Position *t_2 = base.get_object<Position>(Pair);
    test_assert(t == t_2);

    instance.add_object<Position>(Pair);
    t = instance.get_object<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t != t_2);

    instance.remove_trait_tag<Position>(Pair);
    t = instance.get_object<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t == t_2); 
}

void Traits_get_mut_trait() {
    flecs::world world;

    auto e = world.entity();

    bool added = false;
    Pair *t = e.get_trait_mut<Pair, Position>(&added);
    test_assert(t != NULL);
    test_bool(added, true);
    t->value = 10;

    const Pair *t_2 = e.get<Pair, Position>();
    test_assert(t == t_2);
    test_int(t->value, 10);
}

void Traits_get_mut_trait_existing() {
    flecs::world world;

    auto e = world.entity()
        .set<Pair, Position>({20});

    bool added = false;
    Pair *t = e.get_trait_mut<Pair, Position>(&added);
    test_assert(t != NULL);
    test_bool(added, false);
    test_int(t->value, 20);
    t->value = 10;

    const Pair *t_2 = e.get<Pair, Position>();
    test_assert(t == t_2);
    test_int(t->value, 10);
}

void Traits_get_mut_trait_tag() {
    flecs::world world;

    auto Pair = world.entity();

    auto e = world.entity();

    bool added = false;
    Position *p = e.get_trait_tag_mut<Position>(Pair, &added);
    test_assert(p != NULL);
    test_bool(added, true);
    p->x = 10;
    p->y = 20;

    const Position *p_2 = e.get_object<Position>(Pair);
    test_assert(p == p_2);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Traits_get_mut_trait_tag_existing() {
    flecs::world world;

    auto Pair = world.entity();

    auto e = world.entity()
        .set_object<Position>(Pair, {10, 20});

    bool added = false;
    Position *p = e.get_trait_tag_mut<Position>(Pair, &added);
    test_assert(p != NULL);
    test_bool(added, false);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_2 = e.get_object<Position>(Pair);
    test_assert(p == p_2);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Traits_type_w_trait() {
    flecs::world world;

    auto Type = world.type()
        .add<Pair, Position>();

    auto e = world.entity()
        .add(Type);

    test_assert((e.has<Pair, Position>()));
}

void Traits_type_w_trait_tag() {
    flecs::world world;

    auto Tag = world.entity();

    auto Type = world.type()
        .add<Pair>(Tag);

    auto e = world.entity()
        .add(Type);

    test_assert((e.has<Pair>(Tag)));
}

void Traits_type_w_trait_tags() {
    flecs::world world;

    auto Tag = world.entity();
    auto Pair = world.entity();

    auto Type = world.type()
        .add(Pair, Tag);

    auto e = world.entity()
        .add(Type);

    test_assert((e.has(Pair, Tag)));
}

void Traits_type_w_tag_trait() {
    flecs::world world;

    auto Tag = world.entity();

    auto Type = world.type()
        .add<Pair>(Tag);

    auto e = world.entity()
        .add(Type);

    test_assert((e.has<Pair>(Tag)));
}

void Traits_override_trait_w_type() {
    flecs::world world;

    auto Prefab = world.prefab("Prefab")
        .set<Pair, Position>({10});

    auto Type = world.type()
        .add_instanceof(Prefab)
        .add<Pair, Position>();

    auto e = world.entity()
        .add(Type);

    test_assert((e.has<Pair, Position>()));

    const Pair *t_1 = Prefab.get<Pair, Position>();
    test_assert(t_1 != nullptr);
    test_int(t_1->value, 10);

    const Pair *t_2 = e.get<Pair, Position>();
    test_assert(t_2 != nullptr);

    test_assert(t_1 != t_2);
    test_int(t_2->value, 10);
}

void Traits_override_trait_w_type_tag() {
    flecs::world world;

    auto Tag = world.entity();

    auto Prefab = world.prefab("Prefab")
        .set<Pair>(Tag, {10});

    auto Type = world.type()
        .add_instanceof(Prefab)
        .add<Pair>(Tag);

    auto e = world.entity()
        .add(Type);

    test_assert((e.has<Pair>(Tag)));

    const Pair *t_1 = Prefab.get<Pair>(Tag);
    test_assert(t_1 != nullptr);
    test_int(t_1->value, 10);

    const Pair *t_2 = e.get<Pair>(Tag);
    test_assert(t_2 != nullptr);

    test_assert(t_1 != t_2);
    test_int(t_2->value, 10);    
}

void Traits_override_tag_trait_w_type() {
    flecs::world world;

    auto Pair = world.entity();

    auto Prefab = world.prefab("Prefab")
        .set_object<Position>(Pair, {10, 20});

    auto Type = world.type()
        .add_instanceof(Prefab)
        .add_object<Position>(Pair);

    auto e = world.entity()
        .add(Type);

    test_assert((e.has_object<Position>(Pair)));

    const Position *p_1 = Prefab.get_object<Position>(Pair);
    test_assert(p_1 != nullptr);
    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Position *p_2 = e.get_object<Position>(Pair);
    test_assert(p_2 != nullptr);

    test_assert(p_1 != p_2);
    test_int(p_2->x, 10);
    test_int(p_2->y, 20);
}
