#include <api.h>

static
int sig_count(ecs_sig_t *sig) {
    test_assert(sig != NULL);
    return ecs_vector_count(sig->terms);
}

static
ecs_term_t* sig_columns(ecs_sig_t *sig) {
    test_assert(sig != NULL);
    ecs_term_t *result = ecs_vector_first(sig->terms, ecs_term_t);
    test_assert(result != NULL);
    return result;
}

#define test_pred(column, e, isa)\
    test_int(column.pred.entity, e);\
    test_int(column.pred.set, isa);

#define test_subj(column, e, isa)\
    test_int(column.args[0].entity, e);\
    test_int(column.args[0].set, isa);

#define test_obj(column, e, isa)\
    test_int(column.args[1].entity, e);\
    test_int(column.args[1].set, isa);

#define test_pred_var(column, e, isa, str)\
    test_pred(column, e, isa);\
    test_str(column.pred.name, str);

#define test_subj_var(column, e, isa, str)\
    test_subj(column, e, isa);\
    test_str(column.args[0].name, str);

#define test_obj_var(column, e, isa, str)\
    test_obj(column, e, isa);\
    test_str(column.args[1].name, str);

#define test_legacy(sig)\
{\
    int32_t i, count = sig_count(&sig);\
    ecs_term_t *columns = sig_columns(&sig);\
    for (i = 0; i < count; i ++) {\
        ecs_term_t *term = &columns[i];\
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
        if (term->from_kind != EcsFromEntity && term->from_kind != EcsFromEmpty) {\
            test_int(EcsThis, term->args[0].entity);\
            if (term->args[0].set == 0) {\
                test_int(term->from_kind, EcsFromOwned);\
            }\
        } else {\
            if (term->args[0].entity != 0) {\
                test_int(term->from_kind, EcsFromEntity);\
            } else {\
                test_int(term->from_kind, EcsFromEmpty);\
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

void Parser_resolve_singleton() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_lookup_fullpath(world, "$");
    test_assert(e != 0);
    test_assert(e == EcsSingleton);

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

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "0", &sig) == 0);
    test_int(sig_count(&sig), 0);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(Subj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Subj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(.)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this_by_name() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(This)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_explicit_subject_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(*)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsWildcard, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "(Pred, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_wildcard_pred() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "(*, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], EcsWildcard, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_wildcard_obj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "(Pred, *)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], EcsWildcard, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_this_pred() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "(., Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], EcsThis, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_this_obj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "(Pred, .)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(Subj, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Subj, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(., Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_this_by_name() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(This, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_pred() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "*(This, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], EcsWildcard, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(*, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsWildcard, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_wildcard_obj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(This, *)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], EcsWildcard, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_in_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[in] Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsIn);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_in_component_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[in] Pred(Subj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Subj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsIn);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_in_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[in] (Pred, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsIn);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_in_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[in] Pred(Subj, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Subj, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsIn);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_inout_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOut);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_inout_component_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] Pred(Subj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Subj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOut);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_inout_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] (Pred, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOut);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_inout_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] Pred(Subj, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Subj, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOut);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_out_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[out] Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsOut);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_out_component_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[out] Pred(Subj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Subj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsOut);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_out_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[out] (Pred, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsOut);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_out_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[out] Pred(Subj, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Subj, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsOut);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_singleton() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "$Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Pred, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_this_singleton() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "$.", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], EcsThis, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_implicit_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred()", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], 0, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_explicit_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(0)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], 0, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(0, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], 0, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_variable_single_char() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, X);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(X)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj_var(columns[0], 0, EcsDefaultSet, "X");
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_variable_multi_char() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XYZ);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(XYZ)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj_var(columns[0], 0, EcsDefaultSet, "XYZ");
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_variable_multi_char_w_underscore() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XY_Z);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(XY_Z)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj_var(columns[0], 0, EcsDefaultSet, "XY_Z");
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_variable_multi_char_w_number() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XY_1);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(XY_1)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj_var(columns[0], 0, EcsDefaultSet, "XY_1");
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_escaped_all_caps_single_char() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, X);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(\\X)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], X, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_escaped_all_caps_multi_char() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, XYZ);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(\\XYZ)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], XYZ, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "!Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsNot);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_implicit_subject_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "!(Pred, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsNot);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pair_explicit_subject_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Subj);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "!Pred(Subj, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], Subj, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsNot);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pred_implicit_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "DISABLED | Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].role, ECS_DISABLED);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);    
}

