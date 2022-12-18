#include <cpp_api.h>

void Table_each() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();

    ecs.filter<Position>()
        .each([&](flecs::entity e, Position& p) {
            e2.add<Mass>();
        });

    test_assert(e2.has<Mass>());
}

void Table_each_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    ecs.filter<Position>()
        .each([&](flecs::entity e, Position& p) {
            test_expect_abort();
            e1.add<Mass>();
        });

    test_assert(false);
}

void Table_each_without_entity() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();

    ecs.filter<Position>()
        .each([&](Position& p) {
            e2.add<Mass>();
        });

    test_assert(e2.has<Mass>());
}

void Table_each_without_entity_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    ecs.filter<Position>()
        .each([&](Position& p) {
            test_expect_abort();
            e1.add<Mass>();
        });

    test_assert(false);
}

void Table_iter() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();

    ecs.filter<Position>()
        .iter([&](flecs::iter& it, Position* p) {
            e2.add<Mass>();
        });

    test_assert(e2.has<Mass>());
}

void Table_iter_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    ecs.filter<Position>()
        .iter([&](flecs::iter& it, Position* p) {
            test_expect_abort();
            e1.add<Mass>();
        });

    test_assert(false);
}

void Table_iter_without_components() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();

    ecs.filter<Position>()
        .iter([&](flecs::iter& it) {
            e2.add<Mass>();
        });

    test_assert(e2.has<Mass>());
}

void Table_iter_without_components_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    ecs.filter<Position>()
        .iter([&](flecs::iter& it) {
            test_expect_abort();
            e1.add<Mass>();
        });

    test_assert(false);
}

void Table_multi_get() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    test_bool(true, e1.get([&](const Position& p, const Velocity& v) {
        e2.add<Mass>();
    }));

    test_assert(e2.has<Mass>());
}

void Table_multi_get_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    test_bool(true, e1.get([&](const Position& p, const Velocity& v) {
        test_expect_abort();
        e2.add<Velocity>();
    }));

    test_assert(false);
}

void Table_multi_set() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    e1.set([&](Position& p, Velocity& v) {
        e2.add<Mass>();
    });

    test_assert(e2.has<Mass>());
}

void Table_multi_set_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    e1.set([&](Position& p, Velocity& v) {
        test_expect_abort();
        e2.add<Velocity>();
    });

    test_assert(false);
}

void Table_count() {
    flecs::world ecs;

    flecs::entity e = ecs.entity().set<Position>({10, 20});
    ecs.entity().set<Position>({20, 30});
    ecs.entity().set<Position>({30, 40});

    flecs::table table = e.table();
    test_int(table.count(), 3);
}

void Table_has_id() {
    flecs::world ecs;
    
    flecs::entity t1 = ecs.entity();
    flecs::entity t2 = ecs.entity();
    flecs::entity t3 = ecs.entity();

    flecs::entity e = ecs.entity()
        .add(t1)
        .add(t2);
    ecs.entity()
        .add(t1)
        .add(t2);
    ecs.entity()
        .add(t1)
        .add(t2);

    flecs::table table = e.table();
    test_assert(table.has(t1));
    test_assert(table.has(t2));
    test_assert(!table.has(t3));
}

void Table_has_T() {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});
    ecs.entity()
        .set<Position>({20, 30})
        .set<Velocity>({2, 3});
    ecs.entity()
        .set<Position>({30, 40})
        .set<Velocity>({3, 4});

    flecs::table table = e.table();
    
    test_assert(table.has<Position>());
    test_assert(table.has<Velocity>());
    test_assert(!table.has<Mass>());
}

void Table_has_pair_r_t() {
    flecs::world ecs;
    
    flecs::entity r = ecs.entity();
    flecs::entity t1 = ecs.entity();
    flecs::entity t2 = ecs.entity();
    flecs::entity t3 = ecs.entity();

    flecs::entity e = ecs.entity()
        .add(r, t1)
        .add(r, t2);
    ecs.entity()
        .add(r, t1)
        .add(r, t2);
    ecs.entity()
        .add(r, t1)
        .add(r, t2);

    flecs::table table = e.table();
    test_assert(table.has(r, t1));
    test_assert(table.has(r, t2));
    test_assert(!table.has(r, t3));
}

void Table_has_pair_R_t() {
    flecs::world ecs;
    
    struct R { };
    flecs::entity t1 = ecs.entity();
    flecs::entity t2 = ecs.entity();
    flecs::entity t3 = ecs.entity();

    flecs::entity e = ecs.entity()
        .add<R>(t1)
        .add<R>(t2);
    ecs.entity()
        .add<R>(t1)
        .add<R>(t2);
    ecs.entity()
        .add<R>(t1)
        .add<R>(t2);

    flecs::table table = e.table();
    test_assert(table.has<R>(t1));
    test_assert(table.has<R>(t2));
    test_assert(!table.has<R>(t3));
}

