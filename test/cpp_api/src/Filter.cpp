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
    ecs.each<Foo>([&](flecs::entity e, Foo) {
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

void Filter_default_ctor_no_assign() {
    flecs::filter<> f;
    
    // Make sure code compiles & works
    test_assert(true);
}

void Filter_term_get_id() {
    flecs::world ecs;

    auto Foo = ecs.entity();
    auto Bar = ecs.entity();

    auto q = ecs.query_builder()
        .term<Position>()
        .term<Velocity>()
        .term(Foo, Bar)
        .build();

    test_int(q.field_count(), 3);
    
    flecs::term 
    t = q.term(0);
    test_assert(t.id() == ecs.id<Position>());
    t = q.term(1);
    test_assert(t.id() == ecs.id<Velocity>());
    t = q.term(2);
    test_assert(t.id() == ecs.pair(Foo, Bar));
}

void Filter_term_get_subj() {
    flecs::world ecs;

    auto Foo = ecs.entity();
    auto Bar = ecs.entity();
    auto Src = ecs.entity();

    auto q = ecs.query_builder()
        .term<Position>()
        .term<Velocity>().src(Src)
        .term(Foo, Bar)
        .build();

    test_int(q.field_count(), 3);
    
    flecs::term 
    t = q.term(0);
    test_assert(t.get_src() == flecs::This);
    t = q.term(1);
    test_assert(t.get_src() == Src);
    t = q.term(2);
    test_assert(t.get_src() == flecs::This);
}

void Filter_term_get_pred() {
    flecs::world ecs;

    auto Foo = ecs.entity();
    auto Bar = ecs.entity();
    auto Src = ecs.entity();

    auto q = ecs.query_builder()
        .term<Position>()
        .term<Velocity>().src(Src)
        .term(Foo, Bar)
        .build();

    test_int(q.field_count(), 3);
    
    flecs::term 
    t = q.term(0);
    test_assert(t.get_first() == ecs.id<Position>());
    t = q.term(1);
    test_assert(t.get_first() == ecs.id<Velocity>());
    t = q.term(2);
    test_assert(t.get_first() == Foo);
}

void Filter_term_get_obj() {
    flecs::world ecs;

    auto Foo = ecs.entity();
    auto Bar = ecs.entity();
    auto Src = ecs.entity();

    auto q = ecs.query_builder()
        .term<Position>()
        .term<Velocity>().src(Src)
        .term(Foo, Bar)
        .build();

    test_int(q.field_count(), 3);
    
    flecs::term 
    t = q.term(0);
    test_assert(t.get_second() == 0);
    t = q.term(1);
    test_assert(t.get_second() == 0);
    t = q.term(2);
    test_assert(t.get_second() == Bar);
}

void Filter_get_first() {
    flecs::world ecs;

    struct A {};

    auto e1 = ecs.entity().add<A>();
    ecs.entity().add<A>();
    ecs.entity().add<A>();

    auto q = ecs.filter<A>();

    auto first = q.iter().first();
    test_assert(first != 0);
    test_assert(first == e1);
}

void Filter_get_count_direct() {
    flecs::world ecs;

    struct A {};

    ecs.entity().add<A>();
    ecs.entity().add<A>();
    ecs.entity().add<A>();

    auto q = ecs.filter<A>();

    test_int(3, q.count());
}

void Filter_get_is_true_direct() {
    flecs::world ecs;

    struct A {};
    struct B {};

    ecs.entity().add<A>();
    ecs.entity().add<A>();
    ecs.entity().add<A>();

    auto q_1 = ecs.filter<A>();
    auto q_2 = ecs.filter<B>();

    test_bool(true, q_1.is_true());
    test_bool(false, q_2.is_true());
}

void Filter_get_first_direct() {
    flecs::world ecs;

    struct A {};

    auto e1 = ecs.entity().add<A>();
    ecs.entity().add<A>();
    ecs.entity().add<A>();

    auto q = ecs.filter<A>();

    auto first = q.first();
    test_assert(first != 0);
    test_assert(first == e1);
}

void Filter_each_w_no_this() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto f = ecs.filter_builder<Position, Velocity>()
        .arg(1).src(e)
        .arg(2).src(e)
        .build();

    int32_t count = 0;

    f.each([&](Position& p, Velocity& v) {
        count ++;
        test_int(p.x, 10);
        test_int(p.y, 20);
        test_int(v.x, 1);
        test_int(v.y, 2);
    });

    test_int(count, 1);
}

void Filter_each_w_iter_no_this() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto f = ecs.filter_builder<Position, Velocity>()
        .arg(1).src(e)
        .arg(2).src(e)
        .build();

    int32_t count = 0;

    f.each([&](flecs::iter& it, size_t index, Position& p, Velocity& v) {
        count ++;
        test_int(p.x, 10);
        test_int(p.y, 20);
        test_int(v.x, 1);
        test_int(v.y, 2);
        test_int(index, 0);
        test_int(it.count(), 0);
    });

    test_int(count, 1);
}

void Filter_invalid_each_w_no_this() {
    install_test_abort();

    flecs::world ecs;

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto f = ecs.filter_builder<Position, Velocity>()
        .arg(1).src(e)
        .arg(2).src(e)
        .build();

    test_expect_abort();

    f.each([&](flecs::entity e, Position& p, Velocity& v) { });
}

void Filter_named_filter() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();

    auto q = ecs.filter<Position>("my_query");

    int32_t count = 0;
    q.each([&](flecs::entity e, Position&) {
        test_assert(e == e1 || e == e2);
        count ++;
    });
    test_int(count, 2);

    flecs::entity qe = q.entity();
    test_assert(qe != 0);
    test_str(qe.name(), "my_query");
}

void Filter_named_scoped_filter() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();

    auto q = ecs.filter<Position>("my::query");

    int32_t count = 0;
    q.each([&](flecs::entity e, Position&) {
        test_assert(e == e1 || e == e2);
        count ++;
    });
    test_int(count, 2);

    flecs::entity qe = q.entity();
    test_assert(qe != 0);
    test_str(qe.name(), "query");
    test_str(qe.path(), "::my::query");
}
