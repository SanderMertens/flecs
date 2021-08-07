#include <api.h>

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
    test_int(column.args[0].entity, e);\
    test_int(column.args[0].set.mask, isa);

#define test_obj(column, e, isa)\
    test_int(column.args[1].entity, e);\
    test_int(column.args[1].set.mask, isa);

#define test_pred_var(column, e, isa, str)\
    test_pred(column, e, isa);\
    test_str(column.pred.name, str);

#define test_subj_var(column, e, isa, str)\
    test_subj(column, e, isa);\
    test_str(column.args[0].name, str);

#define test_obj_var(column, e, isa, str)\
    test_obj(column, e, isa);\
    test_str(column.args[1].name, str);

#define test_legacy(f)\
{\
    int32_t i, count = filter_count(&f);\
    ecs_term_t *terms = filter_terms(&f);\
    for (i = 0; i < count; i ++) {\
        ecs_term_t *term = &terms[i];\
        if (term->oper != EcsOr) {\
            if (term->args[1].entity) {\
                if (term->role) {\
                    test_int(ECS_ROLE_MASK & term->id, term->role);\
                } else {\
                    test_assert(ECS_HAS_ROLE(term->id, PAIR));\
                }\
                test_int(ECS_PAIR_RELATION(term->id), ecs_entity_t_lo(term->pred.entity));\
                test_int(ECS_PAIR_OBJECT(term->id), ecs_entity_t_lo(term->args[1].entity));\
            } else {\
                test_int(ECS_COMPONENT_MASK & term->id, term->pred.entity);\
                test_int(ECS_ROLE_MASK & term->id, term->role);\
            }\
        }\
    }\
}\

void Parser_resolve_this() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_lookup_fullpath(world, ".");
    test_assert(e != 0);
    test_assert(e == EcsThis);

    ecs_fini(world);
}

void Parser_resolve_wildcard() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_lookup_fullpath(world, "*");
    test_assert(e != 0);
    test_assert(e == EcsWildcard);

    ecs_fini(world);
}

void Parser_resolve_is_a() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_lookup_fullpath(world, "IsA");
    test_assert(e != 0);
    test_assert(e == EcsIsA);

    ecs_fini(world);
}

void Parser_0() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "0"
    }));

    test_int(filter_count(&f), 0);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(Subj)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(.)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this_by_name() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(This)"
    }));

    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_subject_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(*)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsWildcard, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_wildcard_pred() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(*, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], EcsWildcard, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_wildcard_obj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred, *)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], EcsWildcard, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_this_pred() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(., Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], EcsThis, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_this_obj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred, .)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(., Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this_by_name() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_pred() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "*(This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], EcsWildcard, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(*, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsWildcard, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_obj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(This, *)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], EcsWildcard, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_in_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[in] Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_in_component_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[in] Pred(Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_in_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[in] (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_in_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[in] Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsIn);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_component_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] Pred(Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_inout_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_out_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[out] Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_out_component_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[out] Pred(Subj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_out_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[out] (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_out_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[out] Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsOut);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_singleton() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "$Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Pred, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_this_singleton() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "$."
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], EcsThis, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_implicit_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_explicit_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(0)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(0, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], 0, EcsNothing);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_variable_single_char() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, X);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(X)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj_var(terms[0], 0, EcsDefaultSet, "X");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_variable_multi_char() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XYZ);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(XYZ)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj_var(terms[0], 0, EcsDefaultSet, "XYZ");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_variable_multi_char_w_underscore() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XY_Z);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(XY_Z)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj_var(terms[0], 0, EcsDefaultSet, "XY_Z");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_variable_multi_char_w_number() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XY_1);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(XY_1)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj_var(terms[0], 0, EcsDefaultSet, "XY_1");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_escaped_all_caps_single_char() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, X);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(\\X)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], X, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_escaped_all_caps_multi_char() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XYZ);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(\\XYZ)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], XYZ, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "!Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "!(Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "!Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_component_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "!Pred_1, !Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_component_not_no_space() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "!Pred_1,!Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsNot);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsNot);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_component_optional() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "?Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_component_optional() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "?Pred_1, ?Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsOptional);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_2_component_optional_no_space() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "?Pred_1,?Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsOptional);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_from_and() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "AND | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAndFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_from_or() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "OR | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsOrFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_from_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "NOT | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsNotFrom);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_optional() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "?(Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_optional() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "?Pred(Subj, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], Subj, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_implicit_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "DISABLED | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);    
}

void Parser_pred_explicit_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "DISABLED | Pred(.)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);    
}

void Parser_pred_no_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "DISABLED | Pred()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_pair_implicit_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "PAIR | (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_PAIR);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);   
}

void Parser_pair_explicit_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "PAIR | Pred(., Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_PAIR);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_inout_role_pred_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] DISABLED | Pred"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pred_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] DISABLED | Pred()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pred_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] DISABLED | Pred(This)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_DISABLED);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] PAIR | (Pred, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_PAIR);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_inout_role_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] PAIR | Pred(This, Obj)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOut);
    test_int(terms[0].role, ECS_PAIR);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);  
}

