#include <cpp_api.h>

struct Pair {
    float value;
};

void Query_action() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::query<Position, Velocity> q(world);

    q.iter([](flecs::iter& it, Position *p, Velocity *v) {
        for (auto i : it) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
        }
    });

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void Query_action_const() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::query<Position, const Velocity> q(world);

    q.iter([](flecs::iter& it, Position *p, const Velocity *v) {
        for (auto i : it) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
        }
    });

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void Query_action_shared() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto base = flecs::entity(world)
        .set<Velocity>({1, 2});

    auto e1 = flecs::entity(world)
        .set<Position>({10, 20})
        .add(flecs::IsA, base);

    auto e2 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    flecs::query<Position> q(world, "ANY:Velocity");

    q.iter([](flecs::iter&it, Position *p) {
            auto v = it.term<const Velocity>(2);

            if (v.is_shared()) {
                for (auto i : it) {
                    p[i].x += v->x;
                    p[i].y += v->y;
                }
            } else {
                for (auto i : it) {
                    p[i].x += v[i].x;
                    p[i].y += v[i].y;
                }                
            }
        });

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 13);
    test_int(p->y, 24);  
}

void Query_action_optional() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");
    flecs::component<Mass>(world, "Mass");

    auto e1 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .set<Mass>({1});

    auto e2 = flecs::entity(world)
        .set<Position>({30, 40})
        .set<Velocity>({3, 4})
        .set<Mass>({1});        

    auto e3 = flecs::entity(world)
        .set<Position>({50, 60});

    auto e4 = flecs::entity(world)
        .set<Position>({70, 80});

    flecs::query<Position, Velocity*, Mass*> q(world);

    q.iter([](flecs::iter& it, Position *p, Velocity *v, Mass *m) {
        if (it.is_set(2) && it.is_set(3)) {
            for (auto i : it) {
                p[i].x += v[i].x * m[i].value;
                p[i].y += v[i].y * m[i].value;
            }
        } else {
            for (auto i : it) {
                p[i].x ++;
                p[i].y ++;
            }
        }
    });

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 33);
    test_int(p->y, 44);

    p = e3.get<Position>();
    test_int(p->x, 51);
    test_int(p->y, 61);

    p = e4.get<Position>();
    test_int(p->x, 71);
    test_int(p->y, 81);
}

void Query_each() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::query<Position, Velocity> q(world);

    q.each([](flecs::entity e, Position& p, Velocity& v) {
        p.x += v.x;
        p.y += v.y;
    });

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void Query_each_const() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::query<Position, const Velocity> q(world);

    q.each([](flecs::entity e, Position& p, const Velocity& v) {
        p.x += v.x;
        p.y += v.y;
    });

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void Query_each_shared() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto base = flecs::entity(world)
        .set<Velocity>({1, 2});

    auto e1 = flecs::entity(world)
        .set<Position>({10, 20})
        .add(flecs::IsA, base);

    auto e2 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    flecs::query<Position, const Velocity> q(world);

    q.each([](flecs::entity e, Position& p, const Velocity& v) {
        p.x += v.x;
        p.y += v.y;
    });

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 13);
    test_int(p->y, 24); 
}

void Query_each_optional() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");
    flecs::component<Mass>(world, "Mass");

    auto e1 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .set<Mass>({1});

    auto e2 = flecs::entity(world)
        .set<Position>({30, 40})
        .set<Velocity>({3, 4})
        .set<Mass>({1});        

    auto e3 = flecs::entity(world)
        .set<Position>({50, 60});

    auto e4 = flecs::entity(world)
        .set<Position>({70, 80});

    flecs::query<Position, Velocity*, Mass*> q(world);

    q.each([](flecs::entity e, Position& p, Velocity* v, Mass *m) {
        if (v && m) {
            p.x += v->x * m->value;
            p.y += v->y * m->value;
        } else {
            p.x ++;
            p.y ++;
        }
    });

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 33);
    test_int(p->y, 44);

    p = e3.get<Position>();
    test_int(p->x, 51);
    test_int(p->y, 61);

    p = e4.get<Position>();
    test_int(p->x, 71);
    test_int(p->y, 81);  
}

void Query_signature() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::query<> q(world, "Position, Velocity");

    q.iter([](flecs::iter& it) {
        auto p = it.term<Position>(1);
        auto v = it.term<Velocity>(2);

        for (auto i : it) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
        }
    });

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void Query_signature_const() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::query<> q(world, "Position, [in] Velocity");

    q.iter([](flecs::iter& it) {
        auto p = it.term<Position>(1);
        auto v = it.term<const Velocity>(2);
        
        for (auto i : it) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
        }
    });

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void Query_signature_shared() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto base = flecs::entity(world)
        .set<Velocity>({1, 2});

    auto e1 = flecs::entity(world)
        .set<Position>({10, 20})
        .add(flecs::IsA, base);

    auto e2 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    flecs::query<> q(world, "Position, [in] ANY:Velocity");
    
    q.iter([](flecs::iter&it) {
        auto p = it.term<Position>(1);
        auto v = it.term<const Velocity>(2);

        if (v.is_shared()) {
            for (auto i : it) {
                p[i].x += v->x;
                p[i].y += v->y;
            }
        } else {
            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }                
        }
    });

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 13);
    test_int(p->y, 24); 
}

