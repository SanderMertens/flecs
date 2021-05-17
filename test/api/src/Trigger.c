#include <api.h>

static int on_remove_count = 0;

void Trigger(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
}

void TriggerAdd(ecs_iter_t *it) {
    ecs_id_t id = *(ecs_id_t*)it->ctx;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(it->world, it->entities[i], id);
    }
}

void TriggerRemove(ecs_iter_t *it) {
    ecs_id_t id = *(ecs_id_t*)it->ctx;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_remove_id(it->world, it->entities[i], id);
    }
}

void TriggerClear(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_clear(it->world, it->entities[i]);
    }
}

void TriggerDelete(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_delete(it->world, it->entities[i]);
    }
}

void Trigger_on_add_trigger_before_table() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    /* Create entity/table after trigger, should invoke trigger */
    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_on_add_trigger_after_table() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    /* Create entity/before trigger */
    ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });

    /* Create trigger after table, should send notification to table */
    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_on_remove_trigger_before_table() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    /* Create trigger after table */
    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    /* Create entity/table after trigger, should invoke trigger */
    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_on_remove_trigger_after_table() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    /* Create entity/before trigger */
    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    /* Create trigger after table, should send notification to table */
    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Trigger_on_add_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_on_add_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_id(Position),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_id(Position)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_on_add_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = EcsWildcard,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ctx = (Probe){0};

    ecs_add_id(world, e, TagB);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagB);

    ecs_fini(world);
}

void Trigger_on_add_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_pair(Pred, Obj),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Pred, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, Obj));

    ecs_fini(world);
}

void Trigger_on_add_pair_obj_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_pair(Pred, EcsWildcard),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Pred, ObjA)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, ObjA));

    ctx = (Probe){0};

    ecs_add_pair(world, e, Pred, ObjB);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, ObjB));

    ecs_fini(world);
}

void Trigger_on_add_pair_pred_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, PredA);
    ECS_TAG(world, PredB);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_pair(EcsWildcard, Obj),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(PredA, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(PredA, Obj));

    ctx = (Probe){0};

    ecs_add_pair(world, e, PredB, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(PredB, Obj));

    ecs_fini(world);
}

void Trigger_on_add_pair_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_pair(EcsWildcard, EcsWildcard),
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Pred, Obj)}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, Obj));

    ecs_fini(world);
}

void Trigger_on_remove_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_on_remove_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_id(Position),
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_id(Position)}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Trigger_on_remove_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = EcsWildcard,
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
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ctx = (Probe){0};

    ecs_remove_id(world, e, TagB);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagB);

    ecs_fini(world);
}

void Trigger_on_remove_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_pair(Pred, Obj),
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
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, Obj));

    ecs_fini(world);
}

void Trigger_on_remove_pair_obj_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_pair(Pred, EcsWildcard),
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
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, ObjA));

    ctx = (Probe){0};

    ecs_add_pair(world, e, Pred, ObjB);

    ecs_remove_pair(world, e, Pred, ObjB);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, ObjB));

    ecs_fini(world);
}

void Trigger_on_remove_pair_pred_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, PredA);
    ECS_TAG(world, PredB);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_pair(EcsWildcard, Obj),
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
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(PredA, Obj));

    ctx = (Probe){0};

    ecs_remove_pair(world, e, PredB, Obj);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(PredB, Obj));

    ecs_fini(world);
}

void Trigger_on_remove_pair_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Obj);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = ecs_pair(EcsWildcard, EcsWildcard),
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
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(Pred, Obj));

    ecs_fini(world);
}

void Trigger_on_add_remove() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ctx = (Probe){0};

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_add_twice() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ctx = (Probe){0};

    ecs_add_id(world, e, TagA);

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Trigger_remove_twice() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ctx = (Probe){0};

    ecs_remove_id(world, e, TagA);

    test_int(ctx.invoked, 0);
    
    ecs_fini(world);
}

void Trigger_on_remove_w_clear() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_clear(world, e);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    
    ecs_fini(world);
}

void Trigger_on_remove_w_delete() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_delete(world, e);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    
    ecs_fini(world);
}

void Trigger_on_remove_w_world_fini() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnRemove},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
}

void Trigger_on_add_w_clone() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ctx = (Probe){0};

    ecs_entity_t e2 = ecs_clone(world, 0, e, true);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.e[0], e2);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Trigger_add_in_trigger() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
        .events = {EcsOnAdd},
        .callback = TriggerAdd,
        .ctx = &TagB
    });

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_assert(ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Trigger_remove_in_trigger() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
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
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
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
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term.id = TagA,
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
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    /* Create trigger before table */
    Probe ctx = {0};
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .entity.name = "MyTrigger",
        .term.id = TagA,
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    /* Create entity/table after trigger, should invoke trigger */
    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);

    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

typedef struct Self {
    ecs_entity_t value;
} Self;

void RemoveSelf(ecs_iter_t *it) {
    Self *s = ecs_term(it, Self, 1);
    ecs_id_t ecs_id(Self) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_record_t *r = ecs_record_find(it->world, it->entities[i]);
        printf("%d: record = %d, table = %p [%s]\n", it->entities[i], r->row, r->table,
            ecs_type_str(it->world, ecs_table_get_type(r->table)));

        test_assert(s[i].value == it->entities[i]);
        
        const Self *ptr = ecs_get(it->world, it->entities[i], Self);
        test_assert(ptr != NULL);
        test_assert(ptr->value == it->entities[i]);

        s[i].value = 0;
        on_remove_count ++;
    }
}

void Trigger_on_remove_tree() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Self);

    ECS_TRIGGER(world, RemoveSelf, EcsOnRemove, Self);

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
