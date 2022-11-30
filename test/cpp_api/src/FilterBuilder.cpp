#include <cpp_api.h>

struct Other {
    int32_t value;
};

enum Color {
    Red, Green, Blue
};

void FilterBuilder_builder_assign_same_type() {
    flecs::world ecs;

    flecs::filter<Position, Velocity> q = 
        ecs.filter_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_builder_assign_to_empty() {
    flecs::world ecs;

    flecs::filter<> q = ecs.filter_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_builder_assign_from_empty() {
    flecs::world ecs;

    flecs::filter<> q = ecs.filter_builder<>()
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

template<typename ... Components>
struct FilterWrapper
{
    FilterWrapper(flecs::filter<Components...> f) : f_(f) {}
    flecs::filter<Components...> f_;
};

void FilterBuilder_builder_force_assign_operator() {
    flecs::world ecs;

    auto e1 = ecs.entity().set<Position>({10, 20});

    auto f = ecs.entity().emplace<FilterWrapper<>>(
        ecs.filter_builder().term<Position>().build()
    );

    int32_t count = 0;
    f.get<FilterWrapper<>>()->f_.each([&](flecs::entity e) {
        test_assert(e == e1);
        count ++;
    });
}

void FilterBuilder_builder_build() {
    flecs::world ecs;

    flecs::filter<Position, Velocity> q = 
        ecs.filter_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_builder_build_to_auto() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Position, Velocity>().build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_builder_build_n_statements() {
    flecs::world ecs;

    auto qb = ecs.filter_builder<>();
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

void FilterBuilder_1_type() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Position>().build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_add_1_type() {
    flecs::world ecs;

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_add_2_types() {
    flecs::world ecs;

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_add_1_type_w_1_type() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Position>()
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

void FilterBuilder_add_2_types_w_1_type() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Position>()
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

void FilterBuilder_add_pair() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_add_not() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Position>()
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

void FilterBuilder_add_or() {
    flecs::world ecs;

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_add_optional() {
    flecs::world ecs;

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_ptr_type() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Position, Velocity*>().build();

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

void FilterBuilder_const_type() {
    flecs::world ecs;

    auto q = ecs.filter_builder<const Position>().build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, const Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_string_term() {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_singleton_term() {
    flecs::world ecs;

    ecs.set<Other>({10});

    auto q = ecs.filter_builder<Self>()
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

void FilterBuilder_isa_superset_term() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self>()
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

void FilterBuilder_isa_self_superset_term() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self>()
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

void FilterBuilder_childof_superset_term() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self>()
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

void FilterBuilder_childof_self_superset_term() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self>()
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

void FilterBuilder_isa_superset_term_w_each() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self, Other>()
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

void FilterBuilder_isa_self_superset_term_w_each() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self, Other>()
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

void FilterBuilder_childof_superset_term_w_each() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self, Other>()
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

void FilterBuilder_childof_self_superset_term_w_each() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self, Other>()
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

void FilterBuilder_isa_superset_shortcut() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self, Other>()
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

void FilterBuilder_isa_superset_shortcut_w_self() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self, Other>()
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

void FilterBuilder_childof_superset_shortcut() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self, Other>()
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

void FilterBuilder_childof_superset_shortcut_w_self() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Self, Other>()
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

void FilterBuilder_relation() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.filter_builder<Self>()
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

void FilterBuilder_relation_w_object_wildcard() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.filter_builder<Self>()
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

void FilterBuilder_relation_w_predicate_wildcard() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Dislikes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.filter_builder<Self>()
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

void FilterBuilder_add_pair_w_rel_type() {
    flecs::world ecs;

    struct Likes { };
    
    auto Dislikes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.filter_builder<Self>()
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

void FilterBuilder_template_term() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Position>()
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

void FilterBuilder_explicit_subject_w_id() {
    flecs::world ecs;

    auto q = ecs.filter_builder<Position>()
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

void FilterBuilder_explicit_subject_w_type() {
    flecs::world ecs;

    ecs.set<Position>({10, 20});

    auto q = ecs.filter_builder<Position>()
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

void FilterBuilder_explicit_object_w_id() {
    flecs::world ecs;

    auto Likes = ecs.entity("Likes");
    auto Alice = ecs.entity("Alice");
    auto Bob = ecs.entity("Bob");

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_explicit_object_w_type() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    struct Alice { };
    auto Bob = ecs.entity();

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_explicit_term() {
    flecs::world ecs;

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_explicit_term_w_type() {
    flecs::world ecs;

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_explicit_term_w_pair_type() {
    flecs::world ecs;

    struct Likes { };
    struct Alice { };
    struct Bob { };

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_explicit_term_w_id() {
    flecs::world ecs;

    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_explicit_term_w_pair_id() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto q = ecs.filter_builder<>()
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

void FilterBuilder_1_term_to_empty() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();

    auto qb = ecs.filter_builder<>()
        .term<Position>();

    qb.term(Likes, Apples);

    auto q = qb.build();

    test_int(q.field_count(), 2);
    test_int(q.term(0).id(), ecs.id<Position>());
    test_int(q.term(1).id(), ecs.pair(Likes, Apples));
}

void FilterBuilder_2_subsequent_args() {
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

static
int filter_arg(flecs::filter<Self> f) {
    int32_t count = 0;

    f.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    return count;
}

void FilterBuilder_filter_as_arg() {
    flecs::world ecs;

    auto f = ecs.filter<Self>();

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    test_int(filter_arg(f), 3);
}

static
int filter_move_arg(flecs::filter<Self>&& f) {
    int32_t count = 0;

    f.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    return count;
}

void FilterBuilder_filter_as_move_arg() {
    flecs::world ecs;

    auto f = ecs.filter<Self>();

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    test_int(filter_move_arg(ecs.filter<Self>()), 3);
}

static
flecs::filter<Self> filter_return(flecs::world& ecs) {
    return ecs.filter<Self>();
}

void FilterBuilder_filter_as_return() {
    flecs::world ecs;

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    auto f = filter_return(ecs);

    int32_t count = 0;

    f.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    test_int(count, 3);
}

void FilterBuilder_filter_copy() {
    flecs::world ecs;

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    auto f = ecs.filter<Self>();

    auto f_2 = f;

    int32_t count = 0;

    f_2.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    test_int(count, 3);
}

void FilterBuilder_world_each_filter_1_component() {
    flecs::world ecs;

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    int32_t count = 0;

    ecs.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    test_int(count, 3);
}

void FilterBuilder_world_each_filter_2_components() {
    flecs::world ecs;

    auto e = ecs.entity();
    e.set<Self>({e})
     .set<Position>({10, 20});

    e = ecs.entity();
    e.set<Self>({e})
        .set<Position>({10, 20});

    e = ecs.entity();
    e.set<Self>({e})
     .set<Position>({10, 20});

    int32_t count = 0;

    ecs.each([&](flecs::entity e, Self& s, Position& p) {
        test_assert(e == s.value);
        test_int(p.x, 10);
        test_int(p.y, 20);
        count ++;
    });

    test_int(count, 3);
}

void FilterBuilder_world_each_filter_1_component_no_entity() {
    flecs::world ecs;

    ecs.entity()
        .set<Position>({10, 20});

    ecs.entity()
        .set<Position>({10, 20});

    ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    int32_t count = 0;

    ecs.each([&](Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
        count ++;
    });

    test_int(count, 3);
}

void FilterBuilder_world_each_filter_2_components_no_entity() {
    flecs::world ecs;

    ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity()
        .set<Position>({3, 5});

    ecs.entity()
        .set<Velocity>({20, 40});

    int32_t count = 0;

    ecs.each([&](Position& p, Velocity& v) {
        test_int(p.x, 10);
        test_int(p.y, 20);
        test_int(v.x, 1);
        test_int(v.y, 2);        
        count ++;
    });

    test_int(count, 3);
}

void FilterBuilder_10_terms() {
    flecs::world ecs;

    auto f = ecs.filter_builder<>()
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

void FilterBuilder_20_terms() {
    flecs::world ecs;

    auto f = ecs.filter_builder<>()
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

void FilterBuilder_term_after_arg() {
    flecs::world ecs;

    auto e_1 = ecs.entity()
        .add<TagA>()
        .add<TagB>()
        .add<TagC>();

    ecs.entity()
        .add<TagA>()
        .add<TagB>();

    auto f = ecs.filter_builder<TagA, TagB>()
        .arg(1).src(flecs::This) // dummy
        .term<TagC>()
        .build();

    test_int(f.field_count(), 3);

    int count = 0;
    f.each([&](flecs::entity e, TagA, TagB) {
        test_assert(e == e_1);
        count ++;
    });

    test_int(count, 1);
}

void FilterBuilder_name_arg() {
    flecs::world ecs;

    auto e = ecs.entity("Foo").set<Position>({10, 20});

    auto f = ecs.filter_builder<Position>()
        .arg(1).src().name("Foo")
        .build();

    int32_t count = 0;
    f.iter([&](flecs::iter& it, Position* p) {
        count ++;
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_assert(it.src(1) == e);
    });

    test_int(count, 1);
}

void FilterBuilder_const_in_term() {
    flecs::world ecs;

    ecs.entity().set<Position>({10, 20});

    auto f = ecs.filter_builder<>()
        .term<const Position>()
        .build();

    int32_t count = 0;
    f.iter([&](flecs::iter& it) {
        auto p = it.field<const Position>(1);
        test_assert(it.is_readonly(1));
        for (auto i : it) {
            count ++;
            test_int(p[i].x, 10);
            test_int(p[i].y, 20);
        }
    });

    test_int(count, 1);
}

void FilterBuilder_const_optional() {
    flecs::world ecs;
	
	ecs.entity().set<Position>({10, 20}).add<TagA>();
    ecs.entity().add<TagA>();

    auto f = ecs.filter_builder<TagA, const Position*>().build();
	
    int32_t count = 0, set_count = 0;
    f.iter([&](flecs::iter& it) {
        test_int(it.count(), 1);
        if (it.is_set(2)) {
            auto p = it.field<const Position>(2);
            test_assert(it.is_readonly(2));
            test_int(p->x, 10);
            test_int(p->y, 20);
            set_count ++;
        }
        count++;
	});
	
    test_int(count, 2);
    test_int(set_count, 1);
}

void FilterBuilder_create_w_no_template_args() {
    flecs::world ecs;

    auto q = ecs.filter_builder().term<Position>().build();

    auto e1 = ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_2_terms_w_expr() {
    flecs::world ecs;

    auto a = ecs.entity("A");
    auto b = ecs.entity("B");

    auto e1 = ecs.entity().add(a).add(b);

    auto f = ecs.filter_builder()
        .expr("A, B")
        .build();
    
    test_int(f.field_count(), 2);

    int32_t count = 0;
    f.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == e1) {
            test_assert(it.id(1) == a);
            test_assert(it.id(2) == b);
            count ++;
        }
    });

    test_int(count, 1);
}

void FilterBuilder_assert_on_uninitialized_term() {
    install_test_abort();

    flecs::world ecs;

    ecs.entity("A");
    ecs.entity("B");

    test_expect_abort();

    auto f = ecs.filter_builder()
        .term()
        .term()
        .build();
}

void FilterBuilder_operator_shortcuts() {
    flecs::world ecs;

    flecs::entity a = ecs.entity();
    flecs::entity b = ecs.entity();
    flecs::entity c = ecs.entity();
    flecs::entity d = ecs.entity();
    flecs::entity e = ecs.entity();
    flecs::entity f = ecs.entity();
    flecs::entity g = ecs.entity();
    flecs::entity h = ecs.entity();

    auto filter = ecs.filter_builder()
        .term(a).and_()
        .term(b).or_()
        .term(c).or_()
        .term(d).not_()
        .term(e).optional()
        .term(f).and_from()
        .term(g).or_from()
        .term(h).not_from()
        .build();

    auto t = filter.term(0);
    test_int(t.id(), a);
    test_int(t.oper(), flecs::And);

    t = filter.term(1);
    test_int(t.id(), b);
    test_int(t.oper(), flecs::Or);

    t = filter.term(2);
    test_int(t.id(), c);
    test_int(t.oper(), flecs::Or);

    t = filter.term(3);
    test_int(t.id(), d);
    test_int(t.oper(), flecs::Not);

    t = filter.term(4);
    test_int(t.id(), e);
    test_int(t.oper(), flecs::Optional);

    t = filter.term(5);
    test_int(t.id(), f);
    test_int(t.oper(), flecs::AndFrom);

    t = filter.term(6);
    test_int(t.id(), g);
    test_int(t.oper(), flecs::OrFrom);

    t = filter.term(7);
    test_int(t.id(), h);
    test_int(t.oper(), flecs::NotFrom);
}

void FilterBuilder_inout_shortcuts() {
    flecs::world ecs;

    flecs::entity a = ecs.entity();
    flecs::entity b = ecs.entity();
    flecs::entity c = ecs.entity();
    flecs::entity d = ecs.entity();

    auto filter = ecs.filter_builder()
        .term(a).in()
        .term(b).out()
        .term(c).inout()
        .term(d).inout_none()
        .build();

    auto t = filter.term(0);
    test_int(t.id(), a);
    test_int(t.inout(), flecs::In);

    t = filter.term(1);
    test_int(t.id(), b);
    test_int(t.inout(), flecs::Out);

    t = filter.term(2);
    test_int(t.id(), c);
    test_int(t.inout(), flecs::InOut);

    t = filter.term(3);
    test_int(t.id(), d);
    test_int(t.inout(), flecs::InOutNone);
}

void FilterBuilder_iter_column_w_const_as_array() {
    flecs::world world;

    auto f = world.filter<Position>();

    auto e1 = world.entity().set<Position>({10, 20});
    auto e2 = world.entity().set<Position>({20, 30});

    int32_t count = 0;
    f.iter([&](flecs::iter& it) {
        const auto p = it.field<Position>(1);
        for (auto i : it) {
            p[i].x += 1;
            p[i].y += 2;

            count ++;
        }
    });

    test_int(count, 2);

    const Position *p = e1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.get<Position>();
    test_int(p->x, 21);
    test_int(p->y, 32);
}

void FilterBuilder_iter_column_w_const_as_ptr() {
    flecs::world world;

    auto f = world.filter<Position>();
    
    auto base = world.prefab().set<Position>({10, 20});
    world.entity().is_a(base);
    world.entity().is_a(base);

    int32_t count = 0;
    f.iter([&](flecs::iter& it) {
        const auto p = it.field<Position>(1);
        for (size_t i = 0; i < it.count(); i ++) {
            test_int(p->x, 10);
            test_int(p->y, 20);
            count ++;
        }
    });

    test_int(count, 2);
}

void FilterBuilder_iter_column_w_const_deref() {
    flecs::world world;

    auto f = world.filter<Position>();
    
    auto base = world.prefab().set<Position>({10, 20});
    world.entity().is_a(base);
    world.entity().is_a(base);

    int32_t count = 0;
    f.iter([&](flecs::iter& it) {
        const auto p = it.field<Position>(1);
        Position pv = *p;
        for (size_t i = 0; i < it.count(); i ++) {
            test_int(pv.x, 10);
            test_int(pv.y, 20);
            count ++;
        }
    });

    test_int(count, 2);
}

void FilterBuilder_with_id() {
    flecs::world ecs;

    flecs::filter<> q = 
        ecs.filter_builder()
            .with(ecs.id<Position>())
            .with(ecs.id<Velocity>())
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

void FilterBuilder_with_name() {
    flecs::world ecs;

    ecs.component<Velocity>();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .with("Velocity")
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

void FilterBuilder_with_component() {
    flecs::world ecs;

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .with<Velocity>()
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

void FilterBuilder_with_pair_id() {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity();
    flecs::entity Apples = ecs.entity();
    flecs::entity Pears = ecs.entity();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .with(Likes, Apples)
            .build();

    auto e1 = ecs.entity().add<Position>().add(Likes, Apples);
    ecs.entity().add<Position>().add(Likes, Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_with_pair_name() {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity("Likes");
    flecs::entity Apples = ecs.entity("Apples");
    flecs::entity Pears = ecs.entity("Pears");

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .with("Likes", "Apples")
            .build();

    auto e1 = ecs.entity().add<Position>().add(Likes, Apples);
    ecs.entity().add<Position>().add(Likes, Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_with_pair_components() {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };
    struct Pears { };

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .with<Likes, Apples>()
            .build();

    auto e1 = ecs.entity().add<Position>().add<Likes, Apples>();
    ecs.entity().add<Position>().add<Likes, Pears>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_with_pair_component_id() {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity();
    flecs::entity Pears = ecs.entity();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .with<Likes>(Apples)
            .build();

    auto e1 = ecs.entity().add<Position>().add<Likes>(Apples);
    ecs.entity().add<Position>().add<Likes>(Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_with_pair_component_name() {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity("Apples");
    flecs::entity Pears = ecs.entity("Pears");

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .with<Likes>("Apples")
            .build();

    auto e1 = ecs.entity().add<Position>().add<Likes>(Apples);
    ecs.entity().add<Position>().add<Likes>(Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_with_enum() {
    flecs::world ecs;

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .with(Green)
            .build();

    auto e1 = ecs.entity().add<Position>().add(Green);
    ecs.entity().add<Position>().add(Red);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void FilterBuilder_without_id() {
    flecs::world ecs;

    flecs::filter<> q = 
        ecs.filter_builder()
            .with(ecs.id<Position>())
            .without(ecs.id<Velocity>())
            .build();

    ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e2);
    });
    
    test_int(count, 1);
}

void FilterBuilder_without_name() {
    flecs::world ecs;

    ecs.component<Velocity>();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with(ecs.id<Position>())
            .without("Velocity")
            .build();

    ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e2);
    });
    
    test_int(count, 1);
}

void FilterBuilder_without_component() {
    flecs::world ecs;

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .without<Velocity>()
            .build();

    ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e2);
    });
    
    test_int(count, 1);
}

void FilterBuilder_without_pair_id() {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity();
    flecs::entity Apples = ecs.entity();
    flecs::entity Pears = ecs.entity();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .without(Likes, Apples)
            .build();

    ecs.entity().add<Position>().add(Likes, Apples);
    auto e2 = ecs.entity().add<Position>().add(Likes, Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e2);
    });
    
    test_int(count, 1);
}

void FilterBuilder_without_pair_name() {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity("Likes");
    flecs::entity Apples = ecs.entity("Apples");
    flecs::entity Pears = ecs.entity("Pears");

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .without("Likes", "Apples")
            .build();

    ecs.entity().add<Position>().add(Likes, Apples);
    auto e2 = ecs.entity().add<Position>().add(Likes, Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e2);
    });
    
    test_int(count, 1);
}

void FilterBuilder_without_pair_components() {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };
    struct Pears { };

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .without<Likes, Apples>()
            .build();

    ecs.entity().add<Position>().add<Likes, Apples>();
    auto e2 = ecs.entity().add<Position>().add<Likes, Pears>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e2);
    });
    
    test_int(count, 1);
}

void FilterBuilder_without_pair_component_id() {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity();
    flecs::entity Pears = ecs.entity();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .without<Likes>(Apples)
            .build();

    ecs.entity().add<Position>().add<Likes>(Apples);
    auto e2 = ecs.entity().add<Position>().add<Likes>(Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e2);
    });
    
    test_int(count, 1);
}

void FilterBuilder_without_pair_component_name() {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity("Apples");
    flecs::entity Pears = ecs.entity("Pears");

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .without<Likes>("Apples")
            .build();

    ecs.entity().add<Position>().add<Likes>(Apples);
    auto e2 = ecs.entity().add<Position>().add<Likes>(Pears);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e2);
    });
    
    test_int(count, 1);
}

void FilterBuilder_without_enum() {
    flecs::world ecs;

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .without(Green)
            .build();

    ecs.entity().add<Position>().add(Green);
    auto e2 = ecs.entity().add<Position>().add(Red);

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e2);
    });
    
    test_int(count, 1);
}

void FilterBuilder_write_id() {
    flecs::world ecs;

    auto q = 
        ecs.filter_builder()
            .with<Position>()
            .write(ecs.id<Position>())
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_write_name() {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = 
        ecs.filter_builder()
            .with<Position>()
            .write("Position")
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_write_component() {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = 
        ecs.filter_builder()
            .with<Position>()
            .write<Position>()
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_write_pair_id() {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity();
    flecs::entity Apples = ecs.entity();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .write(Likes, Apples)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == Likes);
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_write_pair_name() {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity("Likes");
    flecs::entity Apples = ecs.entity("Apples");

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .write("Likes", "Apples")
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == Likes);
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_write_pair_components() {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .write<Likes, Apples>()
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == ecs.id<Apples>());
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_write_pair_component_id() {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .write<Likes>(Apples)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_write_pair_component_name() {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity("Apples");

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .write<Likes>("Apples")
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_write_enum() {
    flecs::world ecs;

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .write(Green)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Color>());
    test_assert(q.term(1).get_second() == ecs.to_entity<Color>(Green));
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_read_id() {
    flecs::world ecs;

    auto q = 
        ecs.filter_builder()
            .with<Position>()
            .read(ecs.id<Position>())
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_read_name() {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = 
        ecs.filter_builder()
            .with<Position>()
            .read("Position")
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_read_component() {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = 
        ecs.filter_builder()
            .with<Position>()
            .read<Position>()
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_read_pair_id() {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity();
    flecs::entity Apples = ecs.entity();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .read(Likes, Apples)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == Likes);
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_read_pair_name() {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity("Likes");
    flecs::entity Apples = ecs.entity("Apples");

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .read("Likes", "Apples")
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == Likes);
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_read_pair_components() {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .read<Likes, Apples>()
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == ecs.id<Apples>());
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_read_pair_component_id() {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity();

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .read<Likes>(Apples)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_read_pair_component_name() {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity("Apples");

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .read<Likes>("Apples")
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == Apples);

    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_read_enum() {
    flecs::world ecs;

    flecs::filter<> q = 
        ecs.filter_builder()
            .with<Position>()
            .read(Green)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Color>());
    test_assert(q.term(1).get_second() == ecs.to_entity<Color>(Green));
    test_assert(q.term(1).get_src() == 0);
}

void FilterBuilder_assign_after_init() {
    flecs::world ecs;

    flecs::filter<> f;
    flecs::filter_builder<> fb = ecs.filter_builder();
    fb.with<Position>();
    f = fb.build();

    flecs::entity e1 = ecs.entity().set<Position>({10, 20});

    int32_t count = 0;
    f.each([&](flecs::entity e) {
        test_assert(e == e1);
        count ++;
    });

    test_int(count, 1);
}
