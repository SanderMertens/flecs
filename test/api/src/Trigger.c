#include <api.h>

static int on_remove_count = 0;

static
void Trigger(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
}

static
void Trigger_w_value(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    Position *p = ecs_field(it, Position, 1);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

static
void Trigger_w_value_from_entity(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] == 0);
    test_assert(it->sources[0] != 0);

    Position *p = ecs_field(it, Position, 1);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

static
void Trigger_w_value_instanced(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_assert(it->count > 1);

    Position *p = ecs_field(it, Position, 1);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

static
void Trigger_w_filter_term(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    test_bool(it->flags & EcsIterIsFilter, true);
}

static
void Trigger_n_w_values(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    Position *p = ecs_field(it, Position, 1);

    test_assert(it->entities != NULL);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_assert(it->entities[i] != 0);    
        test_int(p[i].x, 10 + i * 20);
        test_int(p[i].y, 20 + i * 20);
    }
}

static
void TriggerAdd(ecs_iter_t *it) {
    ecs_id_t id = *(ecs_id_t*)it->ctx;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(it->world, it->entities[i], id);
    }
}

static
void TriggerRemove(ecs_iter_t *it) {
    ecs_id_t id = *(ecs_id_t*)it->ctx;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_remove_id(it->world, it->entities[i], id);
    }
}

static
void TriggerClear(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_clear(it->world, it->entities[i]);
    }
}

static
void TriggerDelete(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_delete(it->world, it->entities[i]);
    }
}

void Trigger_on_add_trigger_before_table() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    /* Create entity/table after trigger, should invoke trigger */
    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_on_add_trigger_after_table() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    /* Create entity/before trigger */
    ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });

    /* Create trigger after table, should send notification to table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_on_remove_trigger_before_table() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    /* Create trigger after table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    /* Create entity/table after trigger, should invoke trigger */
    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_on_remove_trigger_after_table() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    /* Create entity/before trigger */
    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    /* Create trigger after table, should send notification to table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_on_add_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_on_add_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_on_add_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = EcsWildcard,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_add_id(world, e, TagB);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagB);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagB);

    ecs_fini(world);
}

void Trigger_on_add_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Pred, Obj),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Pred, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Pred, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, Obj));

    ecs_fini(world);
}

void Trigger_on_add_pair_obj_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Pred, EcsWildcard),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Pred, ObjA)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Pred, ObjA));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, ObjA));

    ecs_os_zeromem(&ctx);

    ecs_add_pair(world, e, Pred, ObjB);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Pred, ObjB));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, ObjB));

    ecs_fini(world);
}

void Trigger_on_add_pair_pred_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, PredA);
    ECS_TAG(world, PredB);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, Obj),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(PredA, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(PredA, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(PredA, Obj));

    ecs_os_zeromem(&ctx);

    ecs_add_pair(world, e, PredB, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(PredB, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(PredB, Obj));

    ecs_fini(world);
}

void Trigger_on_add_pair_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, EcsWildcard),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Pred, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Pred, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, Obj));

    ecs_fini(world);
}

void Trigger_on_add_any() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    Probe *ctx_any = ecs_os_calloc_t(Probe);
    ecs_entity_t t_any = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = EcsAny,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = ctx_any
    });

    Probe *ctx_wc = ecs_os_calloc_t(Probe);
    ecs_entity_t t_wc = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = EcsWildcard,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = ctx_wc
    });

    Probe *ctx_wc_pair = ecs_os_calloc_t(Probe);
    ecs_entity_t t_wc_pair = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, EcsWildcard),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = ctx_wc_pair
    });

    ecs_os_zeromem(ctx_any);
    ecs_os_zeromem(ctx_wc);
    ecs_os_zeromem(ctx_wc_pair);

    ecs_new(world, Tag);
    test_int(ctx_any->invoked, 1);
    test_int(ctx_any->count, 1);
    test_int(ctx_any->system, t_any);
    test_int(ctx_any->event, EcsOnAdd);
    test_int(ctx_any->event_id, Tag);
    test_int(ctx_wc->invoked, 1);
    test_int(ctx_wc->count, 1);
    test_int(ctx_wc->system, t_wc);
    test_int(ctx_wc->event, EcsOnAdd);
    test_int(ctx_wc->event_id, Tag);
    test_int(ctx_wc_pair->invoked, 0);
    ecs_os_zeromem(ctx_any);
    ecs_os_zeromem(ctx_wc);
    ecs_os_zeromem(ctx_wc_pair);

    ecs_new_w_pair(world, Rel, Obj);
    test_int(ctx_any->invoked, 1);
    test_int(ctx_any->count, 1);
    test_int(ctx_any->system, t_any);
    test_int(ctx_any->event, EcsOnAdd);
    test_int(ctx_any->event_id, ecs_pair(Rel, Obj));
    test_int(ctx_wc->invoked, 0);
    test_int(ctx_wc_pair->invoked, 1);
    test_int(ctx_wc_pair->count, 1);
    test_int(ctx_wc_pair->system, t_wc_pair);
    test_int(ctx_wc_pair->event, EcsOnAdd);
    test_int(ctx_wc_pair->event_id, ecs_pair(Rel, Obj));

    ecs_fini(world);

    ecs_os_free(ctx_any);
    ecs_os_free(ctx_wc);
    ecs_os_free(ctx_wc_pair);
}

void Trigger_on_remove_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_on_remove_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_on_remove_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = EcsWildcard,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA, TagB}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_remove_id(world, e, TagB);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagB);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagB);

    ecs_fini(world);
}

void Trigger_on_remove_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Pred, Obj),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Pred, Obj)}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, Pred, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, ecs_pair(Pred, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, Obj));

    ecs_fini(world);
}

void Trigger_on_remove_pair_obj_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Pred, EcsWildcard),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Pred, ObjA), ecs_pair(Pred, ObjB)}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, Pred, ObjA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, ecs_pair(Pred, ObjA));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, ObjA));

    ecs_os_zeromem(&ctx);

    ecs_add_pair(world, e, Pred, ObjB);

    ecs_remove_pair(world, e, Pred, ObjB);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, ecs_pair(Pred, ObjB));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, ObjB));

    ecs_fini(world);
}

void Trigger_on_remove_pair_pred_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, PredA);
    ECS_TAG(world, PredB);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, Obj),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(PredA, Obj), ecs_pair(PredB, Obj)}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, PredA, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, ecs_pair(PredA, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(PredA, Obj));

    ecs_os_zeromem(&ctx);

    ecs_remove_pair(world, e, PredB, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, ecs_pair(PredB, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(PredB, Obj));

    ecs_fini(world);
}

void Trigger_on_remove_pair_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, EcsWildcard),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Pred, Obj)}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, Pred, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, ecs_pair(Pred, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, Obj));

    // Delete trigger so it doesn't go crazy while shutting down the world
    ecs_delete(world, t);

    ecs_fini(world);
}

void Trigger_wildcard_pair_w_pred_component() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(ecs_id(Position), EcsWildcard),
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });
    test_assert(t != 0);

    ecs_entity_t e = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_set_pair(world, e, Position, ObjA, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(ecs_id(Position), ObjA));

    /* Change existing component without triggering OnSet as the callback
     * expects value {10, 20}, then add a new component with {10, 20} */
    Position *p = ecs_get_mut_pair(world, e, Position, ObjA);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    p->x = 30;
    p->y = 40;

    ecs_os_zeromem(&ctx);

    ecs_set_pair(world, e, Position, ObjB, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(ecs_id(Position), ObjB));

    ecs_fini(world);
}

