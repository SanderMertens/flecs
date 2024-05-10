#include <cpp.h>

enum Color {
    Red, Green, Blue
};

void Misc_setup(void) {
    ecs_os_set_api_defaults();
}

void Misc_string_compare_w_char_ptr(void) {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "foo";
    const char *ptr_3 = "bar";

    flecs::string str = flecs::string(ecs_os_strdup(ptr_1));

    test_assert(str == ptr_1);
    test_assert(str == ptr_2);
    test_assert(str != ptr_3);
}

void Misc_string_view_compare_w_char_ptr(void) {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "foo";
    const char *ptr_3 = "bar";

    flecs::string str = flecs::string_view(ptr_1);

    test_assert(str == ptr_1);
    test_assert(str == ptr_2);
    test_assert(str != ptr_3);   
}

void Misc_string_compare_w_char_ptr_length_diff(void) {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "foo";
    const char *ptr_3 = "barrr";
    const char *ptr_4 = "ba";

    flecs::string str = flecs::string_view(ptr_1);

    test_assert(str == ptr_1);
    test_assert(str == ptr_2);
    test_assert(str != ptr_3);
    test_assert(str != ptr_4);
}

void Misc_string_compare_w_string(void) {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "bar";

    flecs::string str_1 = flecs::string(ecs_os_strdup(ptr_1));
    flecs::string str_2 = flecs::string(ecs_os_strdup(ptr_2));

    flecs::string str_3 = flecs::string(ecs_os_strdup(ptr_1));
    flecs::string str_4 = flecs::string(ecs_os_strdup(ptr_2));

    test_assert(str_1 == str_1);
    test_assert(str_1 == str_3);

    test_assert(str_2 == str_2);
    test_assert(str_2 == str_4);

    test_assert(str_1 != str_2);
    test_assert(str_2 != str_1);
}

void Misc_string_view_compare_w_string(void) {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "bar";

    flecs::string str_1 = flecs::string_view(ptr_1);
    flecs::string str_2 = flecs::string_view(ptr_2);

    flecs::string str_3 = flecs::string(ecs_os_strdup(ptr_1));
    flecs::string str_4 = flecs::string(ecs_os_strdup(ptr_2));

    test_assert(str_1 == str_1);
    test_assert(str_1 == str_3);

    test_assert(str_2 == str_2);
    test_assert(str_2 == str_4);

    test_assert(str_1 != str_2);
    test_assert(str_2 != str_1);
}

void Misc_string_compare_nullptr(void) {
    const char *ptr_1 = "foo";

    flecs::string str = flecs::string_view(ptr_1);

    test_assert(str != nullptr);
}

void Misc_nullptr_string_compare(void) {
    const char *ptr = "foo";

    flecs::string str_1 = flecs::string_view(nullptr);
    flecs::string str_2 = flecs::string_view(ptr);

    test_assert(str_1 == "");
    test_assert(str_1 != ptr);
    test_assert(str_1 != str_2);
}

void Misc_nullptr_string_compare_nullptr(void) {
    const char *ptr_1 = "foo";

    flecs::string str = flecs::string_view(nullptr);

    test_assert(str == "");
    test_assert(str != ptr_1);
}

static ECS_COMPONENT_DECLARE(Velocity);
static ECS_DECLARE(TagB);
static ECS_DECLARE(E2);
static int sys_invoked_count = 0;
static int obs_invoked_count = 0;
static int trig_invoked_count = 0;

void Sys(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    test_assert(p != NULL);
    test_assert(v != NULL);

    sys_invoked_count += it->count;
}

void Obs(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    test_assert(p != NULL);
    test_assert(v != NULL);

    obs_invoked_count += it->count;
}

void Trig(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);

    test_assert(p != NULL);

    trig_invoked_count += it->count;
}