void Table_has_pair_R_T() {
    flecs::world ecs;
    
    struct R { };
    struct T1 { };
    struct T2 { };
    struct T3 { };

    flecs::entity e = ecs.entity()
        .add<R,T1>()
        .add<R,T2>();
    ecs.entity()
        .add<R,T1>()
        .add<R,T2>();
    ecs.entity()
        .add<R,T1>()
        .add<R,T2>();

    flecs::table table = e.table();
    test_assert((table.has<R, T1>()));
    test_assert((table.has<R, T2>()));
    test_assert((!table.has<R, T3>()));
}

void Table_get_id() {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});
    ecs.entity()
        .set<Position>({20, 30})
        .set<Velocity>({2, 3});
    ecs.entity()
        .set<Position>({30, 40})
        .set<Velocity>({3, 4});

    flecs::table table = e.table();
    void *ptr = table.get(ecs.id<Position>());
    Position *p = static_cast<Position*>(ptr);
    test_assert(p != NULL);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_int(p[2].x, 30);
    test_int(p[2].y, 40);

    ptr = table.get(ecs.id<Velocity>());
    Velocity *v = static_cast<Velocity*>(ptr);
    test_assert(v != NULL);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);
    test_int(v[2].x, 3);
    test_int(v[2].y, 4);
}

void Table_get_T() {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});
    ecs.entity()
        .set<Position>({20, 30})
        .set<Velocity>({2, 3});
    ecs.entity()
        .set<Position>({30, 40})
        .set<Velocity>({3, 4});

    flecs::table table = e.table();
    Position *p = table.get<Position>();
    test_assert(p != NULL);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_int(p[2].x, 30);
    test_int(p[2].y, 40);

    Velocity *v = table.get<Velocity>();
    test_assert(v != NULL);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);
    test_int(v[2].x, 3);
    test_int(v[2].y, 4);
}

void Table_get_pair_r_t() {
    flecs::world ecs;

    struct Tgt { };

    flecs::entity e = ecs.entity()
        .set<Position, Tgt>({10, 20})
        .set<Velocity, Tgt>({1, 2});
    ecs.entity()
        .set<Position, Tgt>({20, 30})
        .set<Velocity, Tgt>({2, 3});
    ecs.entity()
        .set<Position, Tgt>({30, 40})
        .set<Velocity, Tgt>({3, 4});

    flecs::table table = e.table();
    void *ptr = table.get(ecs.id<Position>(), ecs.id<Tgt>());
    Position *p = static_cast<Position*>(ptr);
    test_assert(p != NULL);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_int(p[2].x, 30);
    test_int(p[2].y, 40);

    ptr = table.get(ecs.id<Velocity>(), ecs.id<Tgt>());
    Velocity *v = static_cast<Velocity*>(ptr);
    test_assert(v != NULL);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);
    test_int(v[2].x, 3);
    test_int(v[2].y, 4);
}

void Table_get_pair_R_t() {
    flecs::world ecs;

    struct Tgt { };

    flecs::entity e = ecs.entity()
        .set<Position, Tgt>({10, 20})
        .set<Velocity, Tgt>({1, 2});
    ecs.entity()
        .set<Position, Tgt>({20, 30})
        .set<Velocity, Tgt>({2, 3});
    ecs.entity()
        .set<Position, Tgt>({30, 40})
        .set<Velocity, Tgt>({3, 4});

    flecs::table table = e.table();
    void *ptr = table.get<Position>(ecs.id<Tgt>());
    Position *p = static_cast<Position*>(ptr);
    test_assert(p != NULL);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_int(p[2].x, 30);
    test_int(p[2].y, 40);

    ptr = table.get<Velocity>(ecs.id<Tgt>());
    Velocity *v = static_cast<Velocity*>(ptr);
    test_assert(v != NULL);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);
    test_int(v[2].x, 3);
    test_int(v[2].y, 4);
}

void Table_get_pair_R_T() {
    flecs::world ecs;

    struct Tgt { };

    flecs::entity e = ecs.entity()
        .set<Position, Tgt>({10, 20})
        .set<Velocity, Tgt>({1, 2});
    ecs.entity()
        .set<Position, Tgt>({20, 30})
        .set<Velocity, Tgt>({2, 3});
    ecs.entity()
        .set<Position, Tgt>({30, 40})
        .set<Velocity, Tgt>({3, 4});

    flecs::table table = e.table();
    void *ptr = table.get<Position, Tgt>();
    Position *p = static_cast<Position*>(ptr);
    test_assert(p != NULL);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_int(p[2].x, 30);
    test_int(p[2].y, 40);

    ptr = table.get<Velocity, Tgt>();
    Velocity *v = static_cast<Velocity*>(ptr);
    test_assert(v != NULL);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);
    test_int(v[2].x, 3);
    test_int(v[2].y, 4);
}

