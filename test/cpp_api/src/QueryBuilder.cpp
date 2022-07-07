#include <cpp_api.h>

struct Other {
    int32_t value;
};

void QueryBuilder_builder_assign_same_type() {
    flecs::world ecs;

    flecs::query<Position, Velocity> q = 
        ecs.query_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_assign_to_empty() {
    flecs::world ecs;

    flecs::query<> q = ecs.query_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_assign_from_empty() {
    flecs::world ecs;

    flecs::query<> q = ecs.query_builder<>()
        .term<Position>()
        .term<Velocity>()
        .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_build() {
    flecs::world ecs;

    flecs::query<Position, Velocity> q = 
        ecs.query_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_build_to_auto() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_build_n_statements() {
    flecs::world ecs;

    auto qb = ecs.query_builder<>();
    qb.term<Position>();
    qb.term<Velocity>();
    auto q = qb.build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_1_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>().build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_1_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term<Position>()
        .build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_2_types() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term<Position>()
        .term<Velocity>()
        .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_1_type_w_1_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>()
        .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_2_types_w_1_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>()
        .term<Mass>()
        .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>().add<Mass>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_pair() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<>()
        .term(Likes, Bob)
        .build();

    auto e1 = ecs.entity().add(Likes, Bob);
    ecs.entity().add(Likes, Alice);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_not() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Velocity>().oper(flecs::Not)
        .build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_add_or() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term<Position>().oper(flecs::Or)
        .term<Velocity>().oper(flecs::Or)
        .build();

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();
    ecs.entity().add<Mass>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1 || e == e2);
    });
    
    test_int(count, 2);
}

void QueryBuilder_add_optional() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term<Position>()
        .term<Velocity>().oper(flecs::Optional)
        .build();

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>().add<Mass>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1 || e == e2);
    });
    
    test_int(count, 2);
}

void QueryBuilder_ptr_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position, Velocity*>().build();

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>().add<Mass>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity* v) {
        count ++;
        test_assert(e == e1 || e == e2);
    });
    
    test_int(count, 2);
}

void QueryBuilder_const_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<const Position>().build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, const Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_string_term() {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = ecs.query_builder<>()
        .expr("Position")
        .build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_singleton_term() {
    flecs::world ecs;

    ecs.set<Other>({10});

    auto q = ecs.query_builder<Self>()
        .term<Other>().singleton()
        .build();

    auto 
    e = ecs.entity(); e.set<Self>({e});
    e = ecs.entity(); e.set<Self>({e});
    e = ecs.entity(); e.set<Self>({e});

    int32_t count = 0;

    q.iter([&](flecs::iter& it, Self *s) {
        auto o = it.term<const Other>(2);
        test_assert(!it.is_owned(2));
        test_int(o->value, 10);
        
        const Other& o_ref = *o;
        test_int(o_ref.value, 10);

        for (auto i : it) {
            test_assert(it.entity(i) == s[i].value);
            count ++;
        }
    });
    
    test_int(count, 3);
}

void QueryBuilder_isa_superset_term() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self>()
        .term<Other>().subj().set(flecs::SuperSet)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});

    int32_t count = 0;

    q.iter([&](flecs::iter& it, Self *s) {
        auto o = it.term<const Other>(2);
        test_assert(!it.is_owned(2));
        test_int(o->value, 10);

        for (auto i : it) {
            test_assert(it.entity(i) == s[i].value);
            count ++;
        }
    });
    
    test_int(count, 3);
}

void QueryBuilder_isa_self_superset_term() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self>()
        .term<Other>().subj().set(flecs::Self | flecs::SuperSet)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().set<Other>({20}); e.set<Self>({e});
    e = ecs.entity().set<Other>({20}); e.set<Self>({e});

    int32_t count = 0;
    int32_t owned_count = 0;

    q.iter([&](flecs::iter& it, Self *s) {
        auto o = it.term<const Other>(2);

        if (!it.is_owned(2)) {
            test_int(o->value, 10);
        } else {
            for (auto i : it) {
                test_int(o[i].value, 20);
                owned_count ++;
            }
        }

        for (auto i : it) {
            test_assert(it.entity(i) == s[i].value);
            count ++;
        }
    });
    
    test_int(count, 5);
    test_int(owned_count, 2);
}

void QueryBuilder_childof_superset_term() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self>()
        .term<Other>().subj().set(flecs::SuperSet, flecs::ChildOf)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});

    int32_t count = 0;

    q.iter([&](flecs::iter& it, Self *s) {
        auto o = it.term<const Other>(2);
        test_assert(!it.is_owned(2));
        test_int(o->value, 10);

        for (auto i : it) {
            test_assert(it.entity(i) == s[i].value);
            count ++;
        }
    });
    
    test_int(count, 3);
}

