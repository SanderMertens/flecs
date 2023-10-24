#include <cpp_api.h>

void Observer_2_terms_on_add(void) {
    flecs::world ecs;

    int32_t count = 0;

    ecs.observer<Position, Velocity>()
        .event(flecs::OnAdd)
        .each([&](Position& p, Velocity& v) {
            count ++;          
        });

    auto e = ecs.entity();
    test_int(count, 0);

    e.set<Position>({10, 20});
    test_int(count, 0);

    e.set<Velocity>({1, 2});
    test_int(count, 1);
}

void Observer_2_terms_on_remove(void) {
    flecs::world ecs;

    int32_t count = 0;

    ecs.observer<Position, Velocity>()
        .event(flecs::OnRemove)
        .each([&](Position& p, Velocity& v) {
            count ++;
            test_int(p.x, 10);
            test_int(p.y, 20);
            test_int(v.x, 1);
            test_int(v.y, 2);
        });

    auto e = ecs.entity();
    test_int(count, 0);

    e.set<Position>({10, 20});
    test_int(count, 0);

    e.set<Velocity>({1, 2});
    test_int(count, 0);

    e.remove<Velocity>();
    test_int(count, 1);

    e.remove<Position>();
    test_int(count, 1);    
}

void Observer_2_terms_on_set(void) {
    flecs::world ecs;

    int32_t count = 0;

    ecs.observer<Position, Velocity>()
        .event(flecs::OnSet)
        .each([&](Position& p, Velocity& v) {
            count ++;
            test_int(p.x, 10);
            test_int(p.y, 20);
            test_int(v.x, 1);
            test_int(v.y, 2);            
        });

    auto e = ecs.entity();
    test_int(count, 0);

    e.set<Position>({10, 20});
    test_int(count, 0);

    e.set<Velocity>({1, 2});
    test_int(count, 1);
}

void Observer_2_terms_un_set(void) {
    flecs::world ecs;

    int32_t count = 0;

    ecs.observer<Position, Velocity>()
        .event(flecs::UnSet)
        .each([&](Position& p, Velocity& v) {
            count ++;
            test_int(p.x, 10);
            test_int(p.y, 20);
            test_int(v.x, 1);
            test_int(v.y, 2);
        });

    auto e = ecs.entity();
    test_int(count, 0);

    e.set<Position>({10, 20});
    test_int(count, 0);

    e.set<Velocity>({1, 2});
    test_int(count, 0);

    e.remove<Velocity>();
    test_int(count, 1);

    e.remove<Position>();
    test_int(count, 1); 
}

void Observer_10_terms(void) {
    flecs::world ecs;

    int count = 0;

    auto e = ecs.entity();

    ecs.observer<>()
        .event(flecs::OnAdd)
        .term<TagA>()
        .term<TagB>()
        .term<TagC>()
        .term<TagD>()
        .term<TagE>()
        .term<TagF>()
        .term<TagG>()
        .term<TagH>()
        .term<TagI>()
        .term<TagJ>()
        .iter([&](flecs::iter& it) {
            test_int(it.count(), 1);
            test_assert(it.entity(0) == e);
            test_int(it.field_count(), 10);
            count ++;
        });

    e.add<TagA>()
        .add<TagB>()
        .add<TagC>()
        .add<TagD>()
        .add<TagE>()
        .add<TagF>()
        .add<TagG>()
        .add<TagH>()
        .add<TagI>()
        .add<TagJ>();

    test_int(count, 1);
}

void Observer_20_terms(void) {
    flecs::world ecs;

    int count = 0;

    auto e = ecs.entity();

    ecs.observer<>()
        .event(flecs::OnAdd)
        .term<TagA>()
        .term<TagB>()
        .term<TagC>()
        .term<TagD>()
        .term<TagE>()
        .term<TagF>()
        .term<TagG>()
        .term<TagH>()
        .term<TagI>()
        .term<TagJ>()
        .term<TagK>()
        .term<TagL>()
        .term<TagM>()
        .term<TagN>()
        .term<TagO>()
        .term<TagP>()
        .term<TagQ>()
        .term<TagR>()
        .term<TagS>()
        .term<TagT>()
        .iter([&](flecs::iter& it) {
            test_int(it.count(), 1);
            test_assert(it.entity(0) == e);
            test_int(it.field_count(), 20);
            count ++;
        });

    e.add<TagA>()
        .add<TagB>()
        .add<TagC>()
        .add<TagD>()
        .add<TagE>()
        .add<TagF>()
        .add<TagG>()
        .add<TagH>()
        .add<TagI>()
        .add<TagJ>()
        .add<TagK>()
        .add<TagL>()
        .add<TagM>()
        .add<TagN>()
        .add<TagO>()
        .add<TagP>()
        .add<TagQ>()
        .add<TagR>()
        .add<TagS>()
        .add<TagT>();

    test_int(count, 1);
}

