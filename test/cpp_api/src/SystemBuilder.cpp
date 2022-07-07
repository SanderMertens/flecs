#include <cpp_api.h>

void SystemBuilder_builder_assign_same_type() {
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

void SystemBuilder_builder_build_to_auto() {
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

void SystemBuilder_builder_build_n_statements() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>();  

    int32_t count = 0;

    auto qb = ecs.system<>();
    qb.term<Position>();
    qb.term<Velocity>();
    auto s = qb.each([&](flecs::entity e) {
        count ++;
        test_assert(e == e1);
    });

    s.run();
    
    test_int(count, 1);
}

void SystemBuilder_1_type() {
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

void SystemBuilder_add_1_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .term<Position>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_2_types() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .term<Position>()
        .term<Velocity>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_1_type_w_1_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .term<Velocity>()
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_2_types_w_1_type() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>().add<Mass>();
    ecs.entity().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .term<Velocity>()
        .term<Mass>()
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_pair() {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();

    auto e1 = ecs.entity().add(Likes, Bob);
    ecs.entity().add(Likes, Alice);

    int32_t count = 0;

    auto s = ecs.system<>()
        .term(Likes, Bob)
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_not() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    ecs.entity().add<Position>().add<Velocity>();

    int32_t count = 0;

    auto s = ecs.system<Position>()
        .term<Velocity>().oper(flecs::Not)
        .each([&](flecs::entity e, Position& p) {
            count ++;
            test_assert(e == e1);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_add_or() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();
    ecs.entity().add<Mass>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .term<Position>().oper(flecs::Or)
        .term<Velocity>().oper(flecs::Or)
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1 || e == e2);
        });
    
    test_int(count, 0);
    s.run();
    test_int(count, 2);
}

void SystemBuilder_add_optional() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Velocity>().add<Mass>();

    int32_t count = 0;

    auto s = ecs.system<>()
        .term<Position>()
        .term<Velocity>().oper(flecs::Optional)
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1 || e == e2);
        });
        
    test_int(count, 0);
    s.run();
    test_int(count, 2);
}

void SystemBuilder_ptr_type() {
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

void SystemBuilder_const_type() {
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

void SystemBuilder_string_term() {
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

void SystemBuilder_singleton_term() {
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
        .term<Singleton>().singleton()
        .iter([&](flecs::iter& it, Entity *e) {
            auto s = it.term<const Singleton>(2);
            test_assert(!it.is_owned(2));
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

void SystemBuilder_10_terms() {
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

    s.run();

    test_int(count, 1);
}

void SystemBuilder_20_terms() {
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
        .add<TagP>()
        .add<TagQ>()
        .add<TagR>()
        .add<TagS>()
        .add<TagT>();

    auto s = ecs.system<>()
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

    s.run();

    test_int(count, 1);
}

void SystemBuilder_name_arg() {
    flecs::world ecs;

    auto s = ecs.system<const Position>("MySystem")
        .arg(1).subj().name("MySystem")
        .iter([](flecs::iter& Iter, const Position* Config)
        { });

    test_assert(s.has<Position>());
}

void SystemBuilder_create_w_no_template_args() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    int32_t count = 0;

    auto s = ecs.system()
        .term<Position>()
        .each([&](flecs::entity e) {
            count ++;
            test_assert(e == e1);
        });

    test_int(count, 0);
    s.run();
    test_int(count, 1);
}

void SystemBuilder_write_annotation() {
    flecs::world ecs;

    struct TagA { };
    struct TagB { };

    auto e1 = ecs.entity().add<TagA>();

    int32_t a_count = 0, b_count = 0;

    ecs.system<TagA>()
        .term<TagB>().write()
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