void QueryBuilder_childof_self_superset_term() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self>()
        .term<Other>().subj().set(flecs::Self | flecs::SuperSet, flecs::ChildOf)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().set<Other>({20}); e.set<Self>({e});
    e = ecs.entity().set<Other>({20}); e.set<Self>({e});

    int32_t count = 0;
    int32_t owned_count = 0;

    q.iter([&](flecs::iter& it, Self *s) {
        auto o = it.term<const Other>(2);

        if (!it.is_owned(2)) {
            test_int(o->value, 10);
        } else {
            for (auto i : it) {
                test_int(o[i].value, 20);
                owned_count ++;
            }
        }

        for (auto i : it) {
            test_assert(it.entity(i) == s[i].value);
            count ++;
        }
    });
    
    test_int(count, 5);
    test_int(owned_count, 2);
}

void QueryBuilder_isa_superset_term_w_each() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).subj().set(flecs::SuperSet)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 3);
}

void QueryBuilder_isa_self_superset_term_w_each() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).subj().set(flecs::Self | flecs::SuperSet)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().set<Other>({10}); e.set<Self>({e});
    e = ecs.entity().set<Other>({10}); e.set<Self>({e});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 5);
}

void QueryBuilder_childof_superset_term_w_each() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).subj().set(flecs::SuperSet, flecs::ChildOf)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 3);
}

void QueryBuilder_childof_self_superset_term_w_each() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).subj().set(flecs::Self | flecs::SuperSet, flecs::ChildOf)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().set<Other>({10}); e.set<Self>({e});
    e = ecs.entity().set<Other>({10}); e.set<Self>({e});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 5);
}

void QueryBuilder_isa_superset_shortcut() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).super()
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().is_a(base); e.set<Self>({e});
    e = ecs.entity().is_a(base); e.set<Self>({e});
    e = ecs.entity().is_a(base); e.set<Self>({e});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 3);
}

void QueryBuilder_isa_superset_shortcut_w_self() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).super(flecs::IsA, flecs::Self)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().is_a(base); e.set<Self>({e});
    e = ecs.entity().is_a(base); e.set<Self>({e});
    e = ecs.entity().is_a(base); e.set<Self>({e});
    e = ecs.entity().set<Other>({10}); e.set<Self>({e});
    e = ecs.entity().set<Other>({10}); e.set<Self>({e});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 5);
}

void QueryBuilder_childof_superset_shortcut() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).super(flecs::ChildOf)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 3);
}

void QueryBuilder_childof_superset_shortcut_w_self() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).super(flecs::ChildOf, flecs::Self)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().set<Other>({10}); e.set<Self>({e});
    e = ecs.entity().set<Other>({10}); e.set<Self>({e});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 5);
}

void QueryBuilder_isa_superset_max_depth_1() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).super().max_depth(1)
        .build();

    auto base_1 = ecs.entity().set<Other>({10});
    auto base_2 = ecs.entity().is_a(base_1);
    auto base_3 = ecs.entity().is_a(base_2);
    auto base_4 = ecs.entity().is_a(base_3);

    auto 
    e = ecs.entity().is_a(base_1); e.set<Self>({e});
    e = ecs.entity().is_a(base_1); e.set<Self>({e});

    e = ecs.entity().is_a(base_2); e.set<Self>({0});
    e = ecs.entity().is_a(base_2); e.set<Self>({0});

    e = ecs.entity().is_a(base_3); e.set<Self>({0});
    e = ecs.entity().is_a(base_3); e.set<Self>({0});

    e = ecs.entity().is_a(base_4); e.set<Self>({0});
    e = ecs.entity().is_a(base_4); e.set<Self>({0});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 2);
}

void QueryBuilder_isa_superset_max_depth_2() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).super().max_depth(2)
        .build();

    auto base_1 = ecs.entity().set<Other>({10});
    auto base_2 = ecs.entity().is_a(base_1);
    auto base_3 = ecs.entity().is_a(base_2);
    auto base_4 = ecs.entity().is_a(base_3);

    auto 
    e = ecs.entity().is_a(base_1); e.set<Self>({e});
    e = ecs.entity().is_a(base_1); e.set<Self>({e});

    e = ecs.entity().is_a(base_2); e.set<Self>({e});
    e = ecs.entity().is_a(base_2); e.set<Self>({e});

    e = ecs.entity().is_a(base_3); e.set<Self>({0});
    e = ecs.entity().is_a(base_3); e.set<Self>({0});

    e = ecs.entity().is_a(base_4); e.set<Self>({0});
    e = ecs.entity().is_a(base_4); e.set<Self>({0});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 4);
}

