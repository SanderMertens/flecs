#include <cpp.h>

static flecs::query_cache_kind_t cache_kind = flecs::QueryCacheDefault;

void QueryBuilder_setup(void) {
    const char *cache_param = test_param("cache_kind");
    if (cache_param) {
        if (!strcmp(cache_param, "default")) {
            // already set to default
        } else if (!strcmp(cache_param, "auto")) {
            cache_kind = flecs::QueryCacheAuto;
        } else {
            printf("unexpected value for cache_param '%s'\n", cache_param);
        }
    }
}

struct Other {
    int32_t value;
};

enum Color {
    Red, Green, Blue
};

void QueryBuilder_builder_assign_same_type(void) {
    flecs::world ecs;

    flecs::query<Position, Velocity> q = 
        ecs.query_builder<Position, Velocity>()
            .cache_kind(cache_kind)
            .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_assign_to_empty(void) {
    flecs::world ecs;

    flecs::query<> q = ecs.query_builder<Position, Velocity>()
        .cache_kind(cache_kind)
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

void QueryBuilder_builder_assign_from_empty(void) {
    flecs::world ecs;

    flecs::query<> q = ecs.query_builder<>()
        .cache_kind(cache_kind)
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

void QueryBuilder_builder_build(void) {
    flecs::world ecs;

    flecs::query<Position, Velocity> q = 
        ecs.query_builder<Position, Velocity>()
            .cache_kind(cache_kind)
            .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_build_to_auto(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Position, Velocity>()
        .cache_kind(cache_kind)
        .build();

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p, Velocity& v) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_builder_build_n_statements(void) {
    flecs::world ecs;

    auto qb = ecs.query_builder<>();
    qb.with<Position>();
    qb.with<Velocity>();
    qb.cache_kind(cache_kind);
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

void QueryBuilder_1_type(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .cache_kind(cache_kind)
        .build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_2_types(void) {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity().set<Velocity>({10, 20});

    auto r = ecs.query_builder<Position, const Velocity>()
        .cache_kind(cache_kind)
        .build();

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

}

void QueryBuilder_id_term(void) {
    flecs::world ecs;

    auto Tag = ecs.entity();

    auto e1 = ecs.entity()
        .add(Tag);

    ecs.entity().set<Velocity>({10, 20});

    auto r = ecs.query_builder()
        .with(Tag)
        .cache_kind(cache_kind)
        .build();

    int count = 0;
    r.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });

    test_int(count, 1);

}

void QueryBuilder_type_term(void) {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set<Position>({10, 20});

    ecs.entity().set<Velocity>({10, 20});

    auto r = ecs.query_builder()
        .with<Position>()
        .cache_kind(cache_kind)
        .build();

    int count = 0;
    r.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });

    test_int(count, 1);

}

void QueryBuilder_id_pair_term(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto e1 = ecs.entity()
        .add(Likes, Apples);

    ecs.entity()
        .add(Likes, Pears);

    auto r = ecs.query_builder()
        .with(Likes, Apples)
        .cache_kind(cache_kind)
        .build();

    int count = 0;
    r.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });

    test_int(count, 1);

}

void QueryBuilder_id_pair_wildcard_term(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto e1 = ecs.entity()
        .add(Likes, Apples);

    auto e2 = ecs.entity()
        .add(Likes, Pears);

    auto r = ecs.query_builder()
        .with(Likes, flecs::Wildcard)
        .cache_kind(cache_kind)
        .build();

    int count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == e1) {
            test_assert(it.id(0) == ecs.pair(Likes, Apples));
            count ++;
        }
        if (it.entity(index) == e2) {
            test_assert(it.id(0) == ecs.pair(Likes, Pears));
            count ++;
        }
    });
    test_int(count, 2);

}

void QueryBuilder_type_pair_term(void) {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };
    struct Pears { };

    auto e1 = ecs.entity()
        .add<Likes, Apples>();

    auto e2 = ecs.entity()
        .add<Likes, Pears>();

    auto r = ecs.query_builder()
        .with<Likes>(flecs::Wildcard)
        .cache_kind(cache_kind)
        .build();

    int count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == e1) {
            test_assert((it.id(0) == ecs.pair<Likes, Apples>()));
            count ++;
        }
        if (it.entity(index) == e2) {
            test_assert((it.id(0) == ecs.pair<Likes, Pears>()));
            count ++;
        }
    });
    test_int(count, 2);

}

void QueryBuilder_pair_term_w_var(void) {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };
    struct Pears { };

    auto e1 = ecs.entity()
        .add<Likes, Apples>();

    auto e2 = ecs.entity()
        .add<Likes, Pears>();

    auto r = ecs.query_builder()
        .with<Likes>().second().var("Food")
        .cache_kind(cache_kind)
        .build();

    int food_var = r.find_var("Food");

    int count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == e1) {
            test_assert((it.id(0) == ecs.pair<Likes, Apples>()));
            test_assert(it.get_var("Food") == ecs.id<Apples>());
            test_assert(it.get_var(food_var) == ecs.id<Apples>());
            count ++;
        }
        if (it.entity(index) == e2) {
            test_assert((it.id(0) == ecs.pair<Likes, Pears>()));
            test_assert(it.get_var("Food") == ecs.id<Pears>());
            test_assert(it.get_var(food_var) == ecs.id<Pears>());
            count ++;
        }
    });
    test_int(count, 2);

}

void QueryBuilder_2_pair_terms_w_var(void) {
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

    auto r = ecs.query_builder()
        .with<Eats>().second().var("Food")
        .with<Likes>().second().var("Person")
        .cache_kind(cache_kind)
        .build();

    int food_var = r.find_var("Food");
    int person_var = r.find_var("Person");

    int count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == Bob) {
            test_assert((it.id(0) == ecs.pair<Eats, Apples>()));
            test_assert(it.get_var("Food") == ecs.id<Apples>());
            test_assert(it.get_var(food_var) == ecs.id<Apples>());

            test_assert((it.id(1) == ecs.pair<Likes>(Alice)));
            test_assert(it.get_var("Person") == Alice);
            test_assert(it.get_var(person_var) == Alice);
            count ++;
        }
        if (it.entity(index) == Alice) {
            test_assert((it.id(0) == ecs.pair<Eats, Pears>()));
            test_assert(it.get_var("Food") == ecs.id<Pears>());
            test_assert(it.get_var(food_var) == ecs.id<Pears>());

            test_assert((it.id(1) == ecs.pair<Likes>(Bob)));
            test_assert(it.get_var("Person") == Bob);
            test_assert(it.get_var(person_var) == Bob);
            count ++;
        }
    });
    test_int(count, 2);

}

void QueryBuilder_set_var(void) {
    flecs::world ecs;

    struct Likes { };
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    ecs.entity()
        .add<Likes>(Apples);

    auto e2 = ecs.entity()
        .add<Likes>(Pears);

    auto r = ecs.query_builder()
        .with<Likes>().second().var("Food")
        .cache_kind(cache_kind)
        .build();

    int food_var = r.find_var("Food");

    int count = 0;
    r.iter()
        .set_var(food_var, Pears)
        .each([&](flecs::iter& it, size_t index) {
            test_assert(it.entity(index) == e2);
            test_assert((it.id(0) == ecs.pair<Likes>(Pears)));
            test_assert(it.get_var("Food") == Pears);
            test_assert(it.get_var(food_var) == Pears);
            count ++;
        });

    test_int(count, 1);
}

void QueryBuilder_set_2_vars(void) {
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

    auto r = ecs.query_builder()
        .with<Eats>().second().var("Food")
        .with<Likes>().second().var("Person")
        .cache_kind(cache_kind)
        .build();

    int food_var = r.find_var("Food");
    int person_var = r.find_var("Person");

    int count = 0;
    r.iter()
        .set_var(food_var, Pears)
        .set_var(person_var, Bob)
        .each([&](flecs::iter& it, size_t index) {
            test_assert(it.entity(index) == Alice);
            test_assert((it.id(0) == ecs.pair<Eats>(Pears)));
            test_assert((it.id(1) == ecs.pair<Likes>(Bob)));
            test_assert(it.get_var("Food") == Pears);
            test_assert(it.get_var(food_var) == Pears);
            test_assert(it.get_var("Person") == Bob);
            test_assert(it.get_var(person_var) == Bob);
            count ++;
        });
    test_int(count, 1);

}

