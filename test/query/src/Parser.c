#include <query.h>

static
int term_count(ecs_query_t *q) {
    test_assert(q != NULL);
    return q->term_count;
}

static
ecs_term_t* query_terms(ecs_query_t *q) {
    test_assert(q != NULL);
    return q->terms;
}

static
void print_term_id_flags(ecs_id_t value) {
    if (value & EcsIsEntity) {
        printf("|EcsIsEntity");
    }
    if (value & EcsIsVariable) {
        printf("|EcsIsVariable");
    }
    if (value & EcsIsName) {
        printf("|EcsIsName");
    }
    if (value & EcsSelf) {
        printf("|EcsSelf");
    }
    if (value & EcsUp) {
        printf("|EcsUp");
    }
    if (value & EcsCascade) {
        printf("|EcsCascade");
    }
    if (value & EcsDesc) {
        printf("|EcsDesc");
    }
}

#define test_id(actual, value)\
    if ((value) != (actual)) {\
        printf("actual:   %u", (uint32_t)(actual));\
        print_term_id_flags(actual);\
        printf("\n");\
        printf("expected: %u", (uint32_t)(value));\
        print_term_id_flags(value);\
        printf("\n");\
        test_assert(false);\
    }

#define test_first(column, e, flgs)\
    test_id(column.first.id, e | flgs);

#define test_src(column, e, flgs)\
    test_id(column.src.id, e | flgs);

#define test_second(column, e, flgs)\
    test_id(column.second.id, e | flgs);

#define test_first_var(column, e, flgs, str)\
    test_first(column, e, flgs|EcsIsVariable);\
    test_str(column.first.name, str);

#define test_src_var(column, e, flgs, str)\
    test_src(column, e, flgs|EcsIsVariable);\
    test_str(column.src.name, str);

#define test_second_var(column, e, flgs, str)\
    test_second(column, e, flgs|EcsIsVariable);\
    test_str(column.second.name, str);

void Parser_resolve_this(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup(world, ".");
    test_assert(e != 0);
    test_assert(e == EcsThis);

    ecs_fini(world);
}

void Parser_resolve_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup(world, "*");
    test_assert(e != 0);
    test_assert(e == EcsWildcard);

    ecs_fini(world);
}

void Parser_resolve_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup(world, "_");
    test_assert(e != 0);
    test_assert(e == EcsAny);

    ecs_fini(world);
}

void Parser_resolve_is_a(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup(world, "IsA");
    test_assert(e != 0);
    test_assert(e == EcsIsA);

    ecs_fini(world);
}

void Parser_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "0"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_assert(terms[0].src.name == NULL);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this_by_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(this)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this_by_var_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($this)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_explicit_subject_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(*)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsWildcard, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_explicit_subject_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(_)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsAny, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_explicit_subject_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(#0)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_as_predicate(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "this(Subj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsThis, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_var_as_predicate(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$this(Subj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_lowercase_var_as_predicate(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$this(Subj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_as_object(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj, this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], EcsThis, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_var_as_object(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj, $this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_wildcard_pred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(*, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsWildcard, EcsSelf|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_wildcard_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, *)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], EcsWildcard, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_any_pred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(_, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsAny, EcsSelf|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_any_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, _)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], EcsAny, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_this_pred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "($this, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_this_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, $this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_pred_w_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred|self, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_obj_w_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, Obj|self)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_pred_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred|up, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsUp|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_obj_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, Obj|up)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_pred_w_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred|self|up, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_obj_w_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, Obj|self|up)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_pred_w_invalid_flags(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred|, Obj)"
    });
    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_obj_w_invalid_flags(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, Obj|)"
    });
    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_pair_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($this, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this_by_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(this, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this_by_var_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($this, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_pred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "*($this, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsWildcard, EcsSelf|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(*, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsWildcard, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($this, *)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], EcsWildcard, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_0_name_object(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "0");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(ChildOf, 0)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], e, EcsIsEntity|EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id, ecs_pair(EcsChildOf, e));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_0_name_not_found_object(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(ChildOf, 0)"
    });
    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_0_digit_object(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(ChildOf, #0)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], 0, EcsIsEntity|EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id, ecs_pair(EcsChildOf, 0));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_0_object(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "ChildOf($this, #0)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], 0, EcsIsEntity|EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id, ecs_pair(EcsChildOf, 0));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(#0, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_in_component_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[in] Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_in_component_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[in] Pred(Subj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_in_pair_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[in] (Pred, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_in_pair_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[in] Pred(Subj, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_inout_component_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_inout_component_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] Pred(Subj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_inout_pair_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] (Pred, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_inout_pair_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] Pred(Subj, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_out_component_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[out] Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_out_component_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[out] Pred(Subj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_out_pair_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[out] (Pred, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_out_pair_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[out] Pred(Subj, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_inout_filter_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[none] Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_inout_w_not_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[out] !Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsOut);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_inout_w_optional_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[in] ?Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$this($)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_implicit_no_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred()"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_explicit_no_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(#0)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_no_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(#0, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_variable_single_char(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, X);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($X)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf, "X");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_variable_multi_char(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XYZ);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($XYZ)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf, "XYZ");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_variable_multi_char_w_underscore(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XY_Z);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($XY_Z)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf, "XY_Z");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_variable_multi_char_w_number(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XY_1);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($XY_1)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf, "XY_1");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_variable_multi_char_not_allcaps(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XYZ);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($xyZ)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf, "xyZ");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pred_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "($Pred, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first_var(terms[0], 0, EcsSelf, "Pred");
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_obj_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, $Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second_var(terms[0], 0, EcsSelf, "Obj");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "!Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "!(Pred, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "!Pred(Subj, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_2_component_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "!Pred_1, !Pred_2"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutNone);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_2_component_not_no_space(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "!Pred_1,!Pred_2"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutNone);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_component_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_2_component_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?Pred_1, ?Pred_2"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsOptional);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_2_component_optional_no_space(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?Pred_1,?Pred_2"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsOptional);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_from_and(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "and | Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAndFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_from_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "or | Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsOrFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_from_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "not | Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsNotFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?(Pred, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?Pred(Subj, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pred_implicit_subject_w_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "toggle | Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);    
}

void Parser_pred_explicit_subject_w_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "toggle | Pred($this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);    
}

void Parser_pred_no_subject_w_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "toggle | Pred()"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);  
}

void Parser_pair_implicit_subject_w_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);   
}

