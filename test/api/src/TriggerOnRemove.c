#include <api.h>

void Deinit(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);

    Velocity *v = NULL;
    if (it->field_count >= 2) {
        v = ecs_field(it, Velocity, 2);
    }

    probe_iter(it);

    /* Write to validate columns point to valid memory */
    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x = 0;
        p[i].y = 0;

        if (v) {
            v[i].x = 0;
            v[i].y = 0;
        }
    }
}

static
void Remove_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (ctx->component) {
            ecs_remove_id(it->world, e, ctx->component);
        }

        if (ctx->component_2) {
            ecs_remove_id(it->world, e, ctx->component_2);
        }

        ctx->entity_count ++;
    }
}

void TriggerOnRemove_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.count, 1);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void TriggerOnRemove_remove_no_match() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Velocity);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnRemove_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 1);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void TriggerOnRemove_delete_no_match() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Velocity);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 0); 

    ecs_fini(world);
}

static Position old_position = {0};

static
void RemovePosition(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);

    test_assert(it->count == 1);

    old_position = p[0];
}

void TriggerOnRemove_remove_watched() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ECS_OBSERVER(world, RemovePosition, EcsOnRemove, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);

    /* Make parent entity watched */
    ecs_new_w_pair(world, EcsChildOf, e);

    ecs_remove(world, e, Position);

    test_int(old_position.x, 10);
    test_int(old_position.y, 20);

    ecs_fini(world);
}


void TriggerOnRemove_delete_watched() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ECS_OBSERVER(world, RemovePosition, EcsOnRemove, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);

    /* Make parent entity watched */
    ecs_new_w_pair(world, EcsChildOf, e);

    ecs_delete(world, e);

    test_int(old_position.x, 10);
    test_int(old_position.y, 20);

    ecs_fini(world);
}

static bool dummy_called = false;

static
void Dummy(ecs_iter_t *it) {
    dummy_called = true;
}

void TriggerOnRemove_on_remove_in_on_update() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Remove_from_current, EcsOnUpdate, Position);
    ECS_OBSERVER(world, Dummy, EcsOnRemove, Velocity);

    IterData ctx = {.component = ecs_id(Velocity)};
    ecs_set_context(world, &ctx);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( !ecs_has(world, e1, Velocity));
    test_assert( !ecs_has(world, e2, Velocity));
    test_assert( !ecs_has(world, e3, Velocity));

    test_assert(dummy_called);

    ecs_fini(world);
}

static int dummy_dtor_invoked = 0;

typedef struct DummyComp {
    int value;
} DummyComp;

static
void RemoveDummyComp(ecs_iter_t *it) { 
    int i;
    for (i = 0; i < it->count; i ++) {
        test_assert(ecs_is_valid(it->world, it->entities[i]));
        test_assert(ecs_is_alive(it->world, it->entities[i]));
    }

    dummy_dtor_invoked ++;
}

void TriggerOnRemove_valid_entity_after_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, DummyComp);
    ECS_OBSERVER(world, RemoveDummyComp, EcsOnRemove, DummyComp);

    ecs_entity_t e = ecs_new(world, DummyComp);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    test_int(dummy_dtor_invoked, 1);

    ecs_fini(world); 
}

void TriggerOnRemove_remove_after_delete_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t trigger = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .events = {EcsOnRemove},
        .callback = Dummy
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));
    test_int(dummy_called, 0);

    ecs_remove(world, e1, Position);
    test_int(dummy_called, 1);

    dummy_called = 0;

    ecs_delete(world, trigger);
    test_int(dummy_called, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));
    test_int(dummy_called, 0);

    ecs_remove(world, e2, Position);
    test_int(dummy_called, 0);

    ecs_fini(world);
}

void TriggerOnRemove_remove_after_delete_wildcard_id_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t trigger = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = EcsWildcard,
        .events = {EcsOnRemove},
        .callback = Dummy
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));
    test_int(dummy_called, 0);

    ecs_remove(world, e1, Position);
    test_int(dummy_called, 1);

    dummy_called = 0;

    ecs_delete(world, trigger);
    
    test_int(dummy_called, 1);

    dummy_called = 0;

    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));
    test_int(dummy_called, 0);

    ecs_remove(world, e2, Position);
    test_int(dummy_called, 0);

    ecs_fini(world);
}

typedef struct on_remove_has_tag_t {
    ecs_entity_t ent;
    ecs_entity_t tag;
} on_remove_has_tag_t;

static
void OnRemoveHasTag(ecs_iter_t *it) {
    on_remove_has_tag_t *ctx = it->ctx;
    test_assert(ctx != NULL);

    test_int(it->count, 1);
    test_assert(it->entities[0] == ctx->ent);
    test_assert(ecs_field_id(it, 1) == ctx->tag);
    test_bool(ecs_has_id(it->world, ctx->ent, ctx->tag), true);

    dummy_called = true;
}

void TriggerOnRemove_has_removed_tag_trigger_1_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world, Tag);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));

    on_remove_has_tag_t ctx = {
        .ent = e,
        .tag = Tag
    };

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = OnRemoveHasTag,
        .ctx = &ctx
    });

    ecs_remove(world, e, Tag);

    test_int(dummy_called, 1);

    ecs_fini(world);
}

void TriggerOnRemove_has_removed_tag_trigger_2_tags() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, TagA));

    ecs_add(world, e, TagB);
    test_assert(ecs_has(world, e, TagB));

    on_remove_has_tag_t ctx = {
        .ent = e,
        .tag = TagA
    };

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .events = {EcsOnRemove},
        .callback = OnRemoveHasTag,
        .ctx = &ctx
    });

    ecs_remove(world, e, TagA);

    test_int(dummy_called, 1);

    ecs_fini(world);
}
