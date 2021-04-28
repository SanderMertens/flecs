#include <api.h>

void Trigger(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->param);
}

void TriggerAdd(ecs_iter_t *it) {
    ecs_id_t id = *(ecs_id_t*)it->param;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(it->world, it->entities[i], id);
    }
}

void TriggerRemove(ecs_iter_t *it) {
    ecs_id_t id = *(ecs_id_t*)it->param;

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
