#include <cpp_api.h>

typedef struct Pair {
    float value;
} Pair;

void Pairs_add_component_pair() {
    flecs::world world;

    auto entity = world.entity()
        .add<Pair, Position>();

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");
}

void Pairs_add_tag_pair() {
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

void Pairs_add_tag_pair_to_tag() {
    flecs::world world;

    auto Tag = world.entity("Tag");
    auto Pair = world.entity("Pair");

    auto entity = world.entity()
        .add(Pair, Tag);

    test_assert(entity.id() != 0);
    test_assert(entity.has(Pair, Tag));
    test_str(entity.type().str().c_str(), "(Pair,Tag)");
}

void Pairs_remove_component_pair() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Pair>(world, "Pair");

    auto entity = world.entity()
        .add<Pair, Position>();

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");

    entity.remove<Position, Pair>();
    test_assert(!(entity.has<Position, Pair>()));
}

void Pairs_remove_tag_pair() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    auto Pair = world.entity("Pair");

    auto entity = world.entity()
        .add_object<Position>(Pair);

    test_assert(entity.id() != 0);
    test_assert(entity.has_object<Position>(Pair));
    test_assert(!entity.has<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");

    entity.remove<Position>(Pair);
    test_assert(!entity.has<Position>(Pair));
}

void Pairs_remove_tag_pair_to_tag() {
    flecs::world world;

    auto Tag = world.entity("Tag");
    auto Pair = world.entity("Pair");

    auto entity = world.entity()
        .add(Pair, Tag);

    test_assert(entity.id() != 0);
    test_assert(entity.has(Pair, Tag));
    test_str(entity.type().str().c_str(), "(Pair,Tag)");

    entity.remove(Tag, Pair);
    test_assert(!entity.has(Tag, Pair));
}

