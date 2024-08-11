#include <query.h>

static
void query_flags_to_str(uint64_t value) {
    if (value & EcsQueryMatchPrefab) {
        printf("EcsQueryMatchPrefab|");
    }
    if (value & EcsQueryMatchDisabled) {
        printf("EcsQueryMatchDisabled|");
    }
    if (value & EcsQueryMatchEmptyTables) {
        printf("EcsQueryMatchEmptyTables|");
    }
    if (value & EcsQueryAllowUnresolvedByName) {
        printf("EcsQueryAllowUnresolvedByName|");
    }
    if (value & EcsQueryTableOnly) {
        printf("EcsQueryTableOnly|");
    }
    if (value & EcsQueryMatchThis) {
        printf("EcsQueryMatchThis|");
    }
    if (value & EcsQueryMatchOnlyThis) {
        printf("EcsQueryMatchOnlyThis|");
    }
    if (value & EcsQueryMatchOnlySelf) {
        printf("EcsQueryMatchOnlySelf|");
    }
    if (value & EcsQueryMatchWildcards) {
        printf("EcsQueryMatchWildcards|");
    }
    if (value & EcsQueryHasCondSet) {
        printf("EcsQueryHasCondSet|");
    }
    if (value & EcsQueryHasPred) {
        printf("EcsQueryHasPred|");
    }
    if (value & EcsQueryHasScopes) {
        printf("EcsQueryHasScopes|");
    }
    if (value & EcsQueryHasRefs) {
        printf("EcsQueryHasRefs|");
    }
    if (value & EcsQueryHasOutTerms) {
        printf("EcsQueryHasOutTerms|");
    }
    if (value & EcsQueryHasNonThisOutTerms) {
        printf("EcsQueryHasNonThisOutTerms|");
    }
    if (value & EcsQueryHasMonitor) {
        printf("EcsQueryHasMonitor|");
    }
    if (value & EcsQueryIsTrivial) {
        printf("EcsQueryIsTrivial|");
    }
    if (value & EcsQueryHasCacheable) {
        printf("EcsQueryHasCacheable|");
    }
    if (value & EcsQueryIsCacheable) {
        printf("EcsQueryIsCacheable|");
    }
    if (value & EcsQueryHasTableThisVar) {
        printf("EcsQueryHasTableThisVar|");
    }
    if (value & EcsQueryCacheYieldEmptyTables) {
        printf("EcsQueryCacheYieldEmptyTables|");
    }
    printf("\n");
}

#define test_query_flags(expect, value)\
    if ((expect) != (value)) {\
        printf("expected: ");\
        query_flags_to_str(expect);\
        printf("got:      ");\
        query_flags_to_str(value);\
        test_uint(expect, value);\
    }

void Validator_validate_1_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{TagA}}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].inout, EcsInOutDefault);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].inout, EcsInOutDefault);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{TagA}, {TagB}}
    });

    test_int(q->term_count, 2);
    test_int(q->field_count, 2);

    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_uint(q->terms[1].id, TagB);
    test_int(q->terms[1].oper, EcsAnd);
    test_int(q->terms[1].field_index, 1);
    test_uint(q->terms[1].first.id, TagB|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_3_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{TagA}, {TagB}, {TagC}}
    });

    test_int(q->term_count, 3);
    test_int(q->field_count, 3);

    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_uint(q->terms[1].id, TagB);
    test_int(q->terms[1].oper, EcsAnd);
    test_int(q->terms[1].field_index, 1);
    test_uint(q->terms[1].first.id, TagB|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_int(q->terms[2].id, TagC);
    test_int(q->terms[2].oper, EcsAnd);
    test_int(q->terms[2].field_index, 2);
    test_int(q->terms[2].first.id, TagC|EcsSelf|EcsIsEntity);
    test_int(q->terms[2].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_3_terms_w_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{TagA}, {TagB, .oper = EcsOr}, {TagC }}
    });

    test_int(q->term_count, 3);
    test_int(q->field_count, 2);

    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_uint(q->terms[1].id, TagB);
    test_int(q->terms[1].oper, EcsOr);
    test_int(q->terms[1].field_index, 1);
    test_uint(q->terms[1].first.id, TagB|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_int(q->terms[2].id, TagC);
    test_int(q->terms[2].oper, EcsAnd);
    test_int(q->terms[2].field_index, 1);
    test_int(q->terms[2].first.id, TagC|EcsSelf|EcsIsEntity);
    test_int(q->terms[2].src.id, EcsThis|EcsSelf|EcsIsVariable);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_4_terms_w_or_at_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{TagA}, {TagB, .oper = EcsOr}, {TagC}, {TagD}}
    });

    test_int(q->term_count, 4);
    test_int(q->field_count, 3);

    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_uint(q->terms[1].id, TagB);
    test_int(q->terms[1].oper, EcsOr);
    test_int(q->terms[1].field_index, 1);
    test_uint(q->terms[1].first.id, TagB|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_int(q->terms[2].id, TagC);
    test_int(q->terms[2].oper, EcsAnd);
    test_int(q->terms[2].field_index, 1);
    test_int(q->terms[2].first.id, TagC|EcsSelf|EcsIsEntity);
    test_int(q->terms[2].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_int(q->terms[3].id, TagD);
    test_int(q->terms[3].oper, EcsAnd);
    test_int(q->terms[3].field_index, 2);
    test_int(q->terms[3].first.id, TagD|EcsSelf|EcsIsEntity);
    test_int(q->terms[3].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ EcsWildcard }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, EcsWildcard);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, EcsWildcard|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].second.id, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ EcsAny }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, EcsAny);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, EcsAny|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].second.id, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_same_subj_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = Rel, .src.name = "Foo", .second.name = "Foo" }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, Foo));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, Foo|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].second.id, Foo|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_acyclic_same_subj_obj(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Traversable);
    ECS_TAG(world, Foo);

    test_assert(NULL == ecs_query(world, {
        .terms = {{ .first.id = Rel, .src.name = "Foo", .second.name = "Foo" }}
    }));

    ecs_fini(world);
}

