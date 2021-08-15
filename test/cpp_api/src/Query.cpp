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
        .set<Position>({20, 30})
        .add(flecs::IsA, base);

    auto e3 = flecs::entity(world)
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
    test_int(p->x, 21);
    test_int(p->y, 32);

    p = e3.get<Position>();
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
    
    flecs::query<> q(world, "(Pair, Velocity)");

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

    auto q = world.query_builder<Position>()
        .order_by(compare_position)
        .build();

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

void Query_default_ctor() {
    flecs::world world;

    flecs::query<Position> q_var;

    int count = 0;
    auto q = world.query<Position>();

    world.entity().set<Position>({10, 20});

    q_var = q;
    
    q_var.each([&](flecs::entity e, Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
        count ++;
    });

    test_int(count, 1);
}

void Query_expr_w_template() {
    flecs::world world;

    auto comp = world.component<Template<int>>();
    test_str(comp.name(), "Template<int>");

    int count = 0;
    auto q = world.query<Position>("Template<int>");

    world.entity()
        .set<Position>({10, 20})
        .set<Template<int>>({30, 40});
    
    q.each([&](flecs::entity e, Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        const Template<int> *t = e.get<Template<int>>();
        test_int(t->x, 30);
        test_int(t->y, 40);        

        count ++;
    });

    test_int(count, 1);
}

void Query_query_type_w_template() {
    flecs::world world;

    auto comp = world.component<Template<int>>();
    test_str(comp.name(), "Template<int>");

    int count = 0;
    auto q = world.query<Position, Template<int>>();

    world.entity()
        .set<Position>({10, 20})
        .set<Template<int>>({30, 40});
    
    q.each([&](flecs::entity e, Position& p, Template<int>& t) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(t.x, 30);
        test_int(t.y, 40);        

        count ++;
    });

    test_int(count, 1);
}

void Query_compare_term_id() {
    flecs::world world;

    int count = 0;
    auto e = world.entity().add<Tag>();

    auto q = world.query_builder<>()
        .term<Tag>()
        .build();
    
    q.iter([&](flecs::iter& it) {
        test_assert(it.term_id(1) == it.world().id<Tag>());
        test_assert(it.entity(0) == e);
        count ++;
    });

    test_int(count, 1);
}

void Query_test_no_defer_each() {
    install_test_abort();

    flecs::world world;

    struct Value { int value; };

    world.entity().add<Tag>().set<Value>({10});

    auto q = world.query_builder<Value>().term<Tag>().build();

    q.each([](flecs::entity e, Value& v) {
        test_expect_abort();
        e.remove<Tag>();
    });

    test_assert(false); // Should never get here
}

void Query_test_no_defer_iter() {
    install_test_abort();

    flecs::world world;

    struct Value { int value; };

    world.entity().add<Tag>().set<Value>({10});

    auto q = world.query_builder<Value>().term<Tag>().build();

    q.iter([](flecs::iter& it, Value *v) {
        for (auto i : it) {
            test_expect_abort();
            it.entity(i).remove<Tag>();
        }
    });

    test_assert(false); // Should never get here
}

void Query_inspect_terms() {
    flecs::world world;

    auto p = world.entity();

    auto q = world.query_builder<Position>()
        .term<Velocity>()
        .term(flecs::ChildOf, p)
        .build();

    test_int(3, q.term_count());

    auto t = q.term(0);
    test_int(t.id(), world.id<Position>());
    test_int(t.oper(), flecs::And);
    test_int(t.inout(), flecs::InOutDefault);

    t = q.term(1);
    test_int(t.id(), world.id<Velocity>());
    test_int(t.oper(), flecs::And);
    test_int(t.inout(), flecs::InOutDefault);

    t = q.term(2);
    test_int(t.id(), world.pair(flecs::ChildOf, p));
    test_int(t.oper(), flecs::And);
    test_int(t.inout(), flecs::InOutDefault);
    test_assert(t.id().object() == p);
}

void Query_inspect_terms_w_each() {
    flecs::world world;

    auto p = world.entity();

    auto q = world.query_builder<Position>()
        .term<Velocity>()
        .term(flecs::ChildOf, p)
        .build();

    int32_t count =  0;
    q.each_term([&](flecs::term& t) {
        if (count == 0) {
            test_int(t.id(), world.id<Position>());
        } else if (count == 1) {
            test_int(t.id(), world.id<Velocity>());
        } else if (count == 2) {
            test_int(t.id(), world.pair(flecs::ChildOf, p));
            test_assert(t.id().object() == p);
        } else {
            test_assert(false);
        }

        test_int(t.oper(), flecs::And);
        test_int(t.inout(), flecs::InOutDefault);

        count ++;
    });

    test_int(count, 3);
}


void Query_comp_to_str() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>()
        .build();
    test_str(q.str(), "Position, Velocity");
}

struct Eats { int amount; };
struct Apples { };
struct Pears { };

void Query_pair_to_str() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>()
        .term<Eats, Apples>()
        .build();
    test_str(q.str(), "Position, Velocity, (Eats, Apples)");
}

void Query_oper_not_to_str() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>().oper(flecs::Not)
        .build();
    test_str(q.str(), "Position, !Velocity");
}

void Query_oper_optional_to_str() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>().oper(flecs::Optional)
        .build();
    test_str(q.str(), "Position, ?Velocity");
}

void Query_oper_or_to_str() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term<Position>().oper(flecs::Or)
        .term<Velocity>().oper(flecs::Or)
        .build();
    test_str(q.str(), "Position || Velocity");
}

using EatsApples = flecs::pair<Eats, Apples>;
using EatsPears = flecs::pair<Eats, Pears>;

