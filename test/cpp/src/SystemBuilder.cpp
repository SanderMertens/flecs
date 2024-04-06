#include <cpp.h>

void SystemBuilder_builder_assign_same_type(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;

    flecs::system s = 
        ecs.system<Position, Velocity>()
            .each([&](flecs::entity e, Position& p, Velocity& v) {
                count ++;
                test_assert(e == e1);
            });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_builder_build_to_auto(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();

    int32_t count = 0;

    auto s = ecs.system<Position, Velocity>()
        .each([&](flecs::entity e, Position& p, Velocity& v) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_builder_build_n_statements(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();  

    int32_t count = 0;

    auto qb = ecs.system<>();
    qb.with<Position>();
    qb.with<Velocity>();
    auto s = qb.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });

    s.run();
    
    test_int(count, 1);
}

void SystemBuilder_1_type(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_1_type(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .with<Position>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_2_types(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .with<Position>()
        .with<Velocity>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_1_type_w_1_type(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .with<Velocity>()
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_2_types_w_1_type(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>().add<Mass>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .with<Velocity>()
        .with<Mass>()
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_pair(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto e1 = ecs.entity().add(Likes, Bob);
    ecs.entity().add(Likes, Alice);

    int32_t count = 0;

    auto s = ecs.system<>()
        .with(Likes, Bob)
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_not(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .with<Velocity>().oper(flecs::Not)
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_or(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();
    ecs.entity().add<Mass>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .with<Position>().oper(flecs::Or)
        .with<Velocity>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1 || e == e2);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 2);
}

void SystemBuilder_add_optional(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>().add<Mass>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .with<Position>()
        .with<Velocity>().oper(flecs::Optional)
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1 || e == e2);
        });
        
    test_int(count, 0);
    s.run();
    test_int(count, 2);
}

void SystemBuilder_ptr_type(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>().add<Mass>();

    int32_t count = 0;

    auto s = ecs.system<Position, Velocity*>()
        .each([&](flecs::entity e, Position& p, Velocity* v) {
            count ++;
            test_assert(e == e1 || e == e2);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 2);
}

void SystemBuilder_const_type(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<const Position>()
        .each([&](flecs::entity e, const Position& p) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_string_term(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .expr("Position")
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    s.run();
    
    test_int(count, 1);
}

void SystemBuilder_singleton_term(void) {
    flecs::world ecs;

    struct Entity {
        flecs::entity_view value;
    };

    struct Singleton {
        int32_t value;
    };

    ecs.set<Singleton>({10});

    int32_t count = 0;

    auto s = ecs.system<Entity>()
        .with<Singleton>().singleton().in()
        .iter([&](flecs::iter& it, Entity *e) {
            auto s = it.field<const Singleton>(1);
            test_assert(!it.is_self(1));
            test_int(s->value, 10);
            
            const Singleton& s_ref = *s;
            test_int(s_ref.value, 10);

            for (auto i : it) {
                test_assert(it.entity(i) == e[i].value);
                count ++;
            }
        });

    auto 
    e = ecs.entity(); e.set<Entity>({e});
    e = ecs.entity(); e.set<Entity>({e});
    e = ecs.entity(); e.set<Entity>({e});

    s.run();
    
    test_int(count, 3);
}

void SystemBuilder_10_terms(void) {
    flecs::world ecs;

    int count = 0;

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

    auto s = ecs.system<>()
        .with<TagA>()
        .with<TagB>()
        .with<TagC>()
        .with<TagD>()
        .with<TagE>()
        .with<TagF>()
        .with<TagG>()
        .with<TagH>()
        .with<TagI>()
        .with<TagJ>()
        .iter([&](flecs::iter& it) {
            test_int(it.count(), 1);
            test_assert(it.entity(0) == e);
            test_int(it.field_count(), 10);
            count ++;
        });

    s.run();

    test_int(count, 1);
}

void SystemBuilder_16_terms(void) {
    flecs::world ecs;

    int count = 0;

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
        .add<TagP>();

    auto s = ecs.system<>()
        .with<TagA>()
        .with<TagB>()
        .with<TagC>()
        .with<TagD>()
        .with<TagE>()
        .with<TagF>()
        .with<TagG>()
        .with<TagH>()
        .with<TagI>()
        .with<TagJ>()
        .with<TagK>()
        .with<TagL>()
        .with<TagM>()
        .with<TagN>()
        .with<TagO>()
        .with<TagP>()
        .iter([&](flecs::iter& it) {
            test_int(it.count(), 1);
            test_assert(it.entity(0) == e);
            test_int(it.field_count(), 16);
            count ++;
        });

    s.run();

    test_int(count, 1);
}

void SystemBuilder_name_arg(void) {
    flecs::world ecs;

    auto s = ecs.system<const Position>("MySystem")
        .term_at(0).src().name("MySystem")
        .iter([](flecs::iter& Iter, const Position* Config)
        { });

    test_assert(s.has<Position>());
}

void SystemBuilder_create_w_no_template_args(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    int32_t count = 0;

    auto s = ecs.system()
        .with<Position>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_write_annotation(void) {
    flecs::world ecs;

    struct TagA { };
    struct TagB { };

    auto e1 = ecs.entity().add<TagA>();

    int32_t a_count = 0, b_count = 0;

    ecs.system<TagA>()
        .with<TagB>().write()
        .each([&](flecs::entity e, TagA) {
            a_count ++;
            test_assert(e == e1);
            e.add<TagB>();
        });
    
    ecs.system<TagB>()
        .each([&](flecs::entity e, TagB) {
            b_count ++;
            test_assert(e == e1);
            test_assert(e.has<TagB>());
        });

    test_int(a_count, 0);
    test_int(b_count, 0);
    
    ecs.progress();
    
    test_int(a_count, 1);
    test_int(b_count, 1);

    test_assert(e1.has<TagB>());
}

void SystemBuilder_name_from_root(void) {
    flecs::world ecs;

    flecs::entity sys = ecs.system("::ns::MySystem")
        .each([](flecs::entity e) { });

    test_str(sys.name(), "MySystem");

    flecs::entity ns = ecs.entity("::ns");
    test_assert(ns == sys.parent());
}
