#include <addons.h>
#include <stdio.h>

void probe_system_w_ctx(
    ecs_iter_t *it,
    Probe *ctx) 
{
    if (!ctx) {
        return;
    }

    ctx->param = it->param;
    ctx->system = it->system;
    ctx->event = it->event;
    ctx->event_id = it->event_id;
    ctx->offset = 0;
    ctx->term_count = it->field_count;
    ctx->term_index = it->term_index;

    int i;
    for (i = 0; i < ctx->term_count; i ++) {
        ctx->c[ctx->invoked][i] = it->ids[i];
        ctx->s[ctx->invoked][i] = ecs_field_src(it, i + 1);

        ecs_id_t e = ecs_field_id(it, i + 1);
        test_assert(e != 0);
    }

    if (it->entities) {
        ecs_entity_t *e = ecs_field(it, ecs_entity_t, 0);
        if (e) {
            test_assert(e != NULL);
            test_assert(it->entities != NULL);
            test_assert(it->entities == e);
            
            for (i = 0; i < it->count; i ++) {
                if (i + ctx->count < 256) {
                    ctx->e[i + ctx->count] = e[i];
                } else {
                    /* can't store more than that, tests shouldn't rely on
                     * getting back more than 256 results */
                }
            }
            ctx->count += it->count;
        }
    }

    ctx->invoked ++;
}

void probe_iter(
    ecs_iter_t *it) 
{
    Probe *ctx = ecs_get_context(it->world);
    if (!ctx) {
        ctx = it->ctx;
    }
    probe_system_w_ctx(it, ctx);
}

void probe_has_entity(Probe *probe, ecs_entity_t e) {
    int i;
    for (i = 0; i < probe->count; i ++) {
        if (probe->e[i] == e) {
            break;
        }
    }

    test_assert(i != probe->count);
}

void install_test_abort() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);
    ecs_log_set_level(-5);
}

const ecs_entity_t* bulk_new_w_type(
    ecs_world_t *world, ecs_entity_t type_ent, int32_t count) 
{
    const ecs_type_t *type = ecs_get_type(world, type_ent);
    test_assert(type != NULL);

    ecs_id_t *ids = type->array;
    int i = 0;
    while ((ecs_id_get_flags(world, ids[i]) & EcsIdDontInherit)) {
        i ++;
    }
    const ecs_entity_t *result = ecs_bulk_new_w_id(world, ids[i], count);
    for (; i < type->count; i ++) {
        for (int e = 0; e < count; e ++) {
            if (ecs_id_get_flags(world, ids[i]) & EcsIdDontInherit) {
                continue;
            }
            ecs_add_id(world, result[e], ids[i]);
        }
    }
    
    return result;
}

int32_t find_entity(
    ecs_world_t *world,
    test_iter_result_t *expect, 
    ecs_entity_t e)
{
    int i;
    for (i = 0; i < ITER_MAX_ENTITIES; i ++) {
        if (expect->entities[i] == e) {
            while (expect->matched[i]) {
                i ++;
                if (!if_test_assert(e == expect->entities[i])) {
                    return -1;
                }
            }

            if (expect->entity_names[i]) {
                if (!if_test_str(ecs_get_name(world, e), expect->entity_names[i])) {
                    return -1;
                }
            }
            return i;
        }
    }

    for (i = 0; i < ITER_MAX_ENTITIES; i ++) {
        if (!expect->entity_names[i]) {
            break;
        }

        if (!strcmp(ecs_get_name(world, e), expect->entity_names[i])) {
            while (expect->matched[i]) {
                i ++;

                // If this fails, the entity is encountered more times than
                // expected.
                if (!if_test_str(ecs_get_name(world, e), 
                    expect->entity_names[i])) 
                {
                    return -1;
                }
            }

            return i;
        }
    }

    return -1;
}

