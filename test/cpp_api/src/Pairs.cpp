#include <cpp_api.h>

typedef struct Pair {
    float value;
} Pair;

void Pairs_add_component_pair() {
    flecs::world ecs;

    auto entity = ecs.entity()
        .add<Pair, Position>();

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");
}

void Pairs_add_tag_pair() {
    flecs::world ecs;

    ecs.component<Position>();
    auto Pair = ecs.entity("Pair");

    auto entity = ecs.entity()
        .add_w_object<Position>(Pair);

    test_assert(entity.id() != 0);
    test_assert(entity.has_w_object<Position>(Pair));
    test_assert(!entity.has<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");
}

void Pairs_add_tag_pair_to_tag() {
    flecs::world ecs;

    auto Tag = ecs.entity("Tag");
    auto Pair = ecs.entity("Pair");

    auto entity = ecs.entity()
        .add(Pair, Tag);

    test_assert(entity.id() != 0);
    test_assert(entity.has(Pair, Tag));
    test_str(entity.type().str().c_str(), "(Pair,Tag)");
}

void Pairs_remove_component_pair() {
    flecs::world ecs;

    ecs.component<Position>();
    ecs.component<Pair>();

    auto entity = ecs.entity()
        .add<Pair, Position>();

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");

    entity.remove<Position, Pair>();
    test_assert(!(entity.has<Position, Pair>()));
}

void Pairs_remove_tag_pair() {
    flecs::world ecs;

    ecs.component<Position>();
    auto Pair = ecs.entity("Pair");

    auto entity = ecs.entity()
        .add_w_object<Position>(Pair);

    test_assert(entity.id() != 0);
    test_assert(entity.has_w_object<Position>(Pair));
    test_assert(!entity.has<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");

    entity.remove<Position>(Pair);
    test_assert(!entity.has<Position>(Pair));
}

void Pairs_remove_tag_pair_to_tag() {
    flecs::world ecs;

    auto Tag = ecs.entity("Tag");
    auto Pair = ecs.entity("Pair");

    auto entity = ecs.entity()
        .add(Pair, Tag);

    test_assert(entity.id() != 0);
    test_assert(entity.has(Pair, Tag));
    test_str(entity.type().str().c_str(), "(Pair,Tag)");

    entity.remove(Tag, Pair);
    test_assert(!entity.has(Tag, Pair));
}

void Pairs_set_component_pair() {
    flecs::world ecs;

    auto entity = ecs.entity()
        .set<Pair, Position>({10});

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");

    const Pair *t = entity.get<Pair, Position>();
    test_int(t->value, 10);
}

void Pairs_set_tag_pair() {
    flecs::world ecs;

    auto Pair = ecs.entity("Pair");

    auto entity = ecs.entity()
        .set_w_object<Position>(Pair, {10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has_w_object<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");

    const Position *p = entity.get_w_object<Position>(Pair);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Pairs_system_1_pair_instance() {
    flecs::world ecs;

    ecs.entity()
        .set<Pair, Position>({10});

    int invoke_count = 0;
    int entity_count = 0;
    int trait_value = 0;

    ecs.system<>()
        .expr("(Pair, *)")
        .iter([&](flecs::iter it) {
            flecs::column<Pair> tr(it, 1);
            invoke_count ++;            
            for (auto i : it) {
                entity_count ++;
                trait_value = tr[i].value;
            }
        });

    ecs.progress();

    test_int(invoke_count, 1);
    test_int(entity_count, 1);
    test_int(trait_value, 10);
}

void Pairs_system_2_pair_instances() {
    flecs::world ecs;

    ecs.entity()
        .set<Pair, Position>({10})
        .set<Pair, Velocity>({20});

    int invoke_count = 0;
    int entity_count = 0;
    int trait_value = 0;

    ecs.system<>()
        .expr("(Pair, *)")
        .iter([&](flecs::iter it) {
            flecs::column<Pair> tr(it, 1);
            invoke_count ++;            
            for (auto i : it) {
                entity_count ++;
                trait_value += tr[i].value;
            }
        });

    ecs.progress();

    test_int(invoke_count, 2);
    test_int(entity_count, 2);
    test_int(trait_value, 30);
}

void Pairs_override_pair() {
    flecs::world ecs;

    auto base = ecs.entity()
        .set<Pair, Position>({10});

    auto instance = ecs.entity()
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
    flecs::world ecs;

    auto Pair = ecs.entity();

    auto base = ecs.entity()
        .set_w_object<Position>(Pair, {10, 20});

    auto instance = ecs.entity()
        .add(flecs::IsA, base);

    test_assert((instance.has_w_object<Position>(Pair)));
    const Position *t = instance.get_w_object<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);

    const Position *t_2 = base.get_w_object<Position>(Pair);
    test_assert(t == t_2);

    instance.add_w_object<Position>(Pair);
    t = instance.get_w_object<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t != t_2);

    instance.remove_w_object<Position>(Pair);
    t = instance.get_w_object<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t == t_2); 
}

void Pairs_get_mut_pair() {
    flecs::world ecs;

    auto e = ecs.entity();

    Pair *t = e.get_mut<Pair, Position>();
    test_assert(t != NULL);
    t->value = 10;

    const Pair *t_2 = e.get<Pair, Position>();
    test_assert(t == t_2);
    test_int(t->value, 10);
}

void Pairs_get_mut_pair_existing() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Pair, Position>({20});

    Pair *t = e.get_mut<Pair, Position>();
    test_assert(t != NULL);
    test_int(t->value, 20);
    t->value = 10;

    const Pair *t_2 = e.get<Pair, Position>();
    test_assert(t == t_2);
    test_int(t->value, 10);
}

void Pairs_get_mut_pair_tag() {
    flecs::world ecs;

    auto Pair = ecs.entity();

    auto e = ecs.entity();

    Position *p = e.get_mut_w_object<Position>(Pair);
    test_assert(p != NULL);
    p->x = 10;
    p->y = 20;

    const Position *p_2 = e.get_w_object<Position>(Pair);
    test_assert(p == p_2);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Pairs_get_mut_pair_tag_existing() {
    flecs::world ecs;

    auto Pair = ecs.entity();

    auto e = ecs.entity()
        .set_w_object<Position>(Pair, {10, 20});

    Position *p = e.get_mut_w_object<Position>(Pair);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_2 = e.get_w_object<Position>(Pair);
    test_assert(p == p_2);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Pairs_get_relation_from_id() {
    flecs::world ecs;

    auto rel = ecs.entity();
    auto obj = ecs.entity();

    flecs::id pair(rel, obj);

    test_assert(pair.relation() == rel);
    test_assert(pair.object() != rel);

    test_assert(pair.relation().is_alive());
    test_assert(pair.relation().is_valid());
}

void Pairs_get_w_object_from_id() {
    flecs::world ecs;

    auto rel = ecs.entity();
    auto obj = ecs.entity();

    flecs::id pair(rel, obj);

    test_assert(pair.relation() != obj);
    test_assert(pair.object() == obj);

    test_assert(pair.object().is_alive());
    test_assert(pair.object().is_valid());
}

void Pairs_get_recycled_relation_from_id() {
    flecs::world ecs;

    auto rel = ecs.entity();
    auto obj = ecs.entity();

    rel.destruct();
    obj.destruct();

    rel = ecs.entity();
    obj = ecs.entity();

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
    flecs::world ecs;

    auto rel = ecs.entity();
    auto obj = ecs.entity();

    rel.destruct();
    obj.destruct();

    rel = ecs.entity();
    obj = ecs.entity();

    // Make sure ids are recycled
    test_assert((uint32_t)rel.id() != rel.id());
    test_assert((uint32_t)obj.id() != obj.id());

    flecs::id pair(rel, obj);

    test_assert(pair.relation() == rel);
    test_assert(pair.object() != rel);

    test_assert(pair.object().is_alive());
    test_assert(pair.object().is_valid());
}

void Pairs_get_rel_obj() {
    flecs::world ecs;

    auto rel = ecs.component<Position>();
    auto obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));
    test_assert(e.has(rel, obj));

    const void *ptr = e.get(rel, obj);
    test_assert(ptr != nullptr);

    test_int(static_cast<const Position*>(ptr)->x, 10);
    test_int(static_cast<const Position*>(ptr)->y, 20);
}

void Pairs_get_rel_obj_id() {
    flecs::world ecs;

    flecs::id rel = ecs.component<Position>();
    flecs::id obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));
    test_assert(e.has(rel, obj));

    const void *ptr = e.get(rel, obj);
    test_assert(ptr != nullptr);

    test_int(static_cast<const Position*>(ptr)->x, 10);
    test_int(static_cast<const Position*>(ptr)->y, 20);
}

