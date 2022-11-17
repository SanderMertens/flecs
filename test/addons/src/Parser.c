#include <addons.h>

static
int filter_count(ecs_filter_t *f) {
    test_assert(f != NULL);
    return f->term_count;
}

static
ecs_term_t* filter_terms(ecs_filter_t *f) {
    test_assert(f != NULL);
    return f->terms;
}

#define test_first(column, e, flgs)\
    test_int(column.first.id, e);\
    test_int(column.first.flags, flgs);

#define test_src(column, e, flgs)\
    test_int(column.src.id, e);\
    test_int(column.src.flags, flgs);

#define test_second(column, e, flgs)\
    test_int(column.second.id, e);\
    test_int(column.second.flags, flgs);

#define test_first_var(column, e, flgs, str)\
    test_first(column, e, flgs|EcsIsVariable);\
    test_str(column.first.name, str);

#define test_src_var(column, e, flgs, str)\
    test_src(column, e, flgs|EcsIsVariable);\
    test_str(column.src.name, str);

#define test_second_var(column, e, flgs, str)\
    test_second(column, e, flgs|EcsIsVariable);\
    test_str(column.second.name, str);

void Parser_resolve_this() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup_fullpath(world, ".");
    test_assert(e != 0);
    test_assert(e == EcsThis);

    ecs_fini(world);
}

void Parser_resolve_wildcard() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup_fullpath(world, "*");
    test_assert(e != 0);
    test_assert(e == EcsWildcard);

    ecs_fini(world);
}

void Parser_resolve_any() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup_fullpath(world, "_");
    test_assert(e != 0);
    test_assert(e == EcsAny);

    ecs_fini(world);
}

void Parser_resolve_is_a() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup_fullpath(world, "IsA");
    test_assert(e != 0);
    test_assert(e == EcsIsA);

    ecs_fini(world);
}

void Parser_0() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "0"
    }));

    test_int(filter_count(&f), 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this_by_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(This)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this_by_var_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($This)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(*)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsWildcard, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_any() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(_)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsAny, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_0() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(0)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_as_predicate() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "This(Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsThis, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_var_as_predicate() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "$This(Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsThis, EcsSelf|EcsDown|EcsIsVariable);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_as_object() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj, This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], EcsThis, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_var_as_object() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj, $This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_wildcard_pred() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(*, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsWildcard, EcsSelf|EcsDown|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_wildcard_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, *)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], EcsWildcard, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_any_pred() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(_, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsAny, EcsSelf|EcsDown|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_any_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, _)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], EcsAny, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_this_pred() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "($This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsThis, EcsSelf|EcsDown|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_this_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, $This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_pred_w_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred:self, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_obj_w_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, Obj:self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_pred_w_up() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred:up, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsUp|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_obj_w_up() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, Obj:up)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_pred_w_self_up() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred:self|up, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_obj_w_self_up() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, Obj:self|up)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_pred_w_up_trav() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred:up(ChildOf), Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsUp|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    
    test_assert(terms[0].first.trav == EcsChildOf);
    test_assert(terms[0].second.trav == 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_obj_w_up_trav() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, Obj:up(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_assert(terms[0].first.trav == EcsIsA);
    test_assert(terms[0].second.trav == EcsChildOf);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_pred_w_invalid_flags() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_log_set_level(-4);
    test_assert(NULL == ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred:, Obj)"
    }));

    ecs_fini(world);
}

void Parser_pair_implicit_subject_obj_w_invalid_flags() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_log_set_level(-4);
    test_assert(NULL == ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, Obj:)"
    }));

    ecs_fini(world);
}