void Table_range_get_id() {
    flecs::world ecs;

    ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});
    flecs::entity e = ecs.entity()
        .set<Position>({20, 30})
        .set<Velocity>({2, 3});
    ecs.entity()
        .set<Position>({30, 40})
        .set<Velocity>({3, 4});

    flecs::table_range table = e.range();
    void *ptr = table.get(ecs.id<Position>());
    Position *p = static_cast<Position*>(ptr);
    test_assert(p != NULL);
    test_int(p[0].x, 20);
    test_int(p[0].y, 30);

    ptr = table.get(ecs.id<Velocity>());
    Velocity *v = static_cast<Velocity*>(ptr);
    test_assert(v != NULL);
    test_int(v[0].x, 2);
    test_int(v[0].y, 3);
}

void Table_range_get_T() {
    flecs::world ecs;

    ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});
    flecs::entity e = ecs.entity()
        .set<Position>({20, 30})
        .set<Velocity>({2, 3});
    ecs.entity()
        .set<Position>({30, 40})
        .set<Velocity>({3, 4});

    flecs::table_range table = e.range();
    Position *p = table.get<Position>();
    test_assert(p != NULL);
    test_int(p[0].x, 20);
    test_int(p[0].y, 30);

    Velocity *v = table.get<Velocity>();
    test_assert(v != NULL);
    test_int(v[0].x, 2);
    test_int(v[0].y, 3);
}

void Table_range_get_pair_r_t() {
    flecs::world ecs;

    struct Tgt { };

    ecs.entity()
        .set<Position, Tgt>({10, 20})
        .set<Velocity, Tgt>({1, 2});
    flecs::entity e = ecs.entity()
        .set<Position, Tgt>({20, 30})
        .set<Velocity, Tgt>({2, 3});
    ecs.entity()
        .set<Position, Tgt>({30, 40})
        .set<Velocity, Tgt>({3, 4});

    flecs::table_range table = e.range();
    void *ptr = table.get(ecs.id<Position>(), ecs.id<Tgt>());
    Position *p = static_cast<Position*>(ptr);
    test_assert(p != NULL);
    test_int(p[0].x, 20);
    test_int(p[0].y, 30);

    ptr = table.get(ecs.id<Velocity>(), ecs.id<Tgt>());
    Velocity *v = static_cast<Velocity*>(ptr);
    test_assert(v != NULL);
    test_int(v[0].x, 2);
    test_int(v[0].y, 3);
}

void Table_range_get_pair_R_t() {
    flecs::world ecs;

    struct Tgt { };

    ecs.entity()
        .set<Position, Tgt>({10, 20})
        .set<Velocity, Tgt>({1, 2});
    flecs::entity e = ecs.entity()
        .set<Position, Tgt>({20, 30})
        .set<Velocity, Tgt>({2, 3});
    ecs.entity()
        .set<Position, Tgt>({30, 40})
        .set<Velocity, Tgt>({3, 4});

    flecs::table_range table = e.range();
    Position *p = table.get<Position>(ecs.id<Tgt>());
    test_assert(p != NULL);
    test_int(p[0].x, 20);
    test_int(p[0].y, 30);

    Velocity *v = table.get<Velocity>(ecs.id<Tgt>());
    test_assert(v != NULL);
    test_int(v[0].x, 2);
    test_int(v[0].y, 3);
}

void Table_range_get_pair_R_T() {
    flecs::world ecs;

    struct Tgt { };

    ecs.entity()
        .set<Position, Tgt>({10, 20})
        .set<Velocity, Tgt>({1, 2});
    flecs::entity e = ecs.entity()
        .set<Position, Tgt>({20, 30})
        .set<Velocity, Tgt>({2, 3});
    ecs.entity()
        .set<Position, Tgt>({30, 40})
        .set<Velocity, Tgt>({3, 4});

    flecs::table_range table = e.range();
    Position *p = table.get<Position, Tgt>();
    test_assert(p != NULL);
    test_int(p[0].x, 20);
    test_int(p[0].y, 30);

    Velocity *v = table.get<Velocity, Tgt>();
    test_assert(v != NULL);
    test_int(v[0].x, 2);
    test_int(v[0].y, 3);
}

void Table_get_depth() {
    flecs::world world;

    flecs::entity e1 = world.entity();
    flecs::entity e2 = world.entity().child_of(e1);
    flecs::entity e3 = world.entity().child_of(e2);
    flecs::entity e4 = world.entity().child_of(e3);

    test_int(1, e2.table().depth(flecs::ChildOf));
    test_int(2, e3.table().depth(flecs::ChildOf));
    test_int(3, e4.table().depth(flecs::ChildOf));
}

void Table_get_depth_w_type() {
    flecs::world world;

    struct Rel { };
    world.component<Rel>().add(flecs::Acyclic);

    flecs::entity e1 = world.entity();
    flecs::entity e2 = world.entity().add<Rel>(e1);
    flecs::entity e3 = world.entity().add<Rel>(e2);
    flecs::entity e4 = world.entity().add<Rel>(e3);

    test_int(1, e2.table().depth<Rel>());
    test_int(2, e3.table().depth<Rel>());
    test_int(3, e4.table().depth<Rel>());
}