void Query_signature_optional() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");
    flecs::component<Mass>(world, "Mass");

    auto e1 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .set<Mass>({1});

    auto e2 = flecs::entity(world)
        .set<Position>({30, 40})
        .set<Velocity>({3, 4})
        .set<Mass>({1});        

    auto e3 = flecs::entity(world)
        .set<Position>({50, 60});

    auto e4 = flecs::entity(world)
        .set<Position>({70, 80});

    flecs::query<> q(world, "Position, ?Velocity, ?Mass");

    q.iter([](flecs::iter& it) {
        auto p = it.term<Position>(1);
        auto v = it.term<const Velocity>(2);
        auto m = it.term<const Mass>(3);

        if (v.is_set() && m.is_set()) {
            for (auto i : it) {
                p[i].x += v[i].x * m[i].value;
                p[i].y += v[i].y * m[i].value;
            }
        } else {
            for (auto i : it) {
                p[i].x ++;
                p[i].y ++;
            }            
        }
    });

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 33);
    test_int(p->y, 44);

    p = e3.get<Position>();
    test_int(p->x, 51);
    test_int(p->y, 61);

    p = e4.get<Position>();
    test_int(p->x, 71);
    test_int(p->y, 81); 
}

void Query_subquery() {
    flecs::world world;

    auto e1 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto e2 = flecs::entity(world)
        .set<Velocity>({1, 2});        

    flecs::query<Position> q(world);
    flecs::query<Velocity> sq(world, q);

    sq.each([](flecs::entity e, Velocity& v) {
        v.x ++;
        v.y ++;
    });

    const Velocity *v = e1.get<Velocity>();
    test_int(v->x, 2);
    test_int(v->y, 3);

    v = e2.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);
}

void Query_subquery_w_expr() {
    flecs::world world;

    auto e1 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto e2 = flecs::entity(world)
        .set<Velocity>({1, 2});        

    flecs::query<Position> q(world);
    flecs::query<> sq(world, q, "Velocity");

    sq.iter([](flecs::iter it) {
        auto v = it.term<Velocity>(1);

        for (auto i : it) {
            v[i].x ++;
            v[i].y ++;
        }
    });

    const Velocity *v = e1.get<Velocity>();
    test_int(v->x, 2);
    test_int(v->y, 3);

    v = e2.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);
}

void Query_query_single_pair() {
    flecs::world world;

    flecs::entity(world).add<Pair, Position>();
    auto e2 = flecs::entity(world).add<Pair, Velocity>();
    
    flecs::query<> q(world, "PAIR | Pair > Velocity");

    int32_t table_count = 0;
    int32_t entity_count = 0;

    q.iter([&](flecs::iter it) {
        table_count ++;
        for (auto i : it) {
            test_assert(it.entity(i) == e2);
            entity_count ++;
        }
    });

    test_int(table_count, 1);
    test_int(entity_count, 1);
}

void Query_tag_w_each() {
    flecs::world world;

    auto q = world.query<Tag>();

    auto e = world.entity()
        .add<Tag>();

    q.each([&](flecs::entity qe, Tag) {
        test_assert(qe == e);
    });
}

void Query_shared_tag_w_each() {
    flecs::world world;

    auto q = world.query<Tag>();

    auto base = world.prefab()
        .add<Tag>();

    auto e = world.entity()
        .add(flecs::IsA, base);

    q.each([&](flecs::entity qe, Tag) {
        test_assert(qe == e);
    });
}

static
int compare_position(
    flecs::entity_t e1,
    const Position *p1,
    flecs::entity_t e2,
    const Position *p2)
{
    return (p1->x > p2->x) - (p1->x < p2->x);
}

void Query_sort_by() {
    flecs::world world;

    world.entity().set<Position>({1, 0});
    world.entity().set<Position>({6, 0});
    world.entity().set<Position>({2, 0});
    world.entity().set<Position>({5, 0});
    world.entity().set<Position>({4, 0});

    auto q = world.query<Position>();

    q.order_by(compare_position);

    q.iter([](flecs::iter it, Position *p) {
        test_int(it.count(), 5);
        test_int(p[0].x, 1);
        test_int(p[1].x, 2);
        test_int(p[2].x, 4);
        test_int(p[3].x, 5);
        test_int(p[4].x, 6);
    });
}

void Query_changed() {
    flecs::world world;

    auto e = world.entity().set<Position>({1, 0});

    auto q = world.query<Position>();

    test_bool(q.changed(), true);

    q.each([](flecs::entity e, Position& p) { });

    test_bool(q.changed(), false);
    
    e.set<Position>({2, 0});

    test_bool(q.changed(), true);
}

void Query_orphaned() {
    flecs::world world;

    auto q = flecs::query<Position>(world);

    auto sq = world.query<Position>(q);
    
    test_assert(!q.orphaned());
    test_assert(!sq.orphaned());

    q.destruct();

    test_assert(sq.orphaned());
}
