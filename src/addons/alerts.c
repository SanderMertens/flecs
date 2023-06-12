/**
 * @file addons/alerts.c
 * @brief Alerts addon.
 */

#include "../private_api.h"

#ifdef FLECS_ALERTS

ECS_COMPONENT_DECLARE(FlecsAlerts);

typedef struct EcsAlert {
    char *message;
    ecs_map_t instances; /* Active instances for metric */
} EcsAlert;

static
ECS_CTOR(EcsAlert, ptr, {
    ptr->message = NULL;
    ecs_map_init(&ptr->instances, NULL);
})

static
ECS_DTOR(EcsAlert, ptr, {
    ecs_os_free(ptr->message);
    ecs_map_fini(&ptr->instances);
})

static
ECS_MOVE(EcsAlert, dst, src, {
    ecs_os_free(dst->message);
    dst->message = src->message;
    src->message = NULL;

    ecs_map_fini(&dst->instances);
    dst->instances = src->instances;
    src->instances = (ecs_map_t){0};
})

static
ECS_CTOR(EcsAlertsActive, ptr, {
    ecs_map_init(&ptr->alerts, NULL);
})

static
ECS_DTOR(EcsAlertsActive, ptr, {
    ecs_map_fini(&ptr->alerts);
})

static
ECS_MOVE(EcsAlertsActive, dst, src, {
    ecs_map_fini(&dst->alerts);
    dst->alerts = src->alerts;
    src->alerts = (ecs_map_t){0};
})