void Parser_pair_explicit_subject_w_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred($this, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_inout_role_pred_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] toggle | Pred"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_query_fini(q);

    ecs_fini(world);  
}

void Parser_inout_role_pred_no_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] toggle | Pred()"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_query_fini(q);

    ecs_fini(world);  
}

void Parser_inout_role_pred_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] toggle | Pred($this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_query_fini(q);

    ecs_fini(world);  
}

void Parser_inout_role_pair_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] (Pred, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_query_fini(q);

    ecs_fini(world);  
}

void Parser_inout_role_pair_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] Pred($this, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_query_fini(q);

    ecs_fini(world);  
}

void Parser_2_pred_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1, Pred_2"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_pred_no_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1(), Pred_2()"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_pred_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1($this), Pred_2($this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_pair_implicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred_1, Obj_1), (Pred_2, Obj_2)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_pair_explicit_subject(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1($this, Obj_1), Pred_2($this, Obj_2)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_pred_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "toggle | Pred_1, toggle | Pred_2"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_pair_implicit_subj_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred_1, Obj_1), (Pred_2, Obj_2)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_pair_explicit_subj_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1($this, Obj_1), Pred_2($this, Obj_2)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_or_pred_implicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1 || Pred_2"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_or_pred_explicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1($this) || Pred_2($this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_or_pair_implicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred_1, Obj_1) || (Pred_2, Obj_2)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);    
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);    
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_or_pair_explicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1($this, Obj_1) || Pred_2($this, Obj_2)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);    
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);    
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_or_pred_inout(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "[inout] Pred_1 || Pred_2"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOut);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_2_or_no_space(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1||Pred_2"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_2_or_w_not_1st_arg(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "!Pred_1 || Pred_2"
    });

    test_assert(q == NULL);

    ecs_fini(world); 
}

void Parser_2_or_w_not_2nd_arg(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1 || !Pred_2"
    });

    test_assert(q == NULL);

    ecs_fini(world); 
}

void Parser_2_or_w_optional_1st_arg(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?Pred_1 || Pred_2"
    });

    test_assert(q == NULL);

    ecs_fini(world); 
}

void Parser_2_or_w_optional_2nd_arg(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1 || ?Pred_2"
    });

    test_assert(q == NULL);

    ecs_fini(world); 
}

