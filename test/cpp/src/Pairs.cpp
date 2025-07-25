#include <cpp.h>

typedef struct Pair {
    float value;
} Pair;

void Pairs_add_component_pair(void) {
    flecs::world ecs;

    auto entity = ecs.entity()
        .add<Pair, Position>();

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");
}

void Pairs_add_tag_pair(void) {
    flecs::world ecs;

    ecs.component<Position>();
    auto Pair = ecs.entity("Pair");

    auto entity = ecs.entity()
        .add_second<Position>(Pair);

    test_assert(entity.id() != 0);
    test_assert(entity.has_second<Position>(Pair));
    test_assert(!entity.has<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");
}

void Pairs_add_tag_pair_to_tag(void) {
    flecs::world ecs;

    auto Tag = ecs.entity("Tag");
    auto Pair = ecs.entity("Pair");

    auto entity = ecs.entity()
        .add(Pair, Tag);

    test_assert(entity.id() != 0);
    test_assert(entity.has(Pair, Tag));
    test_str(entity.type().str().c_str(), "(Pair,Tag)");
}

void Pairs_remove_component_pair(void) {
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

void Pairs_remove_tag_pair(void) {
    flecs::world ecs;

    ecs.component<Position>();
    auto Pair = ecs.entity("Pair");

    auto entity = ecs.entity()
        .add_second<Position>(Pair);

    test_assert(entity.id() != 0);
    test_assert(entity.has_second<Position>(Pair));
    test_assert(!entity.has<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");

    entity.remove<Position>(Pair);
    test_assert(!entity.has<Position>(Pair));
}

void Pairs_remove_tag_pair_to_tag(void) {
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

void Pairs_set_component_pair(void) {
    flecs::world ecs;

    auto entity = ecs.entity()
        .set<Pair, Position>({10});

    test_assert(entity.id() != 0);
    test_assert((entity.has<Pair, Position>()));
    test_assert((!entity.has<Position, Pair>()));

    test_str(entity.type().str().c_str(), "(Pair,Position)");

    const Pair *t = entity.try_get<Pair, Position>();
    test_int(t->value, 10);
}

void Pairs_set_tag_pair(void) {
    flecs::world ecs;

    auto Pair = ecs.entity("Pair");

    auto entity = ecs.entity()
        .set_second<Position>(Pair, {10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has_second<Position>(Pair));
    test_str(entity.type().str().c_str(), "(Pair,Position)");

    const Position *p = entity.try_get_second<Position>(Pair);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Pairs_system_1_pair_instance(void) {
    flecs::world ecs;

    ecs.entity()
        .set<Pair, Position>({10});

    int invoke_count = 0;
    int entity_count = 0;
    int trait_value = 0;

    ecs.system<>()
        .expr("(Pair, *)")
        .run([&](flecs::iter& it) {
            while (it.next()) {
                flecs::field<Pair> tr(it, 0);
                invoke_count ++;            
                for (auto i : it) {
                    entity_count ++;
                    trait_value = tr[i].value;
                }
            }
        });

    ecs.progress();

    test_int(invoke_count, 1);
    test_int(entity_count, 1);
    test_int(trait_value, 10);
}

void Pairs_system_2_pair_instances(void) {
    flecs::world ecs;

    ecs.entity()
        .set<Pair, Position>({10})
        .set<Pair, Velocity>({20});

    int invoke_count = 0;
    int entity_count = 0;
    int trait_value = 0;

    ecs.system<>()
        .expr("(Pair, *)")
        .run([&](flecs::iter it) {
            while (it.next()) {
                flecs::field<Pair> tr(it, 0);
                invoke_count ++;            
                for (auto i : it) {
                    entity_count ++;
                    trait_value += tr[i].value;
                }
            }
        });

    ecs.progress();

    test_int(invoke_count, 2);
    test_int(entity_count, 2);
    test_int(trait_value, 30);
}

void Pairs_override_pair(void) {
    flecs::world ecs;

    ecs.component<Pair>().add(flecs::OnInstantiate, flecs::Inherit);

    auto base = ecs.entity()
        .set<Pair, Position>({10});

    auto instance = ecs.entity()
        .add(flecs::IsA, base);

    test_assert((instance.has<Pair, Position>()));
    const Pair *t = instance.try_get<Pair, Position>();
    test_int(t->value, 10);

    const Pair *t_2 = base.try_get<Pair, Position>();
    test_assert(t == t_2);

    instance.add<Pair, Position>();
    t = instance.try_get<Pair, Position>();
    test_int(t->value, 10);
    test_assert(t != t_2);

    instance.remove<Pair, Position>();
    t = instance.try_get<Pair, Position>();
    test_int(t->value, 10);
    test_assert(t == t_2);    
}

void Pairs_override_tag_pair(void) {
    flecs::world ecs;

    auto Pair = ecs.entity().add(flecs::OnInstantiate, flecs::Inherit);

    auto base = ecs.entity()
        .set_second<Position>(Pair, {10, 20});

    auto instance = ecs.entity()
        .add(flecs::IsA, base);

    test_assert((instance.has_second<Position>(Pair)));
    const Position *t = instance.try_get_second<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);

    const Position *t_2 = base.try_get_second<Position>(Pair);
    test_assert(t == t_2);

    instance.add_second<Position>(Pair);
    t = instance.try_get_second<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t != t_2);

    instance.remove_second<Position>(Pair);
    t = instance.try_get_second<Position>(Pair);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t == t_2); 
}

void Pairs_ensure_pair(void) {
    flecs::world ecs;

    auto e = ecs.entity();

    Pair& t = e.ensure<Pair, Position>();
    t.value = 10;

    const Pair *t_2 = e.try_get<Pair, Position>();
    test_int(t.value, 10);
    test_int(t_2->value, 10);
}

void Pairs_ensure_pair_existing(void) {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Pair, Position>({20});

    Pair& t = e.ensure<Pair, Position>();
    test_int(t.value, 20);
    t.value = 10;

    const Pair *t_2 = e.try_get<Pair, Position>();
    test_int(t.value, 10);
    test_int(t_2->value, 10);
}

void Pairs_ensure_pair_tag(void) {
    flecs::world ecs;

    auto Pair = ecs.entity();

    auto e = ecs.entity();

    Position& p = e.ensure_second<Position>(Pair);
    p.x = 10;
    p.y = 20;

    const Position *p_2 = e.try_get_second<Position>(Pair);
    test_int(p.x, 10);
    test_int(p.y, 20);
    test_int(p_2->x, 10);
    test_int(p_2->y, 20);
}

void Pairs_ensure_pair_tag_existing(void) {
    flecs::world ecs;

    auto Pair = ecs.entity();

    auto e = ecs.entity()
        .set_second<Position>(Pair, {10, 20});

    Position& p = e.ensure_second<Position>(Pair);
    test_int(p.x, 10);
    test_int(p.y, 20);

    const Position *p_2 = e.try_get_second<Position>(Pair);
    test_int(p.x, 10);
    test_int(p.y, 20);
    test_int(p_2->x, 10);
    test_int(p_2->y, 20);
}

void Pairs_ensure_R_tag_O(void) {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Tag, Position>({10, 20});

    Position& t = e.ensure<Tag, Position>();
    test_int(t.x, 10);
    test_int(t.y, 20);
    t.x = 30;
    t.y = 40;

    const Position *t_2 = e.try_get<Tag, Position>();
    test_int(t.x, 30);
    test_int(t.y, 40);
    test_int(t_2->x, 30);
    test_int(t_2->y, 40);
}

void Pairs_get_relation_from_id(void) {
    flecs::world ecs;

    auto rel = ecs.entity();
    auto obj = ecs.entity();

    flecs::id pair(rel, obj);

    test_assert(pair.first() == rel);
    test_assert(pair.second() != rel);

    test_assert(pair.first().is_alive());
    test_assert(pair.first().is_valid());
}

void Pairs_get_second_from_id(void) {
    flecs::world ecs;

    auto rel = ecs.entity();
    auto obj = ecs.entity();

    flecs::id pair(rel, obj);

    test_assert(pair.first() != obj);
    test_assert(pair.second() == obj);

    test_assert(pair.second().is_alive());
    test_assert(pair.second().is_valid());
}

void Pairs_get_recycled_relation_from_id(void) {
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

    test_assert(pair.first() == rel);
    test_assert(pair.second() != rel);

    test_assert(pair.first().is_alive());
    test_assert(pair.first().is_valid());
}

void Pairs_get_recycled_object_from_id(void) {
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

    test_assert(pair.first() == rel);
    test_assert(pair.second() != rel);

    test_assert(pair.second().is_alive());
    test_assert(pair.second().is_valid());
}

void Pairs_get_rel_obj(void) {
    flecs::world ecs;

    auto rel = ecs.component<Position>();
    auto obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));
    test_assert(e.has(rel, obj));

    const void *ptr = e.try_get(rel, obj);
    test_assert(ptr != nullptr);

    test_int(static_cast<const Position*>(ptr)->x, 10);
    test_int(static_cast<const Position*>(ptr)->y, 20);
}

void Pairs_get_rel_obj_id(void) {
    flecs::world ecs;

    flecs::id rel = ecs.component<Position>();
    flecs::id obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));
    test_assert(e.has(rel, obj));

    const void *ptr = e.try_get(rel, obj);
    test_assert(ptr != nullptr);

    test_int(static_cast<const Position*>(ptr)->x, 10);
    test_int(static_cast<const Position*>(ptr)->y, 20);
}

