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

#define test_pred(column, e, isa)\
    test_int(column.pred.entity, e);\
    test_int(column.pred.set.mask, isa);

#define test_subj(column, e, isa)\
    test_int(column.subj.entity, e);\
    test_int(column.subj.set.mask, isa);

#define test_obj(column, e, isa)\
    test_int(column.obj.entity, e);\
    test_int(column.obj.set.mask, isa);

#define test_pred_var(column, e, isa, str)\
    test_pred(column, e, isa);\
    test_str(column.pred.name, str);

#define test_subj_var(column, e, isa, str)\
    test_subj(column, e, isa);\
    test_str(column.subj.name, str);

#define test_obj_var(column, e, isa, str)\
    test_obj(column, e, isa);\
    test_str(column.obj.name, str);

#define test_legacy(f)\
{\
    int32_t i, count = filter_count(&f);\
    ecs_term_t *terms = filter_terms(&f);\
    for (i = 0; i < count; i ++) {\
        ecs_term_t *term = &terms[i];\
        if (term->oper != EcsOr) {\
            if ((term->role == ECS_PAIR) && term->obj.entity != EcsThis) {\
                if (term->role) {\
                    test_int(ECS_ROLE_MASK & term->id, term->role);\
                } else {\
                    test_assert(ECS_HAS_ROLE(term->id, PAIR));\
                }\
                if (term->pred.entity != EcsThis) {\
                    if (ecs_entity_t_lo(term->pred.entity) && term->pred.var != EcsVarIsVariable) {\
                        test_int(ECS_PAIR_FIRST(term->id), ecs_entity_t_lo(term->pred.entity));\
                    }\
                }\
                if (ecs_entity_t_lo(term->obj.entity) && term->obj.var != EcsVarIsVariable) {\
                    test_int(ECS_PAIR_SECOND(term->id), ecs_entity_t_lo(term->obj.entity));\
                }\
            } else {\
                if (term->pred.entity != EcsThis && term->obj.entity != EcsThis) {\
                    test_int(ECS_COMPONENT_MASK & term->id, term->pred.entity);\
                }\
                test_int(ECS_ROLE_MASK & term->id, term->role);\
            }\
        }\
    }\
}\

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(.)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsWildcard, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsAny, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], EcsThis, EcsSelf);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], EcsThis, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], EcsWildcard, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], EcsWildcard, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], EcsAny, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], EcsAny, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "(., Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], EcsThis, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "(Pred, .)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], EcsThis, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred(., Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "*(This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], EcsWildcard, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsWildcard, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred(This, *)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], EcsWildcard, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_0_object() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "(Pred, 0)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], 0, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id, ecs_pair(Pred, 0));

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_0_object() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(This, 0)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], 0, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].id, ecs_pair(Pred, 0));

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsNothing);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_filter_component() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[filter] Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutFilter);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = ".($)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], EcsThis, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsNothing);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj_var(terms[0], 0, EcsSelf|EcsSuperSet, "X");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj_var(terms[0], 0, EcsSelf|EcsSuperSet, "XYZ");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj_var(terms[0], 0, EcsSelf|EcsSuperSet, "XY_Z");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj_var(terms[0], 0, EcsSelf|EcsSuperSet, "XY_1");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj_var(terms[0], 0, EcsSelf|EcsSuperSet, "xyZ");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred_var(terms[0], 0, EcsSelf, "Pred");
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj_var(terms[0], 0, EcsSelf, "Obj");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsOptional);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsOptional);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAndFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsOrFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsNotFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_implicit_subject_w_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "DISABLED | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);    
}

void Parser_pred_explicit_subject_w_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "DISABLED | Pred(.)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);    
}

void Parser_pred_no_subject_w_role() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "DISABLED | Pred()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

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
        .expr = "PAIR | (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_PAIR);

    test_legacy(f);

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
        .expr = "PAIR | Pred(., Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_PAIR);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_inout_role_pred_implicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] DISABLED | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pred_no_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] DISABLED | Pred()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pred_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "[inout] DISABLED | Pred(This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

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
        .expr = "[inout] PAIR | (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_PAIR);

    test_legacy(f);

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
        .expr = "[inout] PAIR | Pred(This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_PAIR);

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], 0, EcsNothing);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

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
        .expr = "Pred_1(This), Pred_2(This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj_1, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[1], Obj_2, EcsSelf);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

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
        .expr = "Pred_1(This, Obj_1), Pred_2(This, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj_1, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[1], Obj_2, EcsSelf);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

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
        .expr = "DISABLED | Pred_1, DISABLED | Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_DISABLED);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 
    test_int(terms[1].role, ECS_DISABLED);   

    test_legacy(f);

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
        .expr = "PAIR | (Pred_1, Obj_1), PAIR | (Pred_2, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj_1, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_PAIR);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[1], Obj_2, EcsSelf);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 
    test_int(terms[1].role, ECS_PAIR);   

    test_legacy(f);

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
        .expr = "PAIR | Pred_1(This, Obj_1), PAIR | Pred_2(This, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj_1, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_PAIR);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[1], Obj_2, EcsSelf);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 
    test_int(terms[1].role, ECS_PAIR);   

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred_1(This) || Pred_2(This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);    
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj_1, EcsSelf);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);    
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[1], Obj_2, EcsSelf);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred_1(This, Obj_1) || Pred_2(This, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);    
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj_1, EcsSelf);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);    
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[1], Obj_2, EcsSelf);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred_1, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOut);

    test_pred(terms[1], Pred_2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], 100, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

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
    test_pred(terms[0], 100, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

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
    test_pred(terms[0], 100, EcsSelf|EcsSubSet);
    test_subj(terms[0], 200, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

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
    test_pred(terms[0], 100, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], 300, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

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
    test_pred(terms[0], 100, EcsSelf|EcsSubSet);
    test_subj(terms[0], 200, EcsSelf|EcsSuperSet);
    test_obj(terms[0], 300, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(super)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(sub)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(self|super)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf | EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_inclusive() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(self|sub)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf | EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(super|cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(sub|cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSubSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(super|cascade|self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf | EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_inclusive_cascade() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(sub|cascade|self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf | EcsSubSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsSelf | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].subj.set.relation, Rel);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsSelf | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].subj.set.relation, Rel);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}


void Parser_pred_implicit_subject_superset_depth_1_digit() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(super(2))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.max_depth, 2);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_depth_1_digit() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(sub(2))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.max_depth, 2);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_implicit_subject_superset_depth_2_digits() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(super(20))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.max_depth, 20);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_depth_2_digits() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(sub(20))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.max_depth, 20);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_implicit_superset_min_max_depth() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(super(2, 3))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.min_depth, 2);
    test_int(terms[0].subj.set.max_depth, 3);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_superset_childof_min_max_depth() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(super(ChildOf, 2, 3))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].subj.set.min_depth, 2);
    test_int(terms[0].subj.set.max_depth, 3);
    test_int(terms[0].subj.set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}