void Trigger_wildcard_pair_w_obj_component() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, ecs_id(Position)),
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });
    test_assert(t != 0);

    ecs_entity_t e = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_set_pair_object(world, e, RelA, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelA, ecs_id(Position)));

    /* Change existing component without triggering OnSet as the callback
     * expects value {10, 20}, then add a new component with {10, 20} */
    Position *p = ecs_get_mut_pair_second(world, e, RelA, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    p->x = 30;
    p->y = 40;

    ecs_os_zeromem(&ctx);

    ecs_set_pair_object(world, e, RelB, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelB, ecs_id(Position)));

    ecs_fini(world);
}


void Trigger_on_set_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .events = {EcsOnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_on_set_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = EcsWildcard,
        .events = {EcsOnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_on_set_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(ecs_id(Position), Obj),
        .events = {EcsOnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(ecs_id(Position), Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_set_pair(world, e, Position, Obj, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(ecs_id(Position), Obj));

    ecs_fini(world);
}

void Trigger_on_set_pair_w_obj_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(ecs_id(Position), EcsWildcard),
        .events = {EcsOnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(ecs_id(Position), Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_set_pair(world, e, Position, Obj, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(ecs_id(Position), Obj));

    ecs_fini(world);
}

void Trigger_on_set_pair_pred_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, Obj),
        .events = {EcsOnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(ecs_id(Position), Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_set_pair(world, e, Position, Obj, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(ecs_id(Position), Obj));

    ecs_fini(world);
}

void Trigger_on_set_pair_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, EcsWildcard),
        .events = {EcsOnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 1); /* Triggers for self */
    ecs_os_zeromem(&ctx);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(ecs_id(Position), Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_set_pair(world, e, Position, Obj, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(ecs_id(Position), Obj));

    ecs_fini(world);
}

void Trigger_on_add_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_on_set_component_after_modified() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .events = {EcsOnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_modified(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_un_set_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .events = {EcsUnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsUnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_un_set_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = EcsWildcard,
        .events = {EcsUnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsUnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_un_set_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    // UnSet only works on components
    ecs_set(world, Rel, EcsComponent, {.size = 1, .alignment = 1});

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, Obj),
        .events = {EcsUnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, Rel, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsUnSet);
    test_int(ctx.event_id, ecs_pair(Rel, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Rel, Obj));

    ecs_fini(world);
}

void Trigger_un_set_pair_w_obj_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    // UnSet only works on components
    ecs_set(world, Rel, EcsComponent, {.size = 1, .alignment = 1});

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, EcsWildcard),
        .events = {EcsUnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, Rel, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsUnSet);
    test_int(ctx.event_id, ecs_pair(Rel, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Rel, Obj));

    ecs_fini(world);
}

void Trigger_un_set_pair_pred_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    // UnSet only works on components
    ecs_set(world, Rel, EcsComponent, {.size = 1, .alignment = 1});

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, Obj),
        .events = {EcsUnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, Rel, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsUnSet);
    test_int(ctx.event_id, ecs_pair(Rel, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Rel, Obj));

    ecs_fini(world);
}

void Trigger_un_set_pair_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    // UnSet only works on components
    ecs_set(world, Rel, EcsComponent, {.size = 1, .alignment = 1});

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(EcsWildcard, EcsWildcard),
        .events = {EcsUnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, Rel, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsUnSet);
    test_int(ctx.event_id, ecs_pair(Rel, Obj));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Rel, Obj));

    // Delete trigger so it doesn't go crazy while shutting down the world
    ecs_delete(world, t);

    ecs_fini(world);
}

void Trigger_on_add_not_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].oper = EcsNot,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_add_id(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_remove_not_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].oper = EcsNot,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_add_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].src.flags = EcsUp,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base_no_comp = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_no_comp);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.s[0][0], base);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_add_superset_2_levels() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].src.flags = EcsUp,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base_no_comp = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_entity_t base_of_base = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_new_w_pair(world, EcsIsA, base_of_base);
    test_int(ctx.invoked, 1);
    ctx = (Probe){0};

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_no_comp);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.s[0][0], base_of_base);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_remove_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].src.flags = EcsUp,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base_no_comp = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_no_comp);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, EcsIsA, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_add_superset_childof() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = TagA,
            .src.trav = EcsChildOf,
            .src.flags = EcsUp
        },
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base_no_comp = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, base_no_comp);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsChildOf, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_remove_superset_childof() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = TagA,
            .src.trav = EcsChildOf,
            .src.flags = EcsUp
        },
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base_no_comp = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, base_no_comp);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsChildOf, base);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, EcsChildOf, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_add_self_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].src.flags = EcsSelf | EcsUp,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base_no_comp = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_new(world, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], base);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_no_comp);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_remove_pair(world, e, EcsIsA, base);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, base);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_remove_self_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].src.flags = EcsSelf | EcsUp,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base_no_comp = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);

    ecs_os_zeromem(&ctx);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_no_comp);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, EcsIsA, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 0);
    
    ecs_remove(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, base);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, EcsIsA, base);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_add_twice() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_add_id(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_remove_twice() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 0);
    
    ecs_fini(world);
}

void Trigger_on_remove_w_clear() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_clear(world, e);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    
    ecs_fini(world);
}

void Trigger_on_remove_w_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_delete(world, e);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    
    ecs_fini(world);
}

void Trigger_on_remove_w_world_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
}