void Pairs_get_rel_obj_id_t(void) {
    flecs::world ecs;

    flecs::id_t rel = ecs.component<Position>();
    flecs::id_t obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));
    test_assert(e.has(rel, obj));

    const void *ptr = e.try_get(rel, obj);
    test_assert(ptr != nullptr);

    test_int(static_cast<const Position*>(ptr)->x, 10);
    test_int(static_cast<const Position*>(ptr)->y, 20);
}

void Pairs_get_R_obj(void) {
    flecs::world ecs;

    auto obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));

    const Position *ptr = e.try_get<Position>(obj);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_R_obj_id(void) {
    flecs::world ecs;

    flecs::id obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));

    const Position *ptr = e.try_get<Position>(obj);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_R_obj_id_t(void) {
    flecs::world ecs;

    flecs::id_t obj = ecs.entity();

    auto e = ecs.entity()
        .set<Position>(obj, {10, 20});

    test_assert(e.has<Position>(obj));

    const Position *ptr = e.try_get<Position>(obj);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_R_O(void) {
    flecs::world ecs;

    auto e = ecs.entity().set<Position, Tag>({10, 20});

    test_assert((e.has<Position, Tag>()));

    const Position *ptr = e.try_get<Position, Tag>();
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_R_tag_O(void) {
    flecs::world ecs;

    auto e = ecs.entity().set<Tag, Position>({10, 20});

    test_assert((e.has<Tag, Position>()));

    const Position *ptr = e.try_get<Tag, Position>();
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_second(void) {
    flecs::world ecs;

    auto rel = ecs.entity();

    auto e = ecs.entity()
        .set_second<Position>(rel, {10, 20});

    test_assert(e.has_second<Position>(rel));

    const Position *ptr = e.try_get_second<Position>(rel);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_second_id(void) {
    flecs::world ecs;

    flecs::id rel = ecs.entity();

    auto e = ecs.entity()
        .set_second<Position>(rel, {10, 20});

    test_assert(e.has_second<Position>(rel));

    const Position *ptr = e.try_get_second<Position>(rel);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_get_second_id_t(void) {
    flecs::world ecs;

    flecs::id_t rel = ecs.entity();

    auto e = ecs.entity()
        .set_second<Position>(rel, {10, 20});

    test_assert(e.has_second<Position>(rel));

    const Position *ptr = e.try_get_second<Position>(rel);
    test_assert(ptr != nullptr);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_each(void) {
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

void Pairs_each_pair(void) {
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

void Pairs_each_pair_by_type(void) {
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

void Pairs_each_pair_w_isa(void) {
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

void Pairs_each_pair_w_recycled_rel(void) {
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

void Pairs_each_pair_w_recycled_obj(void) {
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

void Pairs_match_pair(void) {
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
            test_assert(id.first() == Eats);
            test_assert(id.second() == Apples);
            count ++;
        });
    
    test_int(count, 1);
}

void Pairs_match_pair_obj_wildcard(void) {
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
            test_assert(id.first() == Eats);
            test_assert(id.second() == Apples || id.second() == Pears);
            count ++;
        });
    
    test_int(count, 2);
}

void Pairs_match_pair_rel_wildcard(void) {
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
            test_assert(id.first() == Eats);
            test_assert(id.second() == Pears);
            count ++;
        });
    
    test_int(count, 1);
}

void Pairs_match_pair_both_wildcard(void) {
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

void Pairs_has_tag_w_object(void) {
    flecs::world ecs;

    struct Likes { };

    auto Bob = ecs.entity();
    auto e = ecs.entity().add<Likes>(Bob);
    test_assert(e.has<Likes>(Bob));
}

void Pairs_has_second_tag(void) {
    flecs::world ecs;

    struct Bob { };

    auto Likes = ecs.entity();
    auto e = ecs.entity().add_second<Bob>(Likes);
    test_assert(e.has_second<Bob>(Likes));
}

struct Eats { int amount; };
struct Apples { };
struct Pears { };

using EatsApples = flecs::pair<Eats, Apples>;
using EatsPears = flecs::pair<Eats, Pears>;

void Pairs_add_pair_type(void) {
    flecs::world ecs;

    auto e = ecs.entity().add<EatsApples>();
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));
}

void Pairs_remove_pair_type(void) {
    flecs::world ecs;

    auto e = ecs.entity().add<EatsApples>();
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));

    e.remove<EatsApples>();
    test_assert(!(e.has<Eats, Apples>()));
    test_assert(!(e.has<EatsApples>()));    
}

void Pairs_set_pair_type(void) {
    flecs::world ecs;

    auto e = ecs.entity().set<EatsApples>({10});
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));

    const Eats *ptr = e.try_get<EatsApples>();
    test_int(ptr->amount, 10);

    test_assert((ptr == e.try_get<Eats, Apples>()));
}

void Pairs_has_pair_type(void) {
    flecs::world ecs;

    auto e = ecs.entity().add<Eats, Apples>();
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));
}