void Pairs_get_rel_obj_id_t() {
    flecs::world ecs;

    flecs::id_t rel = ecs.component<Position>();
    flecs::id_t obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));
    test_assert(e.has(rel, obj));

    const void *ptr = e.get(rel, obj);
    test_assert(ptr != nullptr);

    test_int(static_cast<const Position*>(ptr)->x, 10);
    test_int(static_cast<const Position*>(ptr)->y, 20);
}

void Pairs_get_R_obj() {
    flecs::world ecs;

    auto obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));

    const Position *ptr = e.get<Position>(obj);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_R_obj_id() {
    flecs::world ecs;

    flecs::id obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));

    const Position *ptr = e.get<Position>(obj);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_R_obj_id_t() {
    flecs::world ecs;

    flecs::id_t obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));

    const Position *ptr = e.get<Position>(obj);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_w_object() {
    flecs::world ecs;

    auto rel = ecs.entity();

    auto e = ecs.entity()
        .set_w_object<Position>(rel, {10, 20});

    test_assert(e.has_w_object<Position>(rel));

    const Position *ptr = e.get_w_object<Position>(rel);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_w_object_id() {
    flecs::world ecs;

    flecs::id rel = ecs.entity();

    auto e = ecs.entity()
        .set_w_object<Position>(rel, {10, 20});

    test_assert(e.has_w_object<Position>(rel));

    const Position *ptr = e.get_w_object<Position>(rel);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_w_object_id_t() {
    flecs::world ecs;

    flecs::id_t rel = ecs.entity();

    auto e = ecs.entity()
        .set_w_object<Position>(rel, {10, 20});

    test_assert(e.has_w_object<Position>(rel));

    const Position *ptr = e.get_w_object<Position>(rel);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_each() {
    flecs::world ecs;

    auto p_1 = ecs.entity();
    auto p_2 = ecs.entity();

    auto e = ecs.entity()
        .add(p_1)
        .add(p_2);

    int32_t count = 0;

    e.each([&](flecs::id e) {
        if (count == 0) {
            test_assert(e == p_1);
        } else if (count == 1) {
            test_assert(e == p_2);
        } else {
            test_assert(false);
        }

        count ++;
    });

    test_int(count, 2);
}

void Pairs_each_pair() {
    flecs::world ecs;

    auto pair = ecs.component<Pair>();
    auto pos = ecs.component<Position>();
    auto vel = ecs.component<Velocity>();

    auto e = ecs.entity()
        .add<Pair, Position>()
        .add<Pair, Velocity>();

    int32_t count = 0;

    e.each(pair, [&](flecs::entity object) {
        if (count == 0) {
            test_assert(object == pos);
        } else if (count == 1) {
            test_assert(object == vel);
        } else {
            test_assert(false);
        }

        count ++;
    });

    test_int(count, 2);
}

void Pairs_each_pair_by_type() {
    flecs::world ecs;

    auto pos = ecs.component<Position>();
    auto vel = ecs.component<Velocity>();

    auto e = ecs.entity()
        .add<Pair, Position>()
        .add<Pair, Velocity>();

    int32_t count = 0;

    e.each<Pair>([&](flecs::entity object) {
        if (count == 0) {
            test_assert(object == pos);
        } else if (count == 1) {
            test_assert(object == vel);
        } else {
            test_assert(false);
        }

        count ++;
    });

    test_int(count, 2);
}

void Pairs_each_pair_w_isa() {
    flecs::world ecs;

    auto p_1 = ecs.entity();
    auto p_2 = ecs.entity();

    auto e = ecs.entity()
        .is_a(p_1)
        .is_a(p_2);

    int32_t count = 0;

    e.each(flecs::IsA, [&](flecs::entity object) {
        if (count == 0) {
            test_assert(object == p_1);
        } else if (count == 1) {
            test_assert(object == p_2);
        } else {
            test_assert(false);
        }

        count ++;
    });

    test_int(count, 2);
}

void Pairs_each_pair_w_recycled_rel() {
    flecs::world ecs;

    auto e_1 = ecs.entity();
    auto e_2 = ecs.entity();

    ecs.entity().destruct(); // force recycling

    auto pair = ecs.entity();

    test_assert((uint32_t)pair.id() != pair.id()); // ensure recycled

    auto e = ecs.entity()
        .add(pair, e_1)
        .add(pair, e_2);

    int32_t count = 0;

    // should work correctly
    e.each(pair, [&](flecs::entity object) {
        if (count == 0) {
            test_assert(object == e_1);
        } else if (count == 1) {
            test_assert(object == e_2);
        } else {
            test_assert(false);
        }

        count ++;
    });

    test_int(count, 2);
}

void Pairs_each_pair_w_recycled_obj() {
    flecs::world ecs;

    auto pair = ecs.component<Pair>();

    ecs.entity().destruct(); // force recycling
    auto e_1 = ecs.entity();
    test_assert((uint32_t)e_1.id() != e_1.id()); // ensure recycled

    ecs.entity().destruct();
    auto e_2 = ecs.entity();    
    test_assert((uint32_t)e_2.id() != e_2.id());

    auto e = ecs.entity()
        .add<Pair>(e_1)
        .add<Pair>(e_2);

    int32_t count = 0;

    // should work correctly
    e.each(pair, [&](flecs::entity object) {
        if (count == 0) {
            test_assert(object == e_1);
        } else if (count == 1) {
            test_assert(object == e_2);
        } else {
            test_assert(false);
        }

        count ++;
    });

    test_int(count, 2);
}

void Pairs_match_pair() {
    flecs::world ecs;

    auto Eats = ecs.entity();
    auto Dislikes = ecs.entity();

    auto Apples = ecs.entity();
    auto Pears = ecs.entity();
    auto Bananas = ecs.entity();

    auto e = ecs.entity()
        .set<Position>({10, 20}) // should not be matched
        .add(Eats, Apples)
        .add(Eats, Pears)
        .add(Dislikes, Bananas);

    int32_t count = 0;

    e.each(Eats, Apples, 
        [&](flecs::id id) {
            test_assert(id.relation() == Eats);
            test_assert(id.object() == Apples);
            count ++;
        });
    
    test_int(count, 1);
}

void Pairs_match_pair_obj_wildcard() {
    flecs::world ecs;

    auto Eats = ecs.entity();
    auto Dislikes = ecs.entity();

    auto Apples = ecs.entity();
    auto Pears = ecs.entity();
    auto Bananas = ecs.entity();

    auto e = ecs.entity()
        .set<Position>({10, 20}) // should not be matched
        .add(Eats, Apples)
        .add(Eats, Pears)
        .add(Dislikes, Bananas);

    int32_t count = 0;

    e.each(Eats, flecs::Wildcard, 
        [&](flecs::id id) {
            test_assert(id.relation() == Eats);
            test_assert(id.object() == Apples || id.object() == Pears);
            count ++;
        });
    
    test_int(count, 2);
}

void Pairs_match_pair_rel_wildcard() {
    flecs::world ecs;

    auto Eats = ecs.entity();
    auto Dislikes = ecs.entity();

    auto Apples = ecs.entity();
    auto Pears = ecs.entity();
    auto Bananas = ecs.entity();

    auto e = ecs.entity()
        .set<Position>({10, 20}) // should not be matched
        .add(Eats, Apples)
        .add(Eats, Pears)
        .add(Dislikes, Bananas);

    int32_t count = 0;

    e.each(flecs::Wildcard, Pears, 
        [&](flecs::id id) {
            test_assert(id.relation() == Eats);
            test_assert(id.object() == Pears);
            count ++;
        });
    
    test_int(count, 1);
}

void Pairs_match_pair_both_wildcard() {
    flecs::world ecs;

    auto Eats = ecs.entity();
    auto Dislikes = ecs.entity();

    auto Apples = ecs.entity();
    auto Pears = ecs.entity();
    auto Bananas = ecs.entity();

    auto e = ecs.entity()
        .set<Position>({10, 20}) // should not be matched
        .add(Eats, Apples)
        .add(Eats, Pears)
        .add(Dislikes, Bananas);

    int32_t count = 0;

    e.each(flecs::Wildcard, flecs::Wildcard, 
        [&](flecs::id id) {
            count ++;
        });
    
    test_int(count, 3);
}

void Pairs_has_tag_w_object() {
    flecs::world ecs;

    struct Likes { };

    auto Bob = ecs.entity();
    auto e = ecs.entity().add<Likes>(Bob);
    test_assert(e.has<Likes>(Bob));
}

void Pairs_has_w_object_tag() {
    flecs::world ecs;

    struct Bob { };

    auto Likes = ecs.entity();
    auto e = ecs.entity().add_w_object<Bob>(Likes);
    test_assert(e.has_w_object<Bob>(Likes));
}

struct Eats { int amount; };
struct Apples { };
struct Pears { };

using EatsApples = flecs::pair<Eats, Apples>;
using EatsPears = flecs::pair<Eats, Pears>;

void Pairs_add_pair_type() {
    flecs::world ecs;

    auto e = ecs.entity().add<EatsApples>();
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));
}

void Pairs_remove_pair_type() {
    flecs::world ecs;

    auto e = ecs.entity().add<EatsApples>();
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));

    e.remove<EatsApples>();
    test_assert(!(e.has<Eats, Apples>()));
    test_assert(!(e.has<EatsApples>()));    
}

