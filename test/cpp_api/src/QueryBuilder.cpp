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
        auto o = it.field<const Other>(2);
        test_assert(!it.is_self(2));
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
        .term<Other>().src().up()
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});

    int32_t count = 0;

    q.iter([&](flecs::iter& it, Self *s) {
        auto o = it.field<const Other>(2);
        test_assert(!it.is_self(2));
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
        .term<Other>().src().self().up()
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
        auto o = it.field<const Other>(2);

        if (!it.is_self(2)) {
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
        .term<Other>().src().up(flecs::ChildOf)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});

    int32_t count = 0;

    q.iter([&](flecs::iter& it, Self *s) {
        auto o = it.field<const Other>(2);
        test_assert(!it.is_self(2));
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
        .term<Other>().src().self().up(flecs::ChildOf)
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
        auto o = it.field<const Other>(2);

        if (!it.is_self(2)) {
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
        .arg(2).src().up()
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
        .arg(2).src().self().up()
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
        .arg(2).src().up(flecs::ChildOf)
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
        .arg(2).src().self().up(flecs::ChildOf)
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
        .arg(2).up()
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
        .arg(2).self().up(flecs::IsA)
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
        .arg(2).up(flecs::ChildOf)
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
        .arg(2).self().up(flecs::ChildOf)
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
        .term<Position>().id(flecs::This)
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
        .term<Position>().src<Position>()
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
        .term(Likes).second(Alice)
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
        .term(Likes).second<Alice>()
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
        .term(ecs.term(ecs.id<Position>()))
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

    test_int(q.field_count(), 2);
    test_int(q.term(0).id(), ecs.id<Position>());
    test_int(q.term(1).id(), ecs.pair(Likes, Apples));
}

void QueryBuilder_2_subsequent_args() {
    flecs::world ecs;

    struct Rel { int foo; };

    int32_t count = 0;

    auto s = ecs.system<Rel, const Velocity>()
        .arg(1).second(flecs::Wildcard)
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

    test_int(f.field_count(), 10);

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
        test_int(it.field_count(), 10);
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

    test_int(f.field_count(), 20);

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
        test_int(it.field_count(), 20);
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

static
uint64_t group_by_rel(flecs::world_t *world, flecs::table_t *table, flecs::entity_t id, void *ctx) {
    ecs_id_t match;
    if (ecs_search(world, table, ecs_pair(id, EcsWildcard), &match) != -1) {
        return ECS_PAIR_SECOND(match);
    }
    return 0;
}

void QueryBuilder_group_by_iter_one() {
    flecs::world ecs;

    auto Rel = ecs.entity();
    auto TgtA = ecs.entity();
    auto TgtB = ecs.entity();
    auto TgtC = ecs.entity();
    auto Tag = ecs.entity();

    ecs.entity().add(Rel, TgtA);
    auto e2 = ecs.entity().add(Rel, TgtB);
    ecs.entity().add(Rel, TgtC);

    ecs.entity().add(Rel, TgtA).add(Tag);
    auto e5 = ecs.entity().add(Rel, TgtB).add(Tag);
    ecs.entity().add(Rel, TgtC).add(Tag);

    auto q = ecs.query_builder()
        .term(Rel, flecs::Wildcard)
        .group_by(Rel, group_by_rel)
        .build();

    bool e2_found = false;
    bool e5_found = false;
    int32_t count = 0;

    q.iter().set_group(TgtB).each([&](flecs::iter& it, size_t i) {
        flecs::entity e = it.entity(i);
        test_assert(it.group_id() == TgtB);

        if (e == e2) e2_found = true;
        if (e == e5) e5_found = true;
        count ++;
    });

    test_int(2, count);
    test_bool(true, e2_found);
    test_bool(true, e5_found);
}

void QueryBuilder_group_by_iter_one_template() {
    flecs::world ecs;

    struct Rel { };
    struct TgtA { };
    struct TgtB { };
    struct TgtC { };
    struct Tag { };

    ecs.entity().add<Rel, TgtA>();
    auto e2 = ecs.entity().add<Rel, TgtB>();
    ecs.entity().add<Rel, TgtC>();

    ecs.entity().add<Rel, TgtA>().add<Tag>();
    auto e5 = ecs.entity().add<Rel, TgtB>().add<Tag>();
    ecs.entity().add<Rel, TgtC>().add<Tag>();

    auto q = ecs.query_builder()
        .term<Rel>(flecs::Wildcard)
        .group_by<Rel>(group_by_rel)
        .build();

    bool e2_found = false;
    bool e5_found = false;
    int32_t count = 0;

    q.iter().set_group<TgtB>().each([&](flecs::iter& it, size_t i) {
        flecs::entity e = it.entity(i);
        test_assert(it.group_id() == ecs.id<TgtB>());

        if (e == e2) e2_found = true;
        if (e == e5) e5_found = true;
        count ++;
    });

    test_int(2, count);
    test_bool(true, e2_found);
    test_bool(true, e5_found);
}

void QueryBuilder_group_by_iter_one_all_groups() {
    flecs::world ecs;

    auto Rel = ecs.entity();
    auto TgtA = ecs.entity();
    auto TgtB = ecs.entity();
    auto TgtC = ecs.entity();
    auto Tag = ecs.entity();

    auto e1 = ecs.entity().add(Rel, TgtA);
    auto e2 = ecs.entity().add(Rel, TgtB);
    auto e3 = ecs.entity().add(Rel, TgtC);

    auto e4 = ecs.entity().add(Rel, TgtA).add(Tag);
    auto e5 = ecs.entity().add(Rel, TgtB).add(Tag);
    auto e6 = ecs.entity().add(Rel, TgtC).add(Tag);

    auto q = ecs.query_builder()
        .term(Rel, flecs::Wildcard)
        .group_by(Rel, group_by_rel)
        .build();

    int e1_found = 0;
    int e2_found = 0;
    int e3_found = 0;
    int e4_found = 0;
    int e5_found = 0;
    int e6_found = 0;
    int32_t count = 0;
    uint64_t group_id = 0;

    const auto func = [&](flecs::iter& it, size_t i) {
        flecs::entity e = it.entity(i);
        test_assert(it.group_id() == group_id);
        if (e == e1) e1_found ++;
        if (e == e2) e2_found ++;
        if (e == e3) e3_found ++;
        if (e == e4) e4_found ++;
        if (e == e5) e5_found ++;
        if (e == e6) e6_found ++;
        count ++;
    };

    group_id = TgtB;
    q.iter().set_group(TgtB).each(func);
    test_int(2, count);
    test_int(1, e2_found);
    test_int(1, e5_found);

    group_id = TgtA;
    q.iter().set_group(TgtA).each(func);
    test_int(4, count);
    test_int(1, e1_found);
    test_int(1, e4_found);

    group_id = TgtC;
    q.iter().set_group(TgtC).each(func);
    test_int(6, count);
    test_int(1, e3_found);
    test_int(1, e6_found);

    test_int(1, e1_found);
    test_int(1, e2_found);
    test_int(1, e3_found);
    test_int(1, e4_found);
    test_int(1, e5_found);
    test_int(1, e6_found);
}

void QueryBuilder_group_by_default_func_w_id() {
    flecs::world ecs;

    auto Rel = ecs.entity();
    auto TgtA = ecs.entity();
    auto TgtB = ecs.entity();
    auto TgtC = ecs.entity();

    auto e1 = ecs.entity().add(Rel, TgtC);
    auto e2 = ecs.entity().add(Rel, TgtB);
    auto e3 = ecs.entity().add(Rel, TgtA);

    auto q = ecs.query_builder()
        .term(Rel, flecs::Wildcard)
        .group_by(Rel)
        .build();

    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    int32_t count = 0;

    q.each([&](flecs::iter& it, size_t i) {
        flecs::entity e = it.entity(i);
        if (e == e1) {
            test_assert(it.group_id() == TgtC);
            test_assert(!e1_found);
            test_assert(e2_found);
            test_assert(e3_found);
            e1_found = true;
        }
        if (e == e2) {
            test_assert(it.group_id() == TgtB);
            test_assert(!e1_found);
            test_assert(!e2_found);
            test_assert(e3_found);
            e2_found = true;
        }
        if (e == e3) {
            test_assert(it.group_id() == TgtA);
            test_assert(!e1_found);
            test_assert(!e2_found);
            test_assert(!e3_found);
            e3_found = true;
        }
        count ++;
    });

    test_int(3, count);
    test_bool(true, e1_found);
    test_bool(true, e2_found);
    test_bool(true, e3_found);
}

void QueryBuilder_group_by_default_func_w_type() {
    flecs::world ecs;

    struct Rel { };
    auto TgtA = ecs.entity();
    auto TgtB = ecs.entity();
    auto TgtC = ecs.entity();

    auto e1 = ecs.entity().add<Rel>(TgtC);
    auto e2 = ecs.entity().add<Rel>(TgtB);
    auto e3 = ecs.entity().add<Rel>(TgtA);

    auto q = ecs.query_builder()
        .term<Rel>(flecs::Wildcard)
        .group_by<Rel>()
        .build();

    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    int32_t count = 0;

    q.each([&](flecs::iter& it, size_t i) {
        flecs::entity e = it.entity(i);
        if (e == e1) {
            test_assert(it.group_id() == TgtC);
            test_assert(!e1_found);
            test_assert(e2_found);
            test_assert(e3_found);
            e1_found = true;
        }
        if (e == e2) {
            test_assert(it.group_id() == TgtB);
            test_assert(!e1_found);
            test_assert(!e2_found);
            test_assert(e3_found);
            e2_found = true;
        }
        if (e == e3) {
            test_assert(it.group_id() == TgtA);
            test_assert(!e1_found);
            test_assert(!e2_found);
            test_assert(!e3_found);
            e3_found = true;
        }
        count ++;
    });

    test_int(3, count);
    test_bool(true, e1_found);
    test_bool(true, e2_found);
    test_bool(true, e3_found);
}

static int group_by_ctx = 0;

void QueryBuilder_group_by_callbacks() {
    flecs::world ecs;

    struct Rel { };
    auto TgtA = ecs.entity();
    auto TgtB = ecs.entity();
    auto TgtC = ecs.entity();

    auto e1 = ecs.entity().add<Rel>(TgtC);
    auto e2 = ecs.entity().add<Rel>(TgtB);
    auto e3 = ecs.entity().add<Rel>(TgtA);

    auto q = ecs.query_builder()
        .term<Rel>(flecs::Wildcard)
        .group_by<Rel>()
        .group_by_ctx(&group_by_ctx)
        .on_group_create(
            [](flecs::world_t *world, uint64_t id, void *group_by_arg) {
                test_assert(world != nullptr);
                test_assert(id != 0);
                test_assert(group_by_arg != nullptr);
                test_assert(group_by_arg == &group_by_ctx);
                uint64_t *ctx = ecs_os_malloc_t(uint64_t);
                *ctx = id;
                return (void*)ctx;
            })
        .on_group_delete(
            [](flecs::world_t *world, uint64_t id, void *ctx, void *group_by_arg) {
                test_assert(world != nullptr);
                test_assert(id != 0);
                test_assert(group_by_arg != nullptr);
                test_assert(group_by_arg == &group_by_ctx);
                test_assert(ctx != NULL);
                test_uint(*(uint64_t*)ctx, id);
                ecs_os_free(ctx);
            })
        .build();

    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    int32_t count = 0;

    q.each([&](flecs::iter& it, size_t i) {
        flecs::entity e = it.entity(i);
        if (e == e1) {
            test_assert(it.group_id() == TgtC);
            test_assert(!e1_found);
            test_assert(e2_found);
            test_assert(e3_found);
            e1_found = true;
            uint64_t *ctx = (uint64_t*)q.group_ctx(it.group_id());
            test_uint(*ctx, it.group_id());
        }
        if (e == e2) {
            test_assert(it.group_id() == TgtB);
            test_assert(!e1_found);
            test_assert(!e2_found);
            test_assert(e3_found);
            e2_found = true;
            uint64_t *ctx = (uint64_t*)q.group_ctx(it.group_id());
            test_uint(*ctx, it.group_id());
        }
        if (e == e3) {
            test_assert(it.group_id() == TgtA);
            test_assert(!e1_found);
            test_assert(!e2_found);
            test_assert(!e3_found);
            e3_found = true;
            uint64_t *ctx = (uint64_t*)q.group_ctx(it.group_id());
            test_uint(*ctx, it.group_id());
        }
        count ++;
    });

    test_int(3, count);
    test_bool(true, e1_found);
    test_bool(true, e2_found);
    test_bool(true, e3_found);
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

void QueryBuilder_cascade() {
    flecs::world ecs;

    auto Tag = ecs.entity();
    auto Foo = ecs.entity();
    auto Bar = ecs.entity();

    auto e0 = ecs.entity().add(Tag);
    auto e1 = ecs.entity().is_a(e0);
    auto e2 = ecs.entity().is_a(e1);
    auto e3 = ecs.entity().is_a(e2);

    auto q = ecs.query_builder()
        .term(Tag).cascade()
        .build();

    e1.add(Bar);
    e2.add(Foo);

    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;

        if (e == e1) {
            test_bool(e1_found, false);
            test_bool(e2_found, false);
            test_bool(e3_found, false);
            e1_found = true;
        }
        if (e == e2) {
            test_bool(e1_found, true);
            test_bool(e2_found, false);
            test_bool(e3_found, false);
            e2_found = true;
        }
        if (e == e3) {
            test_bool(e1_found, true);
            test_bool(e2_found, true);
            test_bool(e3_found, false);
            e3_found = true;
        }
    });

    test_bool(e1_found, true);
    test_bool(e2_found, true);
    test_bool(e3_found, true);
    test_int(count, 3);
}

void QueryBuilder_cascade_w_relationship() {
    flecs::world ecs;

    auto Tag = ecs.entity();
    auto Foo = ecs.entity();
    auto Bar = ecs.entity();

    auto e0 = ecs.entity().add(Tag);
    auto e1 = ecs.entity().child_of(e0);
    auto e2 = ecs.entity().child_of(e1);
    auto e3 = ecs.entity().child_of(e2);

    auto q = ecs.query_builder()
        .term(Tag).cascade(flecs::ChildOf)
        .build();

    e1.add(Bar);
    e2.add(Foo);

    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;

        if (e == e1) {
            test_bool(e1_found, false);
            test_bool(e2_found, false);
            test_bool(e3_found, false);
            e1_found = true;
        }
        if (e == e2) {
            test_bool(e1_found, true);
            test_bool(e2_found, false);
            test_bool(e3_found, false);
            e2_found = true;
        }
        if (e == e3) {
            test_bool(e1_found, true);
            test_bool(e2_found, true);
            test_bool(e3_found, false);
            e3_found = true;
        }
    });

    test_bool(e1_found, true);
    test_bool(e2_found, true);
    test_bool(e3_found, true);
    test_int(count, 3);
}

void QueryBuilder_up_w_type() {
    flecs::world ecs;

    struct Rel { };

    ecs.component<Rel>().add(flecs::Acyclic);

    auto q = ecs.query_builder<Self>()
        .term<Other>().src().up<Rel>()
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().add<Rel>(base); e.set<Self>({e});
    e = ecs.entity().add<Rel>(base); e.set<Self>({e});
    e = ecs.entity().add<Rel>(base); e.set<Self>({e});

    int32_t count = 0;

    q.iter([&](flecs::iter& it, Self *s) {
        auto o = it.field<const Other>(2);
        test_assert(!it.is_self(2));
        test_int(o->value, 10);

        for (auto i : it) {
            test_assert(it.entity(i) == s[i].value);
            count ++;
        }
    });
    
    test_int(count, 3);
}

void QueryBuilder_cascade_w_type() {
    flecs::world ecs;

    struct Rel { };

    ecs.component<Rel>().add(flecs::Acyclic);

    auto Tag = ecs.entity();
    auto Foo = ecs.entity();
    auto Bar = ecs.entity();

    auto e0 = ecs.entity().add(Tag);
    auto e1 = ecs.entity().add<Rel>(e0);
    auto e2 = ecs.entity().add<Rel>(e1);
    auto e3 = ecs.entity().add<Rel>(e2);

    auto q = ecs.query_builder()
        .term(Tag).cascade<Rel>()
        .build();

    e1.add(Bar);
    e2.add(Foo);

    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;

        if (e == e1) {
            test_bool(e1_found, false);
            test_bool(e2_found, false);
            test_bool(e3_found, false);
            e1_found = true;
        }
        if (e == e2) {
            test_bool(e1_found, true);
            test_bool(e2_found, false);
            test_bool(e3_found, false);
            e2_found = true;
        }
        if (e == e3) {
            test_bool(e1_found, true);
            test_bool(e2_found, true);
            test_bool(e3_found, false);
            e3_found = true;
        }
    });

    test_bool(e1_found, true);
    test_bool(e2_found, true);
    test_bool(e3_found, true);
    test_int(count, 3);
}

void QueryBuilder_named_query() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();

    auto q = ecs.query_builder("my_query")
        .term<Position>()
        .build();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        test_assert(e == e1 || e == e2);
        count ++;
    });
    test_int(count, 2);

    flecs::entity qe = q.entity();
    test_assert(qe != 0);
    test_str(qe.name(), "my_query");
}

void QueryBuilder_term_w_write() {
    flecs::world ecs;

    auto q = ecs.query_builder()
        .term<Position>()
        .term<Position>().write()
        .build();

    auto f = q.filter();
    test_assert(f.term(0).inout() == flecs::InOutDefault);
    test_assert(f.term(0).get_src() == flecs::This);
    test_assert(f.term(1).inout() == flecs::Out);
    test_assert(f.term(1).get_src() == 0);
}

void QueryBuilder_term_w_read() {
    flecs::world ecs;

    auto q = ecs.query_builder()
        .term<Position>()
        .term<Position>().read()
        .build();

    auto f = q.filter();
    test_assert(f.term(0).inout() == flecs::InOutDefault);
    test_assert(f.term(0).get_src() == flecs::This);
    test_assert(f.term(1).inout() == flecs::In);
    test_assert(f.term(1).get_src() == 0);
}
