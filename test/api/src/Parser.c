#include <api.h>

static
int sig_count(ecs_sig_t *sig) {
    test_assert(sig != NULL);
    return ecs_vector_count(sig->columns);
}

static
ecs_sig_column_t* sig_columns(ecs_sig_t *sig) {
    test_assert(sig != NULL);
    ecs_sig_column_t *result = ecs_vector_first(sig->columns, ecs_sig_column_t);
    test_assert(result != NULL);
    return result;
}

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

void Parser_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Subj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(.)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_explicit_subject_this_by_name() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(This)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_explicit_subject_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(*)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsWildcard);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, EcsWildcard);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, EcsWildcard);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, EcsThis);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, EcsThis);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Subj);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, EcsWildcard);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsWildcard);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, EcsWildcard);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_in_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[in] Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsIn);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Subj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsIn);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsIn);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Subj);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsIn);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_inout_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[inout] Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOut);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Subj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOut);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOut);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Subj);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOut);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_out_component_implicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "[out] Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsOut);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Subj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsOut);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsOut);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Subj);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsOut);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_singleton() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "$Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Pred);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, EcsThis);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_implicit_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred()", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, 0);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_explicit_no_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "Pred(0)", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, 0);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, 0);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperAnd);
    test_int(columns[0].inout, EcsInOutDefault);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}

void Parser_component_not() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);

    ecs_sig_t sig;
    test_assert(ecs_sig_init(world, NULL, "!Pred", &sig) == 0);
    test_int(sig_count(&sig), 1);

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].oper, EcsOperNot);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, EcsThis);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperNot);
    test_int(columns[0].inout, EcsInOutDefault);

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

    ecs_sig_column_t *columns = sig_columns(&sig);
    test_int(columns[0].pred.entity, Pred);
    test_int(columns[0].args[0].entity, Subj);
    test_int(columns[0].args[1].entity, Obj);
    test_int(columns[0].oper, EcsOperNot);
    test_int(columns[0].inout, EcsInOutDefault);

    ecs_sig_deinit(&sig);

    ecs_fini(world);
}