bool test_iter(
    ecs_iter_t *it, 
    ecs_iter_next_action_t next, 
    test_iter_result_t *expect) 
{
    int32_t entity_index = -1;

    while (next(it)) {
        int i;

        for (i = 0; (i < it->count) || (i < 1); i ++) {
            ecs_entity_t e = 0;
            int t;

            if (it->count) {
                e = it->entities[i];

                entity_index = find_entity(it->world, expect, e);

                // Matched unexpected entity
                test_assert(entity_index != -1);

                expect->matched[entity_index] = true;

                // Test data
                for (t = 0; t < it->field_count; t++) {
                    size_t size = ecs_field_size(it, t + 1);
                    if (!size) {
                        continue;
                    }

                    void *expect_ptr = expect->term_columns[t];
                    if (!expect_ptr) {
                        continue;
                    }

                    expect_ptr = ECS_OFFSET(expect_ptr, size * entity_index);

                    void *component_ptr = ecs_field_w_size(it, size, t + 1);
                    if (!if_test_assert(component_ptr != NULL)) {
                        return false;
                    }

                    component_ptr = ECS_OFFSET(component_ptr, size * i);
                    if (!if_test_assert(memcpy(component_ptr, expect_ptr, size))) {
                        return false;
                    }
                }                
            } else {
                entity_index ++;
            }


            // Test ids
            ecs_id_t *ids = expect->term_ids[entity_index];
            if (!ids[0]) {
                ids = expect->term_ids[0];
            }

            for (t = 0; t < it->field_count; t++) {
                if (!ids[t]) {
                    break;
                }

                if (!if_test_assert(ecs_field_id(it, t + 1) == ids[t])) {
                    return false;
                }
            }

            if (!if_test_assert(ids[t] == 0)) {
                return false;
            }


            // Test ids by expr
            char **ids_expect = expect->term_ids_expr[entity_index];
            if (!ids_expect) {
                ids_expect = expect->term_ids_expr[0];
            }

            for (t = 0; t < it->field_count; t++) {
                if (!ids_expect[t]) {
                    break;
                }

                char *id_found = ecs_id_str(it->world, ecs_field_id(it, t + 1));
                if (!if_test_str(id_found, ids_expect[t])) {
                    printf(" - term %d\n", t);
                    if (e) {
                        printf(" - matched entity %u (%s, [%s])\n", 
                            (uint32_t)e,
                            ecs_get_name(it->world, e),
                            ecs_type_str(it->world, ecs_get_type(it->world, e)));

                        if (expect->entities[i]) {
                            printf(" - expected entity %u (%s)\n", 
                                (uint32_t)expect->entities[i], 
                                ecs_get_name(it->world, expect->entities[i]));
                        } else if (expect->entity_names[i]) {
                            printf(" - expected entity %s\n", 
                                expect->entity_names[i]);
                        }
                    }

                    printf(" - @ result index %d\n", entity_index);
                    return false;
                }
                ecs_os_free(id_found);
            }

            if (!if_test_assert(ids_expect[t] == NULL)) {
                return false;
            }


            // Test variables
            int v;
            for (v = 0; v < ITER_MAX_VARIABLES; v++) {
                int32_t id = expect->variables[v].id;
                if (!id) {
                    break;
                }
                
                ecs_entity_t e = expect->variables[v].entities[entity_index];
                if (!e) {
                    e = expect->variables[v].entities[0];
                }
                if (e) {
                    ecs_entity_t var = ecs_iter_get_var(it, id);
                    if (!if_test_assert(e == var)) {
                        return false;
                    }
                }

                const char *name = expect->variables[v].entity_names[entity_index];
                if (!name) {
                    name = expect->variables[v].entity_names[0];
                }
                if (name) {
                    ecs_entity_t var = ecs_iter_get_var(it, id);
                    if (!if_test_str(name, ecs_get_name(it->world, var))) {
                        printf(" - variable id %d\n", id);
                        printf(" - index %d\n", entity_index);
                        return false;
                    }                    
                }

                /* If a variable id is set, either an entity or entity name must
                 * be set. */
                if (!if_test_assert(e || name)) {
                    return false;
                }
            }
        }

        expect->table_count_actual ++;
    }

    for (int i = 0; i < ITER_MAX_ENTITIES; i ++) {
        if (expect->entities[i] || expect->entity_names[i]) {
            if (!if_test_assert(expect->matched[i])) {
                printf(" - entity %u (%s) at index %d not matched\n",
                    (uint32_t)expect->entities[i], expect->entity_names[i], i);
                return false;
            }
        }
    }

    if (expect->table_count_expect) {
        if (!if_test_assert(expect->table_count_actual == expect->table_count_expect)) {
            return false;
        }
    }

    return true;
}
