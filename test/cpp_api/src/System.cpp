#include <cpp_api.h>

void System_action() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");

    auto entity = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    world.system<Position, Velocity>()
        .iter([](flecs::iter&it, Position *p, Velocity *v) {
            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }
        });

    world.progress();

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    const Velocity *v = entity.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);       
}

void System_action_const() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");

    auto entity = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    world.system<Position, const Velocity>()
        .iter([](flecs::iter&it, Position *p, const Velocity* v) {
            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }
        });

    world.progress();

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    const Velocity *v = entity.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);
}

void System_action_shared() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");

    auto base = world.entity()
        .set<Velocity>({1, 2});

    auto e1 = world.entity()
        .set<Position>({10, 20})
        .add(flecs::IsA, base);

    auto e2 = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    world.system<Position>().signature("ANY:Velocity")
        .iter([](flecs::iter&it, Position *p) {
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

    world.progress();

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 13);
    test_int(p->y, 24);   
}

void System_action_optional() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");
    flecs::component<Mass>(world, "Mass");

    auto e1 = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .set<Mass>({1});

    auto e2 = world.entity()
        .set<Position>({30, 40})
        .set<Velocity>({3, 4})
        .set<Mass>({1});        

    auto e3 = world.entity()
        .set<Position>({50, 60});

    auto e4 = world.entity()
        .set<Position>({70, 80});

    world.system<Position, Velocity*, Mass*>()
        .iter([](flecs::iter& it, Position *p, Velocity *v, Mass *m) {
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

    world.progress();

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

void System_each() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");

    auto entity = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    world.system<Position, Velocity>()
        .each([](flecs::entity e, Position& p, Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    world.progress();

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void System_each_const() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");

    auto entity = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    world.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    world.progress();

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void System_each_shared() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");

    auto base = world.entity()
        .set<Velocity>({1, 2});

    auto e1 = world.entity()
        .set<Position>({10, 20})
        .add(flecs::IsA, base);

    auto e2 = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    world.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    world.progress();

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 13);
    test_int(p->y, 24); 
}

void System_each_optional() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");
    flecs::component<Mass>(world, "Mass");

    auto e1 = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .set<Mass>({1});

    auto e2 = world.entity()
        .set<Position>({30, 40})
        .set<Velocity>({3, 4})
        .set<Mass>({1});        

    auto e3 = world.entity()
        .set<Position>({50, 60});

    auto e4 = world.entity()
        .set<Position>({70, 80});

    world.system<Position, Velocity*, Mass*>()
        .each([](flecs::entity e, Position& p, Velocity* v, Mass *m) {
        if (v && m) {
            p.x += v->x * m->value;
            p.y += v->y * m->value;
        } else {
            p.x ++;
            p.y ++;
        }
    });

    world.progress();

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


void System_signature() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");

    auto entity = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    world.system<>().signature("Position, Velocity")
        .iter([](flecs::iter&it) {
            flecs::column<Position> p(it, 1);
            flecs::column<Velocity> v(it, 2);

            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }
        });

    world.progress();

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    const Velocity *v = entity.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2); 
}

void System_signature_const() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");

    auto entity = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    world.system<>().signature("Position, [in] Velocity")
        .iter([](flecs::iter&it) {
            flecs::column<Position> p(it, 1);
            flecs::column<const Velocity> v(it, 2);

            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }
        });

    world.progress();

    const Position *p = entity.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    const Velocity *v = entity.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2); 
}

void System_signature_shared() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");

    auto base = world.entity()
        .set<Velocity>({1, 2});

    auto e1 = world.entity()
        .set<Position>({10, 20})
        .add(flecs::IsA, base);

    auto e2 = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    world.system<>().signature("Position, [in] ANY:Velocity")
        .iter([](flecs::iter&it) {
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

    world.progress();

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 13);
    test_int(p->y, 24); 
}

