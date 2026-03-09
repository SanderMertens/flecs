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

    ecs.component<Singleton>().add(flecs::Singleton);

    ecs.set<Singleton>({10});

    int32_t count = 0;

    auto s = ecs.system<Entity>()
        .with<Singleton>().in()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto e = it.field<Entity>(0);
                auto s = it.field<const Singleton>(1);
                test_assert(!it.is_self(1));
                test_int(s->value, 10);
                
                const Singleton& s_ref = *s;
                test_int(s_ref.value, 10);

                for (auto i : it) {
                    test_assert(it.entity(i) == e[i].value);
                    count ++;
                }
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

    auto s = ecs.system<>()
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
        .run([&](flecs::iter& it) {
            while (it.next()) {
                test_int(it.count(), 1);
                test_assert(it.entity(0) == e);
                test_int(it.field_count(), 10);
                count ++;
            }
        });

    s.run();

    test_int(count, 1);
}

void SystemBuilder_16_terms(void) {
    flecs::world ecs;

    int count = 0;

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

    auto s = ecs.system<>()
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
        .run([&](flecs::iter& it) {
            while (it.next()) {
                test_int(it.count(), 1);
                test_assert(it.entity(0) == e);
                test_int(it.field_count(), 16);
                count ++;
            }
        });

    s.run();

    test_int(count, 1);
}