void Misc_c_macros(void) {
    flecs::world world;

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ECS_TAG(world, TagA);
    ECS_TAG_DEFINE(world, TagB);

    ECS_ENTITY(world, E1, 0);
    ECS_ENTITY_DEFINE(world, E2, 0);

    ECS_MODULE(world, MyModule);
    ECS_SYSTEM(world, Sys, EcsOnUpdate, Position, Velocity);
    ECS_OBSERVER(world, Obs, EcsOnAdd, Position, Velocity);
    ECS_OBSERVER(world, Trig, EcsOnAdd, Position);
    ECS_PIPELINE(world, Pip, flecs.system.System, TagA);

    test_assert(ecs_id(Position) != 0);
    test_assert(ecs_id(Velocity) != 0);

    test_assert(TagA != 0);
    test_assert(TagB != 0);

    test_assert(E1 != 0);
    test_assert(E2 != 0);

    test_assert(ecs_id(MyModule) != 0);
    test_assert(ecs_id(Sys) != 0);
    test_assert(ecs_id(Obs) != 0);
    test_assert(ecs_id(Trig) != 0);
    test_assert(Pip != 0);

    ecs_add(world, E1, TagA);
    ecs_add(world, E1, TagB);

    test_assert( ecs_has(world, E1, TagA));
    test_assert( ecs_has(world, E1, TagB));

    ecs_add(world, E1, Position);
    test_int(obs_invoked_count, 0);
    test_int(trig_invoked_count, 1);
    ecs_add(world, E1, Velocity);
    test_int(obs_invoked_count, 1);

    test_assert( ecs_has(world, E1, Position));
    test_assert( ecs_has(world, E1, Velocity));

    test_assert( ecs_has_id(world, ecs_id(MyModule), EcsModule));
    test_assert( ecs_has(world, Pip, EcsPipeline));
    
    ecs_progress(world, 0);

    test_int(sys_invoked_count, 1);
}

void Misc_app_run(void) {
    flecs::world ecs;

    bool system_ran = false;
    ecs.system().run([&](flecs::iter& it) {
        while (it.next()) {
            system_ran = true;
            it.world().quit();
        }
    });

    test_int(ecs.app().run(), 0);
    test_bool(system_ran, true);
}

void Misc_app_run_target_fps(void) {
    flecs::world ecs;

    int32_t count = 0;
    ecs.system().run([&](flecs::iter& it) {
        while (it.next()) {
            count ++;
            test_int(it.world().get_info()->target_fps, 10);
            it.world().quit();
        }
    });

    ecs.set_target_fps(10);

    ecs.app().run();

    test_int(count, 1);
}

void Misc_app_run_set_frames(void) {
    flecs::world ecs;

    int32_t count = 0;
    ecs.system().run([&](flecs::iter& it) {
        while (it.next()) {
            count ++;
        }
    });

    ecs.app().frames(100).run();

    test_int(count, 100);
}

void Misc_app_on_remove_on_fini(void) {
    flecs::world ecs;

    ecs.entity().set<Position>({10, 20});

    int invoked = 0;
    ecs.observer<Position>()
        .event(flecs::OnRemove)
        .each([&](Position& p) {
            invoked ++;
            test_int(p.x, 10);
            test_int(p.y, 20);
        });

    test_int(invoked, 0);
    ecs.app().frames(1).run();
    test_int(invoked, 1);
}