static
void flecs_alerts_add_alert_to_src(
    ecs_world_t *world,
    ecs_entity_t source,
    ecs_entity_t alert,
    ecs_entity_t alert_instance)
{
    EcsAlertsActive *active = ecs_get_mut(
        world, source, EcsAlertsActive);
    ecs_assert(active != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t *ptr = ecs_map_ensure(&active->alerts, alert);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ptr[0] = alert_instance;
    ecs_modified(world, source, EcsAlertsActive);
}

static
void flecs_alerts_remove_alert_from_src(
    ecs_world_t *world,
    ecs_entity_t source,
    ecs_entity_t alert)
{
    EcsAlertsActive *active = ecs_get_mut(
        world, source, EcsAlertsActive);
    ecs_assert(active != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_map_remove(&active->alerts, alert);

    if (!ecs_map_count(&active->alerts)) {
        ecs_remove(world, source, EcsAlertsActive);
    } else {
        ecs_modified(world, source, EcsAlertsActive);
    }
}

static
void MonitorAlerts(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;
    EcsAlert *alert = ecs_field(it, EcsAlert, 1);
    EcsPoly *poly = ecs_field(it, EcsPoly, 2);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t a = it->entities[i]; /* Alert entity */
        ecs_rule_t *rule = poly[i].poly;
        ecs_poly_assert(rule, ecs_rule_t);

        ecs_iter_t rit = ecs_rule_iter(world, rule);
        rit.flags |= EcsIterNoData;
        rit.flags |= EcsIterIsInstanced;

        while (ecs_rule_next(&rit)) {
            int32_t j, alert_src_count = rit.count;
            for (j = 0; j < alert_src_count; j ++) {
                ecs_entity_t e = rit.entities[j];
                ecs_entity_t *aptr = ecs_map_ensure(&alert[i].instances, e);
                ecs_assert(aptr != NULL, ECS_INTERNAL_ERROR, NULL);
                if (!aptr[0]) {
                    /* Alert does not yet exist for entity */
                    ecs_entity_t ai = ecs_new_w_pair(world, EcsChildOf, a);
                    ecs_set(world, ai, EcsAlertInstance, { .message = NULL });
                    ecs_set(world, ai, EcsMetricSource, { .entity = e });
                    ecs_set(world, ai, EcsMetricValue, { .value = 0 });
                    // ecs_doc_set_color(world, ai, "#b5494b");
                    ecs_defer_suspend(it->world);
                    flecs_alerts_add_alert_to_src(world, e, a, ai);
                    ecs_defer_resume(it->world);
                    aptr[0] = ai;
                }
            }
        }
    }
}

static
void MonitorAlertInstances(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;
    EcsAlertInstance *alert_instance = ecs_field(it, EcsAlertInstance, 1);
    EcsMetricSource *source = ecs_field(it, EcsMetricSource, 2);
    EcsMetricValue *value = ecs_field(it, EcsMetricValue, 3);

    /* Get alert component from alert instance parent (the alert) */
    ecs_id_t childof_pair;
    if (ecs_search(world, it->table, ecs_childof(EcsWildcard), &childof_pair) == -1) {
        ecs_err("alert instances must be a child of an alert");
        return;
    }
    ecs_entity_t parent = ecs_pair_second(world, childof_pair);
    ecs_assert(parent != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_has(world, parent, EcsAlert), ECS_INVALID_OPERATION,
        "alert entity does not have Alert component");
    EcsAlert *alert = ecs_get_mut(world, parent, EcsAlert);
    const EcsPoly *poly = ecs_get_pair(world, parent, EcsPoly, EcsQuery);
    ecs_assert(poly != NULL, ECS_INVALID_OPERATION, 
        "alert entity does not have (Poly, Query) component");
    ecs_rule_t *rule = poly->poly;
    ecs_poly_assert(rule, ecs_rule_t);

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t ai = it->entities[i];
        ecs_entity_t e = source[i].entity;

        value[i].value += (double)it->delta_system_time;

        /* Check if alert instance still matches rule */
        ecs_iter_t rit = ecs_rule_iter(world, rule);
        rit.flags |= EcsIterNoData;
        rit.flags |= EcsIterIsInstanced;
        ecs_iter_set_var(&rit, 0, e);

        if (ecs_rule_next(&rit)) {
            bool generate_message = alert->message;
            if (generate_message) {
                if (alert_instance[i].message) {
                    /* If a message was already generated, only regenerate if
                     * rule has multiple variables. Variable values could have 
                     * changed, this ensures the message remains up to date. */
                    generate_message = rit.variable_count > 1;
                }
            }

            if (generate_message) {
                if (alert_instance[i].message) {
                    ecs_os_free(alert_instance[i].message);
                }

                ecs_iter_to_vars(&rit, &vars, 0);
                alert_instance[i].message = ecs_interpolate_string(
                    world, alert->message, &vars);
            }

            /* Alert instance still matches rule, keep it alive */
            ecs_iter_fini(&rit);
            continue;
        }

        /* Alert instance no longer matches rule, remove it */
        flecs_alerts_remove_alert_from_src(world, e, parent);
        ecs_map_remove(&alert->instances, e);
        ecs_delete(world, ai);
    }

    ecs_vars_fini(&vars);
}

ecs_entity_t ecs_alert_init(
    ecs_world_t *world,
    const ecs_alert_desc_t *desc)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!desc->filter.entity || desc->entity == desc->filter.entity, 
        ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = desc->entity;
    if (!result) {
        result = ecs_new(world, 0);
    }

    ecs_filter_desc_t private_desc = desc->filter;
    private_desc.entity = result;

    ecs_rule_t *rule = ecs_rule_init(world, &private_desc);
    if (!rule) {
        return 0;
    }

    const ecs_filter_t *filter = ecs_rule_get_filter(rule);
    if (!(filter->flags & EcsFilterMatchThis)) {
        ecs_err("alert filter must have at least one '$this' term");
        ecs_rule_fini(rule);
        return 0;
    }

    /* Initialize Alert component which identifiers entity as alert */
    EcsAlert *alert = ecs_get_mut(world, result, EcsAlert);
    ecs_assert(alert != NULL, ECS_INTERNAL_ERROR, NULL);
    alert->message = ecs_os_strdup(desc->message);
    ecs_modified(world, result, EcsAlert);

    /* Register alert as metric */
    ecs_add(world, result, EcsMetric);
    ecs_add_pair(world, result, EcsMetric, EcsCounter);

    /* Add severity to alert */
    ecs_entity_t severity = desc->severity;
    if (!severity) {
        severity = EcsAlertError;
    }

    ecs_add_pair(world, result, ecs_id(EcsAlert), severity);

    if (desc->brief) {
#ifdef FLECS_DOC
        ecs_doc_set_brief(world, result, desc->brief);
#else
        ecs_err("cannot set brief for alert, requires FLECS_DOC addon");
        goto error;
#endif
    }

    return result;
