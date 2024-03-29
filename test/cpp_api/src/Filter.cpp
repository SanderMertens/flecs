#include <cpp_api.h>

void Filter_term_each_component(void) {
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

void Filter_term_each_tag(void) {
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

void Filter_term_each_id(void) {
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

void Filter_term_each_pair_type(void) {
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

void Filter_term_each_pair_id(void) {
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

void Filter_term_each_pair_relation_wildcard(void) {
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

void Filter_term_each_pair_object_wildcard(void) {
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

void Filter_default_ctor_no_assign(void) {
    flecs::query<> f;
    
    // Make sure code compiles & works
    test_assert(true);
}

void Filter_term_get_id(void) {
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

void Filter_term_get_subj(void) {
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

void Filter_term_get_pred(void) {
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

void Filter_term_get_obj(void) {
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

void Filter_get_first(void) {
    flecs::world ecs;

    struct A {};

    auto e1 = ecs.entity().add<A>();
    ecs.entity().add<A>();
    ecs.entity().add<A>();

    auto q = ecs.query<A>();

    auto first = q.iter().first();
    test_assert(first != 0);
    test_assert(first == e1);
}

void Filter_get_count_direct(void) {
    flecs::world ecs;

    struct A {};

    ecs.entity().add<A>();
    ecs.entity().add<A>();
    ecs.entity().add<A>();

    auto q = ecs.query<A>();

    test_int(3, q.count());
}

void Filter_get_is_true_direct(void) {
    flecs::world ecs;

    struct A {};
    struct B {};

    ecs.entity().add<A>();
    ecs.entity().add<A>();
    ecs.entity().add<A>();

    auto q_1 = ecs.query<A>();
    auto q_2 = ecs.query<B>();

    test_bool(true, q_1.is_true());
    test_bool(false, q_2.is_true());
}

void Filter_get_first_direct(void) {
    flecs::world ecs;

    struct A {};

    auto e1 = ecs.entity().add<A>();
    ecs.entity().add<A>();
    ecs.entity().add<A>();

    auto q = ecs.query<A>();

    auto first = q.first();
    test_assert(first != 0);
    test_assert(first == e1);
}

void Filter_each_w_no_this(void) {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto f = ecs.query_builder<Position, Velocity>()
        .term_at(1).src(e)
        .term_at(2).src(e)
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

void Filter_each_w_iter_no_this(void) {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto f = ecs.query_builder<Position, Velocity>()
        .term_at(1).src(e)
        .term_at(2).src(e)
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

void Filter_invalid_each_w_no_this(void) {
    install_test_abort();

    flecs::world ecs;

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto f = ecs.query_builder<Position, Velocity>()
        .term_at(1).src(e)
        .term_at(2).src(e)
        .build();

    test_expect_abort();

    f.each([&](flecs::entity e, Position& p, Velocity& v) { });
}

void Filter_named_query(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();

    auto q = ecs.query<Position>("my_query");

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

void Filter_named_scoped_query(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();

    auto q = ecs.query<Position>("my::query");

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

void Filter_set_this_var(void) {
    flecs::world ecs;

    /* auto e_1 = */ ecs.entity().set<Position>({1, 2});
    auto e_2 = ecs.entity().set<Position>({3, 4});
    /* auto e_3 = */ ecs.entity().set<Position>({5, 6});

    auto q = ecs.query<Position>("my::query");

    int32_t count = 0;
    q.iter().set_var(0, e_2).each([&](flecs::entity e, Position&) {
        test_assert(e == e_2);
        count ++;
    });
    test_int(count, 1);
}

void Filter_inspect_terms_w_expr(void) {
    flecs::world ecs;

    flecs::query<> f = ecs.query_builder()
        .expr("(ChildOf,0)")
        .build();

    int32_t count = 0;
    f.each_term([&](flecs::term &term) {
        test_assert(term.id().is_pair());
        count ++;
    });

    test_int(count, 1);
}

void Filter_find(void) {
    flecs::world ecs;

    /* auto e1 = */ ecs.entity().set<Position>({10, 20});
    auto e2 = ecs.entity().set<Position>({20, 30});

    auto q = ecs.query<Position>();

    auto r = q.find([](Position& p) {
        return p.x == 20;
    });

    test_assert(r == e2);
}

void Filter_find_not_found(void) {
    flecs::world ecs;

    /* auto e1 = */ ecs.entity().set<Position>({10, 20});
    /* auto e2 = */ ecs.entity().set<Position>({20, 30});

    auto q = ecs.query<Position>();

    auto r = q.find([](Position& p) {
        return p.x == 30;
    });

    test_assert(!r);
}

void Filter_find_w_entity(void) {
    flecs::world ecs;

    /* auto e1 = */ ecs.entity().set<Position>({10, 20}).set<Velocity>({20, 30});
    auto e2 = ecs.entity().set<Position>({20, 30}).set<Velocity>({20, 30});

    auto q = ecs.query<Position>();

    auto r = q.find([](flecs::entity e, Position& p) {
        return p.x == e.get<Velocity>()->x &&
               p.y == e.get<Velocity>()->y;
    });

    test_assert(r == e2);
}

void Filter_optional_pair_term(void) {
    flecs::world ecs;

    ecs.entity()
        .add<TagA>()
        .emplace<Position, Tag>(1.0f, 2.0f);
    ecs.entity()
        .add<TagA>();

    int32_t with_pair = 0, without_pair = 0;

    auto f = ecs.query_builder<flecs::pair<Position, Tag>*>()
        .with<TagA>()
        .build();
        
    f.each([&](flecs::entity e, Position* p) {
        if (p) {
            with_pair++;
            test_flt(1.0f, p->x);
            test_flt(2.0f, p->y);
        } else {
            without_pair++;
        }
    });

    ecs.progress(1.0);

    test_int(1, with_pair);
    test_int(1, without_pair);
}

void Filter_action(void) {
    // Implement testcase
}

void Filter_action_const(void) {
    // Implement testcase
}

void Filter_action_shared(void) {
    // Implement testcase
}

void Filter_action_optional(void) {
    // Implement testcase
}

void Filter_each(void) {
    // Implement testcase
}

void Filter_each_const(void) {
    // Implement testcase
}

void Filter_each_shared(void) {
    // Implement testcase
}

void Filter_each_optional(void) {
    // Implement testcase
}

void Filter_signature(void) {
    // Implement testcase
}

void Filter_signature_const(void) {
    // Implement testcase
}

void Filter_signature_shared(void) {
    // Implement testcase
}

void Filter_signature_optional(void) {
    // Implement testcase
}

void Filter_query_single_pair(void) {
    // Implement testcase
}

void Filter_tag_w_each(void) {
    // Implement testcase
}

void Filter_shared_tag_w_each(void) {
    // Implement testcase
}

void Filter_sort_by(void) {
    // Implement testcase
}

void Filter_changed(void) {
    // Implement testcase
}

void Filter_default_ctor(void) {
    // Implement testcase
}

void Filter_expr_w_template(void) {
    // Implement testcase
}

void Filter_query_type_w_template(void) {
    // Implement testcase
}

void Filter_compare_term_id(void) {
    // Implement testcase
}

void Filter_test_no_defer_each(void) {
    // Implement testcase
}

void Filter_test_no_defer_iter(void) {
    // Implement testcase
}

void Filter_inspect_terms(void) {
    // Implement testcase
}

void Filter_inspect_terms_w_each(void) {
    // Implement testcase
}

void Filter_comp_to_str(void) {
    // Implement testcase
}

void Filter_pair_to_str(void) {
    // Implement testcase
}

void Filter_oper_not_to_str(void) {
    // Implement testcase
}

void Filter_oper_optional_to_str(void) {
    // Implement testcase
}

void Filter_oper_or_to_str(void) {
    // Implement testcase
}

void Filter_each_pair_type(void) {
    // Implement testcase
}

void Filter_iter_pair_type(void) {
    // Implement testcase
}

void Filter_term_pair_type(void) {
    // Implement testcase
}

void Filter_each_no_entity_1_comp(void) {
    // Implement testcase
}

void Filter_each_no_entity_2_comps(void) {
    // Implement testcase
}

void Filter_iter_no_comps_1_comp(void) {
    // Implement testcase
}

void Filter_iter_no_comps_2_comps(void) {
    // Implement testcase
}

void Filter_iter_no_comps_no_comps(void) {
    // Implement testcase
}

void Filter_each_pair_object(void) {
    // Implement testcase
}

void Filter_iter_pair_object(void) {
    // Implement testcase
}

void Filter_iter_query_in_system(void) {
    // Implement testcase
}

void Filter_iter_type(void) {
    // Implement testcase
}

void Filter_instanced_query_w_singleton_each(void) {
    // Implement testcase
}

void Filter_instanced_query_w_base_each(void) {
    // Implement testcase
}

void Filter_un_instanced_query_w_singleton_each(void) {
    // Implement testcase
}

void Filter_un_instanced_query_w_base_each(void) {
    // Implement testcase
}

void Filter_instanced_query_w_singleton_iter(void) {
    // Implement testcase
}

void Filter_instanced_query_w_base_iter(void) {
    // Implement testcase
}

void Filter_un_instanced_query_w_singleton_iter(void) {
    // Implement testcase
}

void Filter_un_instanced_query_w_base_iter(void) {
    // Implement testcase
}

void Filter_query_each_from_component(void) {
    // Implement testcase
}

void Filter_query_iter_from_component(void) {
    // Implement testcase
}

void Filter_query_each_w_func_ptr(void) {
    // Implement testcase
}

void Filter_query_iter_w_func_ptr(void) {
    // Implement testcase
}

void Filter_query_each_w_func_no_ptr(void) {
    // Implement testcase
}

void Filter_query_iter_w_func_no_ptr(void) {
    // Implement testcase
}

void Filter_query_each_w_iter(void) {
    // Implement testcase
}

void Filter_invalid_field_from_each_w_iter(void) {
    // Implement testcase
}

void Filter_invalid_field_T_from_each_w_iter(void) {
    // Implement testcase
}

void Filter_invalid_field_const_T_from_each_w_iter(void) {
    // Implement testcase
}

void Filter_field_at_from_each_w_iter(void) {
    // Implement testcase
}

void Filter_field_at_T_from_each_w_iter(void) {
    // Implement testcase
}

void Filter_field_at_const_T_from_each_w_iter(void) {
    // Implement testcase
}

void Filter_field_at_from_each_w_iter_w_base_type(void) {
    // Implement testcase
}

void Filter_change_tracking(void) {
    // Implement testcase
}

void Filter_not_w_write(void) {
    // Implement testcase
}

void Filter_instanced_nested_query_w_iter(void) {
    // Implement testcase
}

void Filter_instanced_nested_query_w_entity(void) {
    // Implement testcase
}

void Filter_instanced_nested_query_w_world(void) {
    // Implement testcase
}

void Filter_captured_query(void) {
    // Implement testcase
}

void Filter_page_iter_captured_query(void) {
    // Implement testcase
}

void Filter_worker_iter_captured_query(void) {
    // Implement testcase
}

void Filter_iter_entities(void) {
    // Implement testcase
}

void Filter_iter_get_pair_w_id(void) {
    // Implement testcase
}

void Filter_named_filter(void) {
    // Implement testcase
}

void Filter_named_scoped_filter(void) {
    // Implement testcase
}