void Parser_2_pred_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred_1, Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pred_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred_1(), Pred_2()"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], 0, EcsNothing);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pred_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred_1(This), Pred_2(This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred_1, Obj_1), (Pred_2, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj_1, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_obj(terms[1], Obj_2, EcsDefaultSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred_1(This, Obj_1), Pred_2(This, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj_1, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_obj(terms[1], Obj_2, EcsDefaultSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault);    

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pred_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "DISABLED | Pred_1, DISABLED | Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_DISABLED);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 
    test_int(terms[1].role, ECS_DISABLED);   

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pair_implicit_subj_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "PAIR | (Pred_1, Obj_1), PAIR | (Pred_2, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj_1, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_PAIR);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_obj(terms[1], Obj_2, EcsDefaultSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 
    test_int(terms[1].role, ECS_PAIR);   

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_pair_explicit_subj_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "PAIR | Pred_1(This, Obj_1), PAIR | Pred_2(This, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj_1, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].role, ECS_PAIR);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_obj(terms[1], Obj_2, EcsDefaultSet);
    test_int(terms[1].oper, EcsAnd);
    test_int(terms[1].inout, EcsInOutDefault); 
    test_int(terms[1].role, ECS_PAIR);   

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pred_implicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred_1 || Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pred_explicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred_1(This) || Pred_2(This)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pair_implicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred_1, Obj_1) || (Pred_2, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);    
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj_1, EcsDefaultSet);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);    
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_obj(terms[1], Obj_2, EcsDefaultSet);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pair_explicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred_1(This, Obj_1) || Pred_2(This, Obj_2)"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);    
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], Obj_1, EcsDefaultSet);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOutDefault);

    test_pred(terms[1], Pred_2, EcsDefaultSet);    
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_obj(terms[1], Obj_2, EcsDefaultSet);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_2_or_pred_inout() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "[inout] Pred_1 || Pred_2"
    }));
    test_int(filter_count(&f), 2);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred_1, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsOr);
    test_int(terms[0].inout, EcsInOut);

    test_pred(terms[1], Pred_2, EcsDefaultSet);
    test_subj(terms[1], EcsThis, EcsDefaultSet);
    test_int(terms[1].oper, EcsOr);
    test_int(terms[1].inout, EcsInOutDefault);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_1_digit_pred_implicit_subj() {
    ecs_world_t *world = ecs_init();

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "100"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], 100, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_1_digit_pred_no_subj() {
    ecs_world_t *world = ecs_init();

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "100()"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], 100, EcsDefaultSet);
    test_subj(terms[0], 0, EcsNothing);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_1_digit_pred_explicit_subj() {
    ecs_world_t *world = ecs_init();

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "100(200)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], 100, EcsDefaultSet);
    test_subj(terms[0], 200, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_1_digit_pair_implicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(100, 300)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], 100, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_obj(terms[0], 300, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_1_digit_pair_explicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "100(200, 300)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], 100, EcsDefaultSet);
    test_subj(terms[0], 200, EcsDefaultSet);
    test_obj(terms[0], 300, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_self() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(superset)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(subset)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(self|superset)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSelf | EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_inclusive() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(self|subset)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSelf | EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(superset|cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_cascade() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(subset|cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSubSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive_cascade() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(superset|cascade|self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSelf | EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_inclusive_cascade() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(subset|cascade|self)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSelf | EcsSubSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_cascade() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .expr = "Pred(cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_inclusive_cascade() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(self|cascade)"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsSelf | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].args[0].set.relation, EcsIsA);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_cascade_w_rel() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Rel);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .expr = "Pred(cascade(Rel))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].args[0].set.relation, Rel);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_implicit_superset_inclusive_cascade_w_rel() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Rel);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(self|cascade(Rel))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsSelf | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].args[0].set.relation, Rel);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}


void Parser_pred_implicit_subject_superset_depth_1_digit() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(superset(2))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.max_depth, 2);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_depth_1_digit() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(subset(2))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.max_depth, 2);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_implicit_subject_superset_depth_2_digits() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(superset(20))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.max_depth, 20);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_depth_2_digits() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(subset(20))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.max_depth, 20);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_pred_implicit_superset_min_max_depth() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(superset(2, 3))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.min_depth, 2);
    test_int(terms[0].args[0].set.max_depth, 3);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_superset_childof_min_max_depth() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(superset(ChildOf, 2, 3))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);
    test_int(terms[0].args[0].set.min_depth, 2);
    test_int(terms[0].args[0].set.max_depth, 3);
    test_int(terms[0].args[0].set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}


void Parser_pred_implicit_subject_superset_childof() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(superset(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].args[0].set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_cascade_superset_childof() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(cascade|superset(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].args[0].set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade_childof() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(superset|cascade(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].args[0].set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_cascade_childof_optional() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "?Pred(superset|cascade(ChildOf))"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], Pred, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsSuperSet | EcsCascade);
    test_int(terms[0].oper, EcsOptional);
    test_int(terms[0].inout, EcsInOutDefault);  
    test_int(terms[0].args[0].set.relation, EcsChildOf);

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Parser_expr_w_symbol() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_entity_t comp = ecs_component_init(world, &(ecs_component_desc_t) {
        .entity = {
            .name = "Foo",
            .symbol = "FooBar"
        }
    });

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "FooBar"
    }));
    test_int(filter_count(&f), 1);

    ecs_term_t *terms = filter_terms(&f);
    test_pred(terms[0], comp, EcsDefaultSet);
    test_subj(terms[0], EcsThis, EcsDefaultSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);  

    test_legacy(f);

    ecs_filter_fini(&f);

    ecs_fini(world);
}