void Pairs_set_component_pair() {
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

void Pairs_set_tag_pair() {
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

void Pairs_system_1_pair_instance() {
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

void Pairs_system_2_pair_instances() {
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

void Pairs_override_pair() {
    flecs::world world;

    auto base = world.entity()
        .set<Pair, Position>({10});

    auto instance = world.entity()
        .add(flecs::IsA, base);

    test_assert((instance.has<Pair, Position>()));
    const Pair *t = instance.get<Pair, Position>();
    test_int(t->value, 10);

    const Pair *t_2 = base.get<Pair, Position>();
    test_assert(t == t_2);

    instance.add<Pair, Position>();
    t = instance.get<Pair, Position>();
    test_int(t->value, 10);
    test_assert(t != t_2);

    instance.remove<Pair, Position>();
    t = instance.get<Pair, Position>();
    test_int(t->value, 10);
    test_assert(t == t_2);    
}

void Pairs_override_tag_pair() {
    flecs::world world;

    auto Pair = world.entity();

    auto base = world.entity()
        .set_object<Position>(Pair, {10, 20});

    auto instance = world.entity()
        .add(flecs::IsA, base);

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

    instance.remove_object<Position>(Pair);
    t = instance.get_object<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t == t_2); 
}

void Pairs_get_mut_pair() {
    flecs::world world;

    auto e = world.entity();

    bool added = false;
    Pair *t = e.get_mut<Pair, Position>(&added);
    test_assert(t != NULL);
    test_bool(added, true);
    t->value = 10;

    const Pair *t_2 = e.get<Pair, Position>();
    test_assert(t == t_2);
    test_int(t->value, 10);
}

void Pairs_get_mut_pair_existing() {
    flecs::world world;

    auto e = world.entity()
        .set<Pair, Position>({20});

    bool added = false;
    Pair *t = e.get_mut<Pair, Position>(&added);
    test_assert(t != NULL);
    test_bool(added, false);
    test_int(t->value, 20);
    t->value = 10;

    const Pair *t_2 = e.get<Pair, Position>();
    test_assert(t == t_2);
    test_int(t->value, 10);
}

void Pairs_get_mut_pair_tag() {
    flecs::world world;

    auto Pair = world.entity();

    auto e = world.entity();

    bool added = false;
    Position *p = e.get_mut_object<Position>(Pair, &added);
    test_assert(p != NULL);
    test_bool(added, true);
    p->x = 10;
    p->y = 20;

    const Position *p_2 = e.get_object<Position>(Pair);
    test_assert(p == p_2);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Pairs_get_mut_pair_tag_existing() {
    flecs::world world;

    auto Pair = world.entity();

    auto e = world.entity()
        .set_object<Position>(Pair, {10, 20});

    bool added = false;
    Position *p = e.get_mut_object<Position>(Pair, &added);
    test_assert(p != NULL);
    test_bool(added, false);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_2 = e.get_object<Position>(Pair);
    test_assert(p == p_2);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Pairs_type_w_pair() {
    flecs::world world;

    auto Type = world.type()
        .add<Pair, Position>();

    auto e = world.entity()
        .add(Type);

    test_assert((e.has<Pair, Position>()));
}

void Pairs_type_w_pair_tag() {
    flecs::world world;

    auto Tag = world.entity();

    auto Type = world.type()
        .add<Pair>(Tag);

    auto e = world.entity()
        .add(Type);

    test_assert((e.has<Pair>(Tag)));
}

void Pairs_type_w_pair_tags() {
    flecs::world world;

    auto Tag = world.entity();
    auto Pair = world.entity();

    auto Type = world.type()
        .add(Pair, Tag);

    auto e = world.entity()
        .add(Type);

    test_assert((e.has(Pair, Tag)));
}

void Pairs_type_w_tag_pair() {
    flecs::world world;

    auto Tag = world.entity();

    auto Type = world.type()
        .add<Pair>(Tag);

    auto e = world.entity()
        .add(Type);

    test_assert((e.has<Pair>(Tag)));
}

void Pairs_override_pair_w_type() {
    flecs::world world;

    auto Prefab = world.prefab("Prefab")
        .set<Pair, Position>({10});

    auto Type = world.type()
        .add(flecs::IsA, Prefab)
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

void Pairs_override_pair_w_type_tag() {
    flecs::world world;

    auto Tag = world.entity();

    auto Prefab = world.prefab("Prefab")
        .set<Pair>(Tag, {10});

    auto Type = world.type()
        .add(flecs::IsA, Prefab)
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

void Pairs_override_tag_pair_w_type() {
    flecs::world world;

    auto Pair = world.entity();

    auto Prefab = world.prefab("Prefab")
        .set_object<Position>(Pair, {10, 20});

    auto Type = world.type()
        .add(flecs::IsA, Prefab)
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

void Pairs_get_relation_from_id() {
    flecs::world world;

    auto rel = world.entity();
    auto obj = world.entity();

    flecs::id pair(rel, obj);

    test_assert(pair.relation() == rel);
    test_assert(pair.object() != rel);

    test_assert(pair.relation().is_alive());
    test_assert(pair.relation().is_valid());
}

void Pairs_get_object_from_id() {
    flecs::world world;

    auto rel = world.entity();
    auto obj = world.entity();

    flecs::id pair(rel, obj);

    test_assert(pair.relation() != obj);
    test_assert(pair.object() == obj);

    test_assert(pair.object().is_alive());
    test_assert(pair.object().is_valid());
}

void Pairs_get_recycled_relation_from_id() {
    flecs::world world;

    auto rel = world.entity();
    auto obj = world.entity();

    rel.destruct();
    obj.destruct();

    rel = world.entity();
    obj = world.entity();

    // Make sure ids are recycled
    test_assert((uint32_t)rel.id() != rel.id());
    test_assert((uint32_t)obj.id() != obj.id());

    flecs::id pair(rel, obj);

    test_assert(pair.relation() == rel);
    test_assert(pair.object() != rel);

    test_assert(pair.relation().is_alive());
    test_assert(pair.relation().is_valid());
}

void Pairs_get_recycled_object_from_id() {
    flecs::world world;

    auto rel = world.entity();
    auto obj = world.entity();

    rel.destruct();
    obj.destruct();

    rel = world.entity();
    obj = world.entity();

    // Make sure ids are recycled
    test_assert((uint32_t)rel.id() != rel.id());
    test_assert((uint32_t)obj.id() != obj.id());

    flecs::id pair(rel, obj);

    test_assert(pair.relation() == rel);
    test_assert(pair.object() != rel);

    test_assert(pair.object().is_alive());
    test_assert(pair.object().is_valid());
}