void Parser_1_entity_id_pred_implicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ecs_make_alive(world, 100);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "#100"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], 100, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_1_entity_id_pred_no_subj(void) {
    ecs_world_t *world = ecs_mini();

    ecs_make_alive(world, 100);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "#100()"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], 100, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_1_entity_id_pred_explicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ecs_make_alive(world, 100);
    ecs_make_alive(world, 200);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "#100(#200)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], 100, EcsSelf|EcsIsEntity);
    test_src(terms[0], 200, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_1_entity_id_pair_implicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_make_alive(world, 100);
    ecs_make_alive(world, 300);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(#100, #300)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], 100, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], 300, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_1_entity_id_pair_explicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_make_alive(world, 100);
    ecs_make_alive(world, 200);
    ecs_make_alive(world, 300);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "#100(#200, #300)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], 100, EcsSelf|EcsIsEntity);
    test_src(terms[0], 200, EcsSelf|EcsIsEntity);
    test_second(terms[0], 300, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_1_digit_pred_implicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "100");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "100"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], e, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_1_digit_pred_no_subj(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "100");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "100()"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], e, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_1_digit_pred_explicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_set_name(world, 0, "100");
    ecs_entity_t e2 = ecs_set_name(world, 0, "200");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "100(200)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], e1, EcsSelf|EcsIsEntity);
    test_src(terms[0], e2, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_1_digit_pair_implicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_entity_t e1 = ecs_set_name(world, 0, "100");
    ecs_entity_t e2 = ecs_set_name(world, 0, "300");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(100, 300)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], e1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], e2, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_1_digit_pair_explicit_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_entity_t e1 = ecs_set_name(world, 0, "100");
    ecs_entity_t e2 = ecs_set_name(world, 0, "200");
    ecs_entity_t e3 = ecs_set_name(world, 0, "300");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "100(200, 300)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], e1, EcsSelf|EcsIsEntity);
    test_src(terms[0], e2, EcsSelf|EcsIsEntity);
    test_second(terms[0], e3, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(self)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(up)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(self|up)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(up|cascade)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(up|cascade|self)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(cascade)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_inclusive_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(self|cascade)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsSelf|EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_cascade_w_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_ENTITY(world, Rel, Traversable);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(cascade Rel)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].trav, Rel);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_inclusive_cascade_w_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_ENTITY(world, Rel, Traversable);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(self|cascade Rel)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsSelf|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].trav, Rel);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Pred, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(up IsA)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].trav, EcsIsA);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_cascade_superset_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Pred, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(cascade|up IsA)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].trav, EcsIsA);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Pred, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(up|cascade IsA)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].trav, EcsIsA);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade_childof_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Pred, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?Pred(up|cascade IsA)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].trav, EcsIsA);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_expr_w_symbol(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {
            .name = "Foo",
            .symbol = "FooBar"
        })
    });

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "FooBar"
    });
    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_expr_w_newline(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity(world, { .name = "Foo" });

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Foo\n"
    });
    test_assert(q != NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_subj_entity_w_explicit_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj|self)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_subj_entity_w_explicit_self_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj|self|up)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_uint(terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_subj_entity_w_explicit_superset_relation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Pred, (OnInstantiate, Inherit));
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj|up IsA)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsUp|EcsIsEntity);
    test_int(terms[0].trav, EcsIsA);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_subj_entity_w_explicit_self_superset_relation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Pred, (OnInstantiate, Inherit));
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj|self|up IsA)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].trav, EcsIsA);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_obj_entity_w_explicit_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj, Obj|self)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_obj_entity_w_explicit_self_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj, Obj|self|up)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_obj_entity_w_explicit_superset_relation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj, Obj|up ChildOf)"
    });
    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_obj_entity_w_explicit_self_superset_relation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj, Obj|self|up ChildOf)"
    });
    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_pred_entity_w_explicit_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred|self(Subj)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pred_entity_w_explicit_self_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred|self|up(Subj)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_w_explicit_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred|self"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_w_explicit_self_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred|self|up"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_2_terms_w_explicit_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Pred2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred|self, Pred2"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_2_terms_w_explicit_self_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Pred2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred|self|up, Pred2"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_newline(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\n"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_2_newlines(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\n\n"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_3_newlines(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\n\n"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_space(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = " "
    });
    test_assert(q != NULL);

    test_int(term_count(q), 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_2_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "  "
    });
    test_assert(q != NULL);

    test_int(term_count(q), 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_trailing_newline(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred\n"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_2_trailing_newlines(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred\n\n"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_trailing_space(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred "
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_2_trailing_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred  "
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_template_type(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Pred = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Template<int>"
    });
    test_assert(Pred != 0);
    test_str(ecs_get_name(world, Pred), "Template<int>");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Template<int>"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_template_type_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Pred = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Template<Foo<int>>"
    });
    test_assert(Pred != 0);
    test_str(ecs_get_name(world, Pred), "Template<Foo<int>>");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Template<Foo<int>>"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_template_type_multiple_args(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Pred = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Template<int, float>"
    });
    test_assert(Pred != 0);
    test_str(ecs_get_name(world, Pred), "Template<int, float>");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Template<int, float>"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_template_type_multiple_args_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Pred = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Template<Foo<int>, Foo<float>>"
    });
    test_assert(Pred != 0);
    test_str(ecs_get_name(world, Pred), "Template<Foo<int>, Foo<float>>");

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Template<Foo<int>, Foo<float>>"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_template_type_unbalanced_open(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Template<int"
    });
    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_template_type_unbalanced_close(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Templateint>"
    });
    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_predicate_w_parens(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Pred)"
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_not_alive_pred(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    test_assert(!ecs_is_alive(world, 5000));

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "5000"
    }));

    ecs_fini(world);
}

void Parser_not_alive_subj(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    test_assert(!ecs_is_alive(world, 5000));

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(5000)"
    }));

    ecs_fini(world);
}

void Parser_not_alive_obj(void) {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    test_assert(!ecs_is_alive(world, 5000));

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(Subj, 5000)"
    }));

    ecs_fini(world);
}

