#include <cpp_api.h>

void System_action() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::system<Position, Velocity>(world)
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

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::system<Position, const Velocity>(world)
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

    flecs::system<Position>(world).signature("ANY:Velocity")
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

    flecs::system<Position, Velocity*, Mass*>(world)
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

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::system<Position, Velocity>(world)
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

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::system<Position, const Velocity>(world)
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

    flecs::system<Position, const Velocity>(world)
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

    flecs::system<Position, Velocity*, Mass*>(world)
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

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::system<>(world).signature("Position, Velocity")
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

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    flecs::system<>(world).signature("Position, [in] Velocity")
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

    flecs::system<>(world).signature("Position, [in] ANY:Velocity")
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

    flecs::system<>(world).signature("Position, ?Velocity, ?Mass")
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

    auto system_1 = flecs::system<Position>(world, name)
        .iter([](flecs::iter&it, Position *p) {});

    strcpy(name, "World");
    auto system_2 = flecs::system<Position>(world, name)
        .iter([](flecs::iter&it, Position *p) {});

    test_assert(system_1.id() != system_2.id());
}

void System_nested_system() {
    flecs::world world;

    auto system_1 = flecs::system<Position>(world, "foo::bar")
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

    flecs::system<>(world)
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

    auto sys_from_id = world.system<>(sys);

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
                e.world().entity().mut(e).add<Velocity>()
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
                    it.world().entity().mut(it).add<Velocity>()
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
    auto e1 = world.entity().set<Position>({0, 0}).add(flecs::ChildOf, parent);
    auto e2 = world.entity().set<Position>({0, 0}).add(flecs::ChildOf, parent);
    auto e3 = world.entity().set<Position>({0, 0}).add(flecs::ChildOf, parent);

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
    world.entity().set<Position>({1, 0}).add(flecs::ChildOf, parent);
    world.entity().set<Position>({1, 0}).add(flecs::ChildOf, parent);
    world.entity().set<Position>({1, 0}).add(flecs::ChildOf, parent);

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