void QueryBuilder_set_var_by_name(void) {
    flecs::world ecs;

    struct Likes { };
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    ecs.entity()
        .add<Likes>(Apples);

    auto e2 = ecs.entity()
        .add<Likes>(Pears);

    auto r = ecs.query_builder()
        .with<Likes>().second().var("Food")
        .cache_kind(cache_kind)
        .build();

    int count = 0;
    r.iter()
        .set_var("Food", Pears)
        .each([&](flecs::iter& it, size_t index) {
            test_assert(it.entity(index) == e2);
            test_assert((it.id(0) == ecs.pair<Likes>(Pears)));
            count ++;
        });
    test_int(count, 1);

}

void QueryBuilder_set_2_vars_by_name(void) {
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

    auto r = ecs.query_builder()
        .with<Eats>().second().var("Food")
        .with<Likes>().second().var("Person")
        .cache_kind(cache_kind)
        .build();

    int food_var = r.find_var("Food");
    int person_var = r.find_var("Person");

    int count = 0;
    r.iter()
        .set_var("Food", Pears)
        .set_var("Person", Bob)
        .each([&](flecs::iter& it, size_t index) {
            test_assert(it.entity(index) == Alice);
            test_assert((it.id(0) == ecs.pair<Eats>(Pears)));
            test_assert((it.id(1) == ecs.pair<Likes>(Bob)));
            test_assert(it.get_var("Food") == Pears);
            test_assert(it.get_var(food_var) == Pears);
            test_assert(it.get_var("Person") == Bob);
            test_assert(it.get_var(person_var) == Bob);
            count ++;
        });
    test_int(count, 1);

}

void QueryBuilder_set_var_on_query(void) {
    flecs::world ecs;

    struct Likes { };
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    ecs.entity()
        .add<Likes>(Apples);

    auto e2 = ecs.entity()
        .add<Likes>(Pears);

    auto r = ecs.query_builder()
        .with<Likes>().second().var("Food")
        .cache_kind(cache_kind)
        .build();

    int food_var = r.find_var("Food");

    int count = 0;
    r.set_var(food_var, Pears)
     .each([&](flecs::iter& it, size_t index) {
         test_assert(it.entity(index) == e2);
         test_assert((it.id(0) == ecs.pair<Likes>(Pears)));
         test_assert(it.get_var("Food") == Pears);
         test_assert(it.get_var(food_var) == Pears);
         count ++;
     });

    test_int(count, 1);
}

void QueryBuilder_set_var_by_name_on_query(void) {
    flecs::world ecs;

    struct Likes { };
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    ecs.entity()
        .add<Likes>(Apples);

    auto e2 = ecs.entity()
        .add<Likes>(Pears);

    auto r = ecs.query_builder()
        .with<Likes>().second().var("Food")
        .cache_kind(cache_kind)
        .build();

    int count = 0;
    r.set_var("Food", Pears)
     .each([&](flecs::iter& it, size_t index) {
         test_assert(it.entity(index) == e2);
         test_assert((it.id(0) == ecs.pair<Likes>(Pears)));
         test_assert(it.get_var("Food") == Pears);
         count ++;
     });

    test_int(count, 1);
}

void QueryBuilder_set_table_var(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();
    auto e3 = ecs.entity().add<Position>().add<Velocity>();

    auto r = ecs.query_builder<Position>()
        .cache_kind(cache_kind)
        .build();

    int count = 0;
    r.set_var("this", e1.table())
     .each([&](flecs::iter& it, size_t index, Position&) {
        if (index == 0) {
            test_assert(it.entity(index) == e1);
        } else if (index == 1) {
            test_assert(it.entity(index) == e2);
        }
        count ++;
     });

    test_int(count, 2);

    r.set_var("this", e3.table())
     .each([&](flecs::iter& it, size_t index, Position&) {
        test_assert(it.entity(index) == e3);
        count ++;
     });

    test_int(count, 3);
}

void QueryBuilder_set_range_var(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();
    auto e3 = ecs.entity().add<Position>().add<Velocity>();

    auto r = ecs.query_builder<Position>()
        .cache_kind(cache_kind)
        .build();

    int count = 0;

    r.set_var("this", e1.range())
     .each([&](flecs::iter& it, size_t index, Position&) {
        test_assert(it.entity(index) == e1);
        count ++;
     });

    test_int(count, 1);

    r.set_var("this", e2.range())
     .each([&](flecs::iter& it, size_t index, Position&) {
        test_assert(it.entity(index) == e2);
        count ++;
     });

    test_int(count, 2);

    r.set_var("this", e3.range())
     .each([&](flecs::iter& it, size_t index, Position&) {
        test_assert(it.entity(index) == e3);
        count ++;
     });

    test_int(count, 3);
}

void QueryBuilder_set_table_var_chained(void) {
    flecs::world ecs;

    /* auto e1 = */ ecs.entity().add<Position>();
    /* auto e2 = */ ecs.entity().add<Position>();
    auto e3 = ecs.entity().add<Position>().add<Velocity>();
    /* auto e4 = */ ecs.entity().add<Velocity>();

    auto q1 = ecs.query_builder<Position>()
        .cache_kind(cache_kind)
        .build();

    auto q2 = ecs.query_builder<Velocity>()
        .cache_kind(cache_kind)
        .build();

    int count = 0;

    q1.run([&](flecs::iter& it) {
        while (it.next()) {
            q2.set_var("this", it.table()).each([&](flecs::entity e, Velocity&) {
                test_assert(e == e3);
                count ++;
            });
        }
    });

    test_int(count, 1);
}

void QueryBuilder_set_range_var_chained(void) {
    flecs::world ecs;

    /* auto e1 = */ ecs.entity().add<Position>();
    /* auto e2 = */ ecs.entity().add<Position>();
    auto e3 = ecs.entity().add<Position>().add<Velocity>();
    /* auto e4 = */ ecs.entity().add<Velocity>();

    auto q1 = ecs.query_builder<Position>()
        .cache_kind(cache_kind)
        .build();

    auto q2 = ecs.query_builder<Velocity>()
        .cache_kind(cache_kind)
        .build();

    int count = 0;

    q1.run([&](flecs::iter& it) {
        while (it.next()) {
            q2.set_var("this", it.range()).each([&](flecs::entity e, Velocity&) {
                test_assert(e == e3);
                count ++;
            });
        }
    });

    test_int(count, 1);
}

void QueryBuilder_expr_w_var(void) {
    flecs::world ecs;

    auto rel = ecs.entity("Rel");
    auto obj = ecs.entity();
    auto e = ecs.entity().add(rel, obj);

    auto r = ecs.query_builder()
        .expr("(Rel, $X)")
        .cache_kind(cache_kind)
        .build();

    int x_var = r.find_var("X");
    test_assert(x_var != -1);

    int32_t count = 0;
    r.each([&](flecs::iter& it, size_t index) {
        test_assert(it.entity(index) == e);
        test_assert(it.pair(0).second() == obj);
        count ++;
    });

    test_int(count, 1);

}