void Misc_member_gauge_metric(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    ecs.metric("metrics::position_y")
        .kind<flecs::metrics::Gauge>()
        .member<Position>("y");

    flecs::entity e1 = ecs.entity().set<Position>({10, 20});
    flecs::entity e2 = ecs.entity().set<Position>({20, 30});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 20);
                test_int(i[1].value, 30);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_id_gauge_metric(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.metric("metrics::has_foo")
        .kind<flecs::metrics::Gauge>()
        .id<Tag>();

    flecs::entity e1 = ecs.entity().add<Tag>();
    flecs::entity e2 = ecs.entity().add<Tag>();

    ecs.progress(1.0);

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();
                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 1);
                test_int(i[1].value, 1);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    ecs.progress(1.0);
    test_int(count, 2);
    count = 0;

    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();
                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 1);
                test_int(i[1].value, 1);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_oneof_gauge_metric(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    struct ColorMetric {
        double value[3];
    };

    flecs::entity m = ecs.metric("metrics::has_oneof")
        .kind<flecs::metrics::Gauge>()
        .id<Color>(flecs::Wildcard)
        .targets();

    flecs::entity e1 = ecs.entity().add(Color::Red);
    flecs::entity e2 = ecs.entity().add(Color::Green);
    flecs::entity e3 = ecs.entity().add(Color::Blue);

    ecs.progress(1.0);

    int32_t count = 0;
    ecs.query<flecs::metrics::Source>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                ColorMetric *i = static_cast<ColorMetric*>(it.range().get(m, ecs.id<flecs::metrics::Value>()));
                test_assert(i != nullptr);

                count += it.count();
                test_int(count, 3);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);
                test_uint(s[2].entity, e3);

                test_str(ecs.to_json(m, &i[0]), "{\"red\":1, \"green\":0, \"blue\":0}");
                test_str(ecs.to_json(m, &i[1]), "{\"red\":0, \"green\":1, \"blue\":0}");
                test_str(ecs.to_json(m, &i[2]), "{\"red\":0, \"green\":0, \"blue\":1}");

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
                test_assert(it.entity(2).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 3);
    count = 0;

    ecs.query<flecs::metrics::Source>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                ColorMetric *i = static_cast<ColorMetric*>(it.range().get(m, ecs.id<flecs::metrics::Value>()));
                test_assert(i != nullptr);

                count += it.count();
                test_int(count, 3);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);
                test_uint(s[2].entity, e3);

                test_str(ecs.to_json(m, &i[0]), "{\"red\":1, \"green\":0, \"blue\":0}");
                test_str(ecs.to_json(m, &i[1]), "{\"red\":0, \"green\":1, \"blue\":0}");
                test_str(ecs.to_json(m, &i[2]), "{\"red\":0, \"green\":0, \"blue\":1}");

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
                test_assert(it.entity(2).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 3);
}

void Misc_id_counter_metric(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.metric("metrics::has_foo")
        .kind<flecs::metrics::Counter>()
        .id<Tag>();

    flecs::entity e1 = ecs.entity().add<Tag>();
    flecs::entity e2 = ecs.entity().add<Tag>();

    ecs.progress(1.0);

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();
                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 1);
                test_int(i[1].value, 1);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    ecs.progress(1.0);
    test_int(count, 2);
    count = 0;

    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();
                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 2);
                test_int(i[1].value, 2);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_oneof_counter_metric(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    struct ColorMetric {
        double value[3];
    };

    flecs::entity m = ecs.metric("metrics::has_oneof")
        .kind<flecs::metrics::Counter>()
        .id<Color>(flecs::Wildcard)
        .targets();

    flecs::entity e1 = ecs.entity().add(Color::Red);
    flecs::entity e2 = ecs.entity().add(Color::Green);
    flecs::entity e3 = ecs.entity().add(Color::Blue);

    ecs.progress(1.0);

    int32_t count = 0;
    ecs.query<flecs::metrics::Source>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                ColorMetric *i = static_cast<ColorMetric*>(it.range().get(m, ecs.id<flecs::metrics::Value>()));
                test_assert(i != nullptr);

                count += it.count();
                test_int(count, 3);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);
                test_uint(s[2].entity, e3);

                test_str(ecs.to_json(m, &i[0]), "{\"red\":1, \"green\":0, \"blue\":0}");
                test_str(ecs.to_json(m, &i[1]), "{\"red\":0, \"green\":1, \"blue\":0}");
                test_str(ecs.to_json(m, &i[2]), "{\"red\":0, \"green\":0, \"blue\":1}");

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
                test_assert(it.entity(2).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 3);
    ecs.progress(1.0);
    count = 0;

    ecs.query<flecs::metrics::Source>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                ColorMetric *i = static_cast<ColorMetric*>(it.range().get(m, ecs.id<flecs::metrics::Value>()));
                test_assert(i != nullptr);

                count += it.count();
                test_int(count, 3);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);
                test_uint(s[2].entity, e3);

                test_str(ecs.to_json(m, &i[0]), "{\"red\":2, \"green\":0, \"blue\":0}");
                test_str(ecs.to_json(m, &i[1]), "{\"red\":0, \"green\":2, \"blue\":0}");
                test_str(ecs.to_json(m, &i[2]), "{\"red\":0, \"green\":0, \"blue\":2}");

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
                test_assert(it.entity(2).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 3);
}

