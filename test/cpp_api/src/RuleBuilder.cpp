#include <cpp_api.h>

void RuleBuilder_1_type() {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set<Position>({10, 20});

    ecs.entity().set<Velocity>({10, 20});

    auto r = ecs.rule<Position>();

    int count = 0;
    r.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
        test_int(p.x, 10);
        test_int(p.y, 20);
    });

    test_int(count, 1);

    r.destruct();
}

void RuleBuilder_2_types() {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity().set<Velocity>({10, 20});

    auto r = ecs.rule<Position, const Velocity>();

    int count = 0;
    r.each([&](flecs::entity e, Position& p, const Velocity& v) {
        count ++;
        test_assert(e == e1);
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);
    });

    test_int(count, 1);

    r.destruct();
}

void RuleBuilder_id_term() {
    flecs::world ecs;

    auto Tag = ecs.entity();

    auto e1 = ecs.entity()
        .add(Tag);

    ecs.entity().set<Velocity>({10, 20});

    auto r = ecs.rule_builder()
        .term(Tag)
        .build();

    int count = 0;
    r.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });

    test_int(count, 1);

    r.destruct();
}

void RuleBuilder_type_term() {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set<Position>({10, 20});

    ecs.entity().set<Velocity>({10, 20});

    auto r = ecs.rule_builder()
        .term<Position>()
        .build();

    int count = 0;
    r.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });

    test_int(count, 1);

    r.destruct();
}

void RuleBuilder_id_pair_term() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto e1 = ecs.entity()
        .add(Likes, Apples);

    ecs.entity()
        .add(Likes, Pears);

    auto r = ecs.rule_builder()
        .term(Likes, Apples)
        .build();

    int count = 0;
    r.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });

    test_int(count, 1);

    r.destruct();
}

void RuleBuilder_id_pair_wildcard_term() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto e1 = ecs.entity()
        .add(Likes, Apples);

    auto e2 = ecs.entity()
        .add(Likes, Pears);

    auto r = ecs.rule_builder()
        .term(Likes, flecs::Wildcard)
        .build();

    int count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == e1) {
            test_assert(it.id(1) == ecs.pair(Likes, Apples));
            count ++;
        }
        if (it.entity(index) == e2) {
            test_assert(it.id(1) == ecs.pair(Likes, Pears));
            count ++;
        }
    });
    test_int(count, 2);

    r.destruct();
}

void RuleBuilder_type_pair_term() {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };
    struct Pears { };

    auto e1 = ecs.entity()
        .add<Likes, Apples>();

    auto e2 = ecs.entity()
        .add<Likes, Pears>();

    auto r = ecs.rule_builder()
        .term<Likes>(flecs::Wildcard)
        .build();

    int count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == e1) {
            test_assert((it.id(1) == ecs.pair<Likes, Apples>()));
            count ++;
        }
        if (it.entity(index) == e2) {
            test_assert((it.id(1) == ecs.pair<Likes, Pears>()));
            count ++;
        }
    });
    test_int(count, 2);

    r.destruct();
}

void RuleBuilder_pair_term_w_var() {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };
    struct Pears { };

    auto e1 = ecs.entity()
        .add<Likes, Apples>();

    auto e2 = ecs.entity()
        .add<Likes, Pears>();

    auto r = ecs.rule_builder()
        .term<Likes>().obj().var("Food")
        .build();

    int food_var = r.find_var("Food");

    int count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == e1) {
            test_assert((it.id(1) == ecs.pair<Likes, Apples>()));
            test_assert(it.get_var("Food") == ecs.id<Apples>());
            test_assert(it.get_var(food_var) == ecs.id<Apples>());
            count ++;
        }
        if (it.entity(index) == e2) {
            test_assert((it.id(1) == ecs.pair<Likes, Pears>()));
            test_assert(it.get_var("Food") == ecs.id<Pears>());
            test_assert(it.get_var(food_var) == ecs.id<Pears>());
            count ++;
        }
    });
    test_int(count, 2);

    r.destruct();
}

void RuleBuilder_2_pair_terms_w_var() {
    flecs::world ecs;

    struct Likes { };
    struct Eats { };
    struct Apples { };
    struct Pears { };

    auto Bob = ecs.entity()
        .add<Eats, Apples>();

    auto Alice = ecs.entity()
        .add<Eats, Pears>()
        .add<Likes>(Bob);

    Bob.add<Likes>(Alice);

    auto r = ecs.rule_builder()
        .term<Eats>().obj().var("Food")
        .term<Likes>().obj().var("Person")
        .build();

    int food_var = r.find_var("Food");
    int person_var = r.find_var("Person");

    int count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == Bob) {
            test_assert((it.id(1) == ecs.pair<Eats, Apples>()));
            test_assert(it.get_var("Food") == ecs.id<Apples>());
            test_assert(it.get_var(food_var) == ecs.id<Apples>());

            test_assert((it.id(2) == ecs.pair<Likes>(Alice)));
            test_assert(it.get_var("Person") == Alice);
            test_assert(it.get_var(person_var) == Alice);
            count ++;
        }
        if (it.entity(index) == Alice) {
            test_assert((it.id(1) == ecs.pair<Eats, Pears>()));
            test_assert(it.get_var("Food") == ecs.id<Pears>());
            test_assert(it.get_var(food_var) == ecs.id<Pears>());

            test_assert((it.id(2) == ecs.pair<Likes>(Bob)));
            test_assert(it.get_var("Person") == Bob);
            test_assert(it.get_var(person_var) == Bob);
            count ++;
        }
    });
    test_int(count, 2);

    r.destruct();
}