void Parser_pred_implicit_subject_superset_childof() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(super(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].subj.set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_cascade_superset_childof() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(cascade|super(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].subj.set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade_childof() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred(super|cascade(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].subj.set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade_childof_optional() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "?Pred(super|cascade(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].subj.set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_expr_w_symbol() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_entity_t comp = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = {
            .name = "Foo",
            .symbol = "FooBar"
        }
    });

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "FooBar"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], comp, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred(Subj:self|super)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred(Subj:super(ChildOf))"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsChildOf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred(Subj:self|super(ChildOf))"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsChildOf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred(Subj, Obj:self|super)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred(Subj, Obj:super(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred(Subj, Obj:self|super(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred:self|super:(Subj)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred:super(ChildOf):(Subj)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsChildOf);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred:self|super(ChildOf):(Subj)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsChildOf);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_w_explicit_self_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self|super"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_w_explicit_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:super(ChildOf)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsChildOf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_entity_no_args_w_explicit_self_superset_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "Pred:self|super(ChildOf)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsChildOf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred:self|super, Pred2"
    }));

    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred:super(ChildOf), Pred2"
    }));

    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsChildOf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "Pred:self|super(ChildOf), Pred2"
    }));

    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsChildOf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred2, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
        .expr = "TagA, TagB(This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], TagA, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].pred.var, EcsVarIsEntity);
    test_int(terms[0].subj.var, EcsVarIsVariable);

    test_pred(terms[1], TagB, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);
    test_int(terms[1].pred.var, EcsVarIsEntity);
    test_int(terms[1].subj.var, EcsVarIsVariable);

    test_legacy(f);

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
        .expr = "TagA, TagB(Subj, This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], TagA, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].pred.var, EcsVarIsEntity);
    test_int(terms[0].subj.var, EcsVarIsVariable);

    test_pred(terms[1], TagB, EcsSelf|EcsSubSet);
    test_subj(terms[1], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[1], EcsThis, EcsSelf);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);
    test_int(terms[1].pred.var, EcsVarIsEntity);
    test_int(terms[1].subj.var, EcsVarIsEntity);
    test_int(terms[1].obj.var, EcsVarIsVariable);

    test_legacy(f);

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
        .expr = "TagA, TagB(This, This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], TagA, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].pred.var, EcsVarIsEntity);
    test_int(terms[0].subj.var, EcsVarIsVariable);

    test_pred(terms[1], TagB, EcsSelf|EcsSubSet);
    test_subj(terms[1], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[1], EcsThis, EcsSelf);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);
    test_int(terms[1].pred.var, EcsVarIsEntity);
    test_int(terms[1].subj.var, EcsVarIsVariable);
    test_int(terms[1].obj.var, EcsVarIsVariable);

    test_legacy(f);

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
    test_pred(terms[0], TagA, EcsSelf|EcsSubSet);
    test_subj(terms[0], 0, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    
    test_str(terms[0].subj.name, "Var");

    test_legacy(f);

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
    test_pred(terms[0], TagA, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].pred.name, NULL);

    test_legacy(f);

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
    test_pred(terms[0], TagA, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].subj.name, NULL);

    test_legacy(f);

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
    test_pred(terms[0], TagA, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].obj.name, NULL);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_pred_no_name() {
    ecs_world_t *world = ecs_mini();

    ecs_filter_t f = ECS_FILTER_INIT;
    test_assert(NULL != ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .expr = "This"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], EcsThis, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].pred.name, NULL);

    test_legacy(f);

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
        .expr = "TagA(This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], TagA, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].subj.name, NULL);

    test_legacy(f);

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
        .expr = "TagA(Subj, This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], TagA, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj(terms[0], EcsThis, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_str(terms[0].obj.name, NULL);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], 0, EcsSelf);
    test_obj_var(terms[0], 0, EcsSelf, "Pred");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Pred, EcsSelf|EcsSubSet);
    test_subj(terms[0], Subj, EcsSelf|EcsSuperSet);
    test_obj_var(terms[0], 0, EcsSelf, "Pred");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

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
    test_pred(terms[0], Rel, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
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
    test_pred(terms[0], Rel, EcsSelf|EcsSubSet);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_obj(terms[0], Obj, EcsSelf);
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