void Trigger_on_add_w_clone() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_os_zeromem(&ctx);

    ecs_entity_t e2 = ecs_clone(world, 0, e, true);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e2);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_add_in_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = TriggerAdd,
        .ctx = &TagB
    });

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_assert(ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Trigger_remove_in_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = TriggerRemove,
        .ctx = &TagB
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA, TagB}
    });
    test_assert(e != 0);

    test_assert(!ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Trigger_clear_in_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = TriggerClear,
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA, TagB}
    });
    test_assert(e != 0);

    test_assert(!ecs_has_id(world, e, TagA));
    test_assert(!ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Trigger_delete_in_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = TriggerDelete,
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA, TagB}
    });
    test_assert(e != 0);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Trigger_trigger_w_named_entity() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = ecs_entity(world, {.name = "MyTrigger"}),
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    /* Create entity/table after trigger, should invoke trigger */
    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void RemoveSelf(ecs_iter_t *it) {
    Self *s = ecs_field(it, Self, 1);
    test_assert(s != NULL);

    ecs_id_t ecs_id(Self) = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_assert(s[i].value == it->entities[i]);
        
        const Self *ptr = ecs_get(it->world, it->entities[i], Self);
        test_assert(ptr != NULL);
        test_assert(ptr->value == it->entities[i]);

        // Set to 0 so that if an entity were to get matched twice, it wouldn't
        // silently succeed.
        s[i].value = 0;
        on_remove_count ++;
    }
}

void Trigger_on_remove_tree() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);

    ECS_OBSERVER(world, RemoveSelf, EcsOnRemove, Self);

    ecs_entity_t root = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, root);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, root);
    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, e6);
    ecs_entity_t e8 = ecs_new_w_pair(world, EcsChildOf, e6);
    ecs_entity_t e9 = ecs_new_w_pair(world, EcsChildOf, e7);
    ecs_entity_t e10 = ecs_new_w_pair(world, EcsChildOf, e7);
    
    ecs_set(world, e1, Self, {e1});
    ecs_set(world, e2, Self, {e2});
    ecs_set(world, e3, Self, {e3});
    ecs_set(world, e4, Self, {e4});
    ecs_set(world, e5, Self, {e5});
    ecs_set(world, e6, Self, {e6});
    ecs_set(world, e7, Self, {e7});
    ecs_set(world, e8, Self, {e8});
    ecs_set(world, e9, Self, {e9});
    ecs_set(world, e10, Self, {e10});

    ecs_delete(world, root);

    test_int(on_remove_count, 10);

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));
    test_assert(!ecs_is_alive(world, e4));
    test_assert(!ecs_is_alive(world, e5));
    test_assert(!ecs_is_alive(world, e6));
    test_assert(!ecs_is_alive(world, e7));
    test_assert(!ecs_is_alive(world, e8));
    test_assert(!ecs_is_alive(world, e9));
    test_assert(!ecs_is_alive(world, e10));
    
    ecs_fini(world);
}

void Trigger_set_get_context() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    int32_t ctx_a, ctx_b;
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = ecs_entity(world, {.name = "MyTrigger"}),
        .filter.terms[0].id = Tag,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx_a
    });
    test_assert(t != 0);

    test_assert(ecs_get_observer_ctx(world, t) == &ctx_a);

    test_assert(ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = t,
        .ctx = &ctx_b
    }) == t);

    test_assert(ecs_get_observer_ctx(world, t) == &ctx_b);

    ecs_fini(world);
}

void Trigger_set_get_binding_context() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    int32_t ctx_a, ctx_b;
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = ecs_entity(world, {.name = "MyTrigger"}),
        .filter.terms[0].id = Tag,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .binding_ctx = &ctx_a
    });
    test_assert(t != 0);

    test_assert(ecs_get_observer_binding_ctx(world, t) == &ctx_a);

    test_assert(ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = t,
        .binding_ctx = &ctx_b
    }) == t);

    test_assert(ecs_get_observer_binding_ctx(world, t) == &ctx_b);

    ecs_fini(world);
}

static int ctx_value;
static
void ctx_free(void *ctx) {
    test_assert(&ctx_value == ctx);
    ctx_value ++;
}

static int binding_ctx_value;
static
void binding_ctx_free(void *ctx) {
    test_assert(&binding_ctx_value == ctx);
    binding_ctx_value ++;
}

void Trigger_delete_trigger_w_delete_ctx() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx_value,
        .ctx_free = ctx_free,
        .binding_ctx = &binding_ctx_value,
        .binding_ctx_free = binding_ctx_free
    });
    test_assert(t != 0);

    test_assert(ecs_get_observer_ctx(world, t) == &ctx_value);
    test_assert(ecs_get_observer_binding_ctx(world, t) == &binding_ctx_value);

    ecs_delete(world, t);

    test_int(ctx_value, 1);
    test_int(binding_ctx_value, 1);

    ecs_fini(world);
}

void Trigger_trigger_w_index() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .term_index = 50,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });
    test_assert(t != 0);

    ecs_new(world, Tag);

    test_int(ctx.invoked, 1);
    test_int(ctx.term_index, 50);
    
    ecs_fini(world);
}

static ecs_table_t *trigger_table;

void TypeTrigger(ecs_iter_t *it) {
    trigger_table = it->table;
}

void Trigger_iter_type_set() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnAdd},
        .callback = TypeTrigger,
        .ctx = &ctx
    });
    test_assert(t != 0);

    ecs_new(world, Tag);

    test_assert(trigger_table != NULL);
    const ecs_type_t *type = ecs_table_get_type(trigger_table);
    test_int(type->count, 1);
    test_int(type->array[0], Tag);
    
    ecs_fini(world);
}

void TriggerReadonly(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
    test_bool(ecs_field_is_readonly(it, 1), true);
}

void Trigger_readonly_term() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { TagA, .inout = EcsIn },
        .events = {EcsOnAdd},
        .callback = TriggerReadonly,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);
    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_trigger_on_prefab() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx_1 = {0};
    Probe ctx_2 = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { TagA },
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx_1
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }, { EcsPrefab, .oper = EcsOptional }},
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx_2
    });

    ecs_new(world, TagA);
    test_int(ctx_1.invoked, 1);
    test_int(ctx_2.invoked, 1);
    ecs_os_zeromem(&ctx_1);
    ecs_os_zeromem(&ctx_2);

    ecs_entity_t e = ecs_new_w_id(world, EcsPrefab);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_add(world, e, TagA);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 1);
    ecs_os_zeromem(&ctx_2);

    e = ecs_new_w_id(world, EcsDisabled);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_add(world, e, TagA);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_fini(world);
}