void Validator_validate_1_term_acyclic_reflexive_same_subj_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Traversable, Reflexive);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = Rel, .src.name = "Foo", .second.name = "Foo" }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, Foo));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, Foo|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].second.id, Foo|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_same_subj_obj_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.id = Rel, 
            .src = { .name = "X", .id = EcsIsVariable },
            .second = { .name = "X", .id = EcsIsVariable }
        }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, EcsWildcard));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_str(q->terms[0].src.name, "X");
    test_uint(q->terms[0].src.id, EcsSelf|EcsIsVariable);
    test_str(q->terms[0].second.name, "X");
    test_uint(q->terms[0].second.id, EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_acyclic_same_subj_obj_var(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Traversable);

    test_assert(NULL == ecs_query(world, {
        .terms = {{ 
            .first.id = Rel, 
            .src = { .name = "X", .id = EcsIsVariable },
            .second = { .name = "X", .id = EcsIsVariable }
        }}
    }));

    ecs_fini(world);
}

void Validator_validate_1_term_acyclic_reflexive_same_subj_obj_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Traversable, Reflexive);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.id = Rel, 
            .src = { .name = "X", .id = EcsIsVariable },
            .second = { .name = "X", .id = EcsIsVariable }
        }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, EcsWildcard));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_str(q->terms[0].src.name, "X");
    test_uint(q->terms[0].src.id, EcsSelf|EcsIsVariable);
    test_str(q->terms[0].second.name, "X");
    test_uint(q->terms[0].second.id, EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_non_acyclic_superset(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);

    test_assert(NULL == ecs_query(world, {
        .terms = {{ 
            .id = Tag, 
            .src.id = EcsUp,
            .trav = Rel
        }}
    })); /* cyclic superset is invalid  */

    ecs_fini(world);
}

void Validator_validate_1_term_dont_inherit_default_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, DontInherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Tag }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].trav, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_dont_inherit_pair_default_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, DontInherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Rel, EcsWildcard) }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, EcsWildcard));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].trav, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_inherit_default_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Tag }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].trav, EcsIsA);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_inherit_pair_default_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Rel, EcsWildcard) }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, EcsWildcard));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].trav, EcsIsA);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_override_default_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Override));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Tag }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].trav, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_override_pair_default_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Override));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Rel, EcsWildcard) }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, EcsWildcard));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].trav, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_up_no_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Tag, .src.id = EcsUp }}
    });

    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_up_no_inherit_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Rel, EcsWildcard), .src.id = EcsUp }}
    });

    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, EcsWildcard));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_up_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Override));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Tag, .src.id = EcsUp }}
    });

    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_up_override_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Override));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Rel, EcsWildcard), .src.id = EcsUp }}
    });

    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, EcsWildcard));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_up_isa_no_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Tag, .src.id = EcsUp, .trav = EcsIsA }}
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Validator_validate_1_term_up_isa_no_inherit_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Rel, EcsWildcard), .src.id = EcsUp, .trav = EcsIsA }}
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Validator_validate_1_term_up_isa_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Override));

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Tag, .src.id = EcsUp, .trav = EcsIsA }}
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Validator_validate_1_term_up_isa_override_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Override));

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Rel, EcsWildcard), .src.id = EcsUp, .trav = EcsIsA }}
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Validator_validate_1_term_cascade_implicit_trav(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Tag, .src.id = EcsCascade }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsUp|EcsCascade|EcsIsVariable);
    test_uint(q->terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_cascade_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ecs_add_pair(world, Tag, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .id = Tag, 
            .trav = EcsIsA, .src.id = EcsCascade
        }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsUp|EcsCascade|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_cascade_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ecs_add_pair(world, Tag, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .id = Tag, 
            .trav = EcsChildOf, .src.id = EcsCascade
        }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsUp|EcsCascade|EcsIsVariable);
    test_uint(q->terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_cascade_down(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ecs_add_pair(world, Tag, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .id = Tag, 
            .trav = EcsChildOf, .src.id = EcsCascade
        }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsUp|EcsCascade|EcsIsVariable);
    test_uint(q->terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_optional_only(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .id = Tag, 
            .oper = EcsOptional
        }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsOptional);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_term_transitive_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Transitive);
    ECS_TAG(world, Tgt);
    
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .id = ecs_pair(Rel, Tgt)
        }}
    });
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Rel, Tgt));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].second.id, Tgt|EcsSelf|EcsIsEntity);
    test_assert(q->terms[0].flags_ & EcsTermTransitive);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_variable_as_pred_only(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    test_assert(NULL == ecs_query(world, {
        .terms = {{ .id = EcsVariable }}
    }));

    ecs_fini(world);
}