void Query_each_pair_type() {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set<EatsApples>({10});

    ecs.entity()
        .set<EatsPears>({20});

    auto q = ecs.query<EatsApples>();

    int count = 0;
    q.each([&](flecs::entity e, EatsApples&& a) {
        test_int(a->amount, 10);
        test_assert(e == e1);
        a->amount ++;
        count ++;
    });

    test_int(count, 1);

    auto v = e1.get<EatsApples>();
    test_assert(v != NULL);
    test_int(v->amount, 11);
}

void Query_iter_pair_type() {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set<EatsApples>({10});

    ecs.entity()
        .set<EatsPears>({20});

    auto q = ecs.query<EatsApples>();

    int count = 0;
    q.iter([&](flecs::iter& it, Eats* a) {
        test_int(it.count(), 1);

        test_int(a->amount, 10);
        test_assert(it.entity(0) == e1);

        a->amount ++;
        count ++;
    });

    test_int(count, 1);

    auto v = e1.get<EatsApples>();
    test_assert(v != NULL);
    test_int(v->amount, 11);
}

void Query_term_pair_type() {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set<EatsApples>({10});

    ecs.entity()
        .set<EatsPears>({20});

    auto q = ecs.query_builder<>()
        .term<EatsApples>()
        .build();

    int count = 0;
    q.iter([&](flecs::iter& it) {
        test_int(it.count(), 1);

        auto a = it.term<EatsApples>(1);

        test_int(a->amount, 10);
        test_assert(it.entity(0) == e1);

        a->amount ++;
        count ++;
    });

    test_int(count, 1);

    auto v = e1.get<EatsApples>();
    test_assert(v != NULL);
    test_int(v->amount, 11);
}

void Query_each_no_entity_1_comp() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set(Position{1, 2});

    auto q = ecs.query<Position>();

    int32_t count = 0;
    q.each([&](Position& p) {
        test_int(p.x, 1);
        test_int(p.y, 2);
        p.x += 1;
        p.y += 2;
        count ++;
    });

    test_int(count, 1);
    
    auto pos = e.get<Position>();
    test_int(pos->x, 2);
    test_int(pos->y, 4);
}

void Query_each_no_entity_2_comps() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set(Position{1, 2})
        .set(Velocity{10, 20});

    auto q = ecs.query<Position, Velocity>();

    int32_t count = 0;
    q.each([&](Position& p, Velocity& v) {
        test_int(p.x, 1);
        test_int(p.y, 2);
        test_int(v.x, 10);
        test_int(v.y, 20);

        p.x += 1;
        p.y += 2;
        v.x += 1;
        v.y += 2;
        count ++;
    });

    test_int(count, 1);

    test_bool(e.get([](const Position& p, const Velocity& v) {
        test_int(p.x, 2);
        test_int(p.y, 4);

        test_int(v.x, 11);
        test_int(v.y, 22);
    }), true);

    test_int(count, 1);
}

void Query_iter_no_comps_1_comp() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    auto q = ecs.query<Position>();

    int32_t count = 0;
    q.iter([&](flecs::iter& it) {
        count += it.count();
    });

    test_int(count, 3);
}

void Query_iter_no_comps_2_comps() {
    flecs::world ecs;

    ecs.entity().add<Velocity>();
    ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>().add<Velocity>();

    auto q = ecs.query<Position, Velocity>();

    int32_t count = 0;
    q.iter([&](flecs::iter& it) {

        count += it.count();
    });

    test_int(count, 2);
}

void Query_iter_no_comps_no_comps() {
    flecs::world ecs;

    ecs.entity().add<Velocity>();
    ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>().add<Velocity>();

    auto q = ecs.query_builder<>()
        .term<Position>()
        .build();

    int32_t count = 0;
    q.iter([&](flecs::iter& it) {
        count += it.count();
    });

    test_int(count, 3);
}

#include <iostream>

struct Event {
    const char *value;
};

struct Begin { };
struct End { };

using BeginEvent = flecs::pair<Begin, Event>;
using EndEvent = flecs::pair<End, Event>;

void Query_each_pair_object() {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set_w_object<Begin, Event>({"Big Bang"})
        .set<EndEvent>({"Heat Death"});

    auto q = ecs.query<BeginEvent, EndEvent>();

    int32_t count = 0;
    q.each([&](flecs::entity e, BeginEvent b_e, EndEvent e_e) {
        test_assert(e == e1);
        test_str(b_e->value, "Big Bang");
        test_str(e_e->value, "Heat Death");
        count ++;
    });

    test_int(count, 1);
}

void Query_iter_pair_object() {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set_w_object<Begin, Event>({"Big Bang"})
        .set<EndEvent>({"Heat Death"});

    auto q = ecs.query<BeginEvent, EndEvent>();

    int32_t count = 0;
    q.iter([&](flecs::iter it, Event *b_e, Event *e_e) {
        for (auto i : it) {
            test_assert(it.entity(i) == e1);
            test_str(b_e[i].value, "Big Bang");
            test_str(e_e[i].value, "Heat Death");
            count ++;
        }
    });

    test_int(count, 1);
}

void Query_iter_query_in_system() {
    flecs::world ecs;

    ecs.entity().add<Position>().add<Velocity>();

    auto q = ecs.query<Velocity>();

    int32_t count = 0;
    ecs.system<Position>()
        .each([&](flecs::entity e1, Position&) {
            q.each([&](flecs::entity e2, Velocity&) {
                count ++;
            });
        });

    ecs.progress();
 
    test_int(count, 1);
}

void Query_iter_type() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Velocity>();

    auto q = ecs.query<Position>();

    q.iter([&](flecs::iter it) {
        test_assert(it.type().vector().count() >= 1);
        test_assert(it.type().has<Position>());
    });
}