void Misc_component_mixin_member_metric(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y").metric<flecs::metrics::Gauge>();

    flecs::entity e1 = ecs.entity().set<Position>({10, 20});
    flecs::entity e2 = ecs.entity().set<Position>({20, 30});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 20);
                test_int(i[1].value, 30);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_component_mixin_member_metric_custom_parent_entity(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    flecs::entity parent = ecs.entity();

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y").metric<flecs::metrics::Gauge>(parent);

    test_assert(parent.lookup("y") != 0);

    flecs::entity e1 = ecs.entity().set<Position>({10, 20});
    flecs::entity e2 = ecs.entity().set<Position>({20, 30});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 20);
                test_int(i[1].value, 30);

                test_assert(it.entity(0).parent().parent() == parent);
                test_assert(it.entity(1).parent().parent() == parent);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_metric_description(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity m = ecs.metric("metrics::position_y")
        .kind<flecs::metrics::Gauge>()
        .member<Position>("y")
        .brief("Position y");

    test_str(m.doc_brief(), "Position y");

    flecs::entity e1 = ecs.entity().set<Position>({10, 20});
    flecs::entity e2 = ecs.entity().set<Position>({20, 30});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 20);
                test_int(i[1].value, 30);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_component_mixin_member_metric_description(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y").metric<flecs::metrics::Gauge>(0, "Position y");

    flecs::entity m = ecs.entity("Position::y");
    test_str(m.doc_brief(), "Position y");

    flecs::entity e1 = ecs.entity().set<Position>({10, 20});
    flecs::entity e2 = ecs.entity().set<Position>({20, 30});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 20);
                test_int(i[1].value, 30);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_member_metric_w_value_name(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    flecs::entity parent = ecs.entity();

    ecs.component<Mass>()
        .member<float>("value").metric<flecs::metrics::Gauge>(parent);

    test_assert(parent.lookup("value") == 0);
    test_assert(parent.lookup("mass") != 0);

    flecs::entity e1 = ecs.entity().set<Mass>({10});
    flecs::entity e2 = ecs.entity().set<Mass>({20});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 10);
                test_int(i[1].value, 20);

                test_assert(it.entity(0).parent().parent() == parent);
                test_assert(it.entity(1).parent().parent() == parent);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

struct FooBar {
    float value;
};

void Misc_member_metric_w_value_name_camel_case_type(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    flecs::entity parent = ecs.entity();

    ecs.component<FooBar>()
        .member<float>("value").metric<flecs::metrics::Gauge>(parent);

    test_assert(parent.lookup("value") == 0);
    test_assert(parent.lookup("foo_bar") != 0);

    flecs::entity e1 = ecs.entity().set<FooBar>({10});
    flecs::entity e2 = ecs.entity().set<FooBar>({20});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 10);
                test_int(i[1].value, 20);

                test_assert(it.entity(0).parent().parent() == parent);
                test_assert(it.entity(1).parent().parent() == parent);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_member_metric_w_custom_name(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    flecs::entity parent = ecs.entity();

    ecs.component<FooBar>()
        .member<float>("value").metric<flecs::metrics::Gauge>(parent, nullptr, "custom_name");

    test_assert(parent.lookup("value") == 0);
    test_assert(parent.lookup("custom_name") != 0);

    flecs::entity e1 = ecs.entity().set<FooBar>({10});
    flecs::entity e2 = ecs.entity().set<FooBar>({20});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 10);
                test_int(i[1].value, 20);

                test_assert(it.entity(0).parent().parent() == parent);
                test_assert(it.entity(1).parent().parent() == parent);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_dotmember_metric(void) {
    flecs::world ecs;

    struct Point {
        float x;
        float y;
    };

    struct Position {
        float dummy;
        Point position;
    };

    ecs.import<flecs::metrics>();

    ecs.component<Point>()
        .member<float>("x")
        .member<float>("y");

    ecs.component<Position>()
        .member<float>("dummy")
        .member<Point>("position");

    ecs.metric("metrics::position_y")
        .kind<flecs::metrics::Gauge>()
        .dotmember<Position>("position.y");

    flecs::entity e1 = ecs.entity().set<Position>({10, {20, 30}});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 1);
                test_uint(s[0].entity, e1);

                test_int(i[0].value, 30);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 1);
}