void Observer_2_entities_iter(void) {
    flecs::world ecs;

    auto e1 = ecs.entity();
    auto e2 = ecs.entity();

    int32_t count = 0;
    flecs::entity last;

    ecs.observer<const Position>()
        .event(flecs::OnSet)
        .iter([&](flecs::iter& it, const Position *p) {
            for (auto i : it) {
                count ++;
                if (it.entity(i) == e1) {
                    test_int(p[i].x, 10);
                    test_int(p[i].y, 20);
                } else if (it.entity(i) == e2) {
                    test_int(p[i].x, 30);
                    test_int(p[i].y, 40);
                } else {
                    test_assert(false);
                }

                last = it.entity(i);
            }
        });

    e1.set<Position>({ 10, 20 });
    test_int(count, 1);
    test_assert(last == e1);

    e2.set<Position>({ 30, 40 }); 
    test_int(count, 2);
    test_assert(last == e2);
}

void Observer_2_entities_table_column(void) {
    flecs::world ecs;

    auto e1 = ecs.entity();
    auto e2 = ecs.entity();

    int32_t count = 0;
    flecs::entity last;

    ecs.observer<const Position>()
        .event(flecs::OnSet)
        .iter([&](flecs::iter& it) {
            auto p = it.range().get<Position>();

            for (auto i : it) {
                count ++;
                if (it.entity(i) == e1) {
                    test_int(p[i].x, 10);
                    test_int(p[i].y, 20);
                } else if (it.entity(i) == e2) {
                    test_int(p[i].x, 30);
                    test_int(p[i].y, 40);
                } else {
                    test_assert(false);
                }

                last = it.entity(i);
            }
        });

    e1.set<Position>({ 10, 20 });
    test_int(count, 1);
    test_assert(last == e1);

    e2.set<Position>({ 30, 40 }); 
    test_int(count, 2);
    test_assert(last == e2);
}

void Observer_2_entities_each(void) {
    flecs::world ecs;

    auto e1 = ecs.entity();
    auto e2 = ecs.entity();

    int32_t count = 0;
    flecs::entity last;

    ecs.observer<const Position>()
        .event(flecs::OnSet)
        .each([&](flecs::entity e, const Position& p) {
            count ++;
            if (e == e1) {
                test_int(p.x, 10);
                test_int(p.y, 20);
            } else if (e == e2) {
                test_int(p.x, 30);
                test_int(p.y, 40);
            } else {
                test_assert(false);
            }

            last = e;
        });

    e1.set<Position>({ 10, 20 });
    test_int(count, 1);
    test_assert(last == e1);

    e2.set<Position>({ 30, 40 }); 
    test_int(count, 2);
    test_assert(last == e2);
}

void Observer_create_w_no_template_args(void) {
    flecs::world ecs;

    auto e1 = ecs.entity();

    int32_t count = 0;

    ecs.observer()
        .term<Position>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;          
        });

    e1.set<Position>({10, 20});
    test_int(count, 1);
}

void Observer_yield_existing(void) {
    flecs::world world;

    struct TagA { };
    struct TagB { };

    auto e1 = world.entity().add<TagA>();
    auto e2 = world.entity().add<TagA>();
    auto e3 = world.entity().add<TagA>().add<TagB>();

    int32_t count = 0;

    world.observer<TagA>()
        .event(flecs::OnAdd)
        .yield_existing()
        .each([&](flecs::entity e, TagA) {
            if (e == e1) count ++;
            if (e == e2) count += 2;
            if (e == e3) count += 3;
        });

    test_int(count, 6);
}

void Observer_yield_existing_2_terms(void) {
    flecs::world world;

    struct TagA { };
    struct TagB { };

    auto e1 = world.entity().add<TagA>().add<TagB>();
    auto e2 = world.entity().add<TagA>().add<TagB>();
    auto e3 = world.entity().add<TagA>().add<TagB>().add<TagC>();
    world.entity().add<TagA>();
    world.entity().add<TagB>();

    int32_t count = 0;

    world.observer<TagA, TagB>()
        .event(flecs::OnAdd)
        .yield_existing()
        .each([&](flecs::entity e, TagA, TagB) {
            if (e == e1) count ++;
            if (e == e2) count += 2;
            if (e == e3) count += 3;
        });

    test_int(count, 6);
}