void Parser_this_subj_var_kind(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, TagB($this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_obj_var_kind(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, TagB(Subj, $this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[1], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_subj_obj_var_kind(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, TagB($this, $this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_var_w_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA($Var)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], 0, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);    
    test_str(terms[0].src.name, "Var");

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_entity_pred_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].first.name, NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_entity_subj_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA(Subj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].src.name, NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_entity_obj_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA(Subj, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].second.name, NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_pred_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$this"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    test_str(terms[0].first.name, NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_subj_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA($this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].src.name, NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_this_obj_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Subj);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA(Subj, $this)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_second(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].second.name, NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_invalid_variable_only(void) {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$"
    }));

    ecs_fini(world);
}

void Parser_oneof_self_pred_w_relative_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);
    ECS_ENTITY(world, Obj, (ChildOf, Rel));

    test_assert( ecs_lookup_child(world, 0, "Obj") == 0 ); // sanity check

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Rel, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);
 
    ecs_fini(world);
}

void Parser_oneof_other_pred_w_relative_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Parent);
    ECS_ENTITY(world, Rel, (OneOf, Parent));
    ECS_ENTITY(world, Obj, (ChildOf, Parent));

    test_assert( ecs_lookup_child(world, 0, "Obj") == 0 ); // sanity check

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Rel, Obj)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);
 
    ecs_fini(world);
}

void Parser_oneof_self_pred_w_invalid_obj(void) {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);
    ECS_TAG(world, Obj);

    test_assert( ecs_lookup_child(world, 0, "Obj") != 0 ); // sanity check

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Rel, Obj)"
    }));
 
    ecs_fini(world);
}

void Parser_oneof_other_pred_w_invalid_obj(void) {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Parent);
    ECS_ENTITY(world, Rel, (OneOf, Parent));
    ECS_TAG(world, Obj);

    test_assert( ecs_lookup_child(world, 0, "Obj") != 0 ); // sanity check

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Rel, Obj)"
    }));
 
    ecs_fini(world);
}

void Parser_oneof_w_other_entity_w_same_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);
    ECS_ENTITY(world, Obj, (ChildOf, Rel));
    ecs_entity_t obj_2 = ecs_entity(world, { .name = "Obj" });

    test_uint( ecs_lookup_child(world, 0, "Obj"), obj_2 );
    test_uint( ecs_lookup(world, "Rel.Obj"), Obj );

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, Obj)"
    });

    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);
 
    ecs_fini(world);
}

void Parser_oneof_w_other_entity_w_same_name_w_set_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);
    ECS_ENTITY(world, Obj, (ChildOf, Rel));

    ecs_entity_t p = ecs_entity(world, { .name = "parent" });
    ecs_entity_t obj_2 = ecs_entity(world, { .name = "Obj" });
    ecs_add_pair(world, obj_2, EcsChildOf, p);

    ecs_set_scope(world, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, Obj)"
    });

    test_assert(q != NULL);

    ecs_set_scope(world, 0);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);
 
    ecs_fini(world);
}

void Parser_oneof_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)"
    });
    
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], EcsWildcard, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);
 
    ecs_fini(world);
}

void Parser_oneof_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, _)"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], EcsAny, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);
 
    ecs_fini(world);
}

void Parser_oneof_w_fullpath(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);
    ECS_ENTITY(world, Obj, (ChildOf, Rel));
    ecs_entity_t obj_2 = ecs_entity(world, { .name = "Obj" });

    test_uint( ecs_lookup_child(world, 0, "Obj"), obj_2 );
    test_uint( ecs_lookup(world, "Rel.Obj"), Obj );

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, Rel.Obj)"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);
 
    ecs_fini(world);
}

void Parser_pair_implicit_src_missing_rel(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(, Obj)"
    }));

    ecs_fini(world);
}

void Parser_pair_implicit_src_missing_obj(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Rel, )"
    }));

    ecs_fini(world);
}

void Parser_pair_explicit_src_missing_src(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Rel(, Obj)"
    }));

    ecs_fini(world);
}

void Parser_pair_explicit_src_missing_obj(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Rel($this, )"
    }));

    ecs_fini(world);
}

void Parser_eq_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, First);
    ECS_TAG(world, Second);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "First == Second"
    }));

    ecs_fini(world);
}

void Parser_eq_id_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Second);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$this == Second"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Second, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_eq_var_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, First);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "First == $this"
    }));

    ecs_fini(world);
}

void Parser_eq_var_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, First);
    ECS_TAG(world, Second);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "First($y), $x == $y"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[1], EcsPredEq, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf, "x");
    test_second_var(terms[1], 0, EcsSelf, "y");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_eq_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, First);
    ECS_TAG(world, Second);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "First($this, $y), $x == $y"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[1], EcsPredEq, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf, "x");
    test_second_var(terms[1], 0, EcsSelf, "y");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_neq_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, First);
    ECS_TAG(world, Second);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "First != Second"
    }));

    ecs_fini(world);
}

void Parser_neq_id_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Second);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$this != Second"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Second, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_neq_var_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, First);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "First != $this"
    }));

    ecs_fini(world);
}