void Parser_pair_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this_by_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this_by_var_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_pred() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "*($This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsWildcard, EcsSelf|EcsDown|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(*, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsWildcard, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($This, *)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], EcsWildcard, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_0_object() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(ChildOf, 0)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id, ecs_pair(EcsChildOf, 0));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_0_object() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "ChildOf($This, 0)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsChildOf, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_second(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id, ecs_pair(EcsChildOf, 0));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_0() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(0, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_in_component_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[in] Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_in_component_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[in] Pred(Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_in_pair_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[in] (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_in_pair_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[in] Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_component_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_component_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] Pred(Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_pair_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_pair_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_out_component_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[out] Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_out_component_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[out] Pred(Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_out_pair_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[out] (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_out_pair_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[out] Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_filter_component() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[none] Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutNone);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_singleton() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_singleton() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "$This($)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsThis, EcsSelf|EcsDown|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_implicit_no_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_no_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(0)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_no_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(0, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_variable_single_char() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, X);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($X)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsUp, "X");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_variable_multi_char() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XYZ);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($XYZ)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsUp, "XYZ");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_variable_multi_char_w_underscore() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XY_Z);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($XY_Z)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsUp, "XY_Z");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_variable_multi_char_w_number() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XY_1);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($XY_1)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsUp, "XY_1");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_variable_multi_char_not_allcaps() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XYZ);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($xyZ)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src_var(terms[0], 0, EcsSelf|EcsUp, "xyZ");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "($Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first_var(terms[0], 0, EcsSelf|EcsDown, "Pred");
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_obj_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, $Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second_var(terms[0], 0, EcsSelf, "Obj");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_not() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "!Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_not() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "!(Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_not() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "!Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_component_not() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "!Pred_1, !Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_component_not_no_space() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "!Pred_1,!Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_optional() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "?Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_component_optional() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "?Pred_1, ?Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsOptional);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_component_optional_no_space() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "?Pred_1,?Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsOptional);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_from_and() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "AND | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAndFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_from_or() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "OR | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsOrFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_from_not() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "NOT | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsNotFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_optional() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "?(Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_optional() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "?Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_implicit_subject_w_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TOGGLE | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id_flags, ECS_TOGGLE);

    ecs_filter_fini(&f);

    ecs_fini(world);    
}

void Parser_pred_explicit_subject_w_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TOGGLE | Pred($This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id_flags, ECS_TOGGLE);

    ecs_filter_fini(&f);

    ecs_fini(world);    
}

void Parser_pred_no_subject_w_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TOGGLE | Pred()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id_flags, ECS_TOGGLE);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_pair_implicit_subject_w_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id_flags, ECS_PAIR);

    ecs_filter_fini(&f);

    ecs_fini(world);   
}

void Parser_pair_explicit_subject_w_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred($This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id_flags, ECS_PAIR);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_inout_role_pred_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] TOGGLE | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].id_flags, ECS_TOGGLE);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pred_no_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] TOGGLE | Pred()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].id_flags, ECS_TOGGLE);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pred_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] TOGGLE | Pred($This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].id_flags, ECS_TOGGLE);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pair_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].id_flags, ECS_PAIR);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pair_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] Pred($This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].id_flags, ECS_PAIR);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_2_pred_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred_1, Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pred_no_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred_1(), Pred_2()"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], 0, EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pred_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred_1($This), Pred_2($This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pair_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred_1, Obj_1), (Pred_2, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pair_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred_1($This, Obj_1), Pred_2($This, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pred_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TOGGLE | Pred_1, TOGGLE | Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id_flags, ECS_TOGGLE);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 
    test_int(terms[1].id_flags, ECS_TOGGLE);   


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pair_implicit_subj_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred_1, Obj_1), (Pred_2, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id_flags, ECS_PAIR);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 
    test_int(terms[1].id_flags, ECS_PAIR);   


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pair_explicit_subj_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred_1($This, Obj_1), Pred_2($This, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id_flags, ECS_PAIR);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 
    test_int(terms[1].id_flags, ECS_PAIR);   


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pred_implicit_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred_1 || Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pred_explicit_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred_1($This) || Pred_2($This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pair_implicit_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred_1, Obj_1) || (Pred_2, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);    
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);    
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pair_explicit_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred_1($This, Obj_1) || Pred_2($This, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);    
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj_1, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);    
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[1], Obj_2, EcsSelf|EcsIsEntity);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pred_inout() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] Pred_1 || Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred_1, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOut);

    test_first(terms[1], Pred_2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_1_digit_pred_implicit_subj() {
    ecs_world_t *world = ecs_mini();

    ecs_ensure(world, 100);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "100"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], 100, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_1_digit_pred_no_subj() {
    ecs_world_t *world = ecs_mini();

    ecs_ensure(world, 100);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "100()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], 100, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_1_digit_pred_explicit_subj() {
    ecs_world_t *world = ecs_mini();

    ecs_ensure(world, 100);
    ecs_ensure(world, 200);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "100(200)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], 100, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 200, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_1_digit_pair_implicit_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_ensure(world, 100);
    ecs_ensure(world, 300);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(100, 300)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], 100, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], 300, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_1_digit_pair_explicit_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_ensure(world, 100);
    ecs_ensure(world, 200);
    ecs_ensure(world, 300);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "100(200, 300)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], 100, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 200, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], 300, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(up)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(down)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsDown|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(self|up)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_inclusive() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(self|down)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsDown|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(up|cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(down|cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsDown|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(up|cascade|self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_inclusive_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(down|cascade|self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsDown|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_inclusive_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(self|cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsSelf|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].src.trav, EcsIsA);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_cascade_w_rel() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_ENTITY(world, Rel, Acyclic);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(cascade(Rel))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].src.trav, Rel);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_inclusive_cascade_w_rel() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_ENTITY(world, Rel, Acyclic);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(self|cascade(Rel))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsSelf|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].src.trav, Rel);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_childof() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(up(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].src.trav, EcsChildOf);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_cascade_superset_childof() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(cascade|up(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].src.trav, EcsChildOf);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade_childof() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(up|cascade(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].src.trav, EcsChildOf);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade_childof_optional() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "?Pred(up|cascade(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsUp|EcsCascade|EcsIsVariable);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].src.trav, EcsChildOf);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_expr_w_symbol() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_entity_t comp = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {
            .name = "Foo",
            .symbol = "FooBar"
        })
    });

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "FooBar"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], comp, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  


    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_expr_w_newline() {
    ecs_world_t *world = ecs_mini();

    ecs_new_entity(world, "Foo");

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Foo\n"
    }));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_subj_entity_w_explicit_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj:self)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_subj_entity_w_explicit_self_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj:self|up)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_subj_entity_w_explicit_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj:up(ChildOf))"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsUp|EcsIsEntity);
    test_int(terms[0].src.trav, EcsChildOf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_subj_entity_w_explicit_self_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj:self|up(ChildOf))"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].src.trav, EcsChildOf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_obj_entity_w_explicit_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj, Obj:self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_obj_entity_w_explicit_self_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj, Obj:self|up)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_obj_entity_w_explicit_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj, Obj:up(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_obj_entity_w_explicit_self_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj, Obj:self|up(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_w_explicit_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self:(Subj)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_w_explicit_self_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self|up:(Subj)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_w_explicit_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:up(ChildOf):(Subj)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsUp|EcsIsEntity);
    test_int(terms[0].first.trav, EcsChildOf);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_w_explicit_self_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self|up(ChildOf):(Subj)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].first.trav, EcsChildOf);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_w_explicit_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_w_explicit_self_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self|up"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_w_explicit_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:up(ChildOf)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsUp|EcsIsEntity);
    test_int(terms[0].first.trav, EcsChildOf);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_w_explicit_self_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self|up(ChildOf)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].first.trav, EcsChildOf);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_2_terms_w_explicit_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Pred2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self, Pred2"
    }));

    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_2_terms_w_explicit_self_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Pred2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self|up, Pred2"
    }));

    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_2_terms_w_explicit_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Pred2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:up(ChildOf), Pred2"
    }));

    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsUp|EcsIsEntity);
    test_int(terms[0].first.trav, EcsChildOf);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_2_terms_w_explicit_self_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Pred2);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self|up(ChildOf), Pred2"
    }));

    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].first.trav, EcsChildOf);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], Pred2, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_newline() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "\n"
    }));

    test_int(filter_count(&f), 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_newlines() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "\n\n"
    }));

    test_int(filter_count(&f), 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_3_newlines() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "\n\n"
    }));

    test_int(filter_count(&f), 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_space() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = " "
    }));

    test_int(filter_count(&f), 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_spaces() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "  "
    }));

    test_int(filter_count(&f), 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_trailing_newline() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred\n"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_trailing_newlines() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred\n\n"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_trailing_space() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred "
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_trailing_spaces() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred  "
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_template_type() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Pred = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Template<int>"
    });
    test_assert(Pred != 0);
    test_str(ecs_get_name(world, Pred), "Template<int>");

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Template<int>"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_predicate_w_parens() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_not_alive_pred() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    test_assert(!ecs_is_alive(world, 5000));

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL == ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "5000"
    }));

    ecs_fini(world);
}