void QueryBuilder_isa_superset_min_depth_2() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).super().min_depth(2)
        .build();

    auto base_1 = ecs.entity().set<Other>({10});
    auto base_2 = ecs.entity().is_a(base_1);
    auto base_3 = ecs.entity().is_a(base_2);
    auto base_4 = ecs.entity().is_a(base_3);

    auto 
    e = ecs.entity().is_a(base_1); e.set<Self>({0});
    e = ecs.entity().is_a(base_1); e.set<Self>({0});

    e = ecs.entity().is_a(base_2); e.set<Self>({e});
    e = ecs.entity().is_a(base_2); e.set<Self>({e});

    e = ecs.entity().is_a(base_3); e.set<Self>({e});
    e = ecs.entity().is_a(base_3); e.set<Self>({e});

    e = ecs.entity().is_a(base_4); e.set<Self>({e});
    e = ecs.entity().is_a(base_4); e.set<Self>({e});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 6);
}

void QueryBuilder_isa_superset_min_depth_2_max_depth_3() {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .arg(2).super().min_depth(2).max_depth(3)
        .build();

    auto base_1 = ecs.entity().set<Other>({10});
    auto base_2 = ecs.entity().is_a(base_1);
    auto base_3 = ecs.entity().is_a(base_2);
    auto base_4 = ecs.entity().is_a(base_3);

    auto 
    e = ecs.entity().is_a(base_1); e.set<Self>({0});
    e = ecs.entity().is_a(base_1); e.set<Self>({0});

    e = ecs.entity().is_a(base_2); e.set<Self>({e});
    e = ecs.entity().is_a(base_2); e.set<Self>({e});

    e = ecs.entity().is_a(base_3); e.set<Self>({e});
    e = ecs.entity().is_a(base_3); e.set<Self>({e});

    e = ecs.entity().is_a(base_4); e.set<Self>({0});
    e = ecs.entity().is_a(base_4); e.set<Self>({0});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s, Other& o) {
        test_assert(e == s.value);
        test_int(o.value, 10);
        count ++;
    });
    
    test_int(count, 4);
}

void QueryBuilder_relation() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<Self>()
        .term(Likes, Bob)
        .build();

    auto 
    e = ecs.entity().add(Likes, Bob); e.set<Self>({e});
    e = ecs.entity().add(Likes, Bob); e.set<Self>({e});

    e = ecs.entity().add(Likes, Alice); e.set<Self>({0});
    e = ecs.entity().add(Likes, Alice); e.set<Self>({0});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });
    
    test_int(count, 2);
}

void QueryBuilder_relation_w_object_wildcard() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<Self>()
        .term(Likes, flecs::Wildcard)
        .build();

    auto 
    e = ecs.entity().add(Likes, Bob); e.set<Self>({e});
    e = ecs.entity().add(Likes, Bob); e.set<Self>({e});

    e = ecs.entity().add(Likes, Alice); e.set<Self>({e});
    e = ecs.entity().add(Likes, Alice); e.set<Self>({e});

    e = ecs.entity(); e.set<Self>({0});
    e = ecs.entity(); e.set<Self>({0});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });
    
    test_int(count, 4);
}

void QueryBuilder_relation_w_predicate_wildcard() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Dislikes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<Self>()
        .term(flecs::Wildcard, Alice)
        .build();

    auto 
    e = ecs.entity().add(Likes, Alice); e.set<Self>({e});
    e = ecs.entity().add(Dislikes, Alice); e.set<Self>({e});

    e = ecs.entity().add(Likes, Bob); e.set<Self>({0});
    e = ecs.entity().add(Dislikes, Bob); e.set<Self>({0});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });
    
    test_int(count, 2);
}

void QueryBuilder_add_pair_w_rel_type() {
    flecs::world ecs;

    struct Likes { };
    
    auto Dislikes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<Self>()
        .term<Likes>(flecs::Wildcard)
        .build();

    auto 
    e = ecs.entity().add<Likes>(Alice); e.set<Self>({e});
    e = ecs.entity().add(Dislikes, Alice); e.set<Self>({0});

    e = ecs.entity().add<Likes>(Bob); e.set<Self>({e});
    e = ecs.entity().add(Dislikes, Bob); e.set<Self>({0});

    int32_t count = 0;

    q.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });
    
    test_int(count, 2);
}