void Pairs_set_pair_type() {
    flecs::world ecs;

    auto e = ecs.entity().set<EatsApples>({10});
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));

    const Eats *ptr = e.get<EatsApples>();
    test_int(ptr->amount, 10);

    test_assert((ptr == e.get<Eats, Apples>()));
}

void Pairs_has_pair_type() {
    flecs::world ecs;

    auto e = ecs.entity().add<Eats, Apples>();
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));
}

void Pairs_get_1_pair_arg() {
    flecs::world ecs;

    auto e = ecs.entity().set<EatsApples>({10});
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));

    test_bool(e.get([](const EatsApples& a) {
        test_int(a->amount, 10);
    }), true);
}

void Pairs_get_2_pair_arg() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<EatsApples>({10})
        .set<EatsPears>({20});

    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<Eats, Pears>()));
    test_assert((e.has<EatsApples>()));
    test_assert((e.has<EatsPears>()));

    test_bool(e.get([](const EatsApples& a, const EatsPears& p) {
        test_int(a->amount, 10);
        test_int(p->amount, 20);
    }), true);
}

void Pairs_set_1_pair_arg() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set([](EatsApples&& a) {
            a->amount = 10;
        });

    auto eats = e.get<EatsApples>();
    test_int(eats->amount, 10);
}