void Trigger_trigger_on_disabled() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx_1 = {0};
    Probe ctx_2 = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { TagA },
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx_1
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }, { EcsDisabled, .oper = EcsOptional }},
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx_2
    });

    ecs_new(world, TagA);
    test_int(ctx_1.invoked, 1);
    test_int(ctx_2.invoked, 1);
    ecs_os_zeromem(&ctx_1);
    ecs_os_zeromem(&ctx_2);

    ecs_entity_t e = ecs_new_w_id(world, EcsPrefab);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_add(world, e, TagA);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_os_zeromem(&ctx_2);

    e = ecs_new_w_id(world, EcsDisabled);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_add(world, e, TagA);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 1);

    ecs_fini(world);
}

void Trigger_trigger_on_prefab_tag() {
    ecs_world_t *world = ecs_mini();

    Probe ctx_1 = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { EcsPrefab },
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx_1
    });

    ecs_new_w_id(world, EcsPrefab);
    test_int(ctx_1.invoked, 1);

    ecs_fini(world);
}

void Trigger_trigger_on_disabled_tag() {
    ecs_world_t *world = ecs_mini();

    Probe ctx_1 = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { EcsDisabled },
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx_1
    });

    ecs_new_w_id(world, EcsDisabled);
    test_int(ctx_1.invoked, 1);

    ecs_fini(world);
}

void Trigger_trigger_cleanup_2_w_self_super_id() {
    ecs_world_t * world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = Tag, .src.flags = EcsUp },
        .events = { EcsOnAdd },
        .callback = Trigger
    });

    ecs_entity_t t2 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { Tag },
        .events = { EcsOnAdd },
        .callback = Trigger
    });

    test_assert(t1 != 0);
    test_assert(t2 != 0);

    ecs_fini(world);

    /* Ensure two triggers for Tag and Tag(up) are cleaned up correctly */
}

void Trigger_on_add_yield_existing() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 3);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, Tag);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], Tag);

    // Ensure normal triggering also still works
    ecs_os_zeromem(&ctx);
    ecs_new(world, Tag);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_on_add_yield_existing_2_tables() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_add(world, e3, TagB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 3);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[1][0], TagA);

    // Ensure normal triggering also still works
    ecs_os_zeromem(&ctx);
    ecs_new(world, TagA);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_on_add_yield_existing_wildcard_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, ObjB);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, EcsWildcard),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 3);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Rel, ObjB)); /* last triggered id */
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], ecs_pair(Rel, ObjA));
    test_int(ctx.c[1][0], ecs_pair(Rel, ObjB));

    // Ensure normal triggering also still works
    ecs_os_zeromem(&ctx);
    ecs_new_w_pair(world, Rel, ObjA);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_on_set_yield_existing() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {50, 60});

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .events = {EcsOnSet},
        .callback = Trigger_n_w_values,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 3);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], ecs_id(Position));

    // Ensure normal triggering also still works
    ecs_os_zeromem(&ctx);
    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnSet);

    ecs_fini(world);
}

void Trigger_filter_term() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(Position), .inout = EcsInOutNone },
        .events = {EcsOnSet},
        .callback = Trigger_w_filter_term,
        .ctx = &ctx
    });

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    test_assert(e1 != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_on_add_remove_after_exclusive_add() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Exclusive);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx_add = {0};
    ecs_entity_t t_add = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, EcsWildcard),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx_add
    });

    Probe ctx_remove = {0};
    ecs_entity_t t_remove = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, EcsWildcard),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx_remove
    });

    ecs_entity_t e = ecs_new_id(world);

    ecs_add_pair(world, e, Rel, ObjA);
    test_assert( ecs_has_pair(world, e, Rel, ObjA));

    test_int(ctx_add.invoked, 1);
    test_int(ctx_add.count, 1);
    test_int(ctx_add.system, t_add);
    test_int(ctx_add.event, EcsOnAdd);
    test_int(ctx_add.event_id, ecs_pair(Rel, ObjA));
    test_int(ctx_add.term_count, 1);
    test_null(ctx_add.param);

    test_int(ctx_remove.invoked, 0);

    ecs_os_zeromem(&ctx_add);

    ecs_add_pair(world, e, Rel, ObjB);
    test_assert( ecs_has_pair(world, e, Rel, ObjB));
    test_assert( !ecs_has_pair(world, e, Rel, ObjA));

    test_int(ctx_add.invoked, 1);
    test_int(ctx_add.count, 1);
    test_int(ctx_add.system, t_add);
    test_int(ctx_add.event, EcsOnAdd);
    test_int(ctx_add.event_id, ecs_pair(Rel, ObjB));
    test_int(ctx_add.term_count, 1);
    test_null(ctx_add.param);

    test_int(ctx_remove.invoked, 1);
    test_int(ctx_remove.count, 1);
    test_int(ctx_remove.system, t_remove);
    test_int(ctx_remove.event, EcsOnRemove);
    test_int(ctx_remove.event_id, ecs_pair(Rel, ObjA));
    test_int(ctx_remove.term_count, 1);
    test_null(ctx_remove.param);

    ecs_fini(world);
}