void Misc_counter_id_metric(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    flecs::entity m = ecs.metric("metrics.position")
        .kind<flecs::metrics::CounterId>()
        .id<Position>();

    ecs.entity().set<Position>({10, 20});
    
    ecs.progress(1.0);

    {
        const flecs::metrics::Value *v = m.get<flecs::metrics::Value>();
        test_assert(v != nullptr);
        test_int(v->value, 1);
    }

    ecs.entity().set<Position>({10, 20});
    ecs.entity().set<Position>({10, 20});

    ecs.progress(1.0);

    {
        const flecs::metrics::Value *v = m.get<flecs::metrics::Value>();
        test_assert(v != nullptr);
        test_int(v->value, 4);
    }
}

void Misc_counter_target_metric(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    flecs::entity m = ecs.metric("metrics::color")
        .kind<flecs::metrics::CounterId>()
        .id<Color>(flecs::Wildcard)
        .targets();
    test_assert(m != 0);

    ecs.entity().add(Color::Red);
    ecs.entity().add(Color::Green);
    ecs.entity().add(Color::Blue);
    ecs.entity().add(Color::Blue);

    ecs.progress(1.0);

    {
        flecs::entity red = ecs.lookup("metrics::color::Red");
        test_assert(red != 0);
        const flecs::metrics::Value *v = red.get<flecs::metrics::Value>();
        test_assert(v != nullptr);
        test_int(v->value, 1);
    }

    {
        flecs::entity green = ecs.lookup("metrics::color::Green");
        test_assert(green != 0);
        const flecs::metrics::Value *v = green.get<flecs::metrics::Value>();
        test_assert(v != nullptr);
        test_int(v->value, 1);
    }

    {
        flecs::entity blue = ecs.lookup("metrics::color::Blue");
        test_assert(blue != 0);
        const flecs::metrics::Value *v = blue.get<flecs::metrics::Value>();
        test_assert(v != nullptr);
        test_int(v->value, 2);
    }
}

void Misc_alert(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto e1 = ecs.entity("e1").set<Position>({10, 20});
    auto e2 = ecs.entity("e2").set<Position>({10, 20});
    e1.set<Velocity>({1, 2});

    ecs.progress(1.0);

    test_assert(!e1.has<flecs::alerts::AlertsActive>());
    test_assert(e2.has<flecs::alerts::AlertsActive>());
    test_int(e2.alert_count(), 1);
    test_int(e2.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e2);
        test_assert(ai.parent() == a);
    }

    e2.add<Velocity>();

    ecs.progress(1.0);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai == 0);
    }
}

void Misc_alert_w_message(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .message("$this has position but not velocity")
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto e1 = ecs.entity("e1").set<Position>({10, 20});
    auto e2 = ecs.entity("e2").set<Position>({10, 20});
    e1.set<Velocity>({1, 2});

    ecs.progress(1.0);

    test_assert(!e1.has<flecs::alerts::AlertsActive>());
    test_assert(e2.has<flecs::alerts::AlertsActive>());
    test_int(e2.alert_count(), 1);
    test_int(e2.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e2);
        test_assert(ai.parent() == a);
        test_str(ai.get<flecs::alerts::Instance>()->message, 
            "e2 has position but not velocity");
    }

    e2.add<Velocity>();

    ecs.progress(1.0);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai == 0);
    }
}

void Misc_alert_w_brief(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("has_position")
        .with<Position>()
        .brief("Entity has Position")
        .build();

    test_assert(a != 0);
    test_str(a.name().c_str(), "has_position");
    test_str(a.doc_brief(), "Entity has Position");
}

void Misc_alert_doc_name(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("has_position")
        .with<Position>()
        .doc_name("Has Position")
        .build();

    test_assert(a != 0);
    test_str(a.name().c_str(), "has_position");
    test_str(a.doc_name(), "Has Position");
}

void Misc_alert_severity_info(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("has_position")
        .with<Position>()
        .severity<flecs::alerts::Info>()
        .build();

    test_assert(a != 0);
    test_str(a.name().c_str(), "has_position");
    test_assert(a.target<flecs::alerts::Alert>() == ecs.id<flecs::alerts::Info>());
}

