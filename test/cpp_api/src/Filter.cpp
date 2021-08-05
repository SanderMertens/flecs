#include <cpp_api.h>

void Filter_term_each_component() {
    flecs::world ecs;

    auto e_1 = ecs.entity().set<Position>({1, 2});
    auto e_2 = ecs.entity().set<Position>({3, 4});
    auto e_3 = ecs.entity().set<Position>({5, 6});

    e_3.add<Tag>();

    int32_t count = 0;
    ecs.each<Position>([&](flecs::entity e, Position& p) {
        if (e == e_1) {
            test_int(p.x, 1);
            test_int(p.y, 2);
            count ++;
        }
        if (e == e_2) {
            test_int(p.x, 3);
            test_int(p.y, 4);
            count ++;
        }
        if (e == e_3) {
            test_int(p.x, 5);
            test_int(p.y, 6);
            count ++;
        }                
    });

    test_int(count, 3);
}

void Filter_term_each_tag() {
    flecs::world ecs;

    struct Foo { };

    auto e_1 = ecs.entity().add<Foo>();
    auto e_2 = ecs.entity().add<Foo>();
    auto e_3 = ecs.entity().add<Foo>();

    e_3.add<Tag>();

    int32_t count = 0;
    ecs.each<Foo>([&](flecs::entity e, Foo&) {
        if (e == e_1 || e == e_2 || e == e_3) {
            count ++;
        }            
    });

    test_int(count, 3);
}

void Filter_term_each_id() {
    flecs::world ecs;

    auto foo = ecs.entity();

    auto e_1 = ecs.entity().add(foo);
    auto e_2 = ecs.entity().add(foo);
    auto e_3 = ecs.entity().add(foo);

    e_3.add<Tag>();

    int32_t count = 0;
    ecs.each(foo, [&](flecs::entity e) {
        if (e == e_1 || e == e_2 || e == e_3) {
            count ++;
        }            
    });

    test_int(count, 3);
}

void Filter_term_each_pair_type() {
    flecs::world ecs;

    struct Rel { };
    struct Obj { };

    auto e_1 = ecs.entity().add<Rel, Obj>();
    auto e_2 = ecs.entity().add<Rel, Obj>();
    auto e_3 = ecs.entity().add<Rel, Obj>();

    e_3.add<Tag>();

    int32_t count = 0;
    ecs.each<flecs::pair<Rel, Obj>>([&](flecs::entity e, flecs::pair<Rel,Obj>) {
        if (e == e_1 || e == e_2 || e == e_3) {
            count ++;
        }
    });

    test_int(count, 3);
}

void Filter_term_each_pair_id() {
    flecs::world ecs;

    auto rel = ecs.entity();
    auto obj = ecs.entity();

    auto e_1 = ecs.entity().add(rel, obj);
    auto e_2 = ecs.entity().add(rel, obj);
    auto e_3 = ecs.entity().add(rel, obj);

    e_3.add<Tag>();

    int32_t count = 0;
    ecs.each(ecs.pair(rel, obj), [&](flecs::entity e) {
        if (e == e_1 || e == e_2 || e == e_3) {
            count ++;
        }
    });

    test_int(count, 3);
}

void Filter_term_each_pair_relation_wildcard() {
    flecs::world ecs;

    auto rel_1 = ecs.entity();
    auto rel_2 = ecs.entity();
    auto obj = ecs.entity();

    auto e_1 = ecs.entity().add(rel_1, obj);
    auto e_2 = ecs.entity().add(rel_1, obj);
    auto e_3 = ecs.entity().add(rel_2, obj);

    e_3.add<Tag>();

    int32_t count = 0;
    ecs.each(ecs.pair(flecs::Wildcard, obj), [&](flecs::entity e) {
        if (e == e_1 || e == e_2 || e == e_3) {
            count ++;
        }
    });

    test_int(count, 3);
}

void Filter_term_each_pair_object_wildcard() {
    flecs::world ecs;

    auto rel = ecs.entity();
    auto obj_1 = ecs.entity();
    auto obj_2 = ecs.entity();

    auto e_1 = ecs.entity().add(rel, obj_1);
    auto e_2 = ecs.entity().add(rel, obj_1);
    auto e_3 = ecs.entity().add(rel, obj_2);

    e_3.add<Tag>();

    int32_t count = 0;
    ecs.each(ecs.pair(rel, flecs::Wildcard), [&](flecs::entity e) {
        if (e == e_1 || e == e_2 || e == e_3) {
            count ++;
        }
    });

    test_int(count, 3);
}