void Trigger_on_add_base() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA, /* Implicitly also listens to IsA */
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_add(world, base, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_os_zeromem(&ctx);

    ecs_add(world, base, TagA);
    ecs_add(world, base, TagB);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Trigger_on_remove_base() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA, /* Implicitly also listens to IsA */
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_add(world, base, TagA);
    ecs_add(world, base, TagB);
    test_int(ctx.invoked, 0);

    ecs_remove(world, base, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, base, TagA);
    ecs_remove(world, base, TagB);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Trigger_on_set_base() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position), /* Implicitly also listens to IsA */
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_set(world, base, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_os_zeromem(&ctx);

    ecs_set(world, base, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_os_zeromem(&ctx);

    ecs_set(world, base, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Trigger_on_unset_base() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position), /* Implicitly also listens to IsA */
        .events = {EcsUnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_set(world, base, Position, {10, 20});
    ecs_set(world, base, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_remove(world, base, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsUnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, base, Position);
    ecs_remove(world, base, Velocity);
    test_int(ctx.invoked, 0);
    
    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Trigger_on_add_base_superset_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].src.flags = EcsUp,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_add(world, base, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_os_zeromem(&ctx);

    ecs_add(world, base, TagA);
    ecs_add(world, base, TagB);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_add_base_superset_trigger_2_lvls() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].src.flags = EcsUp,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base_of_base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, base, EcsIsA, base_of_base);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_add(world, base_of_base, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_os_zeromem(&ctx);

    ecs_add(world, base_of_base, TagA);
    ecs_add(world, base_of_base, TagB);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Trigger_on_add_base_2_entities() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA, /* Implicitly also listens to IsA */
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_add(world, base, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_os_zeromem(&ctx);

    ecs_add(world, base, TagA);
    ecs_add(world, base, TagB);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Trigger_on_add_base_2_entities_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA, /* Implicitly also listens to IsA */
        .filter.terms[0].inout = EcsInOutNone,
        .events = {EcsOnAdd},
        .callback = Trigger_w_filter_term,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_add(world, base, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_os_zeromem(&ctx);

    ecs_add(world, base, TagA);
    ecs_add(world, base, TagB);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Trigger_on_set_base_w_value_2_entities() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position), /* Implicitly also listens to IsA */
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_set(world, base, Position, {10, 20});
    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_os_zeromem(&ctx);

    ecs_set(world, base, Position, {10, 20});
    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_os_zeromem(&ctx);

    ecs_set(world, base, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_set_base_w_value_2_entities_instanced() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position), /* Implicitly also listens to IsA */
        .filter.instanced = true,
        .events = {EcsOnSet},
        .callback = Trigger_w_value_instanced,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_set(world, base, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_os_zeromem(&ctx);

    ecs_set(world, base, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_os_zeromem(&ctx);

    ecs_set(world, base, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_add_base_w_override() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA, /* Implicitly also listens to IsA */
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_add(world, base, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.s[0][0], base);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, base, TagA);
    ecs_add(world, base, TagA);
    ecs_add(world, base, TagB);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Trigger_on_set_base_w_override() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position), /* Implicitly also listens to IsA */
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_set(world, base, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.s[0][0], base);

    ecs_os_zeromem(&ctx);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);

    ecs_set(world, base, Position, {10, 20});
    test_int(ctx.invoked, 0); // no trigger, overridden

    ecs_set(world, base, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Trigger_entity_source_1_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t subj = ecs_new_id(world);
    ecs_entity_t dummy = ecs_new_id(world);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .filter.terms[0].src.id = subj,
        .events = {EcsOnSet},
        .callback = Trigger_w_value_from_entity,
        .ctx = &ctx
    });

    ecs_set(world, dummy, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_set(world, subj, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], 0);
    test_int(ctx.s[0][0], subj);

    ecs_fini(world);
}

void Trigger_entity_source_2_triggers() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t subj_a = ecs_new_id(world);
    ecs_entity_t subj_b = ecs_new_id(world);
    ecs_entity_t dummy = ecs_new_id(world);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .filter.terms[0].src.id = subj_a,
        .events = {EcsOnSet},
        .callback = Trigger_w_value_from_entity,
        .ctx = &ctx
    });

    ecs_entity_t t2 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .filter.terms[0].src.id = subj_b,
        .events = {EcsOnSet},
        .callback = Trigger_w_value_from_entity,
        .ctx = &ctx
    });

    ecs_set(world, dummy, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_set(world, subj_a, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t1);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], 0);
    test_int(ctx.s[0][0], subj_a);

    ecs_os_zeromem(&ctx);

    ecs_set(world, subj_b, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t2);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], 0);
    test_int(ctx.s[0][0], subj_b);

    ecs_fini(world);
}

void Trigger_entity_source_base_set() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t subj = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t dummy = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t dummy_no_base = ecs_new_w_pair(world, EcsIsA, base);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .filter.terms[0].src.id = subj,
        .events = {EcsOnSet},
        .callback = Trigger_w_value_from_entity,
        .ctx = &ctx
    });

    ecs_set(world, dummy, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_set(world, dummy_no_base, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_set(world, base, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], 0);
    test_int(ctx.s[0][0], base);

    ecs_fini(world);
}

void Trigger_not_from_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .filter.terms[0].oper = EcsNot,
        .filter.terms[0].src.flags = EcsUp,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_delete(world, inst);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_create_stresstest() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    /* Make sure we can create & delete 100k triggers without running out of
     * memory to verify there are no leaks in the administration (sanitizer 
     * builds don't detect leaks that are cleaned up @ ecs_fini). */
    for (int i = 0; i < 100 * 1000; i ++) {
        ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
            .filter.terms[0].id = Tag,
            .filter.terms[0].src.flags = EcsSelf | EcsUp,
            .events = {EcsOnAdd},
            .callback = Trigger
        });

        ecs_delete(world, t);
    }

    test_assert(true);

    ecs_fini(world);
}

static
void Trigger_w_new_entity_value(ecs_iter_t *it) {
    probe_iter(it);
}

void Trigger_add_non_existing_entity() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, EcsWildcard),
        .events = {EcsOnAdd},
        .callback = Trigger_w_new_entity_value,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, Rel, 1000);

    test_assert( ecs_has_pair(world, e, Rel, 1000));
    test_assert( ecs_is_alive(world, 1000));

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Rel, 1000));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    ecs_fini(world);
}

void Trigger_on_add_self_trigger_with_add_isa() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .filter.terms[0].src.flags = EcsSelf,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    test_assert(t1 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_new_id(world);
    ecs_add(world, base, Tag);
    test_int(ctx.invoked, 1);

    ctx = (Probe){0};

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_set_self_trigger_with_add_isa() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .filter.terms[0].src.flags = EcsSelf,
        .events = {EcsOnSet},
        .callback = Trigger,
        .ctx = &ctx
    });

    test_assert(t1 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_set(world, 0, Position, {10, 20});
    test_int(ctx.invoked, 1);

    ctx = (Probe){0};

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

static int invoke_count = 0;
static ecs_entity_t base_ent = 0;
static ecs_entity_t inst_ent_a = 0;
static ecs_entity_t inst_ent_b = 0;

static void TriggerTwice(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_assert(base_ent != 0);
    test_assert(inst_ent_a != 0);
    test_assert(inst_ent_b != 0);
    test_int(it->count, 1);

    if (invoke_count == 0) {
        test_assert(it->entities[0] == base_ent);
        invoke_count ++;
    } else if (invoke_count == 1) {
        test_assert(it->entities[0] == inst_ent_b);
        invoke_count ++;
    } else {
        test_int(invoke_count, 2);
        test_assert(it->entities[0] == inst_ent_a);
        invoke_count ++;
    }
}

void Trigger_notify_propagated_twice() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = TriggerTwice,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    base_ent = ecs_new_id(world);
    inst_ent_a = ecs_new_w_pair(world, EcsIsA, base_ent);
    ecs_add(world, inst_ent_a, TagB);
    inst_ent_b = ecs_new_w_pair(world, EcsIsA, base_ent);

    test_int(ctx.invoked, 0);

    ecs_add(world, base_ent, TagA);

    test_int(ctx.invoked, 3);
    test_int(invoke_count, 3);

    ecs_fini(world);
}

