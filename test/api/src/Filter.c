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

void Filter_filter_move() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_filter_t f_1;
    test_int(ecs_filter_init(world, &f_1, &(ecs_filter_desc_t) {
        .terms = {{TagA}, {TagB}}
    }), 0);

    test_int(f_1.term_count, 2);
    test_assert(f_1.terms != NULL);
    test_int(f_1.terms[0].id, TagA);
    test_int(f_1.terms[1].id, TagB);

    ecs_filter_t f_2 = {};
    ecs_filter_move(&f_2, &f_1);

    test_int(f_2.term_count, 2);
    test_assert(f_2.terms != NULL);
    test_int(f_2.terms[0].id, TagA);
    test_int(f_2.terms[1].id, TagB);

    test_int(f_1.term_count, 0);
    test_assert(f_1.terms == NULL);

    ecs_filter_fini(&f_1);
    ecs_filter_fini(&f_2);

    ecs_fini(world);
}

void Filter_filter_copy() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_filter_t f_1;
    test_int(ecs_filter_init(world, &f_1, &(ecs_filter_desc_t) {
        .terms = {{TagA}, {TagB}}
    }), 0);

    test_int(f_1.term_count, 2);
    test_assert(f_1.terms != NULL);
    test_int(f_1.terms[0].id, TagA);
    test_int(f_1.terms[1].id, TagB);

    ecs_filter_t f_2 = {};
    ecs_filter_copy(&f_2, &f_1);

    test_int(f_1.term_count, 2);
    test_assert(f_1.terms != NULL);
    test_int(f_1.terms[0].id, TagA);
    test_int(f_1.terms[1].id, TagB);

    test_int(f_2.term_count, 2);
    test_assert(f_2.terms != NULL);
    test_int(f_2.terms[0].id, TagA);
    test_int(f_2.terms[1].id, TagB);

    test_assert(f_1.terms != f_2.terms);

    ecs_filter_fini(&f_1);
    ecs_filter_fini(&f_2);

    ecs_fini(world);
}

void Filter_filter_w_resources_copy() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_filter_t f_1;
    test_int(ecs_filter_init(world, &f_1, &(ecs_filter_desc_t) {
        .terms = {{TagA, .args[0].name = "This"}, {TagB, .args[0].name = "This"}}
    }), 0);

    test_int(f_1.term_count, 2);
    test_assert(f_1.terms != NULL);
    test_int(f_1.terms[0].id, TagA);
    test_int(f_1.terms[1].id, TagB);

    ecs_filter_t f_2 = {};
    ecs_filter_copy(&f_2, &f_1);

    test_int(f_1.term_count, 2);
    test_assert(f_1.terms != NULL);
    test_int(f_1.terms[0].id, TagA);
    test_int(f_1.terms[1].id, TagB);

    test_int(f_2.term_count, 2);
    test_assert(f_2.terms != NULL);
    test_int(f_2.terms[0].id, TagA);
    test_int(f_2.terms[1].id, TagB);

    test_assert(f_1.terms != f_2.terms);

    ecs_filter_fini(&f_1);
    ecs_filter_fini(&f_2);

    ecs_fini(world);
}

void Filter_filter_w_10_terms() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);
    ECS_TAG(world, TagF);
    ECS_TAG(world, TagG);
    ECS_TAG(world, TagH);
    ECS_TAG(world, TagI);
    ECS_TAG(world, TagJ);

    ecs_filter_t f_1;
    test_int(ecs_filter_init(world, &f_1, &(ecs_filter_desc_t) {
        .terms = {
            {TagA}, {TagB}, {TagC}, {TagD}, {TagE}, {TagF}, {TagG}, {TagH}, 
            {TagI}, {TagJ}
        }
    }), 0);

    test_int(f_1.term_count, 10);
    test_assert(f_1.terms != NULL);
    test_int(f_1.terms[0].id, TagA);
    test_int(f_1.terms[1].id, TagB);
    test_int(f_1.terms[2].id, TagC);
    test_int(f_1.terms[3].id, TagD);
    test_int(f_1.terms[4].id, TagE);
    test_int(f_1.terms[5].id, TagF);
    test_int(f_1.terms[6].id, TagG);
    test_int(f_1.terms[7].id, TagH);
    test_int(f_1.terms[8].id, TagI);
    test_int(f_1.terms[9].id, TagJ);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add(world, e, TagC);
    ecs_add(world, e, TagD);
    ecs_add(world, e, TagE);
    ecs_add(world, e, TagF);
    ecs_add(world, e, TagG);
    ecs_add(world, e, TagH);
    ecs_add(world, e, TagI);
    ecs_add(world, e, TagJ);

    ecs_iter_t it = ecs_filter_iter(world, &f_1);
    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.ids[0], TagA);
    test_int(it.ids[1], TagB);
    test_int(it.ids[2], TagC);
    test_int(it.ids[3], TagD);
    test_int(it.ids[4], TagE);
    test_int(it.ids[5], TagF);
    test_int(it.ids[6], TagG);
    test_int(it.ids[7], TagH);
    test_int(it.ids[8], TagI);
    test_int(it.ids[9], TagJ);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f_1);

    ecs_fini(world);
}