void QueryBuilder_template_term() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Template<int>>()
        .build();

    auto e1 = ecs.entity().add<Position>().add<Template<int>>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_explicit_subject_w_id() {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .term<Position>().entity(flecs::This)
        .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_explicit_subject_w_type() {
    flecs::world ecs;

    ecs.set<Position>({10, 20});

    auto q = ecs.query_builder<Position>()
        .term<Position>().subj<Position>()
        .build();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
        count ++;
        test_assert(e == ecs.singleton<Position>());
    });
    
    test_int(count, 1);
}

void QueryBuilder_explicit_object_w_id() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Alice = ecs.entity();
    auto Bob = ecs.entity();

    auto q = ecs.query_builder<>()
        .term(Likes).obj(Alice)
        .build();

    auto e1 = ecs.entity().add(Likes, Alice);
    ecs.entity().add(Likes, Bob);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_explicit_object_w_type() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    struct Alice { };
    auto Bob = ecs.entity();

    auto q = ecs.query_builder<>()
        .term(Likes).obj<Alice>()
        .build();

    auto e1 = ecs.entity().add(Likes, ecs.id<Alice>());
    ecs.entity().add(Likes, Bob);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_explicit_term() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term(ecs.term().id<Position>())
        .build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_explicit_term_w_type() {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .term(ecs.term<Position>())
        .build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_explicit_term_w_pair_type() {
    flecs::world ecs;

    struct Likes { };
    struct Alice { };
    struct Bob { };

    auto q = ecs.query_builder<>()
        .term(ecs.term<Likes, Alice>())
        .build();

    auto e1 = ecs.entity().add<Likes, Alice>();
    ecs.entity().add<Likes, Bob>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_explicit_term_w_id() {
    flecs::world ecs;

    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto q = ecs.query_builder<>()
        .term(ecs.term(Apples))
        .build();

    auto e1 = ecs.entity().add(Apples);
    ecs.entity().add(Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_explicit_term_w_pair_id() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto q = ecs.query_builder<>()
        .term(ecs.term(Likes, Apples))
        .build();

    auto e1 = ecs.entity().add(Likes, Apples);
    ecs.entity().add(Likes, Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_1_term_to_empty() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();

    auto qb = ecs.query_builder<>()
        .term<Position>();

    qb.term(Likes, Apples);

    auto q = qb.build();

    test_int(q.term_count(), 2);
    test_int(q.term(0).id(), ecs.id<Position>());
    test_int(q.term(1).id(), ecs.pair(Likes, Apples));
}

void QueryBuilder_2_subsequent_args() {
    flecs::world ecs;

    struct Rel { int foo; };

    int32_t count = 0;

    auto s = ecs.system<Rel, const Velocity>()
        .arg(1).obj(flecs::Wildcard)
        .arg(2).singleton()
        .iter([&](flecs::iter it){
            count += it.count();
        });
    
    ecs.entity().add<Rel, Tag>();
    ecs.set<Velocity>({});

    s.run();

    test_int(count, 1);
}

void QueryBuilder_optional_tag_is_set() {
    flecs::world ecs;

    struct TagA { };
    struct TagB { };

    auto q = ecs.query_builder()
        .term<TagA>()
        .term<TagB>().oper(flecs::Optional)
        .build();

    auto e_1 = ecs.entity().add<TagA>().add<TagB>();
    auto e_2 = ecs.entity().add<TagA>();

    int count = 0;

    q.iter([&](flecs::iter& it) {
        test_int(it.count(), 1);

        count += it.count();
        
        if (it.entity(0) == e_1) {
            test_bool(it.is_set(1), true);
            test_bool(it.is_set(2), true);
        } else {
            test_assert(it.entity(0) == e_2);
            test_bool(it.is_set(1), true);
            test_bool(it.is_set(2), false);
        }
    });

    test_int(count, 2);
}

void QueryBuilder_10_terms() {
    flecs::world ecs;

    auto f = ecs.query_builder<>()
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
        .build();

    test_int(f.term_count(), 10);

    auto e = ecs.entity()
        .add<TagA>()
        .add<TagB>()
        .add<TagC>()
        .add<TagD>()
        .add<TagE>()
        .add<TagF>()
        .add<TagG>()
        .add<TagH>()
        .add<TagI>()
        .add<TagJ>();

    int count = 0;
    f.iter([&](flecs::iter& it) {
        test_int(it.count(), 1);
        test_assert(it.entity(0) == e);
        test_int(it.term_count(), 10);
        count ++;
    });

    test_int(count, 1);
}

void QueryBuilder_20_terms() {
    flecs::world ecs;

    auto f = ecs.query_builder<>()
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
        .build();

    test_int(f.term_count(), 20);

    auto e = ecs.entity()
        .add<TagA>()
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

    int count = 0;
    f.iter([&](flecs::iter& it) {
        test_int(it.count(), 1);
        test_assert(it.entity(0) == e);
        test_int(it.term_count(), 20);
        count ++;
    });

    test_int(count, 1);
}

uint64_t group_by_first_id(
    flecs::world_t *m_world,
    flecs::table_t *m_table,
    flecs::entity_t id,
    void *ctx)
{
    const flecs::type_t *type = ecs_table_get_type(m_table);
    return type->array[0];
}

uint64_t group_by_first_id_negated(
    flecs::world_t *m_world,
    flecs::table_t *m_table,
    flecs::entity_t id,
    void *ctx)
{
    return ~group_by_first_id(m_world, m_table, id, ctx);
}

void QueryBuilder_group_by_raw() {
    flecs::world ecs;

    struct TagA { };
    struct TagB { };
    struct TagC { };
    struct TagX { };

    ecs.component<TagA>();
    ecs.component<TagB>();
    ecs.component<TagC>();
    ecs.component<TagX>();

    auto q = ecs.query_builder()
        .term<TagX>()
        .group_by(flecs::type_id<TagX>(), group_by_first_id)
        .build();

    auto q_reverse = ecs.query_builder()
        .term<TagX>()
        .group_by(flecs::type_id<TagX>(), group_by_first_id_negated)
        .build();

    auto e3 = ecs.entity().add<TagX>().add<TagC>();
    auto e2 = ecs.entity().add<TagX>().add<TagB>();
    auto e1 = ecs.entity().add<TagX>().add<TagA>();

    int count = 0;

    q.iter([&](flecs::iter& it){
        test_int(it.count(), 1);
        if(count == 0){
            test_bool(it.entity(0) == e1, true);
        }else if(count == 1){
            test_bool(it.entity(0) == e2, true);
        }else if(count == 2){
            test_bool(it.entity(0) == e3, true);
        }else{
            test_assert(false);
        }
        count++;
    });
    test_int(count, 3);
    
    count = 0;
    q_reverse.iter([&](flecs::iter& it){
        test_int(it.count(), 1);
        if(count == 0){
            test_bool(it.entity(0) == e3, true);
        }else if(count == 1){
            test_bool(it.entity(0) == e2, true);
        }else if(count == 2){
            test_bool(it.entity(0) == e1, true);
        }else{
            test_assert(false);
        }
        count++;
    });
    test_int(count, 3);
}

void QueryBuilder_group_by_template() {
    flecs::world ecs;

    struct TagA { };
    struct TagB { };
    struct TagC { };
    struct TagX { };

    ecs.component<TagA>();
    ecs.component<TagB>();
    ecs.component<TagC>();
    ecs.component<TagX>();

    auto q = ecs.query_builder()
        .term<TagX>()
        .group_by<TagX>(group_by_first_id)
        .build();

    auto q_reverse = ecs.query_builder()
        .term<TagX>()
        .group_by<TagX>( group_by_first_id_negated)
        .build();

    auto e3 = ecs.entity().add<TagX>().add<TagC>();
    auto e2 = ecs.entity().add<TagX>().add<TagB>();
    auto e1 = ecs.entity().add<TagX>().add<TagA>();

    int count = 0;

    q.iter([&](flecs::iter& it){
        test_int(it.count(), 1);
        if(count == 0){
            test_bool(it.entity(0) == e1, true);
        }else if(count == 1){
            test_bool(it.entity(0) == e2, true);
        }else if(count == 2){
            test_bool(it.entity(0) == e3, true);
        }else{
            test_assert(false);
        }
        count++;
    });
    test_int(count, 3);
    
    count = 0;
    q_reverse.iter([&](flecs::iter& it){
        test_int(it.count(), 1);
        if(count == 0){
            test_bool(it.entity(0) == e3, true);
        }else if(count == 1){
            test_bool(it.entity(0) == e2, true);
        }else if(count == 2){
            test_bool(it.entity(0) == e1, true);
        }else{
            test_assert(false);
        }
        count++;
    });
    test_int(count, 3);
}

void QueryBuilder_create_w_no_template_args() {
    flecs::world ecs;

    auto q = ecs.query_builder().term<Position>().build();

    auto e1 = ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_any_wildcard() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apple = ecs.entity();
    auto Mango = ecs.entity();

    auto e1 = ecs.entity()
        .add(Likes, Apple)
        .add(Likes, Mango);

    auto q = ecs.query_builder()
        .term(Likes, flecs::Any)
        .build();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}