void Trigger_trigger_superset_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t base = ecs_new_id(world);
    ecs_entity_t inst = ecs_new_id(world);
    ecs_add_pair(world, inst, EcsIsA, base);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, EcsWildcard), 
        .filter.terms[0].src.flags = EcsUp,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });
    test_assert(t != 0);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, base, Rel, ObjA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Rel, ObjA));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base);

    ecs_os_zeromem(&ctx);

    ecs_add_pair(world, base, Rel, ObjB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Rel, ObjB));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base);

    ecs_fini(world);
}

void Trigger_remove_wildcard_1_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, ObjA);
    test_assert( ecs_has_pair(world, e, Rel, ObjA));
    test_assert( ecs_has_pair(world, e, Rel, EcsWildcard));

    Probe ctx_a = {0};
    ecs_entity_t t_a = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, ObjA), 
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx_a
    });
    test_assert(t_a != 0);

    Probe ctx_b = {0};
    ecs_entity_t t_b = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, EcsWildcard),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx_b
    });
    test_assert(t_b != 0);

    test_int(ctx_a.invoked, 0);
    test_int(ctx_b.invoked, 0);

    ecs_remove_pair(world, e, Rel, EcsWildcard);
    test_assert( !ecs_has_pair(world, e, Rel, ObjA));
    test_assert( !ecs_has_pair(world, e, Rel, EcsWildcard));

    test_int(ctx_a.invoked, 1);
    test_int(ctx_a.count, 1);
    test_int(ctx_a.system, t_a);
    test_int(ctx_a.event, EcsOnRemove);
    test_int(ctx_a.event_id, ecs_pair(Rel, ObjA));
    test_int(ctx_a.term_count, 1);
    test_int(ctx_a.e[0], e);

    test_int(ctx_b.invoked, 1);
    test_int(ctx_b.count, 1);
    test_int(ctx_b.system, t_b);
    test_int(ctx_b.event, EcsOnRemove);
    test_int(ctx_b.event_id, ecs_pair(Rel, ObjA));
    test_int(ctx_b.term_count, 1);
    test_int(ctx_b.e[0], e);

    ecs_fini(world);
}

void Trigger_remove_wildcard_2_ids() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe *ctx_array = ecs_os_calloc_n(Probe, 3);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, ObjA);
    ecs_add_pair(world, e, Rel, ObjB);
    test_assert( ecs_has_pair(world, e, Rel, ObjA));
    test_assert( ecs_has_pair(world, e, Rel, ObjB));
    test_assert( ecs_has_pair(world, e, Rel, EcsWildcard));

    Probe *ctx_a = &ctx_array[0];
    ecs_entity_t t_a = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, EcsWildcard), 
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = ctx_a
    });
    test_assert(t_a != 0);

    Probe *ctx_b = &ctx_array[1];
    ecs_entity_t t_b = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, ObjA),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = ctx_b
    });
    test_assert(t_b != 0);

    Probe *ctx_c = &ctx_array[2];
    ecs_entity_t t_c = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_pair(Rel, ObjB),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = ctx_c
    });
    test_assert(t_c != 0);

    test_int(ctx_a->invoked, 0);
    test_int(ctx_b->invoked, 0);
    test_int(ctx_c->invoked, 0);

    ecs_remove_pair(world, e, Rel, EcsWildcard);
    test_assert( !ecs_has_pair(world, e, Rel, ObjA));
    test_assert( !ecs_has_pair(world, e, Rel, ObjB));
    test_assert( !ecs_has_pair(world, e, Rel, EcsWildcard));

    test_int(ctx_a->invoked, 2);
    test_int(ctx_a->count, 2);
    test_int(ctx_a->system, t_a);
    test_int(ctx_a->event, EcsOnRemove);
    test_int(ctx_a->event_id, ecs_pair(Rel, ObjB));
    test_int(ctx_a->term_count, 1);
    test_int(ctx_a->e[0], e);

    test_int(ctx_b->invoked, 1);
    test_int(ctx_b->count, 1);
    test_int(ctx_b->system, t_b);
    test_int(ctx_b->event, EcsOnRemove);
    test_int(ctx_b->event_id, ecs_pair(Rel, ObjA));
    test_int(ctx_b->term_count, 1);
    test_int(ctx_b->e[0], e);

    test_int(ctx_c->invoked, 1);
    test_int(ctx_c->count, 1);
    test_int(ctx_c->system, t_c);
    test_int(ctx_c->event, EcsOnRemove);
    test_int(ctx_c->event_id, ecs_pair(Rel, ObjB));
    test_int(ctx_c->term_count, 1);
    test_int(ctx_c->e[0], e);

    ecs_os_free(ctx_array);

    ecs_fini(world);
}

void Trigger_on_set_w_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnSet},
        .callback = Trigger,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);

    ecs_fini(world);
}

static int nested_trigger_invoked = 0;

static void NestedTrigger(ecs_iter_t *it) {
    nested_trigger_invoked ++;
}

static int create_trigger_invoked = 0;

typedef struct {
    int32_t count;
    ecs_entity_t first;
} CreateTriggers_ctx;

static void CreateTriggers(ecs_iter_t *it) {
    CreateTriggers_ctx *ctx = it->ctx;

    create_trigger_invoked ++;

    ctx->first = ecs_new_id(it->world);

    int i;
    for (i = 0; i < ctx->count - 1; i ++) {
        ecs_new_id(it->world);
    }

    for (i = 0; i < ctx->count; i ++) {
        ecs_observer_init(it->world, &(ecs_observer_desc_t){
            .filter.terms[0].id = ctx->first + i,
            .events = {EcsOnAdd},
            .callback = NestedTrigger
        });
    }
}

void Trigger_create_triggers_in_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    int CreateCount = 10;

    CreateTriggers_ctx ctx = { .count = CreateCount };
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnAdd},
        .callback = CreateTriggers,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_add(world, e, TagA);
    test_int(create_trigger_invoked, 1);
    test_int(nested_trigger_invoked, 0);

    int i;
    for (i = 0; i < CreateCount; i ++) {
        ecs_entity_t t = ecs_new_id(world);
        ecs_add_id(world, t, ctx.first + i);
        test_int(nested_trigger_invoked, i + 1);
    }

    test_int(create_trigger_invoked, 1);
    test_int(nested_trigger_invoked, i);

    ecs_fini(world);
}

static
void Trigger_w_nonzero_value(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    Position *p = ecs_field(it, Position, 1);
    test_assert(p != NULL);
}