void Misc_alert_severity_warning(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("has_position")
        .with<Position>()
        .severity<flecs::alerts::Warning>()
        .build();

    test_assert(a != 0);
    test_str(a.name().c_str(), "has_position");
    test_assert(a.target<flecs::alerts::Alert>() == ecs.id<flecs::alerts::Warning>());
}

void Misc_alert_severity_error(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("has_position")
        .with<Position>()
        .severity<flecs::alerts::Error>()
        .build();

    test_assert(a != 0);
    test_str(a.name().c_str(), "has_position");
    test_assert(a.target<flecs::alerts::Alert>() == ecs.id<flecs::alerts::Error>());
}

void Misc_alert_severity_implicit(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("has_position")
        .with<Position>()
        .build();

    test_assert(a != 0);
    test_str(a.name().c_str(), "has_position");
    test_assert(a.target<flecs::alerts::Alert>() == ecs.id<flecs::alerts::Error>());
}

void Misc_alert_w_retain_period(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .retain_period(1.0)
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto e1 = ecs.entity("e1").set<Position>({10, 20});
    auto e2 = ecs.entity("e2").set<Position>({10, 20});
    e1.set<Velocity>({1, 2});

    ecs.progress(1.0);

    test_assert(!e1.has<flecs::alerts::AlertsActive>());
    test_assert(e2.has<flecs::alerts::AlertsActive>());
    test_int(e2.alert_count(), 1);
    test_int(e2.alert_count(a), 1);

    flecs::entity ai;
    {
        ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e2);
        test_assert(ai.parent() == a);
    }

    e2.add<Velocity>();
    ecs.progress(1.0);

    test_assert(ai.is_alive());
    test_assert(!e1.has<flecs::alerts::AlertsActive>());
    test_assert(!e2.has<flecs::alerts::AlertsActive>());
    test_int(e2.alert_count(), 0);
    test_int(e2.alert_count(a), 0);

    ecs.progress(1.0);

    test_assert(!ai.is_alive());
    test_assert(!e1.has<flecs::alerts::AlertsActive>());
    test_assert(!e2.has<flecs::alerts::AlertsActive>());
    test_int(e2.alert_count(), 0);
    test_int(e2.alert_count(a), 0);
}

void Misc_alert_w_severity_filter(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .severity_filter(ecs.id<flecs::alerts::Warning>(), ecs.id<Mass>())
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto e1 = ecs.entity("e1").set<Position>({10, 20});

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }

    e1.add<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Warning>()));
        test_assert(ai.parent() == a);
    }

    e1.remove<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }
}

void Misc_alert_w_severity_filter_severity_type(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .severity_filter<flecs::alerts::Warning>(ecs.id<Mass>())
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto e1 = ecs.entity("e1").set<Position>({10, 20});

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }

    e1.add<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Warning>()));
        test_assert(ai.parent() == a);
    }

    e1.remove<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }
}

void Misc_alert_w_severity_filter_severity_type_id_type(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .severity_filter<flecs::alerts::Warning, Mass>()
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto e1 = ecs.entity("e1").set<Position>({10, 20});

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }

    e1.add<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Warning>()));
        test_assert(ai.parent() == a);
    }

    e1.remove<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }
}

void Misc_alert_w_severity_filter_severity_type_enum_constant(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .severity_filter<flecs::alerts::Warning>(Green)
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto e1 = ecs.entity("e1").set<Position>({10, 20});

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }

    e1.add(Green);

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Warning>()));
        test_assert(ai.parent() == a);
    }

    e1.add(Blue);

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }

    e1.remove<Color>(flecs::Wildcard);

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }
}

void Misc_alert_w_severity_filter_w_var(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .with(flecs::ChildOf).second("$parent")
        .severity_filter(ecs.id<flecs::alerts::Warning>(), ecs.id<Mass>(), "parent")
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto p = ecs.entity();
    auto e1 = ecs.entity("e1").set<Position>({10, 20}).add<Mass>().child_of(p);

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }

    p.add<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Warning>()));
        test_assert(ai.parent() == a);
    }

    p.remove<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }
}