void Validator_validate_1_variable_as_pred_w_subj(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Src);

    test_assert(NULL == ecs_query(world, {
        .terms = {{ .id = EcsVariable, .src.id = Src }}
    }));

    ecs_fini(world);
}

void Validator_validate_1_variable_as_pred_w_pair(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Src);
    ECS_TAG(world, Tgt);

    test_assert(NULL == ecs_query(world, {
        .terms = {{ 
            .first.id = EcsVariable, 
            .src.id = Src, 
            .second.id = Tgt 
        }}
    }));

    ecs_fini(world);
}

void Validator_validate_1_variable_as_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.id = Foo, 
            .src.id = EcsVariable|EcsIsVariable
        }}
    });
    
    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Foo);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, Foo|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_src_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.id = Foo, 
            .src.id = EcsIsVariable,
            .src.name = "Var"
        }}
    });
    
    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, Foo);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_str(q->terms[0].src.name, "Var");
    test_uint(q->terms[0].src.id, EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_first_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.id = EcsIsVariable,
            .first.name = "Var"
        }}
    });
    
    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, EcsWildcard);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_str(q->terms[0].first.name, "Var");
    test_uint(q->terms[0].first.id, EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_second_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.id = Foo,
            .second.id = EcsIsVariable,
            .second.name = "Var"
        }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Foo, EcsWildcard));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_str(q->terms[0].second.name, "Var");
    test_uint(q->terms[0].second.id, EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_src_var_from_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.id = Foo, 
            .src.name = "$Var"
        }}
    });
    
    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_assert(q->terms != NULL);
    test_int(q->terms[0].id, Foo);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_int(q->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_str(q->terms[0].src.name, "Var");
    test_int(q->terms[0].src.id, EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_first_first_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.name = "$Var"
        }}
    });

    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_assert(q->terms != NULL);
    test_int(q->terms[0].id, EcsWildcard);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_str(q->terms[0].first.name, "Var");
    test_int(q->terms[0].first.id, EcsSelf|EcsIsVariable);
    test_int(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_second_second_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.id = Foo,
            .second.name = "$Var"
        }}
    });

    test_assert(q != NULL);

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_assert(q->terms != NULL);
    test_int(q->terms[0].id, ecs_pair(Foo, EcsWildcard));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_int(q->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_int(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_str(q->terms[0].second.name, "Var");
    test_int(q->terms[0].second.id, EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_variable_as_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 
            .first.id = Foo, 
            .second.id = EcsVariable|EcsIsVariable
        }}
    });
    
    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Foo, Foo));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].second.id, Foo|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_2_terms_or_w_dontinherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, TagB, EcsOnInstantiate, EcsDontInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ TagA, .oper = EcsOr }, { TagB }}
    });

    test_int(q->term_count, 2);
    test_int(q->field_count, 1);

    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsOr);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsUp|EcsIsVariable);

    test_uint(q->terms[1].id, TagB);
    test_int(q->terms[1].oper, EcsAnd);
    test_int(q->terms[1].field_index, 0);
    test_uint(q->terms[1].first.id, TagB|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[1].trav, 0);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_2_terms_or_w_both_dontinherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsDontInherit);
    ecs_add_pair(world, TagB, EcsOnInstantiate, EcsDontInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ TagA, .oper = EcsOr }, { TagB }}
    });

    test_int(q->term_count, 2);
    test_int(q->field_count, 1);

    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsOr);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].trav, 0);

    test_uint(q->terms[1].id, TagB);
    test_int(q->terms[1].oper, EcsAnd);
    test_int(q->terms[1].field_index, 0);
    test_uint(q->terms[1].first.id, TagB|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[1].trav, 0);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_pair_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_id_t pair = ecs_pair(Rel, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{pair}}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, pair);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].second.id, Tgt|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_pred_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_id_t pair = ecs_pair(Rel, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{.first.id = Rel, .second.id = Tgt}}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, pair);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].second.id, Tgt|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_pair_id_and_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);
    ECS_TAG(world, Src);

    ecs_id_t pair = ecs_pair(Rel, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{.id = pair, .src.id = Src}}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, pair);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, Src|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].second.id, Tgt|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_w_pred_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.name = "Tag"
    });

    test_int(q->terms[0].id, Tag);
    test_assert(q->terms[0].first.name ==  NULL);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_w_final_pred_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, Final);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.name = "Tag"
    });

    test_int(q->terms[0].id, Tag);
    test_assert(q->terms[0].first.name ==  NULL);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_w_subj_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Src);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = Tag,
        .terms[0].src.name = "Src"
    });

    test_int(q->terms[0].id, Tag);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_assert(q->terms[0].src.name ==  NULL);
    test_uint(q->terms[0].src.id, Src|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_1_w_obj_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = Tag,
        .terms[0].second.name = "Tgt"
    });

    test_int(q->terms[0].id, ecs_pair(Tag, Tgt));
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_assert(q->terms[0].second.name ==  NULL);
    test_int(q->terms[0].second.id, Tgt|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_this_implicit_variable(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = Tag,
        .terms[0].src.id = EcsThis
    });

    test_int(q->terms[0].id, Tag);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_this_explicit_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = Tag,
        .terms[0].src.id = EcsThis|EcsIsEntity,
    });

    test_int(q->terms[0].id, Tag);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsEntity);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_first_this_implicit_variable(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Src);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = EcsThis,
        .terms[0].src.id = Src
    });

    test_int(q->terms[0].id, EcsWildcard);
    test_uint(q->terms[0].first.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].src.id, Src|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_first_this_explicit_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Src);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = EcsThis|EcsIsEntity,
        .terms[0].src.id = Src
    });

    test_int(q->terms[0].id, EcsThis);
    test_uint(q->terms[0].first.id, EcsThis|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, Src|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_second_this_implicit_variable(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = Tag,
        .terms[0].second.id = EcsThis
    });

    test_int(q->terms[0].id, ecs_pair(Tag, EcsWildcard));
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_int(q->terms[0].second.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_second_this_explicit_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = Tag,
        .terms[0].second.id = EcsThis|EcsIsEntity,
    });

    test_int(q->terms[0].id, ecs_pair(Tag, EcsThis));
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_int(q->terms[0].second.id, EcsThis|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_this_variable_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = Tag,
        .terms[0].src.name = "this",
        .terms[0].src.id = EcsIsVariable
    });

    test_int(q->terms[0].id, Tag);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_assert(q->terms[0].src.name == NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_0_source(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = Tag,
        .terms[0].src.id = EcsIsEntity
    });

    test_int(q->terms[0].id, Tag);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsIsEntity);
    test_uint(q->terms[0].trav, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_0_target(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms[0].first.id = EcsChildOf,
        .terms[0].second.id = EcsIsEntity
    });

    test_uint(q->terms[0].id, ecs_pair(EcsChildOf, 0));
    test_uint(q->terms[0].first.id, EcsChildOf|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].trav, 0);
    test_uint(q->terms[0].second.id, EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_2_terms_w_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{TagA, .oper = EcsOr}, {TagB }}
    });

    test_int(q->terms[0].oper, EcsOr);
    test_uint(q->terms[0].id, TagA);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_int(q->terms[1].oper, EcsAnd);
    test_uint(q->terms[1].id, TagB);
    test_uint(q->terms[1].first.id, TagB|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_2_terms_w_or_mixed_src_flags(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_add_pair(world, TagB, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {TagA, .oper = EcsOr}, 
            {TagB, .oper = EcsAnd, .src.id = EcsUp, .trav = EcsIsA },
        }
    });

    ecs_entity_t base = ecs_new_w(world, TagB);
    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, base);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, it.ids[0]);
    test_uint(0, it.sources[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagB, it.ids[0]);
    test_uint(base, it.sources[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_2_terms_w_or_mixed_src_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ECS_TAG(world, SrcA);
    ECS_TAG(world, SrcB);

    ecs_log_set_level(-4);

    test_assert(NULL == ecs_query(world, {
        .terms = {
            {TagA, .oper = EcsOr, .src.id = SrcA}, 
            {TagB, .oper = EcsOr, .src.id = SrcB}
        }
    }));

    ecs_fini(world);
}

void Validator_validate_2_terms_w_or_mixed_src_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ECS_TAG(world, SrcA);
    ECS_TAG(world, SrcB);

    ecs_log_set_level(-4);

    test_assert(NULL == ecs_query(world, {
        .terms = {
            {TagA, .oper = EcsOr, .src.name = "SrcA"}, 
            {TagB, .oper = EcsOr, .src.name = "SrcB"}
        }
    }));

    ecs_fini(world);
}

void Validator_validate_2_terms_w_or_same_src_w_id_and_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ECS_TAG(world, SrcA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {TagA, .oper = EcsOr, .src.name = "SrcA"}, 
            {TagB, .src.id = SrcA}
        }
    });

    test_int(q->terms[0].oper, EcsOr);
    test_uint(q->terms[0].id, TagA);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, SrcA|EcsSelf|EcsIsEntity);

    test_int(q->terms[1].oper, EcsAnd);
    test_uint(q->terms[1].id, TagB);
    test_uint(q->terms[1].first.id, TagB|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, SrcA|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_and_flag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA, .oper = EcsAndFrom }
        }
    });

    test_int(q->term_count, 1);
    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsAndFrom);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_or_flag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA, .oper = EcsOrFrom }
        }
    });

    test_int(q->term_count, 1);
    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsOrFrom);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_not_flag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA, .oper = EcsNotFrom }
        }
    });

    test_int(q->term_count, 1);
    test_assert(q->terms != NULL);
    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsNotFrom);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_filter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {TagA, .inout = EcsInOutNone}
        }
    });

    test_int(q->term_count, 1);
    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_double_init(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q_1 = ecs_query(world, {
        .terms = {{ Foo }}
    });
    test_assert(q_1 != NULL);

    test_int(q_1->term_count, 1);
    test_int(q_1->terms[0].id, Foo);
    test_int(q_1->terms[0].oper, EcsAnd);
    test_int(q_1->terms[0].field_index, 0);
    test_int(q_1->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_int(q_1->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_desc_t desc = {0};
    ecs_os_memcpy_n(desc.terms, q_1->terms, ecs_term_t, FLECS_TERM_COUNT_MAX);
    ecs_query_t *q_2 = ecs_query_init(world, &desc);
    test_assert(q_2 != NULL);

    test_int(q_2->term_count, 1);
    test_int(q_2->terms[0].id, Foo);
    test_int(q_2->terms[0].oper, EcsAnd);
    test_int(q_2->terms[0].field_index, 0);
    test_int(q_2->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_int(q_2->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q_1);
    ecs_query_fini(q_2);

    ecs_fini(world);
}

void Validator_validate_double_init_w_expr(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q_1 = ecs_query(world, {
        .expr = "Foo"
    });
    test_assert(q_1 != NULL);

    test_int(q_1->term_count, 1);
    test_int(q_1->terms[0].id, Foo);
    test_int(q_1->terms[0].oper, EcsAnd);
    test_int(q_1->terms[0].field_index, 0);
    test_int(q_1->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_int(q_1->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_desc_t desc = {0};
    ecs_os_memcpy_n(desc.terms, q_1->terms, ecs_term_t, FLECS_TERM_COUNT_MAX);
    ecs_query_t *q_2 = ecs_query_init(world, &desc);
    test_assert(q_2 != NULL);

    test_int(q_2->term_count, 1);
    test_int(q_2->terms[0].id, Foo);
    test_int(q_2->terms[0].oper, EcsAnd);
    test_int(q_2->terms[0].field_index, 0);
    test_int(q_2->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_int(q_2->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q_1);
    ecs_query_fini(q_2);

    ecs_fini(world);
}

void Validator_validate_double_init_w_expr_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q_1 = ecs_query(world, {
        .expr = "?Foo"
    });
    test_assert(q_1 != NULL);

    test_int(q_1->term_count, 1);
    test_int(q_1->terms[0].id, Foo);
    test_int(q_1->terms[0].oper, EcsOptional);
    test_int(q_1->terms[0].field_index, 0);
    test_int(q_1->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_int(q_1->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_desc_t desc = {0};
    ecs_os_memcpy_n(desc.terms, q_1->terms, ecs_term_t, FLECS_TERM_COUNT_MAX);
    ecs_query_t *q_2 = ecs_query_init(world, &desc);
    test_assert(q_2 != NULL);

    test_int(q_2->term_count, 1);
    test_int(q_2->terms[0].id, Foo);
    test_int(q_2->terms[0].oper, EcsOptional);
    test_int(q_2->terms[0].field_index, 0);
    test_int(q_2->terms[0].first.id, Foo|EcsSelf|EcsIsEntity);
    test_int(q_2->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q_1);
    ecs_query_fini(q_2);

    ecs_fini(world);
}

void Validator_validate_w_tag_term_is_no_data(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterNoData);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);   
}

void Validator_validate_w_inout_none_term_is_no_data(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .inout = EcsInOutNone }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterNoData);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);  
}

void Validator_validate_w_tag_and_inout_none_term_is_no_data(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsInOutNone },
            { Foo }
        }
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterNoData);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_not_term_is_no_data(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .oper = EcsNot }
        }
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterNoData);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_no_transitive_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, LocatedIn);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = LocatedIn, .second.id = EcsWildcard }
        }
    });

    test_assert(!(q->terms[0].flags_ & EcsTermTransitive));

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_transitive_pair_any_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, LocatedIn, Transitive);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {
                .first.id = LocatedIn, 
                .second.id = EcsWildcard,
                .src.id = EcsAny
            }
        }
    });

    test_assert(!(q->terms[0].flags_ & EcsTermTransitive));

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_transitive_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, LocatedIn, Transitive);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = LocatedIn, .second.id = EcsWildcard }
        }
    });

    test_assert(q->terms[0].flags_ & EcsTermTransitive);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_transitive_tag_no_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, LocatedIn, Transitive);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = LocatedIn }
        }
    });

    test_assert(!(q->terms[0].flags_ & EcsTermTransitive));

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_transitive_tag_self_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, LocatedIn, Transitive);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = LocatedIn, .second.id = EcsWildcard|EcsSelf }
        }
    });

    test_assert(!(q->terms[0].flags_ & EcsTermTransitive));

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_transitive_tag_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, LocatedIn, Transitive);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = LocatedIn, .second.id = EcsAny }
        }
    });

    test_assert(!(q->terms[0].flags_ & EcsTermTransitive));

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_pair_same_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {
                .first.id = Rel, 
                .src.name = "a",
                .src.id = EcsIsVariable,
                .second.name = "a",
                .second.id = EcsIsVariable,
            }
        }
    });

    test_assert(q->terms[0].src.id & EcsIsVariable);
    test_assert(q->terms[0].second.id & EcsIsVariable);
    test_str(q->terms[0].src.name, "a");
    test_str(q->terms[0].second.name, "a");

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_pair_not_same_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {
                .first.id = Rel, 
                .src.name = "a",
                .src.id = EcsIsVariable,
                .second.name = "b",
                .second.id = EcsIsVariable,
            }
        }
    });

    test_assert(q->terms[0].src.id & EcsIsVariable);
    test_assert(q->terms[0].second.id & EcsIsVariable);
    test_str(q->terms[0].src.name, "a");
    test_str(q->terms[0].second.name, "b");

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_pair_no_vars_not_same_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, A);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {
                .first.id = Rel, 
                .src.id = A,
                .second.id = A
            }
        }
    });

    test_uint(q->terms[0].src.id, A|EcsIsEntity|EcsSelf);
    test_uint(q->terms[0].second.id, A|EcsIsEntity|EcsSelf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_pair_wildcard_not_same_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {
                .first.id = Rel, 
                .src.id = EcsWildcard,
                .second.id = EcsWildcard
            }
        }
    });

    test_uint(q->terms[0].src.id, EcsWildcard|EcsIsVariable|EcsSelf);
    test_uint(q->terms[0].second.id, EcsWildcard|EcsIsVariable|EcsSelf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_pair_any_not_same_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {
                .first.id = Rel, 
                .src.id = EcsAny,
                .second.id = EcsAny
            }
        }
    });

    test_uint(q->terms[0].src.id, EcsAny|EcsIsVariable|EcsSelf);
    test_uint(q->terms[0].second.id, EcsAny|EcsIsVariable|EcsSelf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_no_pair_not_same_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {
                .first.id = Rel, 
                .src.name = "a",
                .src.id = EcsIsVariable
            }
        }
    });

    test_assert(q->terms[0].src.id & EcsIsVariable);
    test_assert(!(q->terms[0].second.id & EcsIsVariable));
    test_str(q->terms[0].src.name, "a");

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_not_childof_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {
                .first.id = EcsChildOf, 
                .second.id = EcsAny,
                .oper = EcsNot
            }
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(EcsChildOf, 0));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].inout, EcsInOutDefault);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, EcsChildOf|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].second.id, EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_inherited_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_ENTITY(world, MeleeUnit, (IsA, Unit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = Unit }
        }
    });

    test_assert(q->terms[0].flags_ & EcsTermIdInherited);
    test_uint(q->terms[0].first.id, Unit|EcsIsEntity|EcsSelf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_inherited_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_ENTITY(world, MeleeUnit, (IsA, Unit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = Unit, .second.id = EcsWildcard }
        }
    });

    test_assert(q->terms[0].flags_ & EcsTermIdInherited);
    test_uint(q->terms[0].first.id, Unit|EcsIsEntity|EcsSelf);
    test_uint(q->terms[0].second.id, EcsWildcard|EcsIsVariable|EcsSelf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_non_inherited_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = Unit }
        }
    });

    test_assert(!(q->terms[0].flags_ & EcsTermIdInherited));

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Validator_validate_w_non_inherited_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = Unit, .second.id = EcsWildcard }
        }
    });

    test_assert(!(q->terms[0].flags_ & EcsTermIdInherited));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_first_wildcard_inout_none(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = EcsWildcard }
        }
    });

    test_int(q->terms[0].inout, EcsInOutNone);
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_first_var_inout_none(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.name = "var", .first.id = EcsIsVariable }
        }
    });

    test_int(q->terms[0].inout, EcsInOutNone);
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_pair_wildcard_inout_none(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = EcsWildcard, .second.id = EcsWildcard }
        }
    });

    test_int(q->terms[0].inout, EcsInOutNone);
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_pair_var_inout_none(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {
                .first.name = "x", .first.id = EcsIsVariable,
                .second.name = "y", .second.id = EcsIsVariable,
            }
        }
    });

    test_int(q->terms[0].inout, EcsInOutNone);
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_w_unresolved_by_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query(world, {
        .terms = { { .first.name = "Foo" } },
        .flags = EcsQueryAllowUnresolvedByName
    }));

    ecs_fini(world);
}