void Pairs_set_2_pair_arg() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set([](EatsApples&& a, EatsPears&& p) {
            a->amount = 10;
            p->amount = 20;
        });

    auto eats = e.get<EatsApples>();
    test_int(eats->amount, 10);

    eats = e.get<EatsPears>();
    test_int(eats->amount, 20);
}

void Pairs_get_inline_pair_type() {
    flecs::world ecs;

    auto e = ecs.entity().set<EatsApples>({10});
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));

    test_bool(e.get([](const flecs::pair<Eats, Apples>& a) {
        test_int(a->amount, 10);
    }), true);
}

void Pairs_set_inline_pair_type() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set([](flecs::pair<Eats, Apples>&& a) {
            a->amount = 10;
        });

    auto eats = e.get<EatsApples>();
    test_int(eats->amount, 10);
}

void Pairs_get_pair_type_object() {
    flecs::world ecs;

    auto e = ecs.entity().set_w_object<Apples, Eats>({10});
    test_assert((e.has<Apples, Eats>()));

    test_bool(e.get([](const flecs::pair_object<Apples, Eats>& a) {
        test_int(a->amount, 10);
    }), true);
}

void Pairs_set_pair_type_object() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set([](flecs::pair_object<Apples, Eats>&& a) {
            a->amount = 10;
        });

    auto eats = e.get_w_object<Apples, Eats>();
    test_int(eats->amount, 10);
}