void Misc_alert_w_severity_filter_severity_type_w_var(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .with(flecs::ChildOf).second("$parent")
        .severity_filter<flecs::alerts::Warning>(ecs.id<Mass>(), "parent")
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto p = ecs.entity();
    auto e1 = ecs.entity("e1").set<Position>({10, 20}).add<Mass>().child_of(p);

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }

    p.add<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Warning>()));
        test_assert(ai.parent() == a);
    }

    p.remove<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }
}

void Misc_alert_w_severity_filter_severity_type_id_type_w_var(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .with(flecs::ChildOf).second("$parent")
        .severity_filter<flecs::alerts::Warning, Mass>("parent")
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto p = ecs.entity();
    auto e1 = ecs.entity("e1").set<Position>({10, 20}).add<Mass>().child_of(p);

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }

    p.add<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Warning>()));
        test_assert(ai.parent() == a);
    }

    p.remove<Mass>();

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }
}

void Misc_alert_w_severity_filter_severity_type_enum_constant_w_var(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    flecs::entity a = ecs.alert("position_without_velocity")
        .with<Position>()
        .without<Velocity>()
        .with(flecs::ChildOf).second("$parent")
        .severity_filter<flecs::alerts::Warning>(Color::Green, "parent")
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "position_without_velocity");

    auto p = ecs.entity();
    auto e1 = ecs.entity("e1").set<Position>({10, 20}).add(Color::Green).child_of(p);

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }

    p.add(Color::Green);

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Warning>()));
        test_assert(ai.parent() == a);
    }

    p.remove<Color>(flecs::Wildcard);

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert((ai.has<flecs::alerts::Alert, flecs::alerts::Error>()));
        test_assert(ai.parent() == a);
    }
}

void Misc_alert_for_member_range(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    ecs.component<Mass>()
        .member<float>("value")
            .error_range(0, 100);

    flecs::entity a = ecs.alert("high_mass")
        .with<Mass>()
        .message("$this has high mass")
        .member<Mass>("value")
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "high_mass");

    auto e1 = ecs.entity("e1").set<Mass>({25});

    ecs.progress(1.0);

    test_assert(!e1.has<flecs::alerts::AlertsActive>());

    e1.set<Mass>({150});

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert(ai.parent() == a);
        test_str(ai.get<flecs::alerts::Instance>()->message, 
            "e1 has high mass");
    }

    e1.set<Mass>({0});

    ecs.progress(1.0);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai == 0);
    }
}

void Misc_alert_w_member_range_from_var(void) {
    flecs::world ecs;

    ecs.import<flecs::alerts>();

    ecs.component<Mass>()
        .member<float>("value")
            .error_range(0, 100);

    flecs::entity a = ecs.alert("high_parent_mass")
        .with(flecs::ChildOf).second("$parent")
        .with<Mass>().src("$parent")
        .message("$this has high mass")
        .member<Mass>("value", "parent")
        .build();
    test_assert(a != 0);
    test_str(a.name().c_str(), "high_parent_mass");

    auto p = ecs.entity().set<Mass>({25});
    auto e1 = ecs.entity("e1").child_of(p);

    ecs.progress(1.0);

    test_assert(!e1.has<flecs::alerts::AlertsActive>());

    p.set<Mass>({150});

    ecs.progress(1.0);

    test_assert(e1.has<flecs::alerts::AlertsActive>());
    test_int(e1.alert_count(), 1);
    test_int(e1.alert_count(a), 1);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai != 0);
        test_assert(ai.has<flecs::alerts::Instance>());
        test_assert(ai.has<flecs::metrics::Source>());
        test_assert(ai.get<flecs::metrics::Source>()->entity == e1);
        test_assert(ai.parent() == a);
    }

    p.set<Mass>({0});

    ecs.progress(1.0);

    {
        flecs::entity ai = ecs.query_builder()
            .with<flecs::alerts::Instance>()
            .build()
            .first();
        test_assert(ai == 0);
    }
}

