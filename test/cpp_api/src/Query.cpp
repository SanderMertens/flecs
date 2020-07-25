#include <cpp_api.h>

void Query_action() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::query<Position, Velocity> q(world);

    q.action([](flecs::iter& it, flecs::column<Position> p, flecs::column<Velocity> v) {
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

    q.action([](flecs::iter& it, flecs::column<Position> p, flecs::column<const Velocity> v) {
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
        .add_instanceof(base);

    auto e2 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    flecs::query<Position> q(world, "ANY:Velocity");

    q.action([](flecs::iter&it, flecs::column<Position> p) {
            flecs::column<const Velocity> v(it, 2);

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

    q.action([](flecs::iter& it, flecs::column<Position> p, flecs::column<Velocity> v, flecs::column<Mass> m) {
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
        .add_instanceof(base);

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

    q.action([](flecs::iter& it) {
        flecs::column<Position> p(it, 1);
        flecs::column<Velocity> v(it, 2);

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

    q.action([](flecs::iter& it) {
        flecs::column<Position> p(it, 1);
        flecs::column<const Velocity> v(it, 2);
        
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
        .add_instanceof(base);

    auto e2 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    flecs::query<> q(world, "Position, [in] ANY:Velocity");
    
    q.action([](flecs::iter&it) {
        flecs::column<Position> p(it, 1);
        flecs::column<const Velocity> v(it, 2);

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

    q.action([](flecs::iter& it) {
        flecs::column<Position> p(it, 1);
        flecs::column<Velocity> v(it, 2);
        flecs::column<Mass> m(it, 3);

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