void Parser_neq_var_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, First);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "First($y), $x != $y"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[1], EcsPredEq, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf, "x");
    test_second_var(terms[1], 0, EcsSelf, "y");
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_neq_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, First);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "First($this, $y), $x != $y"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[1], EcsPredEq, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf, "x");
    test_second_var(terms[1], 0, EcsSelf, "y");
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_eq_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\"First\" == \"Second\""
    }));

    ecs_fini(world);
}

void Parser_eq_name_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\"First\" == $y"
    }));

    ecs_fini(world);
}

void Parser_eq_var_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$x == \"Second\""
    });

    test_assert(q != NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_neq_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\"First\" != \"Second\""
    }));

    ecs_fini(world);
}

void Parser_neq_name_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\"First\" != $y"
    }));

    ecs_fini(world);
}

void Parser_neq_var_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$x != \"Second\""
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_uint(terms[0].src.id, (EcsSelf|EcsIsVariable));
    test_str(terms[0].src.name, "x");
    test_uint(terms[0].second.id, (EcsSelf|EcsIsName));
    test_str(terms[0].second.name, "Second");
    test_int(terms[0].oper, EcsNot);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_match_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\"First\" ~= \"Second\""
    }));

    ecs_fini(world);
}

void Parser_match_name_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\"First\" ~= $y"
    }));

    ecs_fini(world);
}

void Parser_match_var_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$x ~= \"Second\""
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredMatch, EcsSelf|EcsIsEntity);
    test_uint(terms[0].src.id, (EcsSelf|EcsIsVariable));
    test_str(terms[0].src.name, "x");
    test_uint(terms[0].second.id, (EcsSelf|EcsIsName));
    test_str(terms[0].second.name, "Second");
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_match_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$x ~= $y"
    }));

    ecs_fini(world);
}

void Parser_nmatch_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\"First\" ~= \"!Second\""
    }));

    ecs_fini(world);
}

void Parser_nmatch_name_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "\"!First\" ~= $y"
    }));

    ecs_fini(world);
}

void Parser_nmatch_var_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$x ~= \"!Second\""
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredMatch, EcsSelf|EcsIsEntity);
    test_uint(terms[0].src.id, (EcsSelf|EcsIsVariable));
    test_str(terms[0].src.name, "x");
    test_uint(terms[0].second.id, (EcsSelf|EcsIsName));
    test_str(terms[0].second.name, "Second");
    test_int(terms[0].oper, EcsNot);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_eq_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$x == $x"
    }));

    ecs_fini(world);
}

void Parser_neq_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$x != $x"
    }));

    ecs_fini(world);
}

void Parser_eq_same_var_this(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$this == $this"
    }));

    ecs_fini(world);
}

void Parser_neq_same_var_this(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "$this != $this"
    }));

    ecs_fini(world);
}

void Parser_eq_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "!$this == Foo"
    }));

    ecs_fini(world);
}

void Parser_neq_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "!$this != Foo"
    }));

    ecs_fini(world);
}

void Parser_match_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "!$this ~= \"Foo\""
    }));

    ecs_fini(world);
}

void Parser_eq_w_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?$this == Foo"
    }));

    ecs_fini(world);
}

void Parser_neq_w_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?$this != Foo"
    }));

    ecs_fini(world);
}

void Parser_match_w_optional(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "?$this ~= \"Foo\""
    }));

    ecs_fini(world);
}