void Filter_filter_w_10_terms_move() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);
    ECS_TAG(world, TagF);
    ECS_TAG(world, TagG);
    ECS_TAG(world, TagH);
    ECS_TAG(world, TagI);
    ECS_TAG(world, TagJ);

    ecs_filter_t f_1;
    test_int(ecs_filter_init(world, &f_1, &(ecs_filter_desc_t) {
        .terms = {
            {TagA}, {TagB}, {TagC}, {TagD}, {TagE}, {TagF}, {TagG}, {TagH}, 
            {TagI}, {TagJ}
        }
    }), 0);

    test_int(f_1.term_count, 10);
    test_assert(f_1.terms != NULL);
    test_int(f_1.terms[0].id, TagA);
    test_int(f_1.terms[1].id, TagB);
    test_int(f_1.terms[2].id, TagC);
    test_int(f_1.terms[3].id, TagD);
    test_int(f_1.terms[4].id, TagE);
    test_int(f_1.terms[5].id, TagF);
    test_int(f_1.terms[6].id, TagG);
    test_int(f_1.terms[7].id, TagH);
    test_int(f_1.terms[8].id, TagI);
    test_int(f_1.terms[9].id, TagJ);

    ecs_filter_t f_2 = {};
    ecs_filter_move(&f_2, &f_1);    

    test_int(f_2.term_count, 10);
    test_assert(f_2.terms != NULL);
    test_int(f_2.terms[0].id, TagA);
    test_int(f_2.terms[1].id, TagB);
    test_int(f_2.terms[2].id, TagC);
    test_int(f_2.terms[3].id, TagD);
    test_int(f_2.terms[4].id, TagE);
    test_int(f_2.terms[5].id, TagF);
    test_int(f_2.terms[6].id, TagG);
    test_int(f_2.terms[7].id, TagH);
    test_int(f_2.terms[8].id, TagI);
    test_int(f_2.terms[9].id, TagJ);

    test_int(f_1.term_count, 0);
    test_assert(f_1.terms == NULL); 

    ecs_filter_fini(&f_1);
    ecs_filter_fini(&f_2);

    ecs_fini(world);
}

void Filter_filter_w_10_terms_copy() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);
    ECS_TAG(world, TagF);
    ECS_TAG(world, TagG);
    ECS_TAG(world, TagH);
    ECS_TAG(world, TagI);
    ECS_TAG(world, TagJ);

    ecs_filter_t f_1;
    test_int(ecs_filter_init(world, &f_1, &(ecs_filter_desc_t) {
        .terms = {
            {TagA}, {TagB}, {TagC}, {TagD}, {TagE}, {TagF}, {TagG}, {TagH}, 
            {TagI}, {TagJ}
        }
    }), 0);

    test_int(f_1.term_count, 10);
    test_assert(f_1.terms != NULL);
    test_int(f_1.terms[0].id, TagA);
    test_int(f_1.terms[1].id, TagB);
    test_int(f_1.terms[2].id, TagC);
    test_int(f_1.terms[3].id, TagD);
    test_int(f_1.terms[4].id, TagE);
    test_int(f_1.terms[5].id, TagF);
    test_int(f_1.terms[6].id, TagG);
    test_int(f_1.terms[7].id, TagH);
    test_int(f_1.terms[8].id, TagI);
    test_int(f_1.terms[9].id, TagJ);

    ecs_filter_t f_2 = {};
    ecs_filter_copy(&f_2, &f_1);    

    test_int(f_1.term_count, 10);
    test_assert(f_1.terms != NULL);
    test_int(f_1.terms[0].id, TagA);
    test_int(f_1.terms[1].id, TagB);
    test_int(f_1.terms[2].id, TagC);
    test_int(f_1.terms[3].id, TagD);
    test_int(f_1.terms[4].id, TagE);
    test_int(f_1.terms[5].id, TagF);
    test_int(f_1.terms[6].id, TagG);
    test_int(f_1.terms[7].id, TagH);
    test_int(f_1.terms[8].id, TagI);
    test_int(f_1.terms[9].id, TagJ);

    test_int(f_2.term_count, 10);
    test_assert(f_2.terms != NULL);
    test_int(f_2.terms[0].id, TagA);
    test_int(f_2.terms[1].id, TagB);
    test_int(f_2.terms[2].id, TagC);
    test_int(f_2.terms[3].id, TagD);
    test_int(f_2.terms[4].id, TagE);
    test_int(f_2.terms[5].id, TagF);
    test_int(f_2.terms[6].id, TagG);
    test_int(f_2.terms[7].id, TagH);
    test_int(f_2.terms[8].id, TagI);
    test_int(f_2.terms[9].id, TagJ);

    test_assert(f_1.terms != f_2.terms); 

    ecs_filter_fini(&f_1);
    ecs_filter_fini(&f_2);

    ecs_fini(world);
}