void Trigger_on_add_superset_w_component() {
	ecs_world_t *world = ecs_mini();

	ECS_COMPONENT(world, Position);

    Probe ctx = {0};
	ecs_observer_init(world, &(ecs_observer_desc_t){
		.filter.terms[0].id = ecs_id(Position),
		.events = {EcsOnAdd},
		.callback = Trigger_w_nonzero_value,
        .ctx = &ctx
	});

	ecs_entity_t b = ecs_new(world, 0);
	ecs_set(world, b, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], b);
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);
	
	ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, b);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], i);
    test_int(ctx.s[0][0], b);

    ecs_fini(world);
}

void Trigger_on_set_superset_w_component() {
	ecs_world_t *world = ecs_mini();

	ECS_COMPONENT(world, Position);

    Probe ctx = {0};
	ecs_observer_init(world, &(ecs_observer_desc_t){
		.filter.terms[0].id = ecs_id(Position),
		.events = {EcsOnSet},
		.callback = Trigger_w_value,
        .ctx = &ctx
	});

	ecs_entity_t b = ecs_new(world, 0);
	ecs_set(world, b, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], b);
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);

	ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, b);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], i);
    test_int(ctx.s[0][0], b);

    ecs_fini(world);
}

void Trigger_on_add_base_superset_w_owned() {
	ecs_world_t *world = ecs_mini();

	ECS_TAG(world, Tag);

    Probe ctx = {0};
	ecs_observer_init(world, &(ecs_observer_desc_t){
		.filter.terms[0] = {
            .id = ecs_id(Tag),
            .src.flags = EcsUp,
        },
		.events = {EcsOnAdd},
		.callback = Trigger,
        .ctx = &ctx
	});

	ecs_entity_t b = ecs_new(world, 0);
	ecs_add(world, b, Tag);
    test_int(ctx.invoked, 0);
	
	ecs_entity_t i = ecs_new(world, Tag);
    test_int(ctx.invoked, 0);
    
    ecs_add_pair(world, i, EcsIsA, b);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], i);
    test_int(ctx.s[0][0], b);

    ecs_fini(world);
}

void Trigger_on_add_base_self_superset_w_owned() {
	ecs_world_t *world = ecs_mini();

	ECS_TAG(world, Tag);

    Probe ctx = {0};
	ecs_observer_init(world, &(ecs_observer_desc_t){
		.filter.terms[0] = {
            .id = ecs_id(Tag),
            .src.flags = EcsSelf|EcsUp,
        },
		.events = {EcsOnAdd},
		.callback = Trigger,
        .ctx = &ctx
	});

	ecs_entity_t b = ecs_new(world, 0);
	ecs_add(world, b, Tag);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], b);
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);
	
	ecs_entity_t i1 = ecs_new(world, Tag);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], i1);
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);

    ecs_add_pair(world, i1, EcsIsA, b);
    test_int(ctx.invoked, 0);

    ecs_entity_t i2 = ecs_new_w_pair(world, EcsIsA, b);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], i2);
    test_int(ctx.s[0][0], b);

    ecs_fini(world);
}

void Trigger_on_set_self_from_child_of_prefab() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t base_child = ecs_new_entity(world, "Child");
    ecs_add_id(world, base_child, EcsPrefab);
    ecs_add_pair(world, base_child, EcsChildOf, base);
    ecs_set(world, base_child, Position, {10, 20});

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = ecs_id(Position),
            .src.flags = EcsSelf
        },
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);

    ecs_entity_t inst_child = ecs_lookup_child(world, inst, "Child");
    test_assert(inst_child != 0);
    test_int(ctx.e[0], inst_child);
    test_int(ctx.s[0][0], 0);

    test_assert(ecs_has(world, inst_child, Position));
    test_assert(ecs_owns(world, inst_child, Position));

    ecs_fini(world);
}

void Trigger_on_set_self_superset_from_child_of_prefab() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t base_child = ecs_new_entity(world, "Child");
    ecs_add_id(world, base_child, EcsPrefab);
    ecs_add_pair(world, base_child, EcsChildOf, base);
    ecs_set(world, base_child, Position, {10, 20});

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = ecs_id(Position),
            .src.flags = EcsSelf | EcsUp
        },
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t inst_child = ecs_lookup_child(world, inst, "Child");
    test_assert(inst_child != 0);
    test_assert(ecs_has(world, inst_child, Position));
    test_assert(ecs_owns(world, inst_child, Position));

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], inst_child);
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void Trigger_on_set_self_from_child_base_of_prefab() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t base_child_base = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, base_child_base, EcsPrefab);
    ecs_set(world, base_child_base, Position, {10, 20});

    ecs_entity_t base_child = ecs_new_entity(world, "Child");
    ecs_add_pair(world, base_child, EcsChildOf, base);
    ecs_add_pair(world, base_child, EcsIsA, base_child_base);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = ecs_id(Position),
            .src.flags = EcsSelf
        },
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t inst_child = ecs_lookup_child(world, inst, "Child");
    test_assert(inst_child != 0);
    test_assert(ecs_has(world, inst_child, Position));
    test_assert(!ecs_owns(world, inst_child, Position));

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_set_self_superset_from_child_base_of_prefab() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t base_child_base = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, base_child_base, EcsPrefab);
    ecs_set(world, base_child_base, Position, {10, 20});

    ecs_entity_t base_child = ecs_new_entity(world, "Child");
    ecs_add_pair(world, base_child, EcsChildOf, base);
    ecs_add_pair(world, base_child, EcsIsA, base_child_base);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = ecs_id(Position),
            .src.flags = EcsSelf | EcsUp
        },
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t inst_child = ecs_lookup_child(world, inst, "Child");
    test_assert(inst_child != 0);
    test_assert(ecs_has(world, inst_child, Position));
    test_assert(!ecs_owns(world, inst_child, Position));

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], inst_child);
    test_int(ctx.s[0][0], base_child_base);

    ecs_fini(world);
}

void Trigger_on_set_self_auto_override() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_set(world, 0, Position, {10, 20});
    ecs_add_id(world, base, ECS_OVERRIDE | ecs_id(Position));

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = ecs_id(Position),
            .src.flags = EcsSelf
        },
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert( ecs_has(world, inst, Position));
    test_assert( ecs_owns(world, inst, Position));

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void Trigger_on_set_self_superset_auto_override() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_set(world, 0, Position, {10, 20});
    ecs_add_id(world, base, ECS_OVERRIDE | ecs_id(Position));

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = ecs_id(Position),
            .src.flags = EcsSelf | EcsUp
        },
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert( ecs_has(world, inst, Position));
    test_assert( ecs_owns(world, inst, Position));

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void Trigger_on_set_superset_auto_override() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_set(world, 0, Position, {10, 20});
    ecs_add_id(world, base, ECS_OVERRIDE | ecs_id(Position));

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = ecs_id(Position),
            .src.flags = EcsUp
        },
        .events = {EcsOnSet},
        .callback = Trigger_w_value,
        .ctx = &ctx
    });

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert( ecs_has(world, inst, Position));
    test_assert( ecs_owns(world, inst, Position));

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base);

    ecs_fini(world);
}

