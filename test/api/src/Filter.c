#include <api.h>

void Filter_filter_1_term() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{TagA}}
    });

    test_int(f.term_count, 1);
    test_int(f.term_count_actual, 1);
    test_assert(f.terms != NULL);
    test_int(f.terms[0].id, TagA);
    test_int(f.terms[0].oper, EcsAnd);
    test_int(f.terms[0].index, 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_2_terms() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{TagA}, {TagB}}
    });

    test_int(f.term_count, 2);
    test_int(f.term_count_actual, 2);
    test_assert(f.terms != NULL);

    test_int(f.terms[0].id, TagA);
    test_int(f.terms[0].oper, EcsAnd);
    test_int(f.terms[0].index, 0);
    test_int(f.terms[1].id, TagB);
    test_int(f.terms[1].oper, EcsAnd);
    test_int(f.terms[1].index, 1);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_3_terms() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{TagA}, {TagB}, {TagC}}
    });

    test_int(f.term_count, 3);
    test_int(f.term_count_actual, 3);
    test_assert(f.terms != NULL);

    test_int(f.terms[0].id, TagA);
    test_int(f.terms[0].oper, EcsAnd);
    test_int(f.terms[0].index, 0);

    test_int(f.terms[1].id, TagB);
    test_int(f.terms[1].oper, EcsAnd);
    test_int(f.terms[1].index, 1);

    test_int(f.terms[2].id, TagC);
    test_int(f.terms[2].oper, EcsAnd);
    test_int(f.terms[2].index, 2);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_3_terms_w_or() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{TagA}, {TagB, .oper = EcsOr}, {TagC, .oper = EcsOr}}
    });

    test_int(f.term_count, 3);
    test_int(f.term_count_actual, 2);
    test_assert(f.terms != NULL);

    test_int(f.terms[0].id, TagA);
    test_int(f.terms[0].oper, EcsAnd);
    test_int(f.terms[0].index, 0);

    test_int(f.terms[1].id, TagB);
    test_int(f.terms[1].oper, EcsOr);
    test_int(f.terms[1].index, 1);

    test_int(f.terms[2].id, TagC);
    test_int(f.terms[2].oper, EcsOr);
    test_int(f.terms[2].index, 1);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_4_terms_w_or_at_1() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{TagA}, {TagB, .oper = EcsOr}, {TagC, .oper = EcsOr}, {TagD}}
    });

    test_int(f.term_count, 4);
    test_int(f.term_count_actual, 3);
    test_assert(f.terms != NULL);

    test_int(f.terms[0].id, TagA);
    test_int(f.terms[0].oper, EcsAnd);
    test_int(f.terms[0].index, 0);

    test_int(f.terms[1].id, TagB);
    test_int(f.terms[1].oper, EcsOr);
    test_int(f.terms[1].index, 1);

    test_int(f.terms[2].id, TagC);
    test_int(f.terms[2].oper, EcsOr);
    test_int(f.terms[2].index, 1);

    test_int(f.terms[3].id, TagD);
    test_int(f.terms[3].oper, EcsAnd);
    test_int(f.terms[3].index, 2);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_w_pair_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_id_t pair = ecs_pair(Rel, Obj);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{pair}}
    });

    test_int(f.term_count, 1);
    test_int(f.term_count_actual, 1);
    test_assert(f.terms != NULL);
    test_int(f.terms[0].id, pair);
    test_int(f.terms[0].oper, EcsAnd);
    test_int(f.terms[0].index, 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_w_pred_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_id_t pair = ecs_pair(Rel, Obj);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{.pred.entity = Rel, .args[1].entity = Obj}}
    });

    test_int(f.term_count, 1);
    test_int(f.term_count_actual, 1);
    test_assert(f.terms != NULL);
    test_int(f.terms[0].id, pair);
    test_int(f.terms[0].oper, EcsAnd);
    test_int(f.terms[0].index, 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_term_w_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_term_t term = {
        .id = Tag
    };

    test_assert(ecs_term_finalize(world, NULL, NULL, &term) == 0);
    test_int(term.id, Tag);
    test_int(term.pred.entity, Tag);
    test_int(term.args[0].entity, EcsThis);
    test_int(term.args[1].entity, 0);

    ecs_fini(world);
}

void Filter_term_w_pair_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_id_t pair = ecs_pair(Rel, Obj);

    ecs_term_t term = {
        .id = pair
    };

    test_assert(ecs_term_finalize(world, NULL, NULL, &term) == 0);
    test_int(term.id, pair);
    test_int(term.pred.entity, Rel);
    test_int(term.args[0].entity, EcsThis);
    test_int(term.args[1].entity, Obj);

    ecs_fini(world);
}

void Filter_term_w_pred_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_id_t pair = ecs_pair(Rel, Obj);

    ecs_term_t term = {
        .pred.entity = Rel,
        .args[1].entity = Obj
    };

    test_assert(ecs_term_finalize(world, NULL, NULL, &term) == 0);
    test_int(term.id, pair);
    test_int(term.pred.entity, Rel);
    test_int(term.args[1].entity, Obj);

    ecs_fini(world);
}

void Filter_term_w_pair_finalize_twice() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_id_t pair = ecs_pair(Rel, Obj);

    ecs_term_t term = {
        .pred.entity = Rel,
        .args[1].entity = Obj
    };

    test_assert(ecs_term_finalize(world, NULL, NULL, &term) == 0);
    test_int(term.id, pair);
    test_int(term.pred.entity, Rel);
    test_int(term.args[1].entity, Obj);

    test_assert(ecs_term_finalize(world, NULL, NULL, &term) == 0);
    test_int(term.id, pair);
    test_int(term.pred.entity, Rel);
    test_int(term.args[1].entity, Obj);

    ecs_fini(world);
}

void Filter_term_w_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_term_t term = {
        .id = Tag,
        .role = ECS_SWITCH
    };

    test_assert(ecs_term_finalize(world, NULL, NULL, &term) == 0);
    test_int(term.id, Tag | ECS_SWITCH);
    test_int(term.role, ECS_SWITCH);
    test_int(term.pred.entity, Tag);

    ecs_fini(world);
}

void Filter_term_w_pred_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_term_t term = {
        .pred.entity = Tag,
        .role = ECS_SWITCH
    };

    test_assert(ecs_term_finalize(world, NULL, NULL, &term) == 0);
    test_int(term.id, Tag | ECS_SWITCH);
    test_int(term.role, ECS_SWITCH);
    test_int(term.pred.entity, Tag);

    ecs_fini(world);
}