struct Event {
    const char *value;
};

struct Begin { };
struct End { };

using BeginEvent = flecs::pair<Begin, Event>;
using EndEvent = flecs::pair<End, Event>;

void Pairs_set_get_w_object_variants() {
    flecs::world ecs;

    auto e1 = ecs.entity().set_w_object<Begin, Event>({"Big Bang"});
    test_assert((e1.has<Begin, Event>()));
    const Event* v = e1.get_w_object<Begin, Event>();
    test_assert(v != NULL);
    test_str(v->value, "Big Bang");

    auto e2 = ecs.entity().set<Begin, Event>({"Big Bang"});
    test_assert((e2.has<Begin, Event>()));
    v = e2.get<Begin, Event>();
    test_assert(v != NULL);
    test_str(v->value, "Big Bang");

    auto e3 = ecs.entity().set<flecs::pair<Begin, Event>>({"Big Bang"});
    test_assert((e3.has<flecs::pair<Begin, Event>>()));
    v = e3.get<flecs::pair<Begin, Event>>();
    test_assert(v != NULL);
    test_str(v->value, "Big Bang");  

    auto e4 = ecs.entity().set<BeginEvent>({"Big Bang"});
    test_assert((e4.has<BeginEvent>()));
    v = e4.get<BeginEvent>();
    test_assert(v != NULL);
    test_str(v->value, "Big Bang");            
}