void Parser_pred_explicit_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "DISABLED | Pred(.)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].role, ECS_DISABLED);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);    
}

void Parser_pred_no_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "DISABLED | Pred()", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], 0, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].role, ECS_DISABLED);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);  
}

void Parser_pair_implicit_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "PAIR | (Pred, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].role, ECS_PAIR);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);   
}

void Parser_pair_explicit_subject_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "PAIR | Pred(., Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].role, ECS_PAIR);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_inout_role_pred_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] DISABLED | Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOut);
    test_int(columns[0].role, ECS_DISABLED);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);  
}

void Parser_inout_role_pred_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] DISABLED | Pred()", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], 0, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOut);
    test_int(columns[0].role, ECS_DISABLED);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);  
}

void Parser_inout_role_pred_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] DISABLED | Pred(This)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOut);
    test_int(columns[0].role, ECS_DISABLED);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);  
}

void Parser_inout_role_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] PAIR | (Pred, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOut);
    test_int(columns[0].role, ECS_PAIR);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);  
}

void Parser_inout_role_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] PAIR | Pred(This, Obj)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOut);
    test_int(columns[0].role, ECS_PAIR);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);  
}

void Parser_2_pred_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred_1, Pred_2", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_int(columns[1].oper, EcsAnd);
    test_int(columns[1].inout, EcsInOutDefault);    

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_pred_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred_1(), Pred_2()", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], 0, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], 0, EcsDefaultSet);
    test_int(columns[1].oper, EcsAnd);
    test_int(columns[1].inout, EcsInOutDefault);    

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_pred_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred_1(This), Pred_2(This)", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_int(columns[1].oper, EcsAnd);
    test_int(columns[1].inout, EcsInOutDefault);    

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_pair_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "(Pred_1, Obj_1), (Pred_2, Obj_2)", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj_1, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_obj(columns[1], Obj_2, EcsDefaultSet);
    test_int(columns[1].oper, EcsAnd);
    test_int(columns[1].inout, EcsInOutDefault);    

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_pair_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred_1(This, Obj_1), Pred_2(This, Obj_2)", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj_1, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_obj(columns[1], Obj_2, EcsDefaultSet);
    test_int(columns[1].oper, EcsAnd);
    test_int(columns[1].inout, EcsInOutDefault);    

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_pred_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "DISABLED | Pred_1, DISABLED | Pred_2", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].role, ECS_DISABLED);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_int(columns[1].oper, EcsAnd);
    test_int(columns[1].inout, EcsInOutDefault); 
    test_int(columns[1].role, ECS_DISABLED);   

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_pair_implicit_subj_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "PAIR | (Pred_1, Obj_1), PAIR | (Pred_2, Obj_2)", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj_1, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].role, ECS_PAIR);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_obj(columns[1], Obj_2, EcsDefaultSet);
    test_int(columns[1].oper, EcsAnd);
    test_int(columns[1].inout, EcsInOutDefault); 
    test_int(columns[1].role, ECS_PAIR);   

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_pair_explicit_subj_role() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "PAIR | Pred_1(This, Obj_1), PAIR | Pred_2(This, Obj_2)", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj_1, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].role, ECS_PAIR);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_obj(columns[1], Obj_2, EcsDefaultSet);
    test_int(columns[1].oper, EcsAnd);
    test_int(columns[1].inout, EcsInOutDefault); 
    test_int(columns[1].role, ECS_PAIR);   

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_or_pred_implicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred_1 || Pred_2", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsOr);
    test_int(columns[0].inout, EcsInOutDefault);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_int(columns[1].oper, EcsOr);
    test_int(columns[1].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_or_pred_explicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred_1(This) || Pred_2(This)", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsOr);
    test_int(columns[0].inout, EcsInOutDefault);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_int(columns[1].oper, EcsOr);
    test_int(columns[1].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_or_pair_implicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "(Pred_1, Obj_1) || (Pred_2, Obj_2)", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);    
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj_1, EcsDefaultSet);
    test_int(columns[0].oper, EcsOr);
    test_int(columns[0].inout, EcsInOutDefault);

    test_pred(columns[1], Pred_2, EcsDefaultSet);    
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_obj(columns[1], Obj_2, EcsDefaultSet);
    test_int(columns[1].oper, EcsOr);
    test_int(columns[1].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_or_pair_explicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred_1(This, Obj_1) || Pred_2(This, Obj_2)", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);    
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], Obj_1, EcsDefaultSet);
    test_int(columns[0].oper, EcsOr);
    test_int(columns[0].inout, EcsInOutDefault);

    test_pred(columns[1], Pred_2, EcsDefaultSet);    
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_obj(columns[1], Obj_2, EcsDefaultSet);
    test_int(columns[1].oper, EcsOr);
    test_int(columns[1].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_2_or_pred_inout() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] Pred_1 || Pred_2", &sig) == 0);
    test_int(sig_count(&sig), 2);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred_1, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsOr);
    test_int(columns[0].inout, EcsInOut);

    test_pred(columns[1], Pred_2, EcsDefaultSet);
    test_subj(columns[1], EcsThis, EcsDefaultSet);
    test_int(columns[1].oper, EcsOr);
    test_int(columns[1].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_1_digit_pred_implicit_subj() {
    ecs_world_t *world = ecs_init();

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "100", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], 100, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_1_digit_pred_no_subj() {
    ecs_world_t *world = ecs_init();

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "100()", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], 100, EcsDefaultSet);
    test_subj(columns[0], 0, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_1_digit_pred_explicit_subj() {
    ecs_world_t *world = ecs_init();

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "100(200)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], 100, EcsDefaultSet);
    test_subj(columns[0], 200, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_1_digit_pair_implicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "(100, 300)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], 100, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsDefaultSet);
    test_obj(columns[0], 300, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_1_digit_pair_explicit_subj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred_1);
    ECS_TAG(world, Pred_2);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "100(200, 300)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], 100, EcsDefaultSet);
    test_subj(columns[0], 200, EcsDefaultSet);
    test_obj(columns[0], 300, EcsDefaultSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_self() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(self)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSelf);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(superset)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSuperSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  
    test_int(columns[0].from_kind, EcsFromShared);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(subset)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSubSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(self|superset)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSelf | EcsSuperSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  
    test_int(columns[0].from_kind, EcsFromAny);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_inclusive() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(self|subset)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSelf | EcsSubSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_full() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(superset|all)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSuperSet | EcsAll);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_full() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(subset|all)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSubSet | EcsAll);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_inclusive_full() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(superset|all|self)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSelf | EcsSuperSet | EcsAll);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_inclusive_full() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(subset|all|self)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSelf | EcsSubSet | EcsAll);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_depth_1_digit() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(superset(2))", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSuperSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].args[0].depth, 2);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_depth_1_digit() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(subset(2))", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSubSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].args[0].depth, 2);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pred_implicit_subject_superset_depth_2_digits() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(superset(20))", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSuperSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].args[0].depth, 20);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_subset_depth_2_digits() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(subset(20))", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSubSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);
    test_int(columns[0].args[0].depth, 20);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_pred_implicit_subject_superset_childof() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(superset(ChildOf))", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSuperSet);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  
    test_int(columns[0].args[0].relation, EcsChildOf);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_full_superset_childof() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(all|superset(ChildOf))", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSuperSet | EcsAll);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  
    test_int(columns[0].args[0].relation, EcsChildOf);
    test_int(columns[0].from_kind, EcsCascade);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}

void Parser_pred_implicit_subject_superset_full_childof() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(superset|all(ChildOf))", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_term_t *columns = sig_columns(&sig);
    test_pred(columns[0], Pred, EcsDefaultSet);
    test_subj(columns[0], EcsThis, EcsSuperSet | EcsAll);
    test_int(columns[0].oper, EcsAnd);
    test_int(columns[0].inout, EcsInOutDefault);  
    test_int(columns[0].args[0].relation, EcsChildOf);
    test_int(columns[0].from_kind, EcsCascade);

    test_legacy(sig);

    ecs_sig_deinit(&sig);

    ecs_fini(world); 
}