void Parser_eq_0(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == #0"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_uint(terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_str(terms[0].src.name, NULL);
    test_uint(terms[0].second.id, EcsSelf|EcsIsEntity);
    test_str(terms[0].second.name, NULL);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_neq_0(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != #0"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_uint(terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_str(terms[0].src.name, NULL);
    test_uint(terms[0].second.id, EcsSelf|EcsIsEntity);
    test_str(terms[0].second.name, NULL);
    test_int(terms[0].oper, EcsNot);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_eq_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == *"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_uint(terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_str(terms[0].src.name, NULL);
    test_uint(terms[0].second.id, EcsWildcard|EcsSelf|EcsIsVariable);
    test_str(terms[0].second.name, NULL);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_neq_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != *"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_uint(terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_str(terms[0].src.name, NULL);
    test_uint(terms[0].second.id, EcsAny|EcsSelf|EcsIsVariable);
    test_str(terms[0].second.name, NULL);
    test_int(terms[0].oper, EcsNot);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_eq_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == _"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_uint(terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_str(terms[0].src.name, NULL);
    test_uint(terms[0].second.id, EcsAny|EcsSelf|EcsIsVariable);
    test_str(terms[0].second.name, NULL);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_neq_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != _"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_uint(terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_str(terms[0].src.name, NULL);
    test_uint(terms[0].second.id, EcsAny|EcsSelf|EcsIsVariable);
    test_str(terms[0].second.name, NULL);
    test_int(terms[0].oper, EcsNot);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_query_scope_1_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, {TagB}"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 4);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    test_first(terms[2], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[3], 0, EcsIsEntity);
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_query_scope_1_term_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, { TagB }"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 4);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    test_first(terms[2], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[3], 0, EcsIsEntity);
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_query_scope_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, { TagB, TagC }"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 5);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    test_first(terms[2], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], TagC, EcsSelf|EcsIsEntity);
    test_src(terms[3], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutDefault);

    test_first(terms[4], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[4], 0, EcsIsEntity);
    test_int(terms[4].oper, EcsAnd);
    test_int(terms[4].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_query_nested_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, {TagB, {TagC}}"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 7);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    test_first(terms[2], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[3], 0, EcsIsEntity);
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutNone);

    test_first(terms[4], TagC, EcsSelf|EcsIsEntity);
    test_src(terms[4], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[4].oper, EcsAnd);
    test_int(terms[4].inout, EcsInOutDefault);

    test_first(terms[5], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[5], 0, EcsIsEntity);
    test_int(terms[5].oper, EcsAnd);
    test_int(terms[5].inout, EcsInOutNone);

    test_first(terms[6], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[6], 0, EcsIsEntity);
    test_int(terms[6].oper, EcsAnd);
    test_int(terms[6].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_query_nested_scope_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, { TagB, { TagC } }"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 7);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    test_first(terms[2], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[3], 0, EcsIsEntity);
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutNone);

    test_first(terms[4], TagC, EcsSelf|EcsIsEntity);
    test_src(terms[4], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[4].oper, EcsAnd);
    test_int(terms[4].inout, EcsInOutDefault);

    test_first(terms[5], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[5], 0, EcsIsEntity);
    test_int(terms[5].oper, EcsAnd);
    test_int(terms[5].inout, EcsInOutNone);

    test_first(terms[6], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[6], 0, EcsIsEntity);
    test_int(terms[6].oper, EcsAnd);
    test_int(terms[6].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_query_scope_unbalanced(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, { TagB"
    }));

    ecs_fini(world);
}

void Parser_query_not_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, !{TagB}"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 4);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutNone);

    test_first(terms[2], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[3], 0, EcsIsEntity);
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_query_empty_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "TagA, !{}"
    }));

    ecs_fini(world);
}

void Parser_query_scope_newline_after_open(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, {\nTagB}"
    });
    test_assert(q != NULL);
    test_int(q->term_count, 4);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    test_first(terms[2], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[3], 0, EcsIsEntity);
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_query_scope_newline_after_close(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, {TagB\n}"
    });
    test_assert(q != NULL);
    test_int(q->term_count, 4);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    test_first(terms[2], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[3], 0, EcsIsEntity);
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_query_term_after_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, {TagB}, TagC"
    });

    test_assert(q != NULL);
    test_int(q->term_count, 5);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsScopeOpen, EcsSelf|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutNone);

    test_first(terms[2], TagB, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], EcsScopeClose, EcsSelf|EcsIsEntity);
    test_src(terms[3], 0, EcsIsEntity);
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutNone);

    test_first(terms[4], TagC, EcsSelf|EcsIsEntity);
    test_src(terms[4], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[4].oper, EcsAnd);
    test_int(terms[4].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_override_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "auto_override|Tag"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Tag, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_override_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "auto_override|(Rel, Tgt)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Tgt, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_3_args(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "ChildOf($this, $parent, $grandparent)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "parent");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "parent");
    test_second_var(terms[1], 0, EcsSelf|EcsIsVariable, "grandparent");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_3_args_implicit_this(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(ChildOf, $parent, $grandparent)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "parent");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "parent");
    test_second_var(terms[1], 0, EcsSelf|EcsIsVariable, "grandparent");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_4_args(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "ChildOf($this, $parent, $grandparent, $greatgrandparent)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 3);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "parent");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "parent");
    test_second_var(terms[1], 0, EcsSelf|EcsIsVariable, "grandparent");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[2], 0, EcsSelf|EcsIsVariable, "grandparent");
    test_second_var(terms[2], 0, EcsSelf|EcsIsVariable, "greatgrandparent");
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_4_args_implicit_this(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(ChildOf, $parent, $grandparent, $greatgrandparent)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 3);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "parent");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "parent");
    test_second_var(terms[1], 0, EcsSelf|EcsIsVariable, "grandparent");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[2], 0, EcsSelf|EcsIsVariable, "grandparent");
    test_second_var(terms[2], 0, EcsSelf|EcsIsVariable, "greatgrandparent");
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_3_args_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "ChildOf($this, $parent, $grandparent), Rel($this, $parent)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 3);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "parent");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "parent");
    test_second_var(terms[1], 0, EcsSelf|EcsIsVariable, "grandparent");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_second_var(terms[2], 0, EcsSelf|EcsIsVariable, "parent");
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_cascade_desc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred(cascade|desc)"
    });
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsDesc|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].trav, EcsChildOf);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_3_args_this_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "ChildOf($grandchild, $child, $this)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 2);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsIsVariable, "grandchild");
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "child");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "child");
    test_second(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_3_args_2_terms_this_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "ChildOf($grandchild, $child, $this), Rel($this)"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 3);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsIsVariable, "grandchild");
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "child");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "child");
    test_second(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_3_args_2_terms_this_tgt_implicit_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "ChildOf($grandchild, $child, $this), Rel"
    });
    test_assert(q != NULL);

    test_int(term_count(q), 3);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsIsVariable, "grandchild");
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "child");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "child");
    test_second(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_3_or_args(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf($this, Foo || Bar)"
    });

    test_int(q->term_count, 2);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Foo, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Bar, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_3_or_args_implicit_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(ChildOf, Foo || Bar)"
    });

    test_int(q->term_count, 2);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Foo, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Bar, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_4_or_args(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf($this, Foo || Bar || Hello)"
    });

    test_int(q->term_count, 3);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Foo, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Bar, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[2], Hello, EcsSelf|EcsIsEntity);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_4_or_args_implicit_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(ChildOf, Foo || Bar || Hello)"
    });

    test_int(q->term_count, 3);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Foo, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[1], Bar, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[2], Hello, EcsSelf|EcsIsEntity);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_or_before_and_oper(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query(world, {
        .expr = "ChildOf($this, Foo || Bar, Hello)"
    }));

    ecs_fini(world);
}