void Filter_term_iter_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e_2 = ecs_set(world, 0, Position, {3, 4});
    ecs_entity_t e_3 = ecs_set(world, 0, Position, {5, 6});

    ecs_add(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        ecs_id(Position)
    });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));

    Position *p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p[0].x, 1);
    test_int(p[0].y, 2);

    test_int(p[1].x, 3);
    test_int(p[1].y, 4);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));

    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p[0].x, 5);
    test_int(p[0].y, 6);

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag1);
    ECS_TAG(world, Tag2);

    ecs_entity_t e_1 = ecs_new(world, Tag1);
    ecs_entity_t e_2 = ecs_new(world, Tag1);
    ecs_entity_t e_3 = ecs_new(world, Tag1);

    ecs_add(world, e_3, Tag2);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) { Tag1 });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), Tag1);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), Tag1);

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, Obj);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, Obj);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, Obj);

    ecs_add(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) { ecs_pair(Rel, Obj) });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));
    
    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_pair_w_rel_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel_1);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel_1, Obj);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel_1, Obj);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel_2, Obj);

    ecs_add(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) { 
        ecs_pair(EcsWildcard, Obj) 
    });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel_2, Obj));

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel_1, Obj));

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_pair_w_obj_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, Obj_1);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, Obj_1);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, Obj_2);

    ecs_add(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) { 
        ecs_pair(Rel, EcsWildcard) 
    });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj_2));

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj_1));

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_w_superset() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t base = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, base);

    ecs_add_id(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        .id = ecs_id(Position),
        .args[0].set = {
            .mask = EcsSuperSet
        }
    });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), base);

    Position *ptr = ecs_term(&it, Position, 1);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), base);

    test_assert(ptr == ecs_term(&it, Position, 1));

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_w_superset_childof() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add_id(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        .id = ecs_id(Position),
        .args[0].set = {
            .relation = EcsChildOf,
            .mask = EcsSuperSet
        }
    });

    {
        test_assert(ecs_term_next(&it));
        test_int(it.count, 2);
        test_int(it.entities[0], e_1);
        test_int(it.entities[1], e_2);
        test_int(ecs_term_id(&it, 1), ecs_id(Position));
        test_int(ecs_term_source(&it, 1), parent);

        Position *ptr = ecs_term(&it, Position, 1);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    
    {
        test_assert(ecs_term_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_term_id(&it, 1), ecs_id(Position));
        test_int(ecs_term_source(&it, 1), parent);

        Position *ptr = ecs_term(&it, Position, 1);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_w_superset_self() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t base = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_0 = ecs_set(world, 0, Position, {11, 22});
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, base);

    ecs_add_id(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        .id = ecs_id(Position),
        .args[0].set = {
            .mask = EcsSelf | EcsSuperSet
        }
    });

    Position *ptr;

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], base);
    test_int(it.entities[1], e_0);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), 0);

    ptr = ecs_term(&it, Position, 1);
    test_int(ptr[0].x, 10);
    test_int(ptr[0].y, 20);
    test_int(ptr[1].x, 11);
    test_int(ptr[1].y, 22);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), base);

    ptr = ecs_term(&it, Position, 1);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), base);

    test_assert(ptr == ecs_term(&it, Position, 1));

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_w_superset_self_childof() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_0 = ecs_set(world, 0, Position, {11, 22});
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, parent);

    ecs_add_id(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        .id = ecs_id(Position),
        .args[0].set = {
            .mask = EcsSelf | EcsSuperSet
        }
    });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], parent);
    test_int(it.entities[1], e_0);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), 0);

    Position *ptr = ecs_term(&it, Position, 1);
    test_int(ptr[0].x, 10);
    test_int(ptr[0].y, 20);
    test_int(ptr[1].x, 11);
    test_int(ptr[1].y, 22);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), parent);

    ptr = ecs_term(&it, Position, 1);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), parent);

    test_assert(ptr == ecs_term(&it, Position, 1));

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_w_superset_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t base = ecs_new(world, TagA);
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, base);

    ecs_add_id(world, e_3, TagB);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        .id = TagA,
        .args[0].set = {
            .mask = EcsSuperSet
        }
    });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_source(&it, 1), base);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_source(&it, 1), base);

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_w_superset_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Tag);

    ecs_entity_t base = ecs_new_w_pair(world, Rel, Obj);
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, base);

    ecs_add_id(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        .id = ecs_pair(Rel, Obj),
        .args[0].set = {
            .mask = EcsSuperSet
        }
    });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));
    test_int(ecs_term_source(&it, 1), base);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));
    test_int(ecs_term_source(&it, 1), base);

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_w_superset_pair_obj_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);
    ECS_TAG(world, Tag);

    ecs_entity_t base_1 = ecs_new_w_pair(world, Rel, Obj_1);
    ecs_entity_t base_2 = ecs_new_w_pair(world, Rel, Obj_2);
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, base_1);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, base_1);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, base_2);
    ecs_entity_t e_4 = ecs_new_w_pair(world, EcsIsA, base_2);

    ecs_add_id(world, e_3, Tag);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        .id = ecs_pair(Rel, EcsWildcard),
        .args[0].set = {
            .mask = EcsSuperSet
        }
    });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj_1));
    test_int(ecs_term_source(&it, 1), base_1);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_4);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj_2));
    test_int(ecs_term_source(&it, 1), base_2);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj_2));
    test_int(ecs_term_source(&it, 1), base_2);

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_term_iter_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_add(world, e2, TagB);

    ecs_staging_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_iter_t it = ecs_term_iter(stage, &(ecs_term_t) { .id = Tag });
    test_assert(it.real_world == world);
    test_assert(it.world == stage);

    test_bool(ecs_term_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(it.ids[0], Tag);

    test_bool(ecs_term_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(it.ids[0], Tag);

    ecs_staging_end(world);

    ecs_fini(world);
}

void Filter_filter_iter_1_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_entity_t e_3 = ecs_new(world, TagA);

    ecs_add_id(world, e_3, TagB);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ TagA }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_entity_t e_3 = ecs_new(world, TagA);
    
    ecs_new(world, TagA); /* Non matching entity */

    ecs_add_id(world, e_1, TagB);
    ecs_add_id(world, e_2, TagB);
    ecs_add_id(world, e_3, TagB);

    ecs_add_id(world, e_3, TagC);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ TagA }, { TagB }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_2_tags_1_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_entity_t e_3 = ecs_new(world, TagA);

    ecs_add_id(world, e_3, TagC);
    
    ecs_entity_t e_4 = ecs_new(world, TagA); /* Non matching entity */
    ecs_add_id(world, e_4, TagB);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ TagA }, { TagB, .oper = EcsNot }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_3_tags_2_or() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_entity_t e_3 = ecs_new(world, TagA);

    ecs_add_id(world, e_1, TagB);
    ecs_add_id(world, e_2, TagB);
    ecs_add_id(world, e_3, TagC);
    
    ecs_entity_t e_4 = ecs_new(world, TagA); /* Non matching entity */
    ecs_add_id(world, e_4, TagD);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ TagA }, { TagB, .oper = EcsOr }, { TagC, .oper = EcsOr }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagC);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_1_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagB);

    ecs_entity_t e_1 = ecs_set(world, 0, Position, {10, 21});
    ecs_entity_t e_2 = ecs_set(world, 0, Position, {12, 23});
    ecs_entity_t e_3 = ecs_set(world, 0, Position, {14, 25});

    ecs_add_id(world, e_3, TagB);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), 0);

    Position *p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p[0].x, 10);
    test_int(p[0].y, 21);
    test_int(p[1].x, 12);
    test_int(p[1].y, 23);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), 0);

    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p[0].x, 14);
    test_int(p[0].y, 25);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_2_components() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, TagB);

    ecs_entity_t e_1 = ecs_set(world, 0, Position, {10, 21});
    ecs_entity_t e_2 = ecs_set(world, 0, Position, {12, 23});
    ecs_entity_t e_3 = ecs_set(world, 0, Position, {14, 25});

    ecs_set(world, e_1, Velocity, {0, 1});
    ecs_set(world, e_2, Velocity, {2, 3});
    ecs_set(world, e_3, Velocity, {4, 5});

    ecs_add_id(world, e_3, TagB);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ ecs_id(Position) }, { ecs_id(Velocity) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    {
        test_assert(ecs_filter_next(&it));
        test_int(it.count, 2);
        test_int(it.entities[0], e_1);
        test_int(it.entities[1], e_2);
        test_int(ecs_term_id(&it, 1), ecs_id(Position));
        test_int(ecs_term_source(&it, 1), 0);
        
        Position *p = ecs_term(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 21);
        test_int(p[1].x, 12);
        test_int(p[1].y, 23);

        Velocity *v = ecs_term(&it, Velocity, 2);
        test_assert(v != NULL);
        test_int(v[0].x, 0);
        test_int(v[0].y, 1);
        test_int(v[1].x, 2);
        test_int(v[1].y, 3);
    }

    {
        test_assert(ecs_filter_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_term_id(&it, 1), ecs_id(Position));
        test_int(ecs_term_source(&it, 1), 0);

        Position *p = ecs_term(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 14);
        test_int(p[0].y, 25);

        Velocity *v = ecs_term(&it, Velocity, 2);
        test_assert(v != NULL);
        test_int(v[0].x, 4);
        test_int(v[0].y, 5);
    }

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_pair_id() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, Obj);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, Obj);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, Obj);

    ecs_add_id(world, e_3, Tag);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ ecs_pair(Rel, EcsWildcard) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_2_pair_ids() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, RelA, ObjA);
    ecs_entity_t e_2 = ecs_new_w_pair(world, RelA, ObjA);
    ecs_entity_t e_3 = ecs_new_w_pair(world, RelA, ObjA);

    ecs_add_pair(world, e_1, RelB, ObjB);
    ecs_add_pair(world, e_2, RelB, ObjB);
    ecs_add_pair(world, e_3, RelB, ObjB);

    ecs_add_id(world, e_3, Tag);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {
            { ecs_pair(RelA, ObjA) },
            { ecs_pair(RelB, EcsWildcard) }
        }
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(RelA, ObjA));
    test_int(ecs_term_id(&it, 2), ecs_pair(RelB, ObjB));
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(RelA, ObjA));
    test_int(ecs_term_id(&it, 2), ecs_pair(RelB, ObjB));
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_pair_pred_obj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, Obj);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, Obj);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, Obj);

    ecs_add_id(world, e_3, Tag);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ .pred.entity = Rel, .args[1].entity = EcsWildcard }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_pair_2_pred_obj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, RelA, ObjA);
    ecs_entity_t e_2 = ecs_new_w_pair(world, RelA, ObjA);
    ecs_entity_t e_3 = ecs_new_w_pair(world, RelA, ObjA);

    ecs_add_pair(world, e_1, RelB, ObjB);
    ecs_add_pair(world, e_2, RelB, ObjB);
    ecs_add_pair(world, e_3, RelB, ObjB);

    ecs_add_id(world, e_3, Tag);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {
            { .pred.entity = RelA, .args[1].entity = ObjA },
            { .pred.entity = RelB, .args[1].entity = EcsWildcard }
        }
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(RelA, ObjA));
    test_int(ecs_term_id(&it, 2), ecs_pair(RelB, ObjB));
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(RelA, ObjA));
    test_int(ecs_term_id(&it, 2), ecs_pair(RelB, ObjB));
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_null() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagB);
    ecs_entity_t e_3 = ecs_new(world, TagB);
    ecs_add_id(world, e_3, TagC);
    
    bool e_1_found = false;
    bool e_2_found = false;
    bool e_3_found = false;

    ecs_iter_t it = ecs_filter_iter(world, NULL);

    while (ecs_filter_next(&it)) {
        int i;
        for (i = 0; i < it.count; i ++) {
            if (it.entities[i] == e_1) {
                test_bool(e_1_found, false);
                e_1_found = true;
            } else
            if (it.entities[i] == e_2) {
                test_bool(e_2_found, false);
                e_2_found = true;
            } else
            if (it.entities[i] == e_3) {
                test_bool(e_3_found, false);
                e_3_found = true;
            }                        
        }
    }

    test_bool (e_1_found, true);
    test_bool (e_2_found, true);
    test_bool (e_3_found, true);

    ecs_fini(world);
}