void SystemBuilder_name_arg(void) {
    flecs::world ecs;

    auto s = ecs.system<const Position>("MySystem")
        .term_at(0).src().name("MySystem")
        .run([](flecs::iter& it) {
            while (it.next()) {}
        });

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

void SystemBuilder_deduce_terms_from_each_callback(void) {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity()
        .set<Position>({20, 30});

    int32_t term_count = 0;
    int32_t count = 0;

    auto s = ecs.system()
        .each([&](Position& p, const Velocity& v) {
            test_int(p.x, 10);
            test_int(p.y, 20);
            test_int(v.x, 1);
            test_int(v.y, 2);
            p.x += v.x;
            p.y += v.y;
            count ++;
        });

    s.query().each_term([&](flecs::term& t) {
        if (term_count == 0) {
            test_int(t.id(), ecs.id<Position>());
            test_int(t.inout(), flecs::InOutDefault);
        } else if (term_count == 1) {
            test_int(t.id(), ecs.id<Velocity>());
            test_int(t.inout(), flecs::In);
        } else {
            test_assert(false);
        }

        test_int(t.oper(), flecs::And);
        term_count ++;
    });

    test_int(term_count, 2);

    s.run();

    test_int(count, 1);

    const Position *p = e.try_get<Position>();
    test_assert(p != nullptr);
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void SystemBuilder_deduce_optional_terms_from_each_callback(void) {
    flecs::world ecs;

    auto e1 = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .set<Mass>({1});

    auto e2 = ecs.entity()
        .set<Position>({30, 40})
        .set<Velocity>({3, 4});

    auto e3 = ecs.entity()
        .set<Position>({50, 60});

    int32_t term_count = 0;
    int32_t count = 0;

    auto s = ecs.system()
        .each([&](Position& p, Velocity* v, Mass* m) {
            if (v && m) {
                p.x += v->x * m->value;
                p.y += v->y * m->value;
            } else if (v) {
                p.x += v->x;
                p.y += v->y;
            } else {
                p.x ++;
                p.y ++;
            }

            count ++;
        });

    s.query().each_term([&](flecs::term& t) {
        if (term_count == 0) {
            test_int(t.id(), ecs.id<Position>());
            test_int(t.inout(), flecs::InOutDefault);
            test_int(t.oper(), flecs::And);
        } else if (term_count == 1) {
            test_int(t.id(), ecs.id<Velocity>());
            test_int(t.inout(), flecs::InOutDefault);
            test_int(t.oper(), flecs::Optional);
        } else if (term_count == 2) {
            test_int(t.id(), ecs.id<Mass>());
            test_int(t.inout(), flecs::InOutDefault);
            test_int(t.oper(), flecs::Optional);
        } else {
            test_assert(false);
        }

        term_count ++;
    });

    test_int(term_count, 3);

    s.run();

    test_int(count, 3);

    const Position *p = e1.try_get<Position>();
    test_assert(p != nullptr);
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.try_get<Position>();
    test_assert(p != nullptr);
    test_int(p->x, 33);
    test_int(p->y, 44);

    p = e3.try_get<Position>();
    test_assert(p != nullptr);
    test_int(p->x, 51);
    test_int(p->y, 61);
}

void SystemBuilder_deduce_pair_term_from_each_callback(void) {
    flecs::world ecs;

    ecs.entity()
        .add<Tag0>()
        .emplace<Position, Tag>(1.0f, 2.0f);

    ecs.entity()
        .add<Tag0>();

    int32_t term_count = 0;
    int32_t count = 0;
    auto pair_id = ecs.id<flecs::pair<Position, Tag>>();

    auto s = ecs.system()
        .with<Tag0>()
        .each([&](flecs::entity e, flecs::pair<Position, Tag> p) {
            test_assert(e.has<Tag0>());
            test_flt(1.0f, p->x);
            test_flt(2.0f, p->y);
            p->x += 1.0f;
            p->y += 1.0f;
            count ++;
        });

    s.query().each_term([&](flecs::term& t) {
        if (term_count == 0) {
            test_int(t.id(), pair_id);
            test_int(t.inout(), flecs::InOutDefault);
        } else if (term_count == 1) {
            test_int(t.id(), ecs.id<Tag0>());
            test_int(t.inout(), flecs::InOutDefault);
        } else {
            test_assert(false);
        }

        test_int(t.oper(), flecs::And);
        term_count ++;
    });

    test_int(term_count, 2);

    s.run();

    test_int(count, 1);

    int32_t with_pair = 0;
    int32_t without_pair = 0;

    ecs.each([&](flecs::entity e, Tag0) {
        const Position *p = e.try_get<Position, Tag>();
        if (p) {
            with_pair ++;
            test_flt(2.0f, p->x);
            test_flt(3.0f, p->y);
        } else {
            without_pair ++;
        }
    });

    test_int(with_pair, 1);
    test_int(without_pair, 1);
}

void SystemBuilder_deduce_singleton_term_from_each_callback(void) {
    flecs::world ecs;

    struct Singleton {
        int32_t value;
    };

    ecs.component<Singleton>().add(flecs::Singleton);
    ecs.set<Singleton>({10});

    int32_t term_count = 0;
    int32_t count = 0;

    auto s = ecs.system()
        .each([&](Singleton& singleton) {
            test_int(singleton.value, 10);
            singleton.value += 5;
            count ++;
        });

    s.query().each_term([&](flecs::term& t) {
        test_int(t.id(), ecs.id<Singleton>());
        test_int(t.inout(), flecs::InOutDefault);
        test_int(t.oper(), flecs::And);
        term_count ++;
    });

    test_int(term_count, 1);

    s.run();

    test_int(count, 1);

    const Singleton *singleton = ecs.try_get<Singleton>();
    test_assert(singleton != nullptr);
    test_int(singleton->value, 15);
}

void SystemBuilder_deduce_singleton_and_component_terms_from_each_callback(void) {
    flecs::world ecs;

    ecs.component<Velocity>().add(flecs::Singleton);
    ecs.set<Velocity>({1, 2});

    auto e1 = ecs.entity()
        .set<Position>({10, 20});

    auto e2 = ecs.entity()
        .set<Position>({20, 30});

    int32_t term_count = 0;
    int32_t count = 0;

    auto s = ecs.system()
        .each([&](Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
            count ++;
        });

    s.query().each_term([&](flecs::term& t) {
        if (term_count == 0) {
            test_int(t.id(), ecs.id<Position>());
            test_int(t.inout(), flecs::InOutDefault);
        } else if (term_count == 1) {
            test_int(t.id(), ecs.id<Velocity>());
            test_int(t.inout(), flecs::In);
        } else {
            test_assert(false);
        }

        test_int(t.oper(), flecs::And);
        term_count ++;
    });

    test_int(term_count, 2);

    s.run();

    test_int(count, 2);

    const Position *p = e1.try_get<Position>();
    test_assert(p != nullptr);
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e2.try_get<Position>();
    test_assert(p != nullptr);
    test_int(p->x, 21);
    test_int(p->y, 32);
}

void SystemBuilder_with_terms_after_deduced_terms(void) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Bob = ecs.entity();

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .add<Mass>()
        .add(Likes, Bob);

    ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .add<Mass>();

    int32_t term_count = 0;
    int32_t count = 0;

    auto s = ecs.system()
        .with<Mass>()
        .with(Likes, Bob)
        .each([&](flecs::entity entity, Position& p, const Velocity& v) {
            test_assert(entity == e);
            test_int(p.x, 10);
            test_int(p.y, 20);
            test_int(v.x, 1);
            test_int(v.y, 2);
            count ++;
        });

    s.query().each_term([&](flecs::term& t) {
        if (term_count == 0) {
            test_int(t.id(), ecs.id<Position>());
            test_int(t.inout(), flecs::InOutDefault);
        } else if (term_count == 1) {
            test_int(t.id(), ecs.id<Velocity>());
            test_int(t.inout(), flecs::In);
        } else if (term_count == 2) {
            test_int(t.id(), ecs.id<Mass>());
            test_int(t.inout(), flecs::InOutDefault);
        } else if (term_count == 3) {
            test_int(t.id(), ecs.pair(Likes, Bob));
            test_int(t.inout(), flecs::InOutDefault);
        } else {
            test_assert(false);
        }

        test_int(t.oper(), flecs::And);
        term_count ++;
    });

    test_int(term_count, 4);

    s.run();

    test_int(count, 1);
}

void SystemBuilder_write_annotation(void) {
    flecs::world ecs;

    struct Tag0 { };
    struct Tag1 { };

    auto e1 = ecs.entity().add<Tag0>();

    int32_t a_count = 0, b_count = 0;

    ecs.system<Tag0>()
        .with<Tag1>().write()
        .each([&](flecs::entity e, Tag0) {
            a_count ++;
            test_assert(e == e1);
            e.add<Tag1>();
        });
    
    ecs.system<Tag1>()
        .each([&](flecs::entity e, Tag1) {
            b_count ++;
            test_assert(e == e1);
            test_assert(e.has<Tag1>());
        });

    test_int(a_count, 0);
    test_int(b_count, 0);
    
    ecs.progress();
    
    test_int(a_count, 1);
    test_int(b_count, 1);

    test_assert(e1.has<Tag1>());
}

void SystemBuilder_name_from_root(void) {
    flecs::world ecs;

    flecs::entity sys = ecs.system("::ns::MySystem")
        .each([](flecs::entity e) { });

    test_str(sys.name(), "MySystem");

    flecs::entity ns = ecs.entity("::ns");
    test_assert(ns == sys.parent());
}