void Parser_pair_and_before_or_oper(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_log_set_level(-4);
    test_assert(NULL == ecs_query(world, {
        .expr = "ChildOf($this, Foo, Bar || Hello)"
    }));

    ecs_fini(world);
}

void Parser_pair_3_args_w_vars(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf($a, $bb, $ccc)"
    });

    test_int(q->term_count, 2);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsIsVariable, "a");
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "bb");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "bb");
    test_second_var(terms[1], 0, EcsSelf|EcsIsVariable, "ccc");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_4_args_w_vars(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf($a, $bb, $ccc, $dddd)"
    });

    test_int(q->term_count, 3);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsIsVariable, "a");
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "bb");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "bb");
    test_second_var(terms[1], 0, EcsSelf|EcsIsVariable, "ccc");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[2], 0, EcsSelf|EcsIsVariable, "ccc");
    test_second_var(terms[2], 0, EcsSelf|EcsIsVariable, "dddd");
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_3_args_w_vars_w_term_after(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf($a, $bb, $ccc), Foo($ccc)"
    });

    test_int(q->term_count, 3);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsIsVariable, "a");
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "bb");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "bb");
    test_second_var(terms[1], 0, EcsSelf|EcsIsVariable, "ccc");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], Foo, EcsSelf|EcsIsEntity);
    test_src_var(terms[2], 0, EcsSelf|EcsIsVariable, "ccc");
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_4_args_w_vars_w_term_after(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf($a, $bb, $ccc, $dddd), Foo($dddd)"
    });

    test_int(q->term_count, 4);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsIsVariable, "a");
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "bb");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[1], 0, EcsSelf|EcsIsVariable, "bb");
    test_second_var(terms[1], 0, EcsSelf|EcsIsVariable, "ccc");
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src_var(terms[2], 0, EcsSelf|EcsIsVariable, "ccc");
    test_second_var(terms[2], 0, EcsSelf|EcsIsVariable, "dddd");
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    test_first(terms[3], Foo, EcsSelf|EcsIsEntity);
    test_src_var(terms[3], 0, EcsSelf|EcsIsVariable, "dddd");
    test_int(terms[3].oper, EcsAnd);
    test_int(terms[3].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_newline_after_inout(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in]\nTagA($this)"
    });

    test_assert(q != NULL);

    test_int(q->term_count, 1);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_newline_after_term_open(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] TagA(\n$this)"
    });

    test_assert(q != NULL);

    test_int(q->term_count, 1);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_newline_after_term_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] TagA($this\n)"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_newline_after_term_src_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] TagA($this\n,Tgt)"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Tgt, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_newline_after_term_pair_comma(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] TagA($this,\nTgt)"
    });
    
    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Tgt, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_newline_after_term_pair_second(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] TagA($this,Tgt\n)"
    });
    
    test_assert(q != NULL);
    
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], TagA, EcsSelf|EcsIsEntity);
    test_second(terms[0], Tgt, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_tag_w_space_implicit_this(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_set_name(world, 0, "Tag A");

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag\\ A"
    });
    
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], tag, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_tag_w_space(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_set_name(world, 0, "Tag A");

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag\\ A($this)"
    });
    
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], tag, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_first_w_space_implicit_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tgt);

    ecs_entity_t tag = ecs_set_name(world, 0, "Tag A");

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Tag\\ A, Tgt)"
    });
    
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], tag, EcsSelf|EcsIsEntity);
    test_second(terms[0], Tgt, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_first_w_space(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tgt);

    ecs_entity_t tag = ecs_set_name(world, 0, "Tag A");

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag\\ A($this, Tgt)"
    });
    
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], tag, EcsSelf|EcsIsEntity);
    test_second(terms[0], Tgt, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_second_w_space_implicit_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t tgt = ecs_set_name(world, 0, "Tgt A");

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, Tgt\\ A)"
    });
    
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_second(terms[0], tgt, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_second_w_space(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t tgt = ecs_set_name(world, 0, "Tgt A");

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel($this, Tgt\\ A)"
    });
    
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_second(terms[0], tgt, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_pair_src_w_space(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t src = ecs_set_name(world, 0, "Src A");

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel(Src\\ A, Tgt)"
    });
    
    test_assert(q != NULL);
    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_second(terms[0], Tgt, EcsSelf|EcsIsEntity);
    test_src(terms[0], src, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_empty_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1,, Pred_2"
    });

    test_assert(q == NULL);

    ecs_fini(world); 
}