void Filter_filter_iter_1_not_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagB);
    ecs_entity_t e_3 = ecs_new(world, TagB);
    ecs_add_id(world, e_3, TagC);
    
    bool e_1_found = false;
    bool e_2_found = false;
    bool e_3_found = false;

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ .id = TagC, .oper = EcsNot }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    while (ecs_filter_next(&it)) {
        int i;
        for (i = 0; i < it.count; i ++) {
            if (it.entities[i] == e_1) {
                test_bool(e_1_found, false);
                e_1_found = true;
            } else
            if (it.entities[i] == e_2) {
                test_bool(e_2_found, false);
                e_2_found = true;
            } else
            if (it.entities[i] == e_3) {
                e_3_found = true;
            }                        
        }
    }

    test_bool (e_1_found, true);
    test_bool (e_2_found, true);
    test_bool (e_3_found, false);

    ecs_fini(world);
}

void Filter_filter_iter_2_tags_1_optional() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_entity_t e_3 = ecs_new(world, TagA);

    ecs_add_id(world, e_3, TagB);
    
    ecs_entity_t e_4 = ecs_new(world, TagB); /* Non matching entity */
    ecs_add_id(world, e_4, TagC);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ TagA }, { TagB, .oper = EcsOptional }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_bool(ecs_term_is_set(&it, 1), true);
    test_bool(ecs_term_is_set(&it, 2), true);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_bool(ecs_term_is_set(&it, 1), true);
    test_bool(ecs_term_is_set(&it, 2), false);
    test_int(ecs_term_source(&it, 1), 0);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world, Tag);

    ecs_staging_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_filter_t f;
    test_int(ecs_filter_init(stage, &f, &(ecs_filter_desc_t) { 
        .terms = {{ Tag }}
    }), 0);

    ecs_iter_t it = ecs_filter_iter(stage, &f);

    test_assert(it.real_world == world);
    test_assert(it.world == stage);
    test_bool(ecs_filter_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.ids[0], Tag);

    ecs_staging_end(world);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_term_iter_type_set() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);

    ecs_add_id(world, e_2, TagB);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        .id = TagA
    });

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_assert(it.type != NULL);
    test_int(ecs_vector_count(it.type), 2);
    test_int(ecs_vector_first(it.type, ecs_id_t)[0], TagA);
    test_int(ecs_vector_first(it.type, ecs_id_t)[1], TagB);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_assert(it.type != NULL);
    test_int(ecs_vector_count(it.type), 1);
    test_int(ecs_vector_first(it.type, ecs_id_t)[0], TagA);

    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Filter_filter_iter_type_set() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);

    ecs_add_id(world, e_2, TagB);

    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {{ TagA }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_assert(it.type != NULL);
    test_int(ecs_vector_count(it.type), 2);
    test_int(ecs_vector_first(it.type, ecs_id_t)[0], TagA);
    test_int(ecs_vector_first(it.type, ecs_id_t)[1], TagB);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_assert(it.type != NULL);
    test_int(ecs_vector_count(it.type), 1);
    test_int(ecs_vector_first(it.type, ecs_id_t)[0], TagA);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_10_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);
    ECS_TAG(world, TagF);
    ECS_TAG(world, TagG);
    ECS_TAG(world, TagH);
    ECS_TAG(world, TagI);
    ECS_TAG(world, TagJ);
    ECS_TAG(world, TagK);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_add_id(world, e_1, TagB);
    ecs_add_id(world, e_1, TagC);
    ecs_add_id(world, e_1, TagD);
    ecs_add_id(world, e_1, TagE);
    ecs_add_id(world, e_1, TagF);
    ecs_add_id(world, e_1, TagG);
    ecs_add_id(world, e_1, TagH);
    ecs_add_id(world, e_1, TagI);
    ecs_add_id(world, e_1, TagJ);
    
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_add_id(world, e_2, TagB);
    ecs_add_id(world, e_2, TagC);
    ecs_add_id(world, e_2, TagD);
    ecs_add_id(world, e_2, TagE);
    ecs_add_id(world, e_2, TagF);
    ecs_add_id(world, e_2, TagG);
    ecs_add_id(world, e_2, TagH);
    ecs_add_id(world, e_2, TagI);
    ecs_add_id(world, e_2, TagJ);
    ecs_add_id(world, e_2, TagK); /* 2nd match in different table */

    ecs_filter_t f;
    test_int(ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {
            {TagA}, {TagB}, {TagC}, {TagD}, {TagE}, {TagF}, {TagG}, {TagH}, 
            {TagI}, {TagJ}
        }
    }), 0);

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_int(ecs_term_id(&it, 3), TagC);
    test_int(ecs_term_id(&it, 4), TagD);
    test_int(ecs_term_id(&it, 5), TagE);
    test_int(ecs_term_id(&it, 6), TagF);
    test_int(ecs_term_id(&it, 7), TagG);
    test_int(ecs_term_id(&it, 8), TagH);
    test_int(ecs_term_id(&it, 9), TagI);
    test_int(ecs_term_id(&it, 10), TagJ);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_int(ecs_term_id(&it, 3), TagC);
    test_int(ecs_term_id(&it, 4), TagD);
    test_int(ecs_term_id(&it, 5), TagE);
    test_int(ecs_term_id(&it, 6), TagF);
    test_int(ecs_term_id(&it, 7), TagG);
    test_int(ecs_term_id(&it, 8), TagH);
    test_int(ecs_term_id(&it, 9), TagI);
    test_int(ecs_term_id(&it, 10), TagJ);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_20_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);
    ECS_TAG(world, TagF);
    ECS_TAG(world, TagG);
    ECS_TAG(world, TagH);
    ECS_TAG(world, TagI);
    ECS_TAG(world, TagJ);
    ECS_TAG(world, TagK);
    ECS_TAG(world, TagL);
    ECS_TAG(world, TagM);
    ECS_TAG(world, TagN);
    ECS_TAG(world, TagO);
    ECS_TAG(world, TagP);
    ECS_TAG(world, TagQ);
    ECS_TAG(world, TagR);
    ECS_TAG(world, TagS);
    ECS_TAG(world, TagT);
    ECS_TAG(world, TagU);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_add_id(world, e_1, TagB);
    ecs_add_id(world, e_1, TagC);
    ecs_add_id(world, e_1, TagD);
    ecs_add_id(world, e_1, TagE);
    ecs_add_id(world, e_1, TagF);
    ecs_add_id(world, e_1, TagG);
    ecs_add_id(world, e_1, TagH);
    ecs_add_id(world, e_1, TagI);
    ecs_add_id(world, e_1, TagJ);
    ecs_add_id(world, e_1, TagK);
    ecs_add_id(world, e_1, TagL);
    ecs_add_id(world, e_1, TagM);
    ecs_add_id(world, e_1, TagN);
    ecs_add_id(world, e_1, TagO);
    ecs_add_id(world, e_1, TagP);
    ecs_add_id(world, e_1, TagQ);
    ecs_add_id(world, e_1, TagR);
    ecs_add_id(world, e_1, TagS);
    ecs_add_id(world, e_1, TagT);
    
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_add_id(world, e_2, TagB);
    ecs_add_id(world, e_2, TagC);
    ecs_add_id(world, e_2, TagD);
    ecs_add_id(world, e_2, TagE);
    ecs_add_id(world, e_2, TagF);
    ecs_add_id(world, e_2, TagG);
    ecs_add_id(world, e_2, TagH);
    ecs_add_id(world, e_2, TagI);
    ecs_add_id(world, e_2, TagJ);
    ecs_add_id(world, e_2, TagK);
    ecs_add_id(world, e_2, TagL);
    ecs_add_id(world, e_2, TagM);
    ecs_add_id(world, e_2, TagN);
    ecs_add_id(world, e_2, TagO);
    ecs_add_id(world, e_2, TagP);
    ecs_add_id(world, e_2, TagQ);
    ecs_add_id(world, e_2, TagR);
    ecs_add_id(world, e_2, TagS);
    ecs_add_id(world, e_2, TagT);
    ecs_add_id(world, e_2, TagU); /* 2nd match in different table */

    ecs_filter_t f;
    test_int(ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms_buffer = (ecs_term_t[]){
            {TagA}, {TagB}, {TagC}, {TagD}, {TagE}, {TagF}, {TagG}, {TagH}, 
            {TagI}, {TagJ}, {TagK}, {TagL}, {TagM}, {TagN}, {TagO}, {TagP},
            {TagQ}, {TagR}, {TagS}, {TagT}
        },
        .terms_buffer_count = 20
    }), 0);

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_int(ecs_term_id(&it, 3), TagC);
    test_int(ecs_term_id(&it, 4), TagD);
    test_int(ecs_term_id(&it, 5), TagE);
    test_int(ecs_term_id(&it, 6), TagF);
    test_int(ecs_term_id(&it, 7), TagG);
    test_int(ecs_term_id(&it, 8), TagH);
    test_int(ecs_term_id(&it, 9), TagI);
    test_int(ecs_term_id(&it, 10), TagJ);
    test_int(ecs_term_id(&it, 11), TagK);
    test_int(ecs_term_id(&it, 12), TagL);
    test_int(ecs_term_id(&it, 13), TagM);
    test_int(ecs_term_id(&it, 14), TagN);
    test_int(ecs_term_id(&it, 15), TagO);
    test_int(ecs_term_id(&it, 16), TagP);
    test_int(ecs_term_id(&it, 17), TagQ);
    test_int(ecs_term_id(&it, 18), TagR);
    test_int(ecs_term_id(&it, 19), TagS);
    test_int(ecs_term_id(&it, 20), TagT);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_term_id(&it, 1), TagA);
    test_int(ecs_term_id(&it, 2), TagB);
    test_int(ecs_term_id(&it, 3), TagC);
    test_int(ecs_term_id(&it, 4), TagD);
    test_int(ecs_term_id(&it, 5), TagE);
    test_int(ecs_term_id(&it, 6), TagF);
    test_int(ecs_term_id(&it, 7), TagG);
    test_int(ecs_term_id(&it, 8), TagH);
    test_int(ecs_term_id(&it, 9), TagI);
    test_int(ecs_term_id(&it, 10), TagJ);
    test_int(ecs_term_id(&it, 11), TagK);
    test_int(ecs_term_id(&it, 12), TagL);
    test_int(ecs_term_id(&it, 13), TagM);
    test_int(ecs_term_id(&it, 14), TagN);
    test_int(ecs_term_id(&it, 15), TagO);
    test_int(ecs_term_id(&it, 16), TagP);
    test_int(ecs_term_id(&it, 17), TagQ);
    test_int(ecs_term_id(&it, 18), TagR);
    test_int(ecs_term_id(&it, 19), TagS);
    test_int(ecs_term_id(&it, 20), TagT);

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