void Parser_not_alive_subj() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    test_assert(!ecs_is_alive(world, 5000));

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL == ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(5000)"
    }));

    ecs_fini(world);
}

void Parser_not_alive_obj() {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    test_assert(!ecs_is_alive(world, 5000));

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL == ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(Subj, 5000)"
    }));

    ecs_fini(world);
}

void Parser_this_subj_var_kind() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TagA, TagB($This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], TagA, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], TagB, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_obj_var_kind() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TagA, TagB(Subj, $This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], TagA, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], TagB, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_subj_obj_var_kind() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TagA, TagB($This, $This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], TagA, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_first(terms[1], TagB, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[1], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[1], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_var_w_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TagA($Var)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], TagA, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], 0, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);    
    test_str(terms[0].src.name, "Var");

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_entity_pred_no_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TagA"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], TagA, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].first.name, NULL);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_entity_subj_no_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TagA(Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], TagA, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].src.name, NULL);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_entity_obj_no_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TagA(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], TagA, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].second.name, NULL);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_pred_no_name() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "$This"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], EcsThis, EcsSelf|EcsDown|EcsIsVariable);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].first.name, NULL);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_subj_no_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TagA($This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], TagA, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].src.name, NULL);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_obj_no_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "TagA(Subj, $This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], TagA, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second(terms[0], EcsThis, EcsSelf|EcsIsVariable);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].second.name, NULL);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_auto_object_variable() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "$(Pred)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second_var(terms[0], 0, EcsSelf, "Pred");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_auto_object_variable_w_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "$(Pred, Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Pred, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], Subj, EcsSelf|EcsUp|EcsIsEntity);
    test_second_var(terms[0], 0, EcsSelf, "Pred");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_invalid_variable_only() {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL == ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "$"
    }));

    ecs_fini(world);
}

