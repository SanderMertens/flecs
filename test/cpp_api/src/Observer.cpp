#include <cpp_api.h>

void Observer_2_terms_on_add() {
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

void Observer_2_terms_on_remove() {
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

void Observer_2_terms_on_set() {
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

void Observer_2_terms_un_set() {
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

void Observer_observer_w_self() {
    flecs::world world;

    auto self = world.entity();

    bool invoked = false;
    world.observer<Position>()
        .event(flecs::OnAdd)
        .self(self)
        .iter([&](flecs::iter& it) {
            test_assert(it.self() == self);
            invoked = true;
        });

    world.entity().set<Position>({10, 20});

    test_bool(invoked, true);
}

void Observer_10_terms() {
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
            test_int(it.term_count(), 10);
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

void Observer_20_terms() {
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
            test_int(it.term_count(), 20);
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

void Observer_2_entities_iter() {
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

void Observer_2_entities_table_column() {
    flecs::world ecs;

    auto e1 = ecs.entity();
    auto e2 = ecs.entity();

    int32_t count = 0;
    flecs::entity last;

    ecs.observer<const Position>()
        .event(flecs::OnSet)
        .iter([&](flecs::iter& it) {
            auto p = it.table_column<Position>();

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

void Observer_2_entities_each() {
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

void Observer_create_w_no_template_args() {
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