void Observer_default_ctor(void) {
    flecs::world world;

    struct TagA { };

    flecs::observer o;
    test_assert(o == 0);

    int32_t count = 0;
    o = world.observer<TagA>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e, TagA) {
            count ++;
        });
    
    world.entity().add<TagA>();
    
    test_int(count, 1);
}

void Observer_entity_ctor(void) {
    flecs::world world;

    struct TagA { };

    flecs::observer o = world.observer<TagA>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e, TagA) { });
    
    flecs::entity oe = o;

    flecs::observer eo = world.observer(oe);
    test_assert(eo == o);
}

void Observer_on_add(void) {
    flecs::world world;

    int invoked = 0;

    world.observer<Position>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e, Position& p) {
            invoked ++;
        });

    world.entity()
        .add<Position>();

    test_int(invoked, 1);
}

void Observer_on_remove(void) {
    flecs::world world;

    int invoked = 0;

    world.observer<Position>()
        .event(flecs::OnRemove)
        .each([&](flecs::entity e, Position& p) {
            invoked ++;
        });

    auto e = world.entity()
        .add<Position>();

    test_int(invoked, 0);
    
    e.remove<Position>();

    test_int(invoked, 1);
}

struct MyTag { };

void Observer_on_add_tag_action(void) {
    flecs::world world;

    int invoked = 0;

    world.observer<MyTag>()
        .event(flecs::OnAdd)
        .iter([&](flecs::iter it, MyTag*) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    test_int(invoked, 1);
}

void Observer_on_add_tag_iter(void) {
    flecs::world world;

    int invoked = 0;

    world.observer<MyTag>()
        .event(flecs::OnAdd)
        .iter([&](flecs::iter it, MyTag*) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    test_int(invoked, 1);
}

void Observer_on_add_tag_each(void) {
    flecs::world world;

    int invoked = 0;

    world.observer<MyTag>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e, MyTag) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    test_int(invoked, 1);
}

void Observer_on_add_expr(void) {
    flecs::world world;

    int invoked = 0;

    world.component<Tag>();

    world.observer<>().expr("Tag")
        .event(flecs::OnAdd)
        .each([&](flecs::entity e) {
            invoked ++;
        });

    auto e = world.entity().add<Tag>();

    test_int(invoked, 1);
    
    e.remove<Tag>();

    test_int(invoked, 1);
}

void Observer_observer_w_filter_term(void) {
    flecs::world world;

    flecs::entity TagA = world.entity();
    flecs::entity TagB = world.entity();

    int invoked = 0;

    world.observer()
        .term(TagA)
        .term(TagB).filter()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e) {
            invoked ++;
        });

    flecs::entity e = world.entity();
    test_int(invoked, 0);

    e.add(TagB);
    test_int(invoked, 0);

    e.add(TagA);
    test_int(invoked, 1);

    e.remove(TagB);
    test_int(invoked, 1);

    e.add(TagB);
    test_int(invoked, 1);

    e.clear();
    test_int(invoked, 1);

    e.add(TagA);
    test_int(invoked, 1);
}

void Observer_run_callback(void) {
    flecs::world ecs;

    int32_t count = 0;

    ecs.observer<Position>()
        .event(flecs::OnAdd)
        .run([](flecs::iter_t *it) {
            while (ecs_iter_next(it)) {
                it->callback(it);
            }
        })
        .each([&](Position& p) {
            count ++;          
        });

    auto e = ecs.entity();
    test_int(count, 0);

    e.set<Position>({10, 20});
    test_int(count, 1);
}

void Observer_get_query(void) {
    flecs::world world;

    world.entity().set<Position>({0, 0});
    world.entity().set<Position>({1, 0});
    world.entity().set<Position>({2, 0});

    int32_t count = 0;

    auto sys = world.observer<const Position>()
        .event(flecs::OnSet)
        .each([&](flecs::entity e, const Position& p) {
            // Not used
        });

    auto q = sys.query();

    q.iter([&](flecs::iter &it) {
        auto pos = it.field<const Position>(1);
        for (auto i : it) {
            test_int(i, pos[i].x);
            count ++;
        }
    });

    test_int(count, 3);
}