void QueryBuilder_add_1_type(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .with<Position>()
        .cache_kind(cache_kind)
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

void QueryBuilder_add_2_types(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .with<Position>()
        .with<Velocity>()
        .cache_kind(cache_kind)
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

void QueryBuilder_add_1_type_w_1_type(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .with<Velocity>()
        .cache_kind(cache_kind)
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

void QueryBuilder_add_2_types_w_1_type(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .with<Velocity>()
        .with<Mass>()
        .cache_kind(cache_kind)
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

void QueryBuilder_add_pair(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<>()
        .with(Likes, Bob)
        .cache_kind(cache_kind)
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

void QueryBuilder_add_not(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .with<Velocity>().oper(flecs::Not)
        .cache_kind(cache_kind)
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

void QueryBuilder_add_or(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .with<Position>().oper(flecs::Or)
        .with<Velocity>()
        .cache_kind(cache_kind)
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

void QueryBuilder_add_optional(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .with<Position>()
        .with<Velocity>().oper(flecs::Optional)
        .cache_kind(cache_kind)
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

void QueryBuilder_ptr_type(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Position, Velocity*>()
        .cache_kind(cache_kind)
        .build();

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

void QueryBuilder_const_type(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<const Position>()
        .cache_kind(cache_kind)
        .build();

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;
    q.each([&](flecs::entity e, const Position& p) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_string_term(void) {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = ecs.query_builder<>()
        .expr("Position")
        .cache_kind(cache_kind)
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

void QueryBuilder_singleton_term(void) {
    flecs::world ecs;

    ecs.set<Other>({10});

    auto q = ecs.query_builder<Self>()
        .with<Other>().singleton().inout()
        .cache_kind(cache_kind)
        .build();

    auto 
    e = ecs.entity(); e.set<Self>({e});
    e = ecs.entity(); e.set<Self>({e});
    e = ecs.entity(); e.set<Self>({e});

    int32_t count = 0;

    q.run([&](flecs::iter& it) {
        while (it.next()) {
            auto s = it.field<Self>(0);
            auto o = it.field<const Other>(1);
            test_assert(!it.is_self(1));
            test_int(o->value, 10);
            
            const Other& o_ref = *o;
            test_int(o_ref.value, 10);

            for (auto i : it) {
                test_assert(it.entity(i) == s[i].value);
                count ++;
            }
        }
    });
    
    test_int(count, 3);
}

void QueryBuilder_isa_superset_term(void) {
    flecs::world ecs;

    ecs.component<Other>().add(flecs::OnInstantiate, flecs::Inherit);

    auto q = ecs.query_builder<Self>()
        .with<Other>().src().up(flecs::IsA).in()
        .cache_kind(cache_kind)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});
    e = ecs.entity().add(flecs::IsA, base); e.set<Self>({e});

    int32_t count = 0;

    q.run([&](flecs::iter& it) {
        while (it.next()) {
            auto s = it.field<Self>(0);
            auto o = it.field<const Other>(1);
            test_assert(!it.is_self(1));
            test_int(o->value, 10);

            for (auto i : it) {
                test_assert(it.entity(i) == s[i].value);
                count ++;
            }
        }
    });
    
    test_int(count, 3);
}

void QueryBuilder_isa_self_superset_term(void) {
    flecs::world ecs;

    ecs.component<Other>().add(flecs::OnInstantiate, flecs::Inherit);

    auto q = ecs.query_builder<Self>()
        .with<Other>().src().self().up(flecs::IsA).in()
        .cache_kind(cache_kind)
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

    q.run([&](flecs::iter& it) {
        while (it.next()) {
            auto s = it.field<Self>(0);
            auto o = it.field<const Other>(1);

            if (!it.is_self(1)) {
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
        }
    });
    
    test_int(count, 5);
    test_int(owned_count, 2);
}

void QueryBuilder_childof_superset_term(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Self>()
        .with<Other>().src().up().in()
        .cache_kind(cache_kind)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});
    e = ecs.entity().child_of(base); e.set<Self>({e});

    int32_t count = 0;

    q.run([&](flecs::iter& it) {
        while (it.next()) {
            auto s = it.field<Self>(0);
            auto o = it.field<const Other>(1);
            test_assert(!it.is_self(1));
            test_int(o->value, 10);

            for (auto i : it) {
                test_assert(it.entity(i) == s[i].value);
                count ++;
            }
        }
    });
    
    test_int(count, 3);
}

void QueryBuilder_childof_self_superset_term(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Self>()
        .with<Other>().src().self().up().in()
        .cache_kind(cache_kind)
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

    q.run([&](flecs::iter& it) {
        while (it.next()) {
            auto s = it.field<Self>(0);
            auto o = it.field<const Other>(1);

            if (!it.is_self(1)) {
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
        }
    });
    
    test_int(count, 5);
    test_int(owned_count, 2);
}

void QueryBuilder_isa_superset_term_w_each(void) {
    flecs::world ecs;

    ecs.component<Other>().add(flecs::OnInstantiate, flecs::Inherit);

    auto q = ecs.query_builder<Self, Other>()
        .term_at(1).src().up(flecs::IsA)
        .cache_kind(cache_kind)
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

void QueryBuilder_isa_self_superset_term_w_each(void) {
    flecs::world ecs;

    ecs.component<Other>().add(flecs::OnInstantiate, flecs::Inherit);

    auto q = ecs.query_builder<Self, Other>()
        .term_at(1).src().self().up(flecs::IsA)
        .cache_kind(cache_kind)
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

void QueryBuilder_childof_superset_term_w_each(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .term_at(1).src().up()
        .cache_kind(cache_kind)
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

void QueryBuilder_childof_self_superset_term_w_each(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .term_at(1).src().self().up()
        .cache_kind(cache_kind)
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

void QueryBuilder_isa_superset_shortcut(void) {
    flecs::world ecs;

    ecs.component<Other>().add(flecs::OnInstantiate, flecs::Inherit);

    auto q = ecs.query_builder<Self, Other>()
        .term_at(1).up(flecs::IsA)
        .cache_kind(cache_kind)
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

void QueryBuilder_isa_superset_shortcut_w_self(void) {
    flecs::world ecs;

    ecs.component<Other>().add(flecs::OnInstantiate, flecs::Inherit);

    auto q = ecs.query_builder<Self, Other>()
        .term_at(1).self().up(flecs::IsA)
        .cache_kind(cache_kind)
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

void QueryBuilder_childof_superset_shortcut(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .term_at(1).up()
        .cache_kind(cache_kind)
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

void QueryBuilder_childof_superset_shortcut_w_self(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Self, Other>()
        .term_at(1).self().up()
        .cache_kind(cache_kind)
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

void QueryBuilder_relation(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<Self>()
        .with(Likes, Bob)
        .cache_kind(cache_kind)
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

void QueryBuilder_relation_w_object_wildcard(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<Self>()
        .with(Likes, flecs::Wildcard)
        .cache_kind(cache_kind)
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

void QueryBuilder_relation_w_predicate_wildcard(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Dislikes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<Self>()
        .with(flecs::Wildcard, Alice)
        .cache_kind(cache_kind)
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

void QueryBuilder_add_pair_w_rel_type(void) {
    flecs::world ecs;

    struct Likes { };
    
    auto Dislikes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto q = ecs.query_builder<Self>()
        .with<Likes>(flecs::Wildcard)
        .cache_kind(cache_kind)
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

void QueryBuilder_template_term(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .with<Template<int>>()
        .cache_kind(cache_kind)
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

void QueryBuilder_explicit_subject_w_id(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .with<Position>().id(flecs::This)
        .cache_kind(cache_kind)
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

void QueryBuilder_explicit_subject_w_type(void) {
    flecs::world ecs;

    ecs.set<Position>({10, 20});

    auto q = ecs.query_builder<Position>()
        .with<Position>().src<Position>()
        .cache_kind(cache_kind)
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

void QueryBuilder_explicit_object_w_id(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Alice = ecs.entity();
    auto Bob = ecs.entity();

    auto q = ecs.query_builder<>()
        .with(Likes).second(Alice)
        .cache_kind(cache_kind)
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

void QueryBuilder_explicit_object_w_type(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    struct Alice { };
    auto Bob = ecs.entity();

    auto q = ecs.query_builder<>()
        .with(Likes).second<Alice>()
        .cache_kind(cache_kind)
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

void QueryBuilder_explicit_term(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .with(ecs.term(ecs.id<Position>()))
        .cache_kind(cache_kind)
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

void QueryBuilder_explicit_term_w_type(void) {
    flecs::world ecs;

    auto q = ecs.query_builder<>()
        .with(ecs.term<Position>())
        .cache_kind(cache_kind)
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

void QueryBuilder_explicit_term_w_pair_type(void) {
    flecs::world ecs;

    struct Likes { };
    struct Alice { };
    struct Bob { };

    auto q = ecs.query_builder<>()
        .with(ecs.term<Likes, Alice>())
        .cache_kind(cache_kind)
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

void QueryBuilder_explicit_term_w_id(void) {
    flecs::world ecs;

    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto q = ecs.query_builder<>()
        .with(ecs.term(Apples))
        .cache_kind(cache_kind)
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

void QueryBuilder_explicit_term_w_pair_id(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();
    auto Pears = ecs.entity();

    auto q = ecs.query_builder<>()
        .with(ecs.term(Likes, Apples))
        .cache_kind(cache_kind)
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

void QueryBuilder_1_term_to_empty(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();

    auto qb = ecs.query_builder<>()
        .with<Position>()
        .cache_kind(cache_kind);

    qb.with(Likes, Apples);

    auto q = qb.build();

    test_int(q.field_count(), 2);
    test_int(q.term(0).id(), ecs.id<Position>());
    test_int(q.term(1).id(), ecs.pair(Likes, Apples));
}

void QueryBuilder_2_subsequent_args(void) {
    flecs::world ecs;

    struct Rel { int foo; };

    int32_t count = 0;

    auto s = ecs.system<Rel, const Velocity>()
        .term_at(0).second(flecs::Wildcard)
        .term_at(1).singleton()
        .run([&](flecs::iter it){
            while (it.next()) {
                count += it.count();
            }
        });
    
    ecs.entity().add<Rel, Tag>();
    ecs.set<Velocity>({});

    s.run();

    test_int(count, 1);
}

void QueryBuilder_optional_tag_is_set(void) {
    flecs::world ecs;

    struct TagA { };
    struct TagB { };

    auto q = ecs.query_builder()
        .with<TagA>()
        .with<TagB>().oper(flecs::Optional)
        .cache_kind(cache_kind)
        .build();

    auto e_1 = ecs.entity().add<TagA>().add<TagB>();
    auto e_2 = ecs.entity().add<TagA>();

    int count = 0;

    q.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);

            count += it.count();
            
            if (it.entity(0) == e_1) {
                test_bool(it.is_set(0), true);
                test_bool(it.is_set(1), true);
            } else {
                test_assert(it.entity(0) == e_2);
                test_bool(it.is_set(0), true);
                test_bool(it.is_set(1), false);
            }
        }
    });

    test_int(count, 2);
}

void QueryBuilder_10_terms(void) {
    flecs::world ecs;

    auto f = ecs.query_builder<>()
        .with<Tag0>()
        .with<Tag1>()
        .with<Tag2>()
        .with<Tag3>()
        .with<Tag4>()
        .with<Tag5>()
        .with<Tag6>()
        .with<Tag7>()
        .with<Tag8>()
        .with<Tag9>()
        .cache_kind(cache_kind)
        .build();

    test_int(f.field_count(), 10);

    auto e = ecs.entity()
        .add<Tag0>()
        .add<Tag1>()
        .add<Tag2>()
        .add<Tag3>()
        .add<Tag4>()
        .add<Tag5>()
        .add<Tag6>()
        .add<Tag7>()
        .add<Tag8>()
        .add<Tag9>();

    int count = 0;
    f.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);
            test_assert(it.entity(0) == e);
            test_int(it.field_count(), 10);
            count ++;
        }
    });

    test_int(count, 1);
}

void QueryBuilder_16_terms(void) {
    flecs::world ecs;

    auto f = ecs.query_builder<>()
        .with<Tag0>()
        .with<Tag1>()
        .with<Tag2>()
        .with<Tag3>()
        .with<Tag4>()
        .with<Tag5>()
        .with<Tag6>()
        .with<Tag7>()
        .with<Tag8>()
        .with<Tag9>()
        .with<Tag10>()
        .with<Tag11>()
        .with<Tag12>()
        .with<Tag13>()
        .with<Tag14>()
        .with<Tag15>()
        .cache_kind(cache_kind)
        .build();

    test_int(f.field_count(), 16);

    auto e = ecs.entity()
        .add<Tag0>()
        .add<Tag1>()
        .add<Tag2>()
        .add<Tag3>()
        .add<Tag4>()
        .add<Tag5>()
        .add<Tag6>()
        .add<Tag7>()
        .add<Tag8>()
        .add<Tag9>()
        .add<Tag10>()
        .add<Tag11>()
        .add<Tag12>()
        .add<Tag13>()
        .add<Tag14>()
        .add<Tag15>();

    int count = 0;
    f.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);
            test_assert(it.entity(0) == e);
            test_int(it.field_count(), 16);
            count ++;
        }
    });

    test_int(count, 1);
}

void QueryBuilder_32_terms(void) {
    flecs::world ecs;

    auto f = ecs.query_builder<>()
        .with<Tag0>()
        .with<Tag1>()
        .with<Tag2>()
        .with<Tag3>()
        .with<Tag4>()
        .with<Tag5>()
        .with<Tag6>()
        .with<Tag7>()
        .with<Tag8>()
        .with<Tag9>()
        .with<Tag10>()
        .with<Tag11>()
        .with<Tag12>()
        .with<Tag13>()
        .with<Tag14>()
        .with<Tag15>()
        .with<Tag16>()
        .with<Tag17>()
        .with<Tag18>()
        .with<Tag19>()
        .with<Tag20>()
        .with<Tag21>()
        .with<Tag22>()
        .with<Tag23>()
        .with<Tag24>()
        .with<Tag25>()
        .with<Tag26>()
        .with<Tag27>()
        .with<Tag28>()
        .with<Tag29>()
        .with<Tag30>()
        .with<Tag31>()
        .cache_kind(cache_kind)
        .build();

    test_int(f.field_count(), 32);

    auto e = ecs.entity()
        .add<Tag0>()
        .add<Tag1>()
        .add<Tag2>()
        .add<Tag3>()
        .add<Tag4>()
        .add<Tag5>()
        .add<Tag6>()
        .add<Tag7>()
        .add<Tag8>()
        .add<Tag9>()
        .add<Tag10>()
        .add<Tag11>()
        .add<Tag12>()
        .add<Tag13>()
        .add<Tag14>()
        .add<Tag15>()
        .add<Tag16>()
        .add<Tag17>()
        .add<Tag18>()
        .add<Tag19>()
        .add<Tag20>()
        .add<Tag21>()
        .add<Tag22>()
        .add<Tag23>()
        .add<Tag24>()
        .add<Tag25>()
        .add<Tag26>()
        .add<Tag27>()
        .add<Tag28>()
        .add<Tag29>()
        .add<Tag30>()
        .add<Tag31>();

    int count = 0;
    f.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);
            test_assert(it.entity(0) == e);
            test_int(it.field_count(), 32);
            count ++;
        }
    });

    test_int(count, 1);
}

void QueryBuilder_33_terms(void) {
    install_test_abort();

    flecs::world ecs;

    test_expect_abort();
    auto f = ecs.query_builder<>()
        .with<Tag0>()
        .with<Tag1>()
        .with<Tag2>()
        .with<Tag3>()
        .with<Tag4>()
        .with<Tag5>()
        .with<Tag6>()
        .with<Tag7>()
        .with<Tag8>()
        .with<Tag9>()
        .with<Tag10>()
        .with<Tag11>()
        .with<Tag12>()
        .with<Tag13>()
        .with<Tag14>()
        .with<Tag15>()
        .with<Tag16>()
        .with<Tag17>()
        .with<Tag18>()
        .with<Tag19>()
        .with<Tag20>()
        .with<Tag21>()
        .with<Tag22>()
        .with<Tag23>()
        .with<Tag24>()
        .with<Tag25>()
        .with<Tag26>()
        .with<Tag27>()
        .with<Tag28>()
        .with<Tag29>()
        .with<Tag30>()
        .with<Tag31>()
        .with<Tag32>()
        .cache_kind(cache_kind)
        .build();
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

void QueryBuilder_group_by_raw(void) {
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
        .with<TagX>()
        .group_by(flecs::type_id<TagX>(), group_by_first_id)
        .build();

    auto q_reverse = ecs.query_builder()
        .with<TagX>()
        .group_by(flecs::type_id<TagX>(), group_by_first_id_negated)
        .build();

    auto e3 = ecs.entity().add<TagX>().add<TagC>();
    auto e2 = ecs.entity().add<TagX>().add<TagB>();
    auto e1 = ecs.entity().add<TagX>().add<TagA>();

    int count = 0;

    q.run([&](flecs::iter& it){
        while (it.next()) {
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
        }
    });
    test_int(count, 3);
    
    count = 0;
    q_reverse.run([&](flecs::iter& it){
        while (it.next()) {
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
        }
    });
    test_int(count, 3);
}

void QueryBuilder_group_by_template(void) {
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
        .with<TagX>()
        .group_by<TagX>(group_by_first_id)
        .build();

    auto q_reverse = ecs.query_builder()
        .with<TagX>()
        .group_by<TagX>( group_by_first_id_negated)
        .build();

    auto e3 = ecs.entity().add<TagX>().add<TagC>();
    auto e2 = ecs.entity().add<TagX>().add<TagB>();
    auto e1 = ecs.entity().add<TagX>().add<TagA>();

    int count = 0;

    q.run([&](flecs::iter& it){
        while (it.next()) {
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
        }
    });
    test_int(count, 3);
    
    count = 0;
    q_reverse.run([&](flecs::iter& it){
        while (it.next()) {
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
        }
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

void QueryBuilder_group_by_iter_one(void) {
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
        .with(Rel, flecs::Wildcard)
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

void QueryBuilder_group_by_iter_one_template(void) {
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
        .with<Rel>(flecs::Wildcard)
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

void QueryBuilder_group_by_iter_one_all_groups(void) {
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
        .with(Rel, flecs::Wildcard)
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

void QueryBuilder_group_by_default_func_w_id(void) {
    flecs::world ecs;

    auto Rel = ecs.entity();
    auto TgtA = ecs.entity();
    auto TgtB = ecs.entity();
    auto TgtC = ecs.entity();

    auto e1 = ecs.entity().add(Rel, TgtC);
    auto e2 = ecs.entity().add(Rel, TgtB);
    auto e3 = ecs.entity().add(Rel, TgtA);

    auto q = ecs.query_builder()
        .with(Rel, flecs::Wildcard)
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

void QueryBuilder_group_by_default_func_w_type(void) {
    flecs::world ecs;

    struct Rel { };
    auto TgtA = ecs.entity();
    auto TgtB = ecs.entity();
    auto TgtC = ecs.entity();

    auto e1 = ecs.entity().add<Rel>(TgtC);
    auto e2 = ecs.entity().add<Rel>(TgtB);
    auto e3 = ecs.entity().add<Rel>(TgtA);

    auto q = ecs.query_builder()
        .with<Rel>(flecs::Wildcard)
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

void QueryBuilder_group_by_callbacks(void) {
    flecs::world ecs;

    struct Rel { };
    auto TgtA = ecs.entity();
    auto TgtB = ecs.entity();
    auto TgtC = ecs.entity();

    auto e1 = ecs.entity().add<Rel>(TgtC);
    auto e2 = ecs.entity().add<Rel>(TgtB);
    auto e3 = ecs.entity().add<Rel>(TgtA);

    auto q = ecs.query_builder()
        .with<Rel>(flecs::Wildcard)
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

void QueryBuilder_set_group_on_query(void) {
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
        .with(Rel, flecs::Wildcard)
        .group_by(Rel, group_by_rel)
        .build();

    bool e2_found = false;
    bool e5_found = false;
    int32_t count = 0;

    q.set_group(TgtB).each([&](flecs::iter& it, size_t i) {
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

void QueryBuilder_set_group_type_on_query(void) {
    flecs::world ecs;

    struct Rel { };
    struct TgtA { };
    struct TgtB { };
    struct TgtC { };

    auto Tag = ecs.entity();

    ecs.entity().add<Rel, TgtA>();
    auto e2 = ecs.entity().add<Rel, TgtB>();
    ecs.entity().add<Rel, TgtC>();

    ecs.entity().add<Rel, TgtA>().add(Tag);
    auto e5 = ecs.entity().add<Rel, TgtB>().add(Tag);
    ecs.entity().add<Rel, TgtC>().add(Tag);

    auto q = ecs.query_builder()
        .with<Rel>(flecs::Wildcard)
        .group_by<Rel>(group_by_rel)
        .build();

    bool e2_found = false;
    bool e5_found = false;
    int32_t count = 0;

    q.set_group<TgtB>().each([&](flecs::iter& it, size_t i) {
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

void QueryBuilder_create_w_no_template_args(void) {
    flecs::world ecs;

    auto q = ecs.query_builder()
        .with<Position>()
        .cache_kind(cache_kind)
        .build();

    auto e1 = ecs.entity().add<Position>();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_any_wildcard(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apple = ecs.entity();
    auto Mango = ecs.entity();

    auto e1 = ecs.entity()
        .add(Likes, Apple)
        .add(Likes, Mango);

    auto q = ecs.query_builder()
        .with(Likes, flecs::Any)
        .cache_kind(cache_kind)
        .build();

    int32_t count = 0;
    q.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });
    
    test_int(count, 1);
}

void QueryBuilder_cascade(void) {
    flecs::world ecs;

    auto Tag = ecs.entity().add(flecs::OnInstantiate, flecs::Inherit);
    auto Foo = ecs.entity();
    auto Bar = ecs.entity();

    auto e0 = ecs.entity().add(Tag);
    auto e1 = ecs.entity().is_a(e0);
    auto e2 = ecs.entity().is_a(e1);
    auto e3 = ecs.entity().is_a(e2);

    auto q = ecs.query_builder()
        .with(Tag).cascade(flecs::IsA)
        .cached()
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

void QueryBuilder_cascade_desc(void) {
    flecs::world ecs;

    auto Tag = ecs.entity().add(flecs::OnInstantiate, flecs::Inherit);
    auto Foo = ecs.entity();
    auto Bar = ecs.entity();

    auto e0 = ecs.entity().add(Tag);
    auto e1 = ecs.entity().is_a(e0);
    auto e2 = ecs.entity().is_a(e1);
    auto e3 = ecs.entity().is_a(e2);

    auto q = ecs.query_builder()
        .with(Tag).cascade(flecs::IsA).desc()
        .cached()
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
            test_bool(e2_found, true);
            test_bool(e3_found, true);
            e1_found = true;
        }
        if (e == e2) {
            test_bool(e1_found, false);
            test_bool(e2_found, false);
            test_bool(e3_found, true);
            e2_found = true;
        }
        if (e == e3) {
            test_bool(e1_found, false);
            test_bool(e2_found, false);
            test_bool(e3_found, false);
            e3_found = true;
        }
    });

    test_bool(e1_found, true);
    test_bool(e2_found, true);
    test_bool(e3_found, true);
    test_int(count, 3);
}

void QueryBuilder_cascade_w_relationship(void) {
    flecs::world ecs;

    auto Tag = ecs.entity();
    auto Foo = ecs.entity();
    auto Bar = ecs.entity();

    auto e0 = ecs.entity().add(Tag);
    auto e1 = ecs.entity().child_of(e0);
    auto e2 = ecs.entity().child_of(e1);
    auto e3 = ecs.entity().child_of(e2);

    auto q = ecs.query_builder()
        .with(Tag).cascade(flecs::ChildOf)
        .cached()
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

void QueryBuilder_up_w_type(void) {
    flecs::world ecs;

    struct Rel { };

    ecs.component<Rel>().add(flecs::Traversable);

    auto q = ecs.query_builder<Self>()
        .with<Other>().src().up<Rel>().in()
        .cache_kind(cache_kind)
        .build();

    auto base = ecs.entity().set<Other>({10});

    auto 
    e = ecs.entity().add<Rel>(base); e.set<Self>({e});
    e = ecs.entity().add<Rel>(base); e.set<Self>({e});
    e = ecs.entity().add<Rel>(base); e.set<Self>({e});

    int32_t count = 0;

    q.run([&](flecs::iter& it) {
        while (it.next()) {
            auto s = it.field<Self>(0);
            auto o = it.field<const Other>(1);
            test_assert(!it.is_self(1));
            test_int(o->value, 10);

            for (auto i : it) {
                test_assert(it.entity(i) == s[i].value);
                count ++;
            }
        }
    });
    
    test_int(count, 3);
}

void QueryBuilder_cascade_w_type(void) {
    flecs::world ecs;

    struct Rel { };

    ecs.component<Rel>().add(flecs::Traversable);

    auto Tag = ecs.entity();
    auto Foo = ecs.entity();
    auto Bar = ecs.entity();

    auto e0 = ecs.entity().add(Tag);
    auto e1 = ecs.entity().add<Rel>(e0);
    auto e2 = ecs.entity().add<Rel>(e1);
    auto e3 = ecs.entity().add<Rel>(e2);

    auto q = ecs.query_builder()
        .with(Tag).cascade<Rel>()
        .cached()
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

void QueryBuilder_named_query(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();

    auto q = ecs.query_builder("my_query")
        .with<Position>()
        .cache_kind(cache_kind)
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

void QueryBuilder_term_w_write(void) {
    flecs::world ecs;

    auto q = ecs.query_builder()
        .with<Position>()
        .with<Position>().write()
        .cache_kind(cache_kind)
        .build();

    test_assert(q.term(0).inout() == flecs::InOutNone);
    test_assert(q.term(0).get_src() == flecs::This);
    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_term_w_read(void) {
    flecs::world ecs;

    auto q = ecs.query_builder()
        .with<Position>()
        .with<Position>().read()
        .cache_kind(cache_kind)
        .build();

    test_assert(q.term(0).inout() == flecs::InOutNone);
    test_assert(q.term(0).get_src() == flecs::This);
    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_iter_w_stage(void) {
    flecs::world ecs;

    ecs.set_stage_count(2);
    flecs::world stage = ecs.get_stage(1);

    auto e1 = ecs.entity().add<Position>();

    auto q = ecs.query<Position>();

    int32_t count = 0;
    q.iter(stage).each([&](flecs::iter& it, size_t i, Position&) {
        test_assert(it.world() == stage);
        test_assert(it.entity(i) == e1);
        count ++;
    });

    test_int(count, 1);
}

template<typename ... Components>
struct QueryWrapper
{
    QueryWrapper(flecs::query<Components...> f) : f_(f) {}
    flecs::query<Components...> f_;
};

void QueryBuilder_builder_force_assign_operator(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().set<Position>({10, 20});

    auto f = ecs.entity().emplace<QueryWrapper<>>(
        ecs.query_builder()
            .with<Position>()
            .cache_kind(cache_kind)
            .build()
    );

    int32_t count = 0;
    f.get<QueryWrapper<>>()->f_.each([&](flecs::entity e) {
        test_assert(e == e1);
        count ++;
    });
}

static
int query_arg(flecs::query<Self> f) {
    int32_t count = 0;

    f.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    return count;
}

void QueryBuilder_query_as_arg(void) {
    flecs::world ecs;

    auto f = ecs.query_builder<Self>()
        .cache_kind(cache_kind)
        .build();

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    test_int(query_arg(f), 3);
}

static
int query_move_arg(flecs::query<Self>&& f) {
    int32_t count = 0;

    f.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    return count;
}

void QueryBuilder_query_as_move_arg(void) {
    flecs::world ecs;

    auto f = ecs.query<Self>();

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    test_int(query_move_arg(ecs.query<Self>()), 3);
}

static
flecs::query<Self> query_return(flecs::world& ecs) {
    return ecs.query<Self>();
}

void QueryBuilder_query_as_return(void) {
    flecs::world ecs;

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    auto f = query_return(ecs);

    int32_t count = 0;

    f.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    test_int(count, 3);
}

void QueryBuilder_query_copy(void) {
    flecs::world ecs;

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    auto f = ecs.query_builder<Self>()
        .cache_kind(cache_kind)
        .build();

    auto f_2 = f;

    int32_t count = 0;

    f_2.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    test_int(count, 3);
}

void QueryBuilder_world_each_query_1_component(void) {
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

void QueryBuilder_world_each_query_2_components(void) {
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

void QueryBuilder_world_each_query_1_component_no_entity(void) {
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

void QueryBuilder_world_each_query_2_components_no_entity(void) {
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

void QueryBuilder_term_after_arg(void) {
    flecs::world ecs;

    auto e_1 = ecs.entity()
        .add<Tag0>()
        .add<Tag1>()
        .add<Tag2>();

    ecs.entity()
        .add<Tag0>()
        .add<Tag1>();

    auto f = ecs.query_builder<Tag0, Tag1>()
        .term_at(0).src(flecs::This) // dummy
        .with<Tag2>()
        .cache_kind(cache_kind)
        .build();

    test_int(f.field_count(), 3);

    int count = 0;
    f.each([&](flecs::entity e, Tag0, Tag1) {
        test_assert(e == e_1);
        count ++;
    });

    test_int(count, 1);
}

void QueryBuilder_name_arg(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo").set<Position>({10, 20});

    auto f = ecs.query_builder<Position>()
        .term_at(0).src().name("Foo")
        .cache_kind(cache_kind)
        .build();

    int32_t count = 0;
    f.run([&](flecs::iter& it) {
        while (it.next()) {
            auto p = it.field<const Position>(0);
            count ++;
            test_int(p->x, 10);
            test_int(p->y, 20);
            test_assert(it.src(0) == e);
        }
    });

    test_int(count, 1);
}

void QueryBuilder_const_in_term(void) {
    flecs::world ecs;

    ecs.entity().set<Position>({10, 20});

    auto f = ecs.query_builder<>()
        .with<const Position>()
        .cache_kind(cache_kind)
        .build();

    int32_t count = 0;
    f.run([&](flecs::iter& it) {
        while (it.next()) {
            auto p = it.field<const Position>(0);
            test_assert(it.is_readonly(0));
            for (auto i : it) {
                count ++;
                test_int(p[i].x, 10);
                test_int(p[i].y, 20);
            }
        }
    });

    test_int(count, 1);
}

void QueryBuilder_const_optional(void) {
    flecs::world ecs;
	
	ecs.entity().set<Position>({10, 20}).add<Tag0>();
    ecs.entity().add<Tag0>();

    auto f = ecs.query_builder<Tag0, const Position*>()
        .cache_kind(cache_kind)
        .build();
	
    int32_t count = 0, set_count = 0;
    f.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);
            if (it.is_set(1)) {
                auto p = it.field<const Position>(1);
                test_assert(it.is_readonly(1));
                test_int(p->x, 10);
                test_int(p->y, 20);
                set_count ++;
            }
            count++;
        }
	});
	
    test_int(count, 2);
    test_int(set_count, 1);
}

void QueryBuilder_2_terms_w_expr(void) {
    flecs::world ecs;

    auto a = ecs.entity("A");
    auto b = ecs.entity("B");

    auto e1 = ecs.entity().add(a).add(b);

    auto f = ecs.query_builder()
        .expr("A, B")
        .cache_kind(cache_kind)
        .build();
    
    test_int(f.field_count(), 2);

    int32_t count = 0;
    f.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == e1) {
            test_assert(it.id(0) == a);
            test_assert(it.id(1) == b);
            count ++;
        }
    });

    test_int(count, 1);
}

void QueryBuilder_assert_on_uninitialized_term(void) {
    install_test_abort();

    flecs::world ecs;

    ecs.entity("A");
    ecs.entity("B");

    test_expect_abort();

    auto f = ecs.query_builder()
        .term()
        .term()
        .cache_kind(cache_kind)
        .build();
}

void QueryBuilder_operator_shortcuts(void) {
    flecs::world ecs;

    flecs::entity a = ecs.entity();
    flecs::entity b = ecs.entity();
    flecs::entity c = ecs.entity();
    flecs::entity d = ecs.entity();
    flecs::entity e = ecs.entity();
    flecs::entity f = ecs.entity();
    flecs::entity g = ecs.entity();
    flecs::entity h = ecs.entity();

    auto query = ecs.query_builder()
        .with(a).and_()
        .with(b).or_()
        .with(c)
        .with(d).not_()
        .with(e).optional()
        .with(f).and_from()
        .with(g).or_from()
        .with(h).not_from()
        .cache_kind(cache_kind)
        .build();

    auto t = query.term(0);
    test_int(t.id(), a);
    test_int(t.oper(), flecs::And);

    t = query.term(1);
    test_int(t.id(), b);
    test_int(t.oper(), flecs::Or);

    t = query.term(2);
    test_int(t.id(), c);
    test_int(t.oper(), flecs::And);

    t = query.term(3);
    test_int(t.id(), d);
    test_int(t.oper(), flecs::Not);

    t = query.term(4);
    test_int(t.id(), e);
    test_int(t.oper(), flecs::Optional);

    t = query.term(5);
    test_int(t.id(), f);
    test_int(t.oper(), flecs::AndFrom);

    t = query.term(6);
    test_int(t.id(), g);
    test_int(t.oper(), flecs::OrFrom);

    t = query.term(7);
    test_int(t.id(), h);
    test_int(t.oper(), flecs::NotFrom);
}

void QueryBuilder_inout_shortcuts(void) {
    flecs::world ecs;

    flecs::entity a = ecs.entity();
    flecs::entity b = ecs.entity();
    flecs::entity c = ecs.entity();
    flecs::entity d = ecs.entity();

    auto query = ecs.query_builder()
        .with(a).in()
        .with(b).out()
        .with(c).inout()
        .with(d).inout_none()
        .cache_kind(cache_kind)
        .build();

    auto t = query.term(0);
    test_int(t.id(), a);
    test_int(t.inout(), flecs::In);

    t = query.term(1);
    test_int(t.id(), b);
    test_int(t.inout(), flecs::Out);

    t = query.term(2);
    test_int(t.id(), c);
    test_int(t.inout(), flecs::InOut);

    t = query.term(3);
    test_int(t.id(), d);
    test_int(t.inout(), flecs::InOutNone);
}

void QueryBuilder_iter_column_w_const_as_array(void) {
    flecs::world world;

    auto f = world.query_builder<Position>()
        .cache_kind(cache_kind)
        .build();

    auto e1 = world.entity().set<Position>({10, 20});
    auto e2 = world.entity().set<Position>({20, 30});

    int32_t count = 0;
    f.run([&](flecs::iter& it) {
        while (it.next()) {
            const auto p = it.field<Position>(0);
            for (auto i : it) {
                p[i].x += 1;
                p[i].y += 2;

                count ++;
            }
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

void QueryBuilder_iter_column_w_const_as_ptr(void) {
    flecs::world world;

    auto f = world.query_builder<Position>()
        .cache_kind(cache_kind)
        .build();
    
    auto base = world.prefab().set<Position>({10, 20});
    world.entity().is_a(base);
    world.entity().is_a(base);

    int32_t count = 0;
    f.run([&](flecs::iter& it) {
        while (it.next()) {
            const auto p = it.field<Position>(0);
            for (size_t i = 0; i < it.count(); i ++) {
                test_int(p->x, 10);
                test_int(p->y, 20);
                count ++;
            }
        }
    });

    test_int(count, 2);
}

void QueryBuilder_iter_column_w_const_deref(void) {
    flecs::world world;

    auto f = world.query_builder<Position>()
        .cache_kind(cache_kind)
        .build();
    
    auto base = world.prefab().set<Position>({10, 20});
    world.entity().is_a(base);
    world.entity().is_a(base);

    int32_t count = 0;
    f.run([&](flecs::iter& it) {
        while (it.next()) {
            const auto p = it.field<Position>(0);
            Position pv = *p;
            for (size_t i = 0; i < it.count(); i ++) {
                test_int(pv.x, 10);
                test_int(pv.y, 20);
                count ++;
            }
        }
    });

    test_int(count, 2);
}

void QueryBuilder_with_id(void) {
    flecs::world ecs;

    flecs::query<> q = 
        ecs.query_builder()
            .with(ecs.id<Position>())
            .with(ecs.id<Velocity>())
            .cache_kind(cache_kind)
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

void QueryBuilder_with_name(void) {
    flecs::world ecs;

    ecs.component<Velocity>();

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .with("Velocity")
            .cache_kind(cache_kind)
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

void QueryBuilder_with_component(void) {
    flecs::world ecs;

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .with<Velocity>()
            .cache_kind(cache_kind)
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

void QueryBuilder_with_pair_id(void) {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity();
    flecs::entity Apples = ecs.entity();
    flecs::entity Pears = ecs.entity();

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .with(Likes, Apples)
            .cache_kind(cache_kind)
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

void QueryBuilder_with_pair_name(void) {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity("Likes");
    flecs::entity Apples = ecs.entity("Apples");
    flecs::entity Pears = ecs.entity("Pears");

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .with("Likes", "Apples")
            .cache_kind(cache_kind)
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

void QueryBuilder_with_pair_components(void) {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };
    struct Pears { };

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .with<Likes, Apples>()
            .cache_kind(cache_kind)
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

void QueryBuilder_with_pair_component_id(void) {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity();
    flecs::entity Pears = ecs.entity();

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .with<Likes>(Apples)
            .cache_kind(cache_kind)
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

void QueryBuilder_with_pair_component_name(void) {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity("Apples");
    flecs::entity Pears = ecs.entity("Pears");

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .with<Likes>("Apples")
            .cache_kind(cache_kind)
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

void QueryBuilder_with_enum(void) {
    flecs::world ecs;

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .with(Green)
            .cache_kind(cache_kind)
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

void QueryBuilder_without_id(void) {
    flecs::world ecs;

    flecs::query<> q = 
        ecs.query_builder()
            .with(ecs.id<Position>())
            .without(ecs.id<Velocity>())
            .cache_kind(cache_kind)
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

void QueryBuilder_without_name(void) {
    flecs::world ecs;

    ecs.component<Velocity>();

    flecs::query<> q = 
        ecs.query_builder()
            .with(ecs.id<Position>())
            .without("Velocity")
            .cache_kind(cache_kind)
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

void QueryBuilder_without_component(void) {
    flecs::world ecs;

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .without<Velocity>()
            .cache_kind(cache_kind)
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

void QueryBuilder_without_pair_id(void) {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity();
    flecs::entity Apples = ecs.entity();
    flecs::entity Pears = ecs.entity();

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .without(Likes, Apples)
            .cache_kind(cache_kind)
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

void QueryBuilder_without_pair_name(void) {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity("Likes");
    flecs::entity Apples = ecs.entity("Apples");
    flecs::entity Pears = ecs.entity("Pears");

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .without("Likes", "Apples")
            .cache_kind(cache_kind)
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

void QueryBuilder_without_pair_components(void) {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };
    struct Pears { };

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .without<Likes, Apples>()
            .cache_kind(cache_kind)
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

void QueryBuilder_without_pair_component_id(void) {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity();
    flecs::entity Pears = ecs.entity();

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .without<Likes>(Apples)
            .cache_kind(cache_kind)
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

void QueryBuilder_without_pair_component_name(void) {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity("Apples");
    flecs::entity Pears = ecs.entity("Pears");

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .without<Likes>("Apples")
            .cache_kind(cache_kind)
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

void QueryBuilder_without_enum(void) {
    flecs::world ecs;

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .without(Green)
            .cache_kind(cache_kind)
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

void QueryBuilder_write_id(void) {
    flecs::world ecs;

    auto q = 
        ecs.query_builder()
            .with<Position>()
            .write(ecs.id<Position>())
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_write_name(void) {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = 
        ecs.query_builder()
            .with<Position>()
            .write("Position")
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_write_component(void) {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = 
        ecs.query_builder()
            .with<Position>()
            .write<Position>()
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_write_pair_id(void) {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity();
    flecs::entity Apples = ecs.entity();

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .write(Likes, Apples)
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == Likes);
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_write_pair_name(void) {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity("Likes");
    flecs::entity Apples = ecs.entity("Apples");

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .write("Likes", "Apples")
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == Likes);
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_write_pair_components(void) {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .write<Likes, Apples>()
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == ecs.id<Apples>());
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_write_pair_component_id(void) {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity();

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .write<Likes>(Apples)
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_write_pair_component_name(void) {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity("Apples");

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .write<Likes>("Apples")
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_write_enum(void) {
    flecs::world ecs;

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .write(Green)
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::Out);
    test_assert(q.term(1).get_first() == ecs.id<Color>());
    test_assert(q.term(1).get_second() == ecs.to_entity<Color>(Green));
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_read_id(void) {
    flecs::world ecs;

    auto q = 
        ecs.query_builder()
            .with<Position>()
            .read(ecs.id<Position>())
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_read_name(void) {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = 
        ecs.query_builder()
            .with<Position>()
            .read("Position")
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_read_component(void) {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = 
        ecs.query_builder()
            .with<Position>()
            .read<Position>()
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Position>());
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_read_pair_id(void) {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity();
    flecs::entity Apples = ecs.entity();

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .read(Likes, Apples)
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == Likes);
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_read_pair_name(void) {
    flecs::world ecs;

    flecs::entity Likes = ecs.entity("Likes");
    flecs::entity Apples = ecs.entity("Apples");

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .read("Likes", "Apples")
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == Likes);
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_read_pair_components(void) {
    flecs::world ecs;

    struct Likes { };
    struct Apples { };

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .read<Likes, Apples>()
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == ecs.id<Apples>());
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_read_pair_component_id(void) {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity();

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .read<Likes>(Apples)
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == Apples);
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_read_pair_component_name(void) {
    flecs::world ecs;

    struct Likes { };
    flecs::entity Apples = ecs.entity("Apples");

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .read<Likes>("Apples")
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Likes>());
    test_assert(q.term(1).get_second() == Apples);

    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_read_enum(void) {
    flecs::world ecs;

    flecs::query<> q = 
        ecs.query_builder()
            .with<Position>()
            .read(Green)
            .cache_kind(cache_kind)
            .build();

    test_assert(q.term(1).inout() == flecs::In);
    test_assert(q.term(1).get_first() == ecs.id<Color>());
    test_assert(q.term(1).get_second() == ecs.to_entity<Color>(Green));
    test_assert(q.term(1).get_src() == 0);
}

void QueryBuilder_assign_after_init(void) {
    flecs::world ecs;

    flecs::query<> f;
    flecs::query_builder<> fb = ecs.query_builder();
    fb.with<Position>();
    fb.cache_kind(cache_kind);
    f = fb.build();

    flecs::entity e1 = ecs.entity().set<Position>({10, 20});

    int32_t count = 0;
    f.each([&](flecs::entity e) {
        test_assert(e == e1);
        count ++;
    });

    test_int(count, 1);
}

void QueryBuilder_with_t_inout(void) {
    flecs::world ecs;

    flecs::query<> f = ecs.query_builder()
        .with(ecs.id<Position>())
        .cache_kind(cache_kind)
        .build();

    test_assert(f.term(0).inout() == flecs::InOutNone);
}

void QueryBuilder_with_T_inout(void) {
    flecs::world ecs;

    flecs::query<> f = ecs.query_builder()
        .with<Position>()
        .cache_kind(cache_kind)
        .build();

    test_assert(f.term(0).inout() == flecs::InOutNone);
}

void QueryBuilder_with_R_T_inout(void) {
    flecs::world ecs;

    flecs::query<> f = ecs.query_builder()
        .with<Position, Velocity>()
        .cache_kind(cache_kind)
        .build();

    test_assert(f.term(0).inout() == flecs::InOutNone);
}

void QueryBuilder_with_R_t_inout(void) {
    flecs::world ecs;

    flecs::query<> f = ecs.query_builder()
        .with<Position>(ecs.id<Velocity>())
        .cache_kind(cache_kind)
        .build();

    test_assert(f.term(0).inout() == flecs::InOutNone);
}

void QueryBuilder_with_r_t_inout(void) {
    flecs::world ecs;

    flecs::query<> f = ecs.query_builder()
        .with(ecs.id<Position>(), ecs.id<Velocity>())
        .cache_kind(cache_kind)
        .build();

    test_assert(f.term(0).inout() == flecs::InOutNone);
}

static
int filter_move_arg(flecs::query<Self>&& f) {
    int32_t count = 0;

    f.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    return count;
}

void QueryBuilder_filter_as_move_arg(void) {
    flecs::world ecs;

    auto f = ecs.query_builder<Self>()
        .cache_kind(cache_kind)
        .build();

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    test_int(filter_move_arg(ecs.query<Self>()), 3);
}

static
flecs::query<Self> filter_return(flecs::world& ecs) {
    return ecs.query_builder<Self>()
        .cache_kind(cache_kind)
        .build();
}

void QueryBuilder_filter_as_return(void) {
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

void QueryBuilder_filter_copy(void) {
    flecs::world ecs;

    auto e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    e = ecs.entity();
    e.set<Self>({e});

    auto f = ecs.query_builder<Self>()
        .cache_kind(cache_kind)
        .build();

    auto f_2 = f;

    int32_t count = 0;

    f_2.each([&](flecs::entity e, Self& s) {
        test_assert(e == s.value);
        count ++;
    });

    test_int(count, 3);
}

void QueryBuilder_world_each_filter_1_component(void) {
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

void QueryBuilder_world_each_filter_2_components(void) {
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

void QueryBuilder_world_each_filter_1_component_no_entity(void) {
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

void QueryBuilder_world_each_filter_2_components_no_entity(void) {
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

void QueryBuilder_var_src_w_prefixed_name(void) {
    flecs::world ecs;

    struct Foo { };

    auto r = ecs.query_builder()
        .with<Foo>().src("$Var")
        .cache_kind(cache_kind)
        .build();

    auto e = ecs.entity().add<Foo>();

    int32_t count = 0;
    r.run([&](flecs::iter& it) {
        while (it.next()) {
            test_assert(it.get_var("Var") == e);
            count ++;
        }
    });

    test_int(count, 1);

}

void QueryBuilder_var_first_w_prefixed_name(void) {
    flecs::world ecs;

    struct Foo { };

    auto r = ecs.query_builder()
        .with<Foo>()
        .term().first("$Var")
        .cache_kind(cache_kind)
        .build();

    auto e = ecs.entity().add<Foo>();

    int32_t count = 0;
    r.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);
            test_uint(it.entity(0), e);
            test_assert(it.get_var("Var") == ecs.id<Foo>());
            count ++;
        }
    });

    test_int(count, 1);

}

void QueryBuilder_var_second_w_prefixed_name(void) {
    flecs::world ecs;

    struct Foo { };

    auto r = ecs.query_builder()
        .with<Foo>().second("$Var")
        .cache_kind(cache_kind)
        .build();

    auto t = ecs.entity();
    auto e = ecs.entity().add<Foo>(t);

    int32_t count = 0;
    r.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);
            test_uint(it.entity(0), e);
            test_assert(it.get_var("Var") == t);
            count ++;
        }
    });

    test_int(count, 1);

}

void QueryBuilder_term_w_second_var_string(void) {
    flecs::world ecs;

    flecs::entity Foo = ecs.entity();

    auto r = ecs.query_builder()
        .with(Foo, "$Var")
        .cache_kind(cache_kind)
        .build();

    auto t = ecs.entity();
    auto e = ecs.entity().add(Foo, t);

    int32_t count = 0;
    r.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);
            test_uint(it.entity(0), e);
            test_assert(it.get_var("Var") == t);
            count ++;
        }
    });

    test_int(count, 1);

}

void QueryBuilder_term_type_w_second_var_string(void) {
    flecs::world ecs;

    struct Foo { };

    auto r = ecs.query_builder()
        .with<Foo>("$Var")
        .cache_kind(cache_kind)
        .build();

    auto t = ecs.entity();
    auto e = ecs.entity().add<Foo>(t);

    int32_t count = 0;
    r.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);
            test_uint(it.entity(0), e);
            test_assert(it.get_var("Var") == t);
            count ++;
        }
    });

    test_int(count, 1);

}

void QueryBuilder_named_rule(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();

    auto q = ecs.query_builder<Position>("my_query")
        .cache_kind(cache_kind)
        .build();

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

void QueryBuilder_named_scoped_rule(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>();

    auto q = ecs.query_builder<Position>("my::query")
        .cache_kind(cache_kind)
        .build();

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

void QueryBuilder_is_valid(void) {
    flecs::world ecs;

    auto q_1 = ecs.query<Position>();
    test_assert(q_1);

    flecs::log::set_level(-4);
    auto q_2 = ecs.query_builder()
        .expr("foo")
        .cache_kind(cache_kind)
        .build();
    test_assert(!q_2);
}

void QueryBuilder_unresolved_by_name(void) {
    flecs::world ecs;

    auto q = ecs.query_builder()
        .query_flags(EcsQueryAllowUnresolvedByName)
        .expr("$this == Foo")
        .cache_kind(cache_kind)
        .build();

    test_assert(q);

    test_false(q.iter().is_true());

    ecs.entity("Foo");

    test_true(q.iter().is_true());

}

void QueryBuilder_scope(void) {
    flecs::world ecs;

    flecs::entity Root = ecs.entity();
    flecs::entity TagA = ecs.entity();
    flecs::entity TagB = ecs.entity();

    ecs.entity()
        .add(Root)
        .add(TagA)
        .add(TagB);

    auto e2 = ecs.entity()
        .add(Root)
        .add(TagA);

    ecs.entity()
        .add(Root)
        .add(TagB);

    ecs.entity()
        .add(Root);

    auto r = ecs.query_builder()
        .with(Root)
        .scope_open().not_()
            .with(TagA)
            .without(TagB)
        .scope_close()
        .cache_kind(cache_kind)
        .build();

    int32_t count = 0;
    r.each([&](flecs::entity e) {
        test_assert(e != e2);
        count ++;
    });

    test_int(count, 3);

}