void Validator_validate_w_unresolved_by_name_eq(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == Foo",
        .flags = EcsQueryAllowUnresolvedByName
    });

    test_assert(q->terms[0].second.id & EcsIsName);
    test_str(q->terms[0].second.name, "Foo");

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_childof_this(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = { { .first.id = EcsChildOf, .second.id = EcsThis } }
    });

    test_uint(q->terms[0].id, ecs_pair(EcsChildOf, EcsWildcard));
    test_uint(q->terms[0].first.id, EcsChildOf|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].second.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_childof_this_entity(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = { { .first.id = EcsChildOf, .second.id = EcsThis|EcsIsEntity } }
    });

    test_uint(q->terms[0].id, ecs_pair(EcsChildOf, EcsThis));
    test_uint(q->terms[0].first.id, EcsChildOf|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].second.id, EcsThis|EcsSelf|EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_childof_this_by_id(void) {
    ecs_world_t *world = ecs_mini();

    
    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query(world, {
        .terms = { { .id = ecs_pair(EcsChildOf, EcsThis) } }
    }));

    ecs_fini(world);
}

void Validator_validate_filter_flag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .inout = EcsInOutFilter, .id = ecs_id(Position) }
        }
    });

    test_assert(q != NULL);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_first_0_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.name = "#0" }}
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Validator_validate_src_0_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = Tag, .src.name = "#0" }}
    });

    test_assert(q != NULL);
    test_int(q->term_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_uint(q->terms[0].first.id, Tag|EcsIsEntity|EcsSelf);
    test_uint(q->terms[0].src.id, EcsIsEntity);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_second_0_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = Tag, .second.name = "#0" }}
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Validator_validate_singleton_src_w_first_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = Tag, .src.id = EcsVariable }}
    });

    test_assert(q != NULL);
    test_int(q->term_count, 1);
    test_uint(q->terms[0].id, Tag);
    test_uint(q->terms[0].first.id, Tag|EcsIsEntity|EcsSelf);
    test_uint(q->terms[0].src.id, Tag|EcsIsEntity|EcsSelf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_singleton_second_w_first_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = Tag, .second.id = EcsVariable }}
    });

    test_assert(q != NULL);
    test_int(q->term_count, 1);
    test_uint(q->terms[0].id, ecs_pair(Tag, Tag));
    test_uint(q->terms[0].first.id, Tag|EcsIsEntity|EcsSelf);
    test_uint(q->terms[0].src.id, EcsThis|EcsIsVariable|EcsSelf);
    test_uint(q->terms[0].second.id, Tag|EcsIsEntity|EcsSelf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_not_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = EcsWildcard, .oper = EcsNot }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, EcsAny);
    test_int(q->terms[0].oper, EcsNot);
    test_int(q->terms[0].inout, EcsInOutNone);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, EcsAny|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_not_first_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = EcsWildcard, .second.id = ecs_id(Position), .oper = EcsNot }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(EcsAny, ecs_id(Position)));
    test_int(q->terms[0].oper, EcsNot);
    test_int(q->terms[0].inout, EcsInOutNone);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, EcsAny|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].second.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_not_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = ecs_id(Position), .second.id = EcsWildcard, .oper = EcsNot }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(ecs_id(Position), EcsAny));
    test_int(q->terms[0].oper, EcsNot);
    test_int(q->terms[0].inout, EcsInOutNone);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].second.id, EcsAny|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_not_wildcard_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ EcsWildcard, .oper = EcsNot }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, EcsAny);
    test_int(q->terms[0].oper, EcsNot);
    test_int(q->terms[0].inout, EcsInOutNone);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, EcsAny|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_not_wildcard_first_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(EcsWildcard, ecs_id(Position)), .oper = EcsNot }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair(EcsAny, ecs_id(Position)));
    test_int(q->terms[0].oper, EcsNot);
    test_int(q->terms[0].inout, EcsInOutNone);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, EcsAny|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].second.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_not_wildcard_second_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair_t(Position, EcsWildcard), .oper = EcsNot }}
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, ecs_pair_t(Position, EcsAny));
    test_int(q->terms[0].oper, EcsNot);
    test_int(q->terms[0].inout, EcsInOutNone);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].second.id, EcsAny|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_or_same_type(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, TgtA), .oper = EcsOr }, 
            { ecs_pair_t(Position, TgtB) }
        }
    });

    test_int(q->term_count, 2);
    test_int(q->field_count, 1);

    test_uint(q->terms[0].id, ecs_pair_t(Position, TgtA));
    test_int(q->terms[0].oper, EcsOr);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_uint(q->terms[1].id, ecs_pair_t(Position, TgtB));
    test_int(q->terms[1].oper, EcsAnd);
    test_int(q->terms[1].field_index, 0);
    test_uint(q->terms[1].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[1].trav, 0);

    test_assert(q->ids[0] != 0);
    test_int(q->sizes[0], ECS_SIZEOF(Position));
    test_assert(q->data_fields & 1);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_or_different_types(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .oper = EcsOr }, { ecs_id(Velocity) }}
    });

    test_int(q->term_count, 2);
    test_int(q->field_count, 1);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsOr);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_uint(q->terms[1].id, ecs_id(Velocity));
    test_int(q->terms[1].oper, EcsAnd);
    test_int(q->terms[1].field_index, 0);
    test_uint(q->terms[1].first.id, ecs_id(Velocity)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[1].trav, 0);

    test_assert(q->ids[0] == 0);
    test_int(q->sizes[0], 0);
    test_assert(!(q->data_fields & 1));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_or_different_types_1_and_2_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA }, 
            { ecs_id(Position), .oper = EcsOr }, 
            { ecs_id(Velocity) }
        }
    });

    test_int(q->term_count, 3);
    test_int(q->field_count, 2);

    test_uint(q->terms[0].id, TagA);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, TagA|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_uint(q->terms[1].id, ecs_id(Position));
    test_int(q->terms[1].oper, EcsOr);
    test_int(q->terms[1].field_index, 1);
    test_uint(q->terms[1].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);

    test_uint(q->terms[2].id, ecs_id(Velocity));
    test_int(q->terms[2].oper, EcsAnd);
    test_int(q->terms[2].field_index, 1);
    test_uint(q->terms[2].first.id, ecs_id(Velocity)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[2].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[2].trav, 0);

    test_assert(q->ids[1] == 0);
    test_int(q->sizes[1], 0);
    test_assert(!(q->data_fields & (1u << 1)));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_trav_isa_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { EcsWildcard, .trav = EcsIsA }
        }
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Validator_validate_trav_isa_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { EcsAny, .trav = EcsIsA }
        }
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Validator_validate_custom_trav_w_inherit_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Traversable);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .trav = Rel },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].trav, Rel);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_custom_trav_w_inherit_id_w_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Traversable);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .trav = Rel, .src.id = EcsSelf|EcsUp },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].trav, Rel);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_custom_trav_w_inherit_id_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Traversable);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .trav = Rel, .src.id = EcsUp },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].trav, Rel);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_1_term_is_cacheable(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermIsCacheable|EcsTermIsTrivial|EcsTermKeepAlive);

    test_assert((q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryIsTrivial|EcsQueryIsCacheable|
        EcsQueryHasCacheable|EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar|EcsQueryHasOutTerms,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_1_term_tag_is_cacheable(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_new_w(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Tag },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, Tag);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Tag|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermIsCacheable|EcsTermIsTrivial|EcsTermKeepAlive);

    test_assert(!(q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryIsTrivial|EcsQueryIsCacheable|
        EcsQueryHasCacheable|EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_1_term_pair_is_cacheable(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new(world);
    ecs_entity_t tgt = ecs_new(world);

    ecs_new_w_pair(world, rel, tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(rel, tgt) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_pair(rel, tgt));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].second.id, tgt|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermIsCacheable|EcsTermIsTrivial|EcsTermKeepAlive);

    test_assert(!(q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryIsTrivial|EcsQueryIsCacheable|
        EcsQueryHasCacheable|EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_1_term_pair_recycled_is_cacheable(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new(world);
    ecs_delete(world, rel);
    rel = ecs_new(world);
    test_assert((uint32_t)rel != rel);

    ecs_entity_t tgt = ecs_new(world);
    ecs_delete(world, tgt);
    tgt = ecs_new(world);
    test_assert((uint32_t)tgt != tgt);

    ecs_new_w_pair(world, rel, tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(rel, tgt) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_pair(rel, tgt));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].second.id, tgt|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermIsCacheable|EcsTermIsTrivial|EcsTermKeepAlive);

    test_assert(!(q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryIsTrivial|EcsQueryIsCacheable|
        EcsQueryHasCacheable|EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_2_term_is_cacheable(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) },
            { ecs_id(Velocity) },
        }
    });

    test_int(q->term_count, 2);
    test_int(q->field_count, 2);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermIsCacheable|EcsTermIsTrivial|EcsTermKeepAlive);

    test_uint(q->terms[1].id, ecs_id(Velocity));
    test_int(q->terms[1].oper, EcsAnd);
    test_int(q->terms[1].field_index, 1);
    test_uint(q->terms[1].first.id, ecs_id(Velocity)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[1].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[1].flags_, EcsTermIsCacheable|EcsTermIsTrivial|EcsTermKeepAlive);

    test_assert((q->data_fields & (1 << 0)));
    test_assert((q->data_fields & (1 << 1)));

    test_query_flags(EcsQueryIsTrivial|EcsQueryIsCacheable|
        EcsQueryHasCacheable|EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar|EcsQueryHasOutTerms,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_can_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsUp|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermIsCacheable|EcsTermIsTrivial|EcsTermKeepAlive);

    test_assert((q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryIsTrivial|EcsQueryIsCacheable|
        EcsQueryHasCacheable|EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryHasTableThisVar|EcsQueryHasOutTerms,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_can_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermKeepAlive|EcsTermIsToggle|EcsTermIsCacheable);

    test_assert((q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryMatchOnlyThis|EcsQueryMatchThis|EcsQueryHasCacheable|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar|EcsQueryHasOutTerms,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_sparse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermKeepAlive|EcsTermIsSparse);

    test_assert((q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar|EcsQueryHasOutTerms,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_union(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_add_id(world, Rel, EcsUnion);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Rel },
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, Rel);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermKeepAlive|EcsTermIsCacheable|EcsTermIsTrivial);

    test_assert(!(q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryIsTrivial|EcsQueryIsCacheable|
        EcsQueryHasCacheable|EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_union_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, Rel, EcsUnion);

    ecs_new_w_pair(world, Rel, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, Tgt) },
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_pair(Rel, Tgt));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Rel|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermIsUnion);

    test_assert(!(q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_transitive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsTransitive);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermIsCacheable|EcsTermIsTrivial|EcsTermKeepAlive);

    test_assert((q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryIsTrivial|EcsQueryIsCacheable|
        EcsQueryHasCacheable|EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar|EcsQueryHasOutTerms,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_transitive_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, ecs_id(Position), EcsTransitive);

    ecs_new_w_pair(world, ecs_id(Position), Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, Tgt) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_pair_t(Position, Tgt));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermKeepAlive|EcsTermTransitive);

    test_assert((q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar|EcsQueryHasOutTerms,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_reflexive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsReflexive);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_id(Position));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermIsCacheable|EcsTermIsTrivial|EcsTermKeepAlive);

    test_assert((q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryIsTrivial|EcsQueryIsCacheable|
        EcsQueryHasCacheable|EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar|EcsQueryHasOutTerms,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_reflexive_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, ecs_id(Position), EcsReflexive);

    ecs_new_w_pair(world, ecs_id(Position), Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, Tgt) },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, ecs_pair_t(Position, Tgt));
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, ecs_id(Position)|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermKeepAlive|EcsTermReflexive);

    test_assert((q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar|EcsQueryHasOutTerms,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Validator_validate_simple_w_inherited_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_ENTITY(world, MeleeUnit, (IsA, Unit));

    ecs_new_w(world, Unit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Unit },
        }
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_int(q->var_count, 1);
    test_assert(q->vars != NULL);
    test_assert(q->vars[0] == NULL);

    test_uint(q->terms[0].id, Unit);
    test_int(q->terms[0].oper, EcsAnd);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, Unit|EcsSelf|EcsIsEntity);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].flags_, EcsTermKeepAlive|EcsTermIdInherited);

    test_assert(!(q->data_fields & (1 << 0)));

    test_query_flags(EcsQueryMatchOnlyThis|EcsQueryMatchThis|
        EcsQueryMatchOnlySelf|EcsQueryHasTableThisVar,
        q->flags);
    
    ecs_query_fini(q);

    ecs_fini(world);
}