typedef struct {
    float v;
} CompA, CompB, CompC, CompD, CompE, CompF, CompG, CompH, CompI, CompJ, CompK,
  CompL, CompM, CompN, CompO, CompP, CompQ, CompR, CompS, CompT, CompU;

void Filter_filter_iter_10_components() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, CompA);
    ECS_COMPONENT(world, CompB);
    ECS_COMPONENT(world, CompC);
    ECS_COMPONENT(world, CompD);
    ECS_COMPONENT(world, CompE);
    ECS_COMPONENT(world, CompF);
    ECS_COMPONENT(world, CompG);
    ECS_COMPONENT(world, CompH);
    ECS_COMPONENT(world, CompI);
    ECS_COMPONENT(world, CompJ);
    ECS_COMPONENT(world, CompK);

    ecs_entity_t e_1 = ecs_set(world, 0, CompA, {10});
    ecs_set(world, e_1, CompB, {10});
    ecs_set(world, e_1, CompC, {10});
    ecs_set(world, e_1, CompD, {10});
    ecs_set(world, e_1, CompE, {10});
    ecs_set(world, e_1, CompF, {10});
    ecs_set(world, e_1, CompG, {10});
    ecs_set(world, e_1, CompH, {10});
    ecs_set(world, e_1, CompI, {10});
    ecs_set(world, e_1, CompJ, {10});
    
    ecs_entity_t e_2 = ecs_set(world, 0, CompA, {10});
    ecs_set(world, e_2, CompB, {10});
    ecs_set(world, e_2, CompC, {10});
    ecs_set(world, e_2, CompD, {10});
    ecs_set(world, e_2, CompE, {10});
    ecs_set(world, e_2, CompF, {10});
    ecs_set(world, e_2, CompG, {10});
    ecs_set(world, e_2, CompH, {10});
    ecs_set(world, e_2, CompI, {10});
    ecs_set(world, e_2, CompJ, {10});
    ecs_set(world, e_2, CompK, {10});

    ecs_filter_t f;
    test_int(ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {
            {ecs_id(CompA)}, {ecs_id(CompB)}, {ecs_id(CompC)}, {ecs_id(CompD)}, 
            {ecs_id(CompE)}, {ecs_id(CompF)}, {ecs_id(CompG)}, {ecs_id(CompH)}, 
            {ecs_id(CompI)}, {ecs_id(CompJ)}
        }
    }), 0);

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_term_id(&it, 1), ecs_id(CompA));
    test_int(ecs_term_id(&it, 2), ecs_id(CompB));
    test_int(ecs_term_id(&it, 3), ecs_id(CompC));
    test_int(ecs_term_id(&it, 4), ecs_id(CompD));
    test_int(ecs_term_id(&it, 5), ecs_id(CompE));
    test_int(ecs_term_id(&it, 6), ecs_id(CompF));
    test_int(ecs_term_id(&it, 7), ecs_id(CompG));
    test_int(ecs_term_id(&it, 8), ecs_id(CompH));
    test_int(ecs_term_id(&it, 9), ecs_id(CompI));
    test_int(ecs_term_id(&it, 10), ecs_id(CompJ));

    int i;
    for (i = 0; i < 10; i ++) {
        CompA *ptr = ecs_term_w_size(&it, 0, i + 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].v, 10);
    }

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(CompA));
    test_int(ecs_term_id(&it, 2), ecs_id(CompB));
    test_int(ecs_term_id(&it, 3), ecs_id(CompC));
    test_int(ecs_term_id(&it, 4), ecs_id(CompD));
    test_int(ecs_term_id(&it, 5), ecs_id(CompE));
    test_int(ecs_term_id(&it, 6), ecs_id(CompF));
    test_int(ecs_term_id(&it, 7), ecs_id(CompG));
    test_int(ecs_term_id(&it, 8), ecs_id(CompH));
    test_int(ecs_term_id(&it, 9), ecs_id(CompI));
    test_int(ecs_term_id(&it, 10), ecs_id(CompJ));

    for (i = 0; i < 10; i ++) {
        CompA *ptr = ecs_term_w_size(&it, 0, i + 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].v, 10);
    }

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Filter_filter_iter_20_components() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, CompA);
    ECS_COMPONENT(world, CompB);
    ECS_COMPONENT(world, CompC);
    ECS_COMPONENT(world, CompD);
    ECS_COMPONENT(world, CompE);
    ECS_COMPONENT(world, CompF);
    ECS_COMPONENT(world, CompG);
    ECS_COMPONENT(world, CompH);
    ECS_COMPONENT(world, CompI);
    ECS_COMPONENT(world, CompJ);
    ECS_COMPONENT(world, CompK);
    ECS_COMPONENT(world, CompL);
    ECS_COMPONENT(world, CompM);
    ECS_COMPONENT(world, CompN);
    ECS_COMPONENT(world, CompO);
    ECS_COMPONENT(world, CompP);
    ECS_COMPONENT(world, CompQ);
    ECS_COMPONENT(world, CompR);
    ECS_COMPONENT(world, CompS);
    ECS_COMPONENT(world, CompT);
    ECS_COMPONENT(world, CompU);

    ecs_entity_t e_1 = ecs_set(world, 0, CompA, {10});
    ecs_set(world, e_1, CompB, {10});
    ecs_set(world, e_1, CompC, {10});
    ecs_set(world, e_1, CompD, {10});
    ecs_set(world, e_1, CompE, {10});
    ecs_set(world, e_1, CompF, {10});
    ecs_set(world, e_1, CompG, {10});
    ecs_set(world, e_1, CompH, {10});
    ecs_set(world, e_1, CompI, {10});
    ecs_set(world, e_1, CompJ, {10});
    ecs_set(world, e_1, CompK, {10});
    ecs_set(world, e_1, CompL, {10});
    ecs_set(world, e_1, CompM, {10});
    ecs_set(world, e_1, CompN, {10});
    ecs_set(world, e_1, CompO, {10});
    ecs_set(world, e_1, CompP, {10});
    ecs_set(world, e_1, CompQ, {10});
    ecs_set(world, e_1, CompR, {10});
    ecs_set(world, e_1, CompS, {10});
    ecs_set(world, e_1, CompT, {10});

    ecs_entity_t e_2 = ecs_set(world, 0, CompA, {10});
    ecs_set(world, e_2, CompB, {10});
    ecs_set(world, e_2, CompC, {10});
    ecs_set(world, e_2, CompD, {10});
    ecs_set(world, e_2, CompE, {10});
    ecs_set(world, e_2, CompF, {10});
    ecs_set(world, e_2, CompG, {10});
    ecs_set(world, e_2, CompH, {10});
    ecs_set(world, e_2, CompI, {10});
    ecs_set(world, e_2, CompJ, {10});
    ecs_set(world, e_2, CompK, {10});
    ecs_set(world, e_2, CompL, {10});
    ecs_set(world, e_2, CompM, {10});
    ecs_set(world, e_2, CompN, {10});
    ecs_set(world, e_2, CompO, {10});
    ecs_set(world, e_2, CompP, {10});
    ecs_set(world, e_2, CompQ, {10});
    ecs_set(world, e_2, CompR, {10});
    ecs_set(world, e_2, CompS, {10});
    ecs_set(world, e_2, CompT, {10});
    ecs_set(world, e_2, CompU, {10});

    ecs_filter_t f;
    test_int(ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms_buffer = (ecs_term_t[]){
            {ecs_id(CompA)}, {ecs_id(CompB)}, {ecs_id(CompC)}, {ecs_id(CompD)}, 
            {ecs_id(CompE)}, {ecs_id(CompF)}, {ecs_id(CompG)}, {ecs_id(CompH)}, 
            {ecs_id(CompI)}, {ecs_id(CompJ)}, {ecs_id(CompK)}, {ecs_id(CompL)},
            {ecs_id(CompM)}, {ecs_id(CompN)}, {ecs_id(CompO)}, {ecs_id(CompP)},
            {ecs_id(CompQ)}, {ecs_id(CompR)}, {ecs_id(CompS)}, {ecs_id(CompT)}
        },
        .terms_buffer_count = 20
    }), 0);

    ecs_iter_t it = ecs_filter_iter(world, &f);

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_term_id(&it, 1), ecs_id(CompA));
    test_int(ecs_term_id(&it, 2), ecs_id(CompB));
    test_int(ecs_term_id(&it, 3), ecs_id(CompC));
    test_int(ecs_term_id(&it, 4), ecs_id(CompD));
    test_int(ecs_term_id(&it, 5), ecs_id(CompE));
    test_int(ecs_term_id(&it, 6), ecs_id(CompF));
    test_int(ecs_term_id(&it, 7), ecs_id(CompG));
    test_int(ecs_term_id(&it, 8), ecs_id(CompH));
    test_int(ecs_term_id(&it, 9), ecs_id(CompI));
    test_int(ecs_term_id(&it, 10), ecs_id(CompJ));
    test_int(ecs_term_id(&it, 11), ecs_id(CompK));
    test_int(ecs_term_id(&it, 12), ecs_id(CompL));
    test_int(ecs_term_id(&it, 13), ecs_id(CompM));
    test_int(ecs_term_id(&it, 14), ecs_id(CompN));
    test_int(ecs_term_id(&it, 15), ecs_id(CompO));
    test_int(ecs_term_id(&it, 16), ecs_id(CompP));
    test_int(ecs_term_id(&it, 17), ecs_id(CompQ));
    test_int(ecs_term_id(&it, 18), ecs_id(CompR));
    test_int(ecs_term_id(&it, 19), ecs_id(CompS));
    test_int(ecs_term_id(&it, 20), ecs_id(CompT));

    int i;
    for (i = 0; i < 20; i ++) {
        CompA *ptr = ecs_term_w_size(&it, 0, i + 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].v, 10);
    }

    test_assert(ecs_filter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(CompA));
    test_int(ecs_term_id(&it, 2), ecs_id(CompB));
    test_int(ecs_term_id(&it, 3), ecs_id(CompC));
    test_int(ecs_term_id(&it, 4), ecs_id(CompD));
    test_int(ecs_term_id(&it, 5), ecs_id(CompE));
    test_int(ecs_term_id(&it, 6), ecs_id(CompF));
    test_int(ecs_term_id(&it, 7), ecs_id(CompG));
    test_int(ecs_term_id(&it, 8), ecs_id(CompH));
    test_int(ecs_term_id(&it, 9), ecs_id(CompI));
    test_int(ecs_term_id(&it, 10), ecs_id(CompJ));
    test_int(ecs_term_id(&it, 11), ecs_id(CompK));
    test_int(ecs_term_id(&it, 12), ecs_id(CompL));
    test_int(ecs_term_id(&it, 13), ecs_id(CompM));
    test_int(ecs_term_id(&it, 14), ecs_id(CompN));
    test_int(ecs_term_id(&it, 15), ecs_id(CompO));
    test_int(ecs_term_id(&it, 16), ecs_id(CompP));
    test_int(ecs_term_id(&it, 17), ecs_id(CompQ));
    test_int(ecs_term_id(&it, 18), ecs_id(CompR));
    test_int(ecs_term_id(&it, 19), ecs_id(CompS));
    test_int(ecs_term_id(&it, 20), ecs_id(CompT));

    for (i = 0; i < 20; i ++) {
        CompA *ptr = ecs_term_w_size(&it, 0, i + 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].v, 10);
    }

    test_assert(!ecs_filter_next(&it));

    ecs_filter_fini(&f);

    ecs_fini(world);
}