void RuleBuilder_set_var() {
    flecs::world ecs;

    struct Likes { };
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    ecs.entity()
        .add<Likes>(Apples);

    auto e2 = ecs.entity()
        .add<Likes>(Pears);

    auto r = ecs.rule_builder()
        .term<Likes>().obj().var("Food")
        .build();

    int food_var = r.find_var("Food");

    int count = 0;
    r.iter()
        .set_var(food_var, Pears)
        .each([&](flecs::iter& it, size_t index) {
            test_assert(it.entity(index) == e2);
            test_assert((it.id(1) == ecs.pair<Likes>(Pears)));
            test_assert(it.get_var("Food") == Pears);
            test_assert(it.get_var(food_var) == Pears);
            count ++;
        });

    test_int(count, 1);

    r.destruct();
}

void RuleBuilder_set_2_vars() {
    flecs::world ecs;

    struct Likes { };
    struct Eats { };
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto Bob = ecs.entity()
        .add<Eats>(Apples);

    auto Alice = ecs.entity()
        .add<Eats>(Pears)
        .add<Likes>(Bob);

    Bob.add<Likes>(Alice);

    auto r = ecs.rule_builder()
        .term<Eats>().obj().var("Food")
        .term<Likes>().obj().var("Person")
        .build();

    int food_var = r.find_var("Food");
    int person_var = r.find_var("Person");

    int count = 0;
    r.iter()
        .set_var(food_var, Pears)
        .set_var(person_var, Bob)
        .each([&](flecs::iter& it, size_t index) {
            test_assert(it.entity(index) == Alice);
            test_assert((it.id(1) == ecs.pair<Eats>(Pears)));
            test_assert((it.id(2) == ecs.pair<Likes>(Bob)));
            test_assert(it.get_var("Food") == Pears);
            test_assert(it.get_var(food_var) == Pears);
            test_assert(it.get_var("Person") == Bob);
            test_assert(it.get_var(person_var) == Bob);
            count ++;
        });
    test_int(count, 1);

    r.destruct();
}

void RuleBuilder_set_var_by_name() {
    flecs::world ecs;

    struct Likes { };
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    ecs.entity()
        .add<Likes>(Apples);

    auto e2 = ecs.entity()
        .add<Likes>(Pears);

    auto r = ecs.rule_builder()
        .term<Likes>().obj().var("Food")
        .build();

    int count = 0;
    r.iter()
        .set_var("Food", Pears)
        .each([&](flecs::iter& it, size_t index) {
            test_assert(it.entity(index) == e2);
            test_assert((it.id(1) == ecs.pair<Likes>(Pears)));
            count ++;
        });
    test_int(count, 1);

    r.destruct();
}

void RuleBuilder_set_2_vars_by_name() {
    flecs::world ecs;

    struct Likes { };
    struct Eats { };
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto Bob = ecs.entity()
        .add<Eats>(Apples);

    auto Alice = ecs.entity()
        .add<Eats>(Pears)
        .add<Likes>(Bob);

    Bob.add<Likes>(Alice);

    auto r = ecs.rule_builder()
        .term<Eats>().obj().var("Food")
        .term<Likes>().obj().var("Person")
        .build();

    int food_var = r.find_var("Food");
    int person_var = r.find_var("Person");

    int count = 0;
    r.iter()
        .set_var("Food", Pears)
        .set_var("Person", Bob)
        .each([&](flecs::iter& it, size_t index) {
            test_assert(it.entity(index) == Alice);
            test_assert((it.id(1) == ecs.pair<Eats>(Pears)));
            test_assert((it.id(2) == ecs.pair<Likes>(Bob)));
            test_assert(it.get_var("Food") == Pears);
            test_assert(it.get_var(food_var) == Pears);
            test_assert(it.get_var("Person") == Bob);
            test_assert(it.get_var(person_var) == Bob);
            count ++;
        });
    test_int(count, 1);

    r.destruct();
}

void RuleBuilder_expr_w_var() {
    flecs::world ecs;

    auto rel = ecs.entity("Rel");
    auto obj = ecs.entity();
    auto e = ecs.entity().add(rel, obj);

    auto r = ecs.rule_builder()
        .term("(Rel, $X)")
        .build();

    int x_var = r.find_var("X");
    test_assert(x_var != -1);

    int32_t count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        test_assert(it.entity(index) == e);
        test_assert(it.pair(1).second() == obj);
        count ++;
    });

    test_int(count, 1);

    r.destruct();
}