void Misc_map_api(void) {
    flecs::world ecs;

    ecs_map_t m;
    ecs_map_init(&m, 0);
    test_int(0, ecs_map_count(&m));

    int32_t *v = ecs_map_insert_alloc_t(&m, int32_t, 10);
    test_assert(v != NULL);

    test_assert(ecs_map_ensure_alloc_t(&m, int32_t, 10) == v);
    test_assert(ecs_map_get_deref(&m, int32_t, 10) == v);
 
    int32_t w;
    ecs_map_insert_ptr(&m, 20, &w);
    test_assert(ecs_map_ensure_alloc_t(&m, int32_t, 20) == &w);
    test_assert(ecs_map_get_deref(&m, int32_t, 20) == &w);
    
    test_assert(ecs_map_count(&m) == 2);

    ecs_map_iter_t it = ecs_map_iter(&m);
    test_bool(true, ecs_map_next(&it));
    test_uint(20, ecs_map_key(&it));
    test_assert(&w == ecs_map_ptr(&it));

    test_bool(true, ecs_map_next(&it));
    test_uint(10, ecs_map_key(&it));
    test_assert(v == ecs_map_ptr(&it));
    test_bool(false, ecs_map_next(&it));

    test_assert(ecs_map_remove_ptr(&m, 10) == v);
    test_assert(ecs_map_remove_ptr(&m, 20) == &w);

    ecs_os_free(v);

    ecs_map_fini(&m);
}

void Misc_member_metric_w_pair_R_T(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.component<Mass>()
        .member<float>("value");

    flecs::entity m = ecs.metric("mass")
        .kind<flecs::metrics::Gauge>()
        .id<Mass, Tag>()
        .member("value");

    flecs::entity e1 = ecs.entity().set<Mass, Tag>({10});
    flecs::entity e2 = ecs.entity().set<Mass, Tag>({20});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 10);
                test_int(i[1].value, 20);

                test_assert(it.entity(0).parent() == m);
                test_assert(it.entity(1).parent() == m);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_member_metric_w_pair_R_t(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.component<Mass>()
        .member<float>("value");

    flecs::entity m = ecs.metric("mass")
        .kind<flecs::metrics::Gauge>()
        .id<Mass>(ecs.id<Tag>())
        .member("value");

    flecs::entity e1 = ecs.entity().set<Mass, Tag>({10});
    flecs::entity e2 = ecs.entity().set<Mass, Tag>({20});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 10);
                test_int(i[1].value, 20);

                test_assert(it.entity(0).parent() == m);
                test_assert(it.entity(1).parent() == m);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_member_metric_w_pair_r_t(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.component<Mass>()
        .member<float>("value");

    flecs::entity m = ecs.metric("mass")
        .kind<flecs::metrics::Gauge>()
        .id(ecs.id<Mass>(), ecs.id<Tag>())
        .member("value");

    flecs::entity e1 = ecs.entity().set<Mass, Tag>({10});
    flecs::entity e2 = ecs.entity().set<Mass, Tag>({20});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 10);
                test_int(i[1].value, 20);

                test_assert(it.entity(0).parent() == m);
                test_assert(it.entity(1).parent() == m);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}

void Misc_member_metric_w_pair_r_T(void) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs.component<Mass>()
        .member<float>("value");

    flecs::entity m = ecs.metric("mass")
        .kind<flecs::metrics::Gauge>()
        .id_second<Tag>(ecs.id<Mass>())
        .member("value");

    flecs::entity e1 = ecs.entity().set<Mass, Tag>({10});
    flecs::entity e2 = ecs.entity().set<Mass, Tag>({20});

    ecs.progress();

    int32_t count = 0;
    ecs.query<flecs::metrics::Source, flecs::metrics::Value>()
        .run([&](flecs::iter& it) {
            while (it.next()) {
                auto s = it.field<flecs::metrics::Source>(0);
                auto i = it.field<flecs::metrics::Value>(1);
                count += it.count();

                test_int(count, 2);
                test_uint(s[0].entity, e1);
                test_uint(s[1].entity, e2);

                test_int(i[0].value, 10);
                test_int(i[1].value, 20);

                test_assert(it.entity(0).parent() == m);
                test_assert(it.entity(1).parent() == m);

                test_assert(it.entity(0).has<flecs::metrics::Instance>());
                test_assert(it.entity(1).has<flecs::metrics::Instance>());
            }
        });

    test_int(count, 2);
}