void Observer_on_set_w_set(void) {
    flecs::world world;

    int32_t count = 0;

    world.observer<Position>()
        .event(flecs::OnSet)
        .each([&](flecs::entity e, Position& p) {
            count ++;          
        });

    flecs::entity e = world.entity();
    test_int(count, 0);

    e.set<Position>({10, 20});
    test_int(count, 1);
}

void Observer_on_set_w_defer_set(void) {
    flecs::world world;

    int32_t count = 0;

    world.observer<Position>()
        .event(flecs::OnSet)
        .each([&](flecs::entity e, Position& p) {
            count ++;          
        });

    flecs::entity e = world.entity();
    test_int(count, 0);

    world.defer_begin();
    e.set<Position>({10, 20});

    test_int(count, 0);
    world.defer_end();

    test_int(count, 1);
}

#include <iostream>

void Observer_on_add_singleton(void) {
    flecs::world world;

    int32_t count = 0;

    world.observer<Position>()
        .term_at(1).singleton()
        .event(flecs::OnSet)
        .each([&](Position& p) {
            test_int(p.x, 10);
            test_int(p.y, 20);
            count ++;
        });

    world.set<Position>({10, 20});

    test_int(count, 1);
}

void Observer_on_add_pair_singleton(void) {
    flecs::world world;

    int32_t count = 0;

    flecs::entity tgt = world.entity();

    world.observer<Position>()
        .term_at(1).second(tgt).singleton()
        .event(flecs::OnSet)
        .each([&](Position& p) {
            test_int(p.x, 10);
            test_int(p.y, 20);
            count ++;
        });

    world.set<Position>(tgt, {10, 20});

    test_int(count, 1);
}

void Observer_on_add_pair_wildcard_singleton(void) {
    flecs::world world;

    int32_t count = 0;

    flecs::entity tgt_1 = world.entity();
    flecs::entity tgt_2 = world.entity();

    world.observer<Position>()
        .term_at(1).second(flecs::Wildcard).singleton()
        .event(flecs::OnSet)
        .each([&](Position& p) {
            test_int(p.x, 10);
            test_int(p.y, 20);
            count ++;
        });

    world.set<Position>(tgt_1, {10, 20});
    test_int(count, 1);

    world.set<Position>(tgt_2, {10, 20});
    test_int(count, 2);
}

void Observer_on_add_with_pair_singleton(void) {
    flecs::world world;

    int32_t count = 0;

    flecs::entity tgt = world.entity();

    world.observer()
        .with<Position>(tgt).singleton()
        .event(flecs::OnSet)
        .each([&](flecs::entity) {
            count ++;
        });

    world.set<Position>(tgt, {10, 20});
    test_int(count, 1);
}

void Observer_add_in_yield_existing(void) {
    flecs::world world;

    flecs::entity e1 = world.entity().set<Position>({});
    flecs::entity e2 = world.entity().set<Position>({});
    flecs::entity e3 = world.entity().set<Position>({});

    world.observer()
        .with<Position>()
        .event(flecs::OnAdd)
        .yield_existing()
        .each([](flecs::entity e) {
            e.add<Velocity>();
        });

    test_assert(e1.has<Position>());
    test_assert(e1.has<Velocity>());

    test_assert(e2.has<Position>());
    test_assert(e2.has<Velocity>());

    test_assert(e3.has<Position>());
    test_assert(e3.has<Velocity>());
}

void Observer_add_in_yield_existing_multi(void) {
    flecs::world world;

    flecs::entity e1 = world.entity().set<Position>({}).set<Mass>({});
    flecs::entity e2 = world.entity().set<Position>({}).set<Mass>({});
    flecs::entity e3 = world.entity().set<Position>({}).set<Mass>({});

    world.observer()
        .with<Position>()
        .with<Mass>()
        .event(flecs::OnAdd)
        .yield_existing()
        .each([](flecs::entity e) {
            e.add<Velocity>();
        });

    test_assert(e1.has<Position>());
    test_assert(e1.has<Mass>());
    test_assert(e1.has<Velocity>());

    test_assert(e2.has<Position>());
    test_assert(e2.has<Mass>());
    test_assert(e2.has<Velocity>());

    test_assert(e3.has<Position>());
    test_assert(e3.has<Mass>());
    test_assert(e3.has<Velocity>());
}

void Observer_name_from_root(void) {
    flecs::world ecs;

    flecs::entity sys = ecs.observer<Position>("::ns::MySystem")
        .event(flecs::OnSet)
        .each([](Position& p) { });

    test_str(sys.name(), "MySystem");

    flecs::entity ns = ecs.entity("::ns");
    test_assert(ns == sys.parent());
}