void Pairs_get_1_pair_arg(void) {
    flecs::world ecs;

    auto e = ecs.entity().set<EatsApples>({10});
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));

    test_bool(e.get([](const EatsApples& a) {
        test_int(a->amount, 10);
    }), true);
}

void Pairs_get_2_pair_arg(void) {
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

void Pairs_set_1_pair_arg(void) {
    flecs::world ecs;

    auto e = ecs.entity()
        .insert([](EatsApples&& a) {
            a->amount = 10;
        });

    auto eats = e.try_get<EatsApples>();
    test_int(eats->amount, 10);
}

void Pairs_set_2_pair_arg(void) {
    flecs::world ecs;

    auto e = ecs.entity()
        .insert([](EatsApples&& a, EatsPears&& p) {
            a->amount = 10;
            p->amount = 20;
        });

    auto eats = e.try_get<EatsApples>();
    test_int(eats->amount, 10);

    eats = e.try_get<EatsPears>();
    test_int(eats->amount, 20);
}

void Pairs_get_inline_pair_type(void) {
    flecs::world ecs;

    auto e = ecs.entity().set<EatsApples>({10});
    test_assert((e.has<Eats, Apples>()));
    test_assert((e.has<EatsApples>()));

    test_bool(e.get([](const flecs::pair<Eats, Apples>& a) {
        test_int(a->amount, 10);
    }), true);
}

void Pairs_set_inline_pair_type(void) {
    flecs::world ecs;

    auto e = ecs.entity()
        .insert([](flecs::pair<Eats, Apples>&& a) {
            a->amount = 10;
        });

    auto eats = e.try_get<EatsApples>();
    test_int(eats->amount, 10);
}

void Pairs_get_pair_type_object(void) {
    flecs::world ecs;

    auto e = ecs.entity().set_second<Apples, Eats>({10});
    test_assert((e.has<Apples, Eats>()));

    test_bool(e.get([](const flecs::pair_object<Apples, Eats>& a) {
        test_int(a->amount, 10);
    }), true);
}

void Pairs_set_pair_type_object(void) {
    flecs::world ecs;

    auto e = ecs.entity()
        .insert([](flecs::pair_object<Apples, Eats>&& a) {
            a->amount = 10;
        });

    auto eats = e.try_get_second<Apples, Eats>();
    test_int(eats->amount, 10);
}

struct Event {
    const char *value;
};

struct Begin { };
struct End { };

using BeginEvent = flecs::pair<Begin, Event>;
using EndEvent = flecs::pair<End, Event>;

void Pairs_set_get_second_variants(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().set_second<Begin, Event>({"Big Bang"});
    test_assert((e1.has<Begin, Event>()));
    const Event* v = e1.try_get_second<Begin, Event>();
    test_assert(v != NULL);
    test_str(v->value, "Big Bang");

    auto e2 = ecs.entity().set<Begin, Event>({"Big Bang"});
    test_assert((e2.has<Begin, Event>()));
    v = e2.try_get<Begin, Event>();
    test_assert(v != NULL);
    test_str(v->value, "Big Bang");

    auto e3 = ecs.entity().set<flecs::pair<Begin, Event>>({"Big Bang"});
    test_assert((e3.has<flecs::pair<Begin, Event>>()));
    v = e3.try_get<flecs::pair<Begin, Event>>();
    test_assert(v != NULL);
    test_str(v->value, "Big Bang");  

    auto e4 = ecs.entity().set<BeginEvent>({"Big Bang"});
    test_assert((e4.has<BeginEvent>()));
    v = e4.try_get<BeginEvent>();
    test_assert(v != NULL);
    test_str(v->value, "Big Bang");            
}

void Pairs_get_object_for_type_self(void) {
    flecs::world ecs;

    ecs.component<Tag>().add(flecs::OnInstantiate, flecs::Inherit);

    auto base = ecs.entity().add<Tag>();
    auto self = ecs.entity().is_a(base).add<Tag>();

    auto obj = self.target_for<Tag>(flecs::IsA);
    test_assert(obj != 0);
    test_assert(obj == self);
}

void Pairs_get_object_for_type_base(void) {
    flecs::world ecs;

    ecs.component<Tag>().add(flecs::OnInstantiate, flecs::Inherit);

    auto base = ecs.entity().add<Tag>();
    auto self = ecs.entity().is_a(base);

    auto obj = self.target_for<Tag>(flecs::IsA);
    test_assert(obj != 0);
    test_assert(obj == base);
}

void Pairs_get_object_for_id_self(void) {
    flecs::world ecs;

    auto tag = ecs.entity().add(flecs::OnInstantiate, flecs::Inherit);
    auto base = ecs.entity().add(tag);
    auto self = ecs.entity().is_a(base).add(tag);

    auto obj = self.target_for(flecs::IsA, tag);
    test_assert(obj != 0);
    test_assert(obj == self);
}

void Pairs_get_object_for_id_base(void) {
    flecs::world ecs;

    auto tag = ecs.entity().add(flecs::OnInstantiate, flecs::Inherit);
    auto base = ecs.entity().add(tag);
    auto self = ecs.entity().is_a(base);

    auto obj = self.target_for(flecs::IsA, tag);
    test_assert(obj != 0);
    test_assert(obj == base);
}

void Pairs_get_object_for_id_not_found(void) {
    flecs::world ecs;

    auto tag = ecs.entity().add(flecs::OnInstantiate, flecs::Inherit);
    auto base = ecs.entity();
    auto self = ecs.entity().is_a(base);

    auto obj = self.target_for(flecs::IsA, tag);
    test_assert(obj == 0);
}

void Pairs_deref_pair(void) {
    flecs::world ecs;

    Position v = {10, 20};

    flecs::pair<Position, Tag> p(v);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position pos = *p;
    test_int(pos.x, 10);
    test_int(pos.y, 20);
}

void Pairs_deref_const_pair(void) {
    flecs::world ecs;

    Position v = {10, 20};

    const flecs::pair<Position, Tag> p(v);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position pos = *p;
    test_int(pos.x, 10);
    test_int(pos.y, 20);
}

void Pairs_deref_pair_obj(void) {
    flecs::world ecs;

    Position v = {10, 20};

    flecs::pair<Tag, Position> p(v);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position pos = *p;
    test_int(pos.x, 10);
    test_int(pos.y, 20);
}

void Pairs_deref_const_pair_obj(void) {
    flecs::world ecs;

    Position v = {10, 20};

    const flecs::pair<Tag, Position> p(v);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position pos = *p;
    test_int(pos.x, 10);
    test_int(pos.y, 20);
}

void Pairs_set_R_existing_value(void) {
    flecs::world ecs;
    
    Position p{10, 20};
    flecs::entity e = ecs.entity().set<Position, Tag>(p);

    const Position *ptr = e.try_get<Position, Tag>();
    test_assert(ptr != nullptr);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Pairs_symmetric_w_childof(void) {
    flecs::world ecs;

    struct Likes { };

    ecs.component<Likes>().add(flecs::Symmetric);

    flecs::entity idk = ecs.entity("Idk");

    flecs::entity bob = ecs.entity("Bob")
        .child_of(idk);
        
    flecs::entity alice = ecs.entity("Alice")
        .child_of(idk)
        .add<Likes>(bob);

    test_assert(bob.has<Likes>(alice));
}

void Pairs_modified_tag_second(void) {
    flecs::world ecs;

    int32_t count = 0;
    ecs.observer<Position>()
        .term_at(0).second<Tag>()
        .event(flecs::OnSet)
        .each([&](Position& p) {
            test_int(p.x, 10);
            test_int(p.y, 20);
            count ++;
        });

    flecs::entity e = ecs.entity();

    Position& p = e.ensure<Position, Tag>();
    p.x = 10;
    p.y = 20;
    e.modified<Position, Tag>();

    test_int(count, 1);
}

void Pairs_modified_tag_first(void) {
    flecs::world ecs;

    int32_t count = 0;
    ecs.observer()
        .with<Tag, Position>()
        .event(flecs::OnSet)
        .each([&](flecs::iter& it, size_t row) {
            auto p = it.field_at<Position>(0, row);
            test_int(p.x, 10);
            test_int(p.y, 20);
            count ++;
        });

    flecs::entity e = ecs.entity();

    Position& p = e.ensure<Tag, Position>();
    p.x = 10;
    p.y = 20;
    e.modified<Tag, Position>();

    test_int(count, 1);
}
