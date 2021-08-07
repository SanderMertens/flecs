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
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), Tag1);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), Tag1);

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
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj));

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
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel_1, Obj));

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel_2, Obj));

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
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj_1));

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_pair(Rel, Obj_2));

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
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), base);

    Position *ptr = ecs_term(&it, Position, 1);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), base);

    test_assert(ptr == ecs_term(&it, Position, 1));

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

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], base);
    test_int(it.entities[1], e_0);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), 0);

    Position *ptr = ecs_term(&it, Position, 1);
    test_int(ptr[0].x, 10);
    test_int(ptr[0].y, 20);
    test_int(ptr[1].x, 11);
    test_int(ptr[1].y, 22);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), base);

    ptr = ecs_term(&it, Position, 1);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), base);

    test_assert(ptr == ecs_term(&it, Position, 1));

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
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), parent);

    ptr = ecs_term(&it, Position, 1);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    test_assert(ecs_term_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e_1);
    test_int(it.entities[1], e_2);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), parent);

    test_assert(ptr == ecs_term(&it, Position, 1));

    ecs_fini(world);
}