void Pairs_get_object_for_type_self() {
    flecs::world ecs;

    auto base = ecs.entity().add<Tag>();
    auto self = ecs.entity().is_a(base).add<Tag>();

    auto obj = self.get_object_for<Tag>(flecs::IsA);
    test_assert(obj != 0);
    test_assert(obj == self);
}

void Pairs_get_object_for_type_base() {
    flecs::world ecs;

    auto base = ecs.entity().add<Tag>();
    auto self = ecs.entity().is_a(base);

    auto obj = self.get_object_for<Tag>(flecs::IsA);
    test_assert(obj != 0);
    test_assert(obj == base);
}

void Pairs_get_object_for_id_self() {
    flecs::world ecs;

    auto tag = ecs.entity();
    auto base = ecs.entity().add(tag);
    auto self = ecs.entity().is_a(base).add(tag);

    auto obj = self.get_object_for(flecs::IsA, tag);
    test_assert(obj != 0);
    test_assert(obj == self);
}

void Pairs_get_object_for_id_base() {
    flecs::world ecs;

    auto tag = ecs.entity();
    auto base = ecs.entity().add(tag);
    auto self = ecs.entity().is_a(base);

    auto obj = self.get_object_for(flecs::IsA, tag);
    test_assert(obj != 0);
    test_assert(obj == base);
}

void Pairs_get_object_for_id_not_found() {
    flecs::world ecs;

    auto tag = ecs.entity();
    auto base = ecs.entity();
    auto self = ecs.entity().is_a(base);

    auto obj = self.get_object_for(flecs::IsA, tag);
    test_assert(obj == 0);
}

void Pairs_deref_pair() {
    flecs::world ecs;

    Position v = {10, 20};

    flecs::pair<Position, Tag> p(v);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position pos = *p;
    test_int(pos.x, 10);
    test_int(pos.y, 20);
}

void Pairs_deref_const_pair() {
    flecs::world ecs;

    Position v = {10, 20};

    const flecs::pair<Position, Tag> p(v);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position pos = *p;
    test_int(pos.x, 10);
    test_int(pos.y, 20);
}

void Pairs_deref_pair_obj() {
    flecs::world ecs;

    Position v = {10, 20};

    flecs::pair<Tag, Position> p(v);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position pos = *p;
    test_int(pos.x, 10);
    test_int(pos.y, 20);
}

void Pairs_deref_const_pair_obj() {
    flecs::world ecs;

    Position v = {10, 20};

    const flecs::pair<Tag, Position> p(v);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position pos = *p;
    test_int(pos.x, 10);
    test_int(pos.y, 20);
}
