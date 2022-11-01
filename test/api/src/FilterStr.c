#include <api.h>

void FilterStr_one_term() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ TagA }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "TagA");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_inout() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {
            { TagA, .inout = EcsIn }
        }
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "[in] TagA");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_two_terms() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {
            { TagA },
            { TagB }
        }
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "TagA, TagB");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_two_terms_w_inout() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {
            { TagA },
            { TagB, .inout = EcsIn }
        }
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "TagA, [in] TagB");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_three_terms_w_or() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {
            { TagA },
            { TagB, .oper = EcsOr },
            { TagC, .oper = EcsOr }
        }
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "TagA, TagB || TagC");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_three_terms_w_or_inout() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {
            { TagA },
            { TagB, .oper = EcsOr, .inout = EcsIn },
            { TagC, .oper = EcsOr, .inout = EcsIn }
        }
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "TagA, [in] TagB || TagC");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_four_terms_three_w_or_inout() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {
            { TagA },
            { TagB, .oper = EcsOr, .inout = EcsIn },
            { TagC, .oper = EcsOr, .inout = EcsIn },
            { TagD, .inout = EcsIn }
        }
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "TagA, [in] TagB || TagC, [in] TagD");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .id = ecs_pair(Rel, Tgt) }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "(Rel,Tgt)");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_pair_entity_src() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Src);
    ECS_TAG(world, Tgt);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .id = ecs_pair(Rel, Tgt), .src.id = Src }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "Rel(Src,Tgt)");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .id = Foo, .src.flags = EcsSelf }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "Foo(self)");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_up() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .id = Foo, .src.flags = EcsUp }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "Foo(up)");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_0() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .id = Foo, .src.flags = EcsIsEntity }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "Foo()");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_singleton() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .id = Foo, .src.id = Foo }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "Foo($)");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_final_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, Final);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .first.id = Foo }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "Foo");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_final_dont_inherit_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, Final, DontInherit);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .first.id = Foo }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "Foo");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_src_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ Tag, .src.flags = EcsIsVariable, .src.name = "Var" }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "Tag($Var)");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_first_var() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .first.flags = EcsIsVariable, .first.name = "Var" }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "$Var");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_second_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .first.id = Rel, .second.flags = EcsIsVariable, .second.name = "Var" }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "(Rel,$Var)");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_first_var_entity_src() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Src);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ .first.flags = EcsIsVariable, .first.name = "Var", .src.id = Src }}
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "$Var(Src)");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterStr_one_term_w_pair_w_0_entity() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {
            { .first.id = Rel, .second.id = Tgt, .src = {
                .id = 0, .flags = EcsIsEntity
            } }
        }
    }));

    char *str = ecs_filter_str(world, &f);
    test_str(str, "Rel(0,Tgt)");
    ecs_os_free(str);

    ecs_filter_fini(&f);

    ecs_fini(world);
}