error:
    return 0;
}

int32_t ecs_get_alert_count(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t alert)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!alert || ecs_has(world, alert, EcsAlert), 
        ECS_INVALID_PARAMETER, NULL);

    const EcsAlertsActive *active = ecs_get(world, entity, EcsAlertsActive);
    if (!active) {
        return 0;
    }

    if (alert) {
        return ecs_map_get(&active->alerts, alert) != NULL;
    }

    return ecs_map_count(&active->alerts);
error:
    return 0;
}

ecs_entity_t ecs_get_alert(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t alert)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(alert != 0, ECS_INVALID_PARAMETER, NULL);

    const EcsAlertsActive *active = ecs_get(world, entity, EcsAlertsActive);
    if (!active) {
        return 0;
    }

    ecs_entity_t *ptr = ecs_map_get(&active->alerts, alert);
    if (ptr) {
        return ptr[0];
    }

error:
    return 0;
}

void FlecsAlertsImport(ecs_world_t *world) {
    ECS_MODULE_DEFINE(world, FlecsAlerts);

    ECS_IMPORT(world, FlecsPipeline);
    ECS_IMPORT(world, FlecsTimer);
    ECS_IMPORT(world, FlecsMetrics);
#ifdef FLECS_DOC
    ECS_IMPORT(world, FlecsDoc);
#endif

    ecs_set_name_prefix(world, "Ecs");
    ECS_COMPONENT_DEFINE(world, EcsAlert);
    ecs_remove_pair(world, ecs_id(EcsAlert), ecs_id(EcsIdentifier), EcsSymbol);

    ecs_set_name_prefix(world, "EcsAlert");
    ECS_COMPONENT_DEFINE(world, EcsAlertInstance);
    ECS_COMPONENT_DEFINE(world, EcsAlertsActive);

    ECS_TAG_DEFINE(world, EcsAlertInfo);
    ECS_TAG_DEFINE(world, EcsAlertWarning);
    ECS_TAG_DEFINE(world, EcsAlertError);
    ECS_TAG_DEFINE(world, EcsAlertCritical);

    ecs_add_id(world, ecs_id(EcsAlertsActive), EcsPrivate);

    ecs_struct(world, {
        .entity = ecs_id(EcsAlertInstance),
        .members = {
            { .name = "message", .type = ecs_id(ecs_string_t) }
        }
    });

    ecs_set_hooks(world, EcsAlert, {
        .ctor = ecs_ctor(EcsAlert),
        .dtor = ecs_dtor(EcsAlert),
        .move = ecs_move(EcsAlert)
    });

    ecs_set_hooks(world, EcsAlertsActive, {
        .ctor = ecs_ctor(EcsAlertsActive),
        .dtor = ecs_dtor(EcsAlertsActive),
        .move = ecs_move(EcsAlertsActive)
    });

    ECS_SYSTEM(world, MonitorAlerts, EcsPreStore, Alert, (Poly, Query));
    ECS_SYSTEM(world, MonitorAlertInstances, EcsOnStore, Instance, 
        flecs.metrics.Source, flecs.metrics.Value);

    ecs_system(world, {
        .entity = ecs_id(MonitorAlerts),
        .no_readonly = true,
        .interval = 1.0
    });

    ecs_system(world, {
        .entity = ecs_id(MonitorAlertInstances),
        .interval = 1.0
    });
}

#endif