void Parser_oneof_self_pred_w_relative_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);
    ECS_ENTITY(world, Obj, (ChildOf, Rel));

    test_assert( ecs_lookup_child(world, 0, "Obj") == 0 ); // sanity check

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Rel, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Rel, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);
 
    ecs_fini(world);
}

void Parser_oneof_other_pred_w_relative_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Parent);
    ECS_ENTITY(world, Rel, (OneOf, Parent));
    ECS_ENTITY(world, Obj, (ChildOf, Parent));

    test_assert( ecs_lookup_child(world, 0, "Obj") == 0 ); // sanity check

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Rel, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_first(terms[0], Rel, EcsSelf|EcsDown|EcsIsEntity);
    test_src(terms[0], EcsThis, EcsSelf|EcsUp|EcsIsVariable);
    test_second(terms[0], Obj, EcsSelf|EcsIsEntity);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);
 
    ecs_fini(world);
}

void Parser_oneof_self_pred_w_invalid_obj() {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);
    ECS_TAG(world, Obj);

    test_assert( ecs_lookup_child(world, 0, "Obj") != 0 ); // sanity check

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL == ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Rel, Obj)"
    }));
 
    ecs_fini(world);
}

void Parser_oneof_other_pred_w_invalid_obj() {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Parent);
    ECS_ENTITY(world, Rel, (OneOf, Parent));
    ECS_TAG(world, Obj);

    test_assert( ecs_lookup_child(world, 0, "Obj") != 0 ); // sanity check

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL == ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Rel, Obj)"
    }));
 
    ecs_fini(world);
}
