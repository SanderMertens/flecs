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
