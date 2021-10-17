#include <api.h>

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
    test_assert(column.pred.var == EcsVarIsEntity);\
    test_str(column.pred.name, str);

#define test_subj_var(column, e, isa, str)\
    test_subj(column, e, isa);\
    test_assert(column.obj.var == EcsVarIsVariable);\
    test_str(column.subj.name, str);

#define test_obj_var(column, e, isa, str)\
    test_obj(column, e, isa);\
    test_assert(column.obj.var == EcsVarIsVariable);\
    test_str(column.obj.name, str);

void FilterDefaultSubstitution_final_pred_no_args() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, Final);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsIsA);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_nonfinal_pred_no_args() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, 0);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsIsA);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsIsA);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_final_transitive_pred() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, Final, Transitive);
    ECS_ENTITY(world, Obj, 0);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred, Obj)",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf);
    
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_obj(terms[0], Obj, EcsSelf|EcsSubSet);
    test_int(terms[0].obj.set.relation, EcsIsA);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_nonfinal_transitive_pred() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, Transitive);
    ECS_ENTITY(world, Obj, 0);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred, Obj)",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsIsA);
    
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsIsA);
    
    test_obj(terms[0], Obj, EcsSelf|EcsSubSet);
    test_int(terms[0].obj.set.relation, EcsIsA);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_final_transitive_pred_final_obj() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, Final, Transitive);
    ECS_ENTITY(world, Obj, Final);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred, Obj)",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf);
    
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_obj(terms[0], Obj, EcsSelf|EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_nonfinal_transitive_pred_final_obj() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, Transitive);
    ECS_ENTITY(world, Obj, Final);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred, Obj)",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsIsA);
    
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_obj(terms[0], Obj, EcsSelf|EcsSubSet);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_nonfinal_transitive_pred_var_obj() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, Transitive);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "(Pred, X)",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf|EcsSuperSet);
    test_int(terms[0].pred.set.relation, EcsIsA);
    
    test_subj_var(terms[0], EcsThis, EcsSelf|EcsSuperSet, NULL);
    test_int(terms[0].subj.set.relation, EcsIsA);

    test_obj_var(terms[0], 0, EcsSelf|EcsSuperSet, "X");
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_nonfinal_pred_no_args_explicit_self_pred() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, 0);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred:self",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsIsA);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_implicit_subj_w_self() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, Final);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(self)",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_explicit_subj_w_self() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, Final);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(This:self)",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void FilterDefaultSubstitution_transitive_pred_w_self_obj() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, Final, Transitive);
    ECS_ENTITY(world, Obj, 0);

    ecs_filter_t f;
    test_int(0, ecs_filter_init(world, &f, &(ecs_filter_desc_t){
        .expr = "Pred(This, Obj:self)",
        .substitute_default = true
    }));

    test_int(f.term_count, 1);

    ecs_term_t *terms = f.terms;
    test_pred(terms[0], Pred, EcsSelf);
    test_subj(terms[0], EcsThis, EcsSelf|EcsSuperSet);
    test_int(terms[0].subj.set.relation, EcsIsA);
    test_obj(terms[0], Obj, EcsSelf);
    test_int(terms[0].oper, EcsAnd);
    test_int(terms[0].inout, EcsInOutDefault);

    ecs_filter_fini(&f);

    ecs_fini(world);
}