void Trigger_not_only() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_observer(world, {
        .filter.terms = {
            { TagA, .oper = EcsNot }
        },
        .events = { EcsOnAdd },
        .callback = Trigger,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_entity_t i = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);

    ecs_delete(world, i);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_not_only_w_base() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_observer(world, {
        .filter.terms = {
            { TagA, .oper = EcsNot }
        },
        .events = { EcsOnAdd },
        .callback = Trigger,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_entity_t p = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    test_int(ctx.invoked, 0);

    ecs_delete(world, i);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_not_only_w_base_no_match() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_observer(world, {
        .filter.terms = {
            { TagB, .oper = EcsNot }
        },
        .events = { EcsOnAdd },
        .callback = Trigger,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_entity_t p = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    test_int(ctx.invoked, 0);

    ecs_delete(world, i);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_on_set_superset_after_filter_observer() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx_1 = {0};
    ecs_observer(world, {
        .filter.terms = {
            { ecs_id(Position) }
        },
        .filter.flags = EcsFilterNoData,
        .events = { EcsOnSet },
        .callback = Trigger,
        .ctx = &ctx_1
    });

    Probe ctx_2 = {0};
    ecs_observer(world, {
        .filter.terms = {
            { ecs_id(Position), .src.flags = EcsUp }
        },
        .events = { EcsOnSet },
        .callback = Trigger_w_value,
        .ctx = &ctx_2
    });

    ecs_entity_t base = ecs_new_id(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_set(world, base, Position, {10, 20});

    test_int(ctx_1.invoked, 2); 

    test_int(ctx_2.invoked, 1);
    test_int(ctx_2.count, 1);
    test_int(ctx_2.e[0], inst);
    test_int(ctx_2.c[0][0], ecs_id(Position));
    test_int(ctx_2.s[0][0], base);

    ecs_fini(world);
}

void Trigger_on_set_superset_after_filter_observer_w_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx_1 = {0};
    ecs_observer(world, {
        .filter.terms = {
            { ecs_id(Position) }
        },
        .filter.flags = EcsFilterNoData,
        .events = { EcsOnAdd },
        .callback = Trigger,
        .ctx = &ctx_1
    });

    Probe ctx_2 = {0};
    ecs_observer(world, {
        .filter.terms = {
            { ecs_id(Position), .src.flags = EcsUp }
        },
        .events = { EcsOnAdd },
        .callback = Trigger,
        .ctx = &ctx_2
    });

    ecs_entity_t base = ecs_new_id(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_set(world, base, Position, {10, 20});

    test_int(ctx_1.invoked, 2);

    test_int(ctx_2.invoked, 1);
    test_int(ctx_2.count, 1);
    test_int(ctx_2.e[0], inst);
    test_int(ctx_2.c[0][0], ecs_id(Position));
    test_int(ctx_2.s[0][0], base);

    ecs_fini(world);
}

void Trigger_on_set_superset_after_filter_observer_w_on_add_isa_after_set() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx_1 = {0};
    ecs_observer(world, {
        .filter.terms = {
            { ecs_id(Position), .src.flags = EcsUp }
        },
        .filter.flags = EcsFilterNoData,
        .events = { EcsOnAdd },
        .callback = Trigger,
        .ctx = &ctx_1
    });

    Probe ctx_2 = {0};
    ecs_observer(world, {
        .filter.terms = {
            { ecs_id(Position), .src.flags = EcsUp }
        },
        .events = { EcsOnAdd },
        .callback = Trigger,
        .ctx = &ctx_2
    });

    ecs_entity_t base = ecs_new_id(world);
    ecs_set(world, base, Position, {10, 20});
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], inst);
    test_int(ctx_1.c[0][0], ecs_id(Position));
    test_int(ctx_1.s[0][0], base);

    test_int(ctx_2.invoked, 1);
    test_int(ctx_2.count, 1);
    test_int(ctx_2.e[0], inst);
    test_int(ctx_2.c[0][0], ecs_id(Position));
    test_int(ctx_2.s[0][0], base);

    ecs_fini(world);
}

void Trigger_on_set_superset_after_filter_observer_w_on_add_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    // first observer causes observer to be initialized with filter flag
    Probe ctx_1 = {0};
    ecs_observer(world, { 
        .filter.terms = {
            { ecs_id(Position) }
        },
        .filter.flags = EcsFilterNoData,
        .events = { EcsOnAdd },
        .callback = Trigger,
        .ctx = &ctx_1
    });

    // second observer is not a filter, so value should be avaulable
    Probe ctx_2 = {0};
    ecs_observer(world, {
        .filter.terms = {
            { ecs_id(Position), .src.flags = EcsUp }
        },
        .events = { EcsOnAdd },
        .callback = Trigger,
        .ctx = &ctx_2
    });

    ecs_entity_t base = ecs_new_id(world);
    ecs_set(world, base, Position, {10, 20});
    ecs_override(world, base, Position); // override causes emit with 2 ids
    
    test_int(ctx_1.invoked, 1);
    test_int(ctx_2.invoked, 0);

    ecs_os_zeromem(&ctx_1);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], inst);
    test_int(ctx_1.c[0][0], ecs_id(Position));
    test_int(ctx_1.s[0][0], 0); // override

    test_int(ctx_2.invoked, 1);
    test_int(ctx_2.count, 1);
    test_int(ctx_2.e[0], inst);
    test_int(ctx_2.c[0][0], ecs_id(Position));
    test_int(ctx_2.s[0][0], base);

    ecs_fini(world);
}

void Trigger_propagate_w_union_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelX);
    ECS_ENTITY(world, RelY, Union);

    ecs_entity_t base = ecs_new_id(world);
    ecs_entity_t tgt = ecs_new_id(world);
    ecs_new_w_pair(world, EcsIsA, base);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = {
            .id = Tag,
            .src.flags = EcsUp
        },
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });
    test_int(ctx.invoked, 0);
    
    ecs_add_id(world, base, Tag);
    test_int(ctx.invoked, 1);

    ecs_os_zeromem(&ctx);
    ecs_add_pair(world, base, RelX, tgt);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, base, RelY, tgt);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}