void Parser_empty_term_w_space(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1, , Pred_2"
    });

    test_assert(q == NULL);

    ecs_fini(world); 
}

void Parser_empty_term_w_newline(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Pred_1,\n, Pred_2"
    });

    test_assert(q == NULL);

    ecs_fini(world); 
}

void Parser_mixed_1_desc_and_1_expr(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .terms = {{ Pred_1 }},
        .expr = "Pred_2"
    });

    test_assert(q != NULL);

    test_int(q->term_count, 2);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world); 
}

void Parser_mixed_2_desc_and_1_expr(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Pred_3);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .terms = {{ Pred_1 }, { Pred_2 }},
        .expr = "Pred_3"
    });

    test_assert(q != NULL);

    test_int(q->term_count, 3);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], Pred_3, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_mixed_1_desc_and_2_expr(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Pred_3);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .terms = {{ Pred_1 }},
        .expr = "Pred_2, Pred_3"
    });

    test_assert(q != NULL);

    test_int(q->term_count, 3);

    ecs_term_t *terms = q->terms;
    test_first(terms[0], Pred_1, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_first(terms[2], Pred_3, EcsSelf|EcsIsEntity);
    test_src(terms[2], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[2].oper, EcsAnd);
    test_int(terms[2].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_not_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!*"
    });

    test_int(q->term_count, 1);
    test_int(q->field_count, 1);
    test_uint(q->terms[0].id, EcsAny);
    test_int(q->terms[0].oper, EcsNot);
    test_int(q->terms[0].inout, EcsInOutNone);
    test_int(q->terms[0].field_index, 0);
    test_uint(q->terms[0].first.id, EcsAny|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].src.id, EcsThis|EcsSelf|EcsIsVariable);
    test_uint(q->terms[0].trav, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_not_first_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!(_, Position)"
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
    test_uint(q->terms[0].trav, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_not_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!(Position, _)"
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
    test_uint(q->terms[0].trav, 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_unterminated_paren(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "("
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_unterminated_after_first_paren(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "Position("
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_unterminated_after_src_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($"
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Parser_eq_name_existing_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == \"Foo\""
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredEq, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_assert((terms[0].second.id & ~EcsTermRefFlags) == 0);
    test_assert((terms[0].second.id & EcsTermRefFlags) == (EcsSelf|EcsIsName));
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_match_existing_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"Foo\""
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredMatch, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_assert((terms[0].second.id & ~EcsTermRefFlags) == 0);
    test_assert((terms[0].second.id & EcsTermRefFlags) == (EcsSelf|EcsIsName));
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_match_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"*\""
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredMatch, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_assert((terms[0].second.id & ~EcsTermRefFlags) == 0);
    test_assert((terms[0].second.id & EcsTermRefFlags) == (EcsSelf|EcsIsName));
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_match_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"_\""
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredMatch, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_assert((terms[0].second.id & ~EcsTermRefFlags) == 0);
    test_assert((terms[0].second.id & EcsTermRefFlags) == (EcsSelf|EcsIsName));
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_match_variable_oper(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"$\""
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsPredMatch, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_assert((terms[0].second.id & ~EcsTermRefFlags) == 0);
    test_assert((terms[0].second.id & EcsTermRefFlags) == (EcsSelf|EcsIsName));
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_escaped_identifier(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { 
        .name = "foo.bar",
        .sep = ""
    });

    ecs_query_t *q = ecs_query(world, {
        .expr = "foo\\.bar"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], e, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_escaped_identifier_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tgt);

    ecs_entity_t e = ecs_entity(world, { 
        .name = "foo.bar",
        .sep = ""
    });

    ecs_query_t *q = ecs_query(world, {
        .expr = "(foo\\.bar, Tgt)"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], e, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], Tgt, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_escaped_identifier_second(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_entity(world, { 
        .name = "foo.bar",
        .sep = ""
    });

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, foo\\.bar)"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], Rel, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], e, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Parser_n_tokens_test(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c1 = ecs_entity(world, { 
        .name = "e_foo.e_bar.e_foobar.e_hello" 
    });

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf(e_foo.e_bar.e_foobar.e_hello,$variable)"
    });

    test_assert(q != NULL);

    test_int(term_count(q), 1);

    ecs_term_t *terms = query_terms(q);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], c1, EcsSelf|EcsIsEntity);
    test_second_var(terms[0], 0, EcsSelf|EcsIsVariable, "variable");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_query_fini(q);

    ecs_fini(world);
}