void System_signature_optional() {
    flecs::world world;

    world.component<Position>("Position");
    world.component<Velocity>("Velocity");
    flecs::component<Mass>(world, "Mass");

    auto e1 = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .set<Mass>({1});

    auto e2 = world.entity()
        .set<Position>({30, 40})
        .set<Velocity>({3, 4})
        .set<Mass>({1});        

    auto e3 = world.entity()
        .set<Position>({50, 60});

    auto e4 = world.entity()
        .set<Position>({70, 80});

    world.system<>().signature("Position, ?Velocity, ?Mass")
        .iter([](flecs::iter& it) {
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

    world.progress();

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


void System_copy_name_on_create() {
    flecs::world world;

    char name[6];
    strcpy(name, "Hello");

    auto system_1 = world.system<Position>(name)
        .iter([](flecs::iter&it, Position *p) {});

    strcpy(name, "World");
    auto system_2 = world.system<Position>(name)
        .iter([](flecs::iter&it, Position *p) {});

    test_assert(system_1.id() != system_2.id());
}

void System_nested_system() {
    flecs::world world;

    auto system_1 = world.system<Position>("foo::bar")
        .iter([](flecs::iter&it, Position *p) {});

    test_str(system_1.name().c_str(), "bar");

    auto e = world.lookup("foo");
    test_assert(e.id() != 0);
    test_str(e.name().c_str(), "foo");

    auto se = e.lookup("bar");
    test_assert(se.id() != 0);
    test_str(se.name().c_str(), "bar");
}

void System_empty_signature() {
    flecs::world world;

    int count = 0;

    world.system<>()
        .iter([&](flecs::iter it) {
            count ++;
        });

    world.progress();

    test_int(count, 1); 
}

struct MyTag { };

void System_action_tag() {
    flecs::world world;

    int invoked = 0;

    world.system<MyTag>()
        .iter([&](flecs::iter it, MyTag*) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    world.progress();

    test_int(invoked, 1);
}

void System_iter_tag() {
    flecs::world world;

    int invoked = 0;

    world.system<MyTag>()
        .iter([&](flecs::iter it, MyTag*) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    world.progress();

    test_int(invoked, 1);
}

void System_each_tag() {
    flecs::world world;

    int invoked = 0;

    world.system<MyTag>()
        .each([&](flecs::entity e, MyTag&) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    world.progress();

    test_int(invoked, 1);
}

void System_system_from_id() {
    flecs::world world;

    uint32_t invoked = 0;
    flecs::entity sys = world.system<>()
        .kind(0)
        .iter([&](flecs::iter& it) {
            invoked ++;
        });

    auto sys_from_id = world.system(sys);

    sys_from_id.run();
    test_int(invoked, 1);
}

void System_set_interval() {
    flecs::world world;

    auto sys = world.system<>()
        .kind(0)
        .interval(1.0f)
        .iter([&](flecs::iter& it) { });

    float i = sys.interval();
    test_int(i, 1.0f);

    sys.interval(2.0f);

    i = sys.interval();
    test_int(i, 2.0f);
}

void System_order_by_type() {
    flecs::world world;

    world.entity().set<Position>({3, 0});
    world.entity().set<Position>({1, 0});
    world.entity().set<Position>({5, 0});
    world.entity().set<Position>({2, 0});
    world.entity().set<Position>({4, 0});

    float last_val = 0;
    int32_t count = 0;

    auto sys = world.system<const Position>()
        .order_by<Position>(
            [](flecs::entity_t e1, const Position *p1, 
               flecs::entity_t e2, const Position *p2) {
                return (p1->x > p2->x) - (p1->x < p2->x);
            })
        .each([&](flecs::entity e, const Position& p) {
            test_assert(p.x > last_val);
            last_val = p.x;
            count ++;
        });

    sys.run();

    test_int(count, 5);
}

void System_order_by_id() {
    flecs::world world;

    auto pos = world.component<Position>();

    world.entity().set<Position>({3, 0});
    world.entity().set<Position>({1, 0});
    world.entity().set<Position>({5, 0});
    world.entity().set<Position>({2, 0});
    world.entity().set<Position>({4, 0});

    float last_val = 0;
    int32_t count = 0;

    auto sys = world.system<const Position>()
        .order_by(pos, [](flecs::entity_t e1, const void *p1, 
                          flecs::entity_t e2, const void *p2) 
            {
                return (static_cast<const Position*>(p1)->x > 
                            static_cast<const Position*>(p2)->x) - 
                       (static_cast<const Position*>(p1)->x < 
                            static_cast<const Position*>(p2)->x);
            })
        .each([&](flecs::entity e, const Position& p) {
            test_assert(p.x > last_val);
            last_val = p.x;
            count ++;
        });

    sys.run();

    test_int(count, 5);
}

void System_order_by_type_after_create() {
    flecs::world world;

    world.entity().set<Position>({3, 0});
    world.entity().set<Position>({1, 0});
    world.entity().set<Position>({5, 0});
    world.entity().set<Position>({2, 0});
    world.entity().set<Position>({4, 0});

    float last_val = 0;
    int32_t count = 0;

    auto sys = world.system<const Position>()
        .each([&](flecs::entity e, const Position& p) {
            test_assert(p.x > last_val);
            last_val = p.x;
            count ++;
        });

    sys.order_by<Position>(
        [](flecs::entity_t e1, const Position *p1, 
            flecs::entity_t e2, const Position *p2) {
            return (p1->x > p2->x) - (p1->x < p2->x);
        });

    sys.run();

    test_int(count, 5);
}

void System_order_by_id_after_create() {
    flecs::world world;

    auto pos = world.component<Position>();

    world.entity().set<Position>({3, 0});
    world.entity().set<Position>({1, 0});
    world.entity().set<Position>({5, 0});
    world.entity().set<Position>({2, 0});
    world.entity().set<Position>({4, 0});

    float last_val = 0;
    int32_t count = 0;

    auto sys = world.system<const Position>()
        .each([&](flecs::entity e, const Position& p) {
            test_assert(p.x > last_val);
            last_val = p.x;
            count ++;
        });

    sys.order_by(pos, [](flecs::entity_t e1, const void *p1, flecs::entity_t e2, const void *p2) {
        return (static_cast<const Position*>(p1)->x > static_cast<const Position*>(p2)->x) - 
               (static_cast<const Position*>(p1)->x < static_cast<const Position*>(p2)->x);
    });

    sys.run();

    test_int(count, 5);
}

void System_get_query() {
    flecs::world world;

    world.entity().set<Position>({0, 0});
    world.entity().set<Position>({1, 0});
    world.entity().set<Position>({2, 0});

    int32_t count = 0;

    auto sys = world.system<const Position>()
        .each([&](flecs::entity e, const Position& p) {
            // Not used
        });

    auto q = sys.query();

    q.iter([&](flecs::iter &it) {
        auto pos = it.term<const Position>(1);
        for (auto i : it) {
            test_int(i, pos[i].x);
            count ++;
        }
    });

    test_int(count, 3);
}

void System_add_from_each() {
    flecs::world world;

    world.component<Position>();
    world.component<Velocity>();

    auto e1 = world.entity().set<Position>({0, 0});
    auto e2 = world.entity().set<Position>({1, 0});
    auto e3 = world.entity().set<Position>({2, 0});

    world.system<const Position>()
        .each([](flecs::entity e, const Position& p) {
            e.add<Velocity>();
            // Add is deferred
            test_assert(!e.has<Velocity>());
        });

    world.progress();

    test_assert(e1.has<Velocity>());
    test_assert(e2.has<Velocity>());
    test_assert(e3.has<Velocity>());
}

void System_delete_from_each() {
    flecs::world world;

    world.component<Position>();
    world.component<Velocity>();

    auto e1 = world.entity().set<Position>({0, 0});
    auto e2 = world.entity().set<Position>({1, 0});
    auto e3 = world.entity().set<Position>({2, 0});

    world.system<const Position>()
        .each([](flecs::entity e, const Position& p) {
            e.destruct();
            // Delete is deferred
            test_assert(e.is_alive());
        });

    world.progress();

    test_assert(!e1.is_alive());
    test_assert(!e2.is_alive());
    test_assert(!e3.is_alive());
}

struct Entity {
    flecs::entity e;
};

void System_add_from_each_world_handle() {
    flecs::world world;

    auto e1 = world.entity().set<Entity>({world.entity()});
    auto e2 = world.entity().set<Entity>({world.entity()});
    auto e3 = world.entity().set<Entity>({world.entity()});

    world.system<const Entity>()
        .each([](flecs::entity e, const Entity& c) {
            c.e.mut(e).add<Position>();
        });

    world.progress();

    test_assert(e1.get<Entity>()->e.has<Position>());
    test_assert(e2.get<Entity>()->e.has<Position>());
    test_assert(e3.get<Entity>()->e.has<Position>());
}

void System_new_from_each() {
    flecs::world world;

    auto e1 = world.entity().set<Position>({0, 0});
    auto e2 = world.entity().set<Position>({0, 0});
    auto e3 = world.entity().set<Position>({0, 0});

    world.system<const Position>()
        .each([](flecs::entity e, const Position& p) {
            e.set<Entity>({
                e.world().entity().add<Velocity>()
            });
        });

    world.progress();

    test_assert(e1.has<Entity>());
    test_assert(e2.has<Entity>());
    test_assert(e3.has<Entity>());

    test_assert(e1.get<Entity>()->e.has<Velocity>());
    test_assert(e2.get<Entity>()->e.has<Velocity>());
    test_assert(e3.get<Entity>()->e.has<Velocity>());
}

void System_add_from_iter() {
    flecs::world world;

    world.component<Position>();
    world.component<Velocity>();

    auto e1 = world.entity().set<Position>({0, 0});
    auto e2 = world.entity().set<Position>({1, 0});
    auto e3 = world.entity().set<Position>({2, 0});

    world.system<const Position>()
        .iter([](flecs::iter& it, const Position* p) {
            for (auto i : it) {
                it.entity(i).add<Velocity>();
                test_assert(!it.entity(i).has<Velocity>());
            }
        });

    world.progress();

    test_assert(e1.has<Velocity>());
    test_assert(e2.has<Velocity>());
    test_assert(e3.has<Velocity>());
}

void System_delete_from_iter() {
    flecs::world world;

    world.component<Position>();
    world.component<Velocity>();

    auto e1 = world.entity().set<Position>({0, 0});
    auto e2 = world.entity().set<Position>({1, 0});
    auto e3 = world.entity().set<Position>({2, 0});

    world.system<const Position>()
        .iter([](const flecs::iter& it, const Position* p) {
            for (auto i : it) {
                it.entity(i).destruct();
                // Delete is deferred
                test_assert(it.entity(i).is_alive());
            }
        });

    world.progress();

    test_assert(!e1.is_alive());
    test_assert(!e2.is_alive());
    test_assert(!e3.is_alive());
}

void System_add_from_iter_world_handle() {
    flecs::world world;

    auto e1 = world.entity().set<Entity>({world.entity()});
    auto e2 = world.entity().set<Entity>({world.entity()});
    auto e3 = world.entity().set<Entity>({world.entity()});

    world.system<const Entity>()
        .iter([](const flecs::iter& it, const Entity* c) {
            for (auto i : it) {
                c[i].e.mut(it).add<Position>();
            }
        });

    world.progress();

    test_assert(e1.get<Entity>()->e.has<Position>());
    test_assert(e2.get<Entity>()->e.has<Position>());
    test_assert(e3.get<Entity>()->e.has<Position>());
}

void System_new_from_iter() {
    flecs::world world;

    auto e1 = world.entity().set<Position>({0, 0});
    auto e2 = world.entity().set<Position>({0, 0});
    auto e3 = world.entity().set<Position>({0, 0});

    world.system<const Position>()
        .iter([](const flecs::iter& it, const Position* p) {
            for (auto i : it) {
                it.entity(i).set<Entity>({
                    it.world().entity().add<Velocity>()
                });
            }
        });

    world.progress();

    test_assert(e1.has<Entity>());
    test_assert(e2.has<Entity>());
    test_assert(e3.has<Entity>());

    test_assert(e1.get<Entity>()->e.has<Velocity>());
    test_assert(e2.get<Entity>()->e.has<Velocity>());
    test_assert(e3.get<Entity>()->e.has<Velocity>());
}

void System_each_w_mut_children_it() {
    flecs::world world;

    auto parent = world.entity().set<Position>({0, 0});
    auto e1 = world.entity().set<Position>({0, 0}).child_of(parent);
    auto e2 = world.entity().set<Position>({0, 0}).child_of(parent);
    auto e3 = world.entity().set<Position>({0, 0}).child_of(parent);

    world.system<const Position>()
        .iter([](const flecs::iter& it, const Position* p) {
            for (auto i : it) {
                for (flecs::iter child_it : it.entity(i).children()) {
                    for (auto c : child_it) {
                        child_it.entity(c).add<Velocity>();
                    }
                }
            }
        });

    world.progress();

    test_assert(e1.has<Velocity>());
    test_assert(e2.has<Velocity>());
    test_assert(e3.has<Velocity>());
}

void System_readonly_children_iter() {
    flecs::world world;

    auto parent = world.entity();
    world.entity().set<Entity>({ parent });
    world.entity().set<Position>({1, 0}).child_of(parent);
    world.entity().set<Position>({1, 0}).child_of(parent);
    world.entity().set<Position>({1, 0}).child_of(parent);

    world.system<const Entity>()
        .iter([](const flecs::iter& it, const Entity* c) {
            for (auto i : it) {
                for (flecs::iter child_it : c[i].e.children()) {
                    for (auto c : child_it) {
                        // Dummy code to ensure we can access the entity
                        const Position *p = child_it.entity(c).get<Position>();
                        test_int(p->x, 1);
                        test_int(p->y, 0);
                    }
                }
            }
        });

    world.progress();
}

void System_rate_filter() {
    flecs::world world;

    int32_t 
    root_count = 0, root_mult = 1, 
    l1_a_count = 0, l1_a_mult = 1, 
    l1_b_count = 0, l1_b_mult = 2, 
    l1_c_count = 0, l1_c_mult = 3,
    l2_a_count = 0, l2_a_mult = 2,
    l2_b_count = 0, l2_b_mult = 4,
    frame_count = 0;

    auto root = world.system<>("root")
        .iter([&](flecs::iter& it) {
            root_count ++;
        });

    auto l1_a = world.system<>("l1_a")
        .rate(root, 1)
        .iter([&](flecs::iter& it) {
            l1_a_count ++;
        });  

    auto l1_b = world.system<>("l1_b")
        .rate(root, 2)
        .iter([&](flecs::iter& it) {
            l1_b_count ++;
        });

    world.system<>("l1_c")
        .rate(root, 3)
        .iter([&](flecs::iter& it) {
            l1_c_count ++;
        });        

    world.system<>("l2_a")
        .rate(l1_a, 2)
        .iter([&](flecs::iter& it) {
            l2_a_count ++;
        });

    world.system<>("l2_b")
        .rate(l1_b, 2)
        .iter([&](flecs::iter& it) {
            l2_b_count ++;
        });

    for (int i = 0; i < 30; i ++) {
        world.progress(); frame_count ++;
        test_int(root_count, frame_count / root_mult);
        test_int(l1_a_count, frame_count / l1_a_mult);
        test_int(l1_b_count, frame_count / l1_b_mult);
        test_int(l1_c_count, frame_count / l1_c_mult);
        test_int(l2_a_count, frame_count / l2_a_mult);
        test_int(l2_b_count, frame_count / l2_b_mult);
    }    
}

void System_update_rate_filter() {
    flecs::world world;

    int32_t 
    root_count = 0, root_mult = 1, 
    l1_count = 0, l1_mult = 2, 
    l2_count = 0, l2_mult = 6,
    frame_count = 0;

    auto root = world.system<>("root")
        .iter([&](flecs::iter& it) {
            root_count ++;
        });

    auto l1 = world.system<>("l1")
        .rate(root, 2)
        .iter([&](flecs::iter& it) {
            l1_count ++;
        });  

    world.system<>("l2")
        .rate(l1, 3)
        .iter([&](flecs::iter& it) {
            l2_count ++;
        }); 

    for (int i = 0; i < 12; i ++) {
        world.progress(); frame_count ++;
        test_int(root_count, frame_count / root_mult);
        test_int(l1_count, frame_count / l1_mult);
        test_int(l2_count, frame_count / l2_mult);
    }

    l1.rate(4); // Run twice as slow
    l1_mult *= 2;
    l2_mult *= 2;

    frame_count = 0;
    l1_count = 0;
    l2_count = 0;
    root_count = 0;

    for (int i = 0; i < 32; i ++) {
        world.progress(); frame_count ++;
        test_int(root_count, frame_count / root_mult);
        test_int(l1_count, frame_count / l1_mult);
        test_int(l2_count, frame_count / l2_mult);
    }      
}

void System_default_ctor() {
    flecs::world world;

    flecs::system<Position> sys_var;

    int count = 0;
    auto sys = world.system<Position>()
        .each([&](flecs::entity e, Position& p) {
            test_int(p.x, 10);
            test_int(p.y, 20);
            count ++;
        });

    world.entity().set<Position>({10, 20});

    sys_var = sys;

    sys_var.run();

    test_int(count, 1);
}

void System_test_auto_defer_each() {
    flecs::world world;

    struct Value { int value; };

    auto e1 = world.entity().add<Tag>().set<Value>({10});
    auto e2 = world.entity().add<Tag>().set<Value>({20});
    auto e3 = world.entity().add<Tag>().set<Value>({30});

    auto s = world.system<Value>()
        .term<Tag>()
        .each([](flecs::entity e, Value& v) {
            v.value ++;
            e.remove<Tag>();
        });

    s.run();

    test_assert(!e1.has<Tag>());
    test_assert(!e2.has<Tag>());
    test_assert(!e3.has<Tag>());

    test_assert(e1.has<Value>());
    test_assert(e2.has<Value>());
    test_assert(e3.has<Value>());

    test_int(e1.get<Value>()->value, 11);
    test_int(e2.get<Value>()->value, 21);
    test_int(e3.get<Value>()->value, 31);
}

void System_test_auto_defer_iter() {
    flecs::world world;

    struct Value { int value; };

    auto e1 = world.entity().add<Tag>().set<Value>({10});
    auto e2 = world.entity().add<Tag>().set<Value>({20});
    auto e3 = world.entity().add<Tag>().set<Value>({30});

    auto s = world.system<Value>()
        .term<Tag>()
        .iter([](flecs::iter& it, Value *v) {
            for (auto i : it) {
                v[i].value ++;
                it.entity(i).remove<Tag>();
            }
        });

    s.run();

    test_assert(!e1.has<Tag>());
    test_assert(!e2.has<Tag>());
    test_assert(!e3.has<Tag>());

    test_assert(e1.has<Value>());
    test_assert(e2.has<Value>());
    test_assert(e3.has<Value>());

    test_int(e1.get<Value>()->value, 11);
    test_int(e2.get<Value>()->value, 21);
    test_int(e3.get<Value>()->value, 31);
}
