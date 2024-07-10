/**
 * @file addons/alerts.c
 * @brief Alerts addon.
 */

#include "../private_api.h"

#ifdef FLECS_ALERTS

ECS_COMPONENT_DECLARE(FlecsAlerts);

typedef struct EcsAlert {
    char *message;
    ecs_map_t instances;        /* Active instances for metric */
    ecs_ftime_t retain_period;  /* How long to retain the alert */
    ecs_vec_t severity_filters; /* Severity filters */
    
    /* Member range monitoring */
    ecs_id_t id;                /* (Component) id that contains to monitor member */
    ecs_entity_t member;        /* Member to monitor */
    int32_t offset;             /* Offset of member in component */
    int32_t size;               /* Size of component */
    ecs_primitive_kind_t kind;  /* Primitive type kind */
    ecs_ref_t ranges;           /* Reference to ranges component */
    int32_t var_id;             /* Variable from which to obtain data (0 = $this) */
} EcsAlert;

typedef struct EcsAlertTimeout {
    ecs_ftime_t inactive_time; /* Time the alert has been inactive */
    ecs_ftime_t expire_time;   /* Expiration duration */
} EcsAlertTimeout;

ECS_COMPONENT_DECLARE(EcsAlertTimeout);

static
ECS_CTOR(EcsAlert, ptr, {
    ecs_os_zeromem(ptr);
    ecs_map_init(&ptr->instances, NULL);
    ecs_vec_init_t(NULL, &ptr->severity_filters, ecs_alert_severity_filter_t, 0);
})

static
ECS_DTOR(EcsAlert, ptr, {
    ecs_os_free(ptr->message);
    ecs_map_fini(&ptr->instances);
    ecs_vec_fini_t(NULL, &ptr->severity_filters, ecs_alert_severity_filter_t);
})

static
ECS_MOVE(EcsAlert, dst, src, {
    ecs_os_free(dst->message);
    dst->message = src->message;
    src->message = NULL;

    ecs_map_fini(&dst->instances);
    dst->instances = src->instances;
    src->instances = (ecs_map_t){0};

    ecs_vec_fini_t(NULL, &dst->severity_filters, ecs_alert_severity_filter_t);
    dst->severity_filters = src->severity_filters;
    src->severity_filters = (ecs_vec_t){0};

    dst->retain_period = src->retain_period;
    dst->id = src->id;
    dst->member = src->member;
    dst->offset = src->offset;
    dst->size = src->size;
    dst->kind = src->kind;
    dst->ranges = src->ranges;
    dst->var_id = src->var_id;
})

static
ECS_CTOR(EcsAlertsActive, ptr, {
    ecs_map_init(&ptr->alerts, NULL);
    ptr->info_count = 0;
    ptr->warning_count = 0;
    ptr->error_count = 0;
})

static
ECS_DTOR(EcsAlertsActive, ptr, {
    ecs_map_fini(&ptr->alerts);
})

static
ECS_MOVE(EcsAlertsActive, dst, src, {
    ecs_map_fini(&dst->alerts);
    dst->alerts = src->alerts;
    dst->info_count = src->info_count;
    dst->warning_count = src->warning_count;
    dst->error_count = src->error_count;
    src->alerts = (ecs_map_t){0};
})

static
ECS_DTOR(EcsAlertInstance, ptr, {
    ecs_os_free(ptr->message);
})

static
ECS_MOVE(EcsAlertInstance, dst, src, {
    ecs_os_free(dst->message);
    dst->message = src->message;
    src->message = NULL;
})

static
ECS_COPY(EcsAlertInstance, dst, src, {
    ecs_os_free(dst->message);
    dst->message = ecs_os_strdup(src->message);
})

static
void flecs_alerts_add_alert_to_src(
    ecs_world_t *world,
    ecs_entity_t source,
    ecs_entity_t alert,
    ecs_entity_t alert_instance)
{
    EcsAlertsActive *active = ecs_ensure(
        world, source, EcsAlertsActive);
    ecs_assert(active != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t severity = ecs_get_target(world, alert, ecs_id(EcsAlert), 0);
    if (severity == EcsAlertInfo) {
        active->info_count ++;
    } else if (severity == EcsAlertWarning) {
        active->warning_count ++;
    } else if (severity == EcsAlertError) {
        active->error_count ++;
    }

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
    EcsAlertsActive *active = ecs_ensure(
        world, source, EcsAlertsActive);
    ecs_assert(active != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_map_remove(&active->alerts, alert);

    ecs_entity_t severity = ecs_get_target(world, alert, ecs_id(EcsAlert), 0);
    if (severity == EcsAlertInfo) {
        active->info_count --;
    } else if (severity == EcsAlertWarning) {
        active->warning_count --;
    } else if (severity == EcsAlertError) {
        active->error_count --;
    }

    if (!ecs_map_count(&active->alerts)) {
        ecs_remove(world, source, EcsAlertsActive);
    } else {
        ecs_modified(world, source, EcsAlertsActive);
    }
}

static
ecs_entity_t flecs_alert_get_severity(
    ecs_world_t *world,
    ecs_iter_t *it,
    EcsAlert *alert)
{
    int32_t i, filter_count = ecs_vec_count(&alert->severity_filters);
    ecs_alert_severity_filter_t *filters = 
        ecs_vec_first(&alert->severity_filters);
    for (i = 0; i < filter_count; i ++) {
        ecs_alert_severity_filter_t *filter = &filters[i];
        if (!filter->var) {
            if (ecs_table_has_id(world, it->table, filters[i].with)) {
                return filters[i].severity;
            }
        } else {
            ecs_entity_t src = ecs_iter_get_var(it, filter->_var_index);
            if (src && src != EcsWildcard) {
                if (ecs_has_id(world, src, filters[i].with)) {
                    return filters[i].severity;
                }
            }
        }
    }

    return 0;
}

static
ecs_entity_t flecs_alert_out_of_range_kind(
    EcsAlert *alert,
    const EcsMemberRanges *ranges,
    const void *value_ptr)
{
    double value = 0;

    switch(alert->kind) {
    case EcsU8: value = *(const uint8_t*)value_ptr; break;
    case EcsU16: value = *(const uint16_t*)value_ptr; break;
    case EcsU32: value = *(const uint32_t*)value_ptr; break;
    case EcsU64: value = (double)*(const uint64_t*)value_ptr; break;
    case EcsI8: value = *(const int8_t*)value_ptr; break;
    case EcsI16: value = *(const int16_t*)value_ptr; break;
    case EcsI32: value = *(const int32_t*)value_ptr; break;
    case EcsI64: value = (double)*(const int64_t*)value_ptr; break;
    case EcsF32: value = (double)*(const float*)value_ptr; break;
    case EcsF64: value = *(const double*)value_ptr; break;
    case EcsBool:
    case EcsChar:
    case EcsByte:
    case EcsUPtr:
    case EcsIPtr:
    case EcsString:
    case EcsEntity:
    case EcsId:
        return 0;
    }

    bool has_error = ECS_NEQ(ranges->error.min, ranges->error.max);
    bool has_warning = ECS_NEQ(ranges->warning.min, ranges->warning.max);

    if (has_error && (value < ranges->error.min || value > ranges->error.max)) {
        return EcsAlertError;
    } else if (has_warning && 
        (value < ranges->warning.min || value > ranges->warning.max)) 
    {
        return EcsAlertWarning;
    } else {
        return 0;
    }
}

static
void MonitorAlerts(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;
    EcsAlert *alert = ecs_field(it, EcsAlert, 0);
    EcsPoly *poly = ecs_field(it, EcsPoly, 1);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t a = it->entities[i]; /* Alert entity */
        ecs_entity_t default_severity = ecs_get_target(
            world, a, ecs_id(EcsAlert), 0);
        ecs_query_t *q = poly[i].poly;
        if (!q) {
            continue;
        }

        flecs_poly_assert(q, ecs_query_t);

        ecs_id_t member_id = alert[i].id;
        const EcsMemberRanges *ranges = NULL;
        if (member_id) {
            ranges = ecs_ref_get(world, &alert[i].ranges, EcsMemberRanges);
        }

        ecs_iter_t rit = ecs_query_iter(world, q);
        rit.flags |= EcsIterNoData;
        rit.flags |= EcsIterIsInstanced;

        while (ecs_query_next(&rit)) {
            ecs_entity_t severity = flecs_alert_get_severity(
                world, &rit, &alert[i]);
            if (!severity) {
                severity = default_severity;
            }

            const void *member_data = NULL;
            ecs_entity_t member_src = 0;
            if (ranges) {
                if (alert[i].var_id) {
                    member_src = ecs_iter_get_var(&rit, alert[i].var_id);
                    if (!member_src || member_src == EcsWildcard) {
                        continue;
                    }
                }
                if (!member_src) {
                    member_data = ecs_table_get_id(
                        world, rit.table, member_id, rit.offset);
                } else {
                    member_data = ecs_get_id(world, member_src, member_id);
                }
                if (!member_data) {
                    continue;
                }
                member_data = ECS_OFFSET(member_data, alert[i].offset);
            }

            int32_t j, alert_src_count = rit.count;
            for (j = 0; j < alert_src_count; j ++) {
                ecs_entity_t src_severity = severity;
                ecs_entity_t e = rit.entities[j];
                if (member_data) {
                    ecs_entity_t range_severity = flecs_alert_out_of_range_kind(
                        &alert[i], ranges, member_data);
                    if (!member_src) {
                        member_data = ECS_OFFSET(member_data, alert[i].size);
                    }
                    if (!range_severity) {
                        continue;
                    }
                    if (range_severity < src_severity) {
                        /* Range severity should not exceed alert severity */
                        src_severity = range_severity;
                    }
                }

                ecs_entity_t *aptr = ecs_map_ensure(&alert[i].instances, e);
                ecs_assert(aptr != NULL, ECS_INTERNAL_ERROR, NULL);
                if (!aptr[0]) {
                    /* Alert does not yet exist for entity */
                    ecs_entity_t ai = ecs_new_w_pair(world, EcsChildOf, a);
                    ecs_set(world, ai, EcsAlertInstance, { .message = NULL });
                    ecs_set(world, ai, EcsMetricSource, { .entity = e });
                    ecs_set(world, ai, EcsMetricValue, { .value = 0 });
                    ecs_add_pair(world, ai, ecs_id(EcsAlert), src_severity);
                    if (ECS_NEQZERO(alert[i].retain_period)) {
                        ecs_set(world, ai, EcsAlertTimeout, {
                            .inactive_time = 0,
                            .expire_time = alert[i].retain_period
                        });
                    }

                    ecs_defer_suspend(it->world);
                    flecs_alerts_add_alert_to_src(world, e, a, ai);
                    ecs_defer_resume(it->world);
                    aptr[0] = ai;
                } else {
                    /* Make sure alert severity is up to date */
                    if (ecs_vec_count(&alert[i].severity_filters) || member_data) {
                        ecs_entity_t cur_severity = ecs_get_target(
                            world, aptr[0], ecs_id(EcsAlert), 0);
                        if (cur_severity != src_severity) {
                            ecs_add_pair(world, aptr[0], ecs_id(EcsAlert), 
                                src_severity);
                        }
                    }
                }
            }
        }
    }
}

static
void MonitorAlertInstances(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;
    EcsAlertInstance *alert_instance = ecs_field(it, EcsAlertInstance, 0);
    EcsMetricSource *source = ecs_field(it, EcsMetricSource, 1);
    EcsMetricValue *value = ecs_field(it, EcsMetricValue, 2);
    EcsAlertTimeout *timeout = ecs_field(it, EcsAlertTimeout, 3);

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

    EcsAlert *alert = ecs_ensure(world, parent, EcsAlert);
    const EcsPoly *poly = ecs_get_pair(world, parent, EcsPoly, EcsQuery);
    ecs_assert(poly != NULL, ECS_INVALID_OPERATION, 
        "alert entity does not have (Poly, Query) component");

    ecs_query_t *query = poly->poly;
    if (!query) {
        return;
    }

    flecs_poly_assert(query, ecs_query_t);

    ecs_id_t member_id = alert->id;
    const EcsMemberRanges *ranges = NULL;
    if (member_id) {
        ranges = ecs_ref_get(world, &alert->ranges, EcsMemberRanges);
    }

    ecs_script_vars_t *vars = ecs_script_vars_init(it->world);
    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t ai = it->entities[i];
        ecs_entity_t e = source[i].entity;

        /* If source of alert is no longer alive, delete alert instance even if
         * the alert has a retain period. */
        if (!ecs_is_alive(world, e)) {
            ecs_delete(world, ai);
            continue;
        }

        /* Check if alert instance still matches query */
        ecs_iter_t rit = ecs_query_iter(world, query);
        rit.flags |= EcsIterNoData;
        rit.flags |= EcsIterIsInstanced;
        ecs_iter_set_var(&rit, 0, e);

        if (ecs_query_next(&rit)) {
            bool match = true;

            /* If alert is monitoring member range, test value against range */
            if (ranges) {
                ecs_entity_t member_src = e;
                if (alert->var_id) {
                    member_src = ecs_iter_get_var(&rit, alert->var_id);
                }

                const void *member_data = ecs_get_id(
                    world, member_src, member_id);
                if (!member_data) {
                    match = false;
                } else {
                    member_data = ECS_OFFSET(member_data, alert->offset);
                    if (flecs_alert_out_of_range_kind(
                        alert, ranges, member_data) == 0) 
                    {
                        match = false;
                    }
                }
            }

            if (match) {
                /* Only increase alert duration if the alert was active */
                value[i].value += (double)it->delta_system_time;

                bool generate_message = alert->message;
                if (generate_message) {
                    if (alert_instance[i].message) {
                        /* If a message was already generated, only regenerate if
                        * query has multiple variables. Variable values could have 
                        * changed, this ensures the message remains up to date. */
                        generate_message = rit.variable_count > 1;
                    }
                }

                if (generate_message) {
                    if (alert_instance[i].message) {
                        ecs_os_free(alert_instance[i].message);
                    }

                    ecs_script_vars_from_iter(&rit, vars, 0);
                    alert_instance[i].message = ecs_script_string_interpolate(
                        world, alert->message, vars);
                }

                if (timeout) {
                    if (ECS_NEQZERO(timeout[i].inactive_time)) {
                        /* The alert just became active. Remove Disabled tag */
                        flecs_alerts_add_alert_to_src(world, e, parent, ai);
                        ecs_remove_id(world, ai, EcsDisabled);
                    }
                    timeout[i].inactive_time = 0;
                }

                /* Alert instance still matches query, keep it alive */
                ecs_iter_fini(&rit);
                continue;
            }

            ecs_iter_fini(&rit);
        }

        /* Alert instance is no longer active */
        if (timeout) {
            if (ECS_EQZERO(timeout[i].inactive_time)) {
                /* The alert just became inactive. Add Disabled tag */
                flecs_alerts_remove_alert_from_src(world, e, parent);
                ecs_add_id(world, ai, EcsDisabled);
            }
            ecs_ftime_t t = timeout[i].inactive_time;
            timeout[i].inactive_time += it->delta_system_time;
            if (t < timeout[i].expire_time) {
                /* Alert instance no longer matches query, but is still
                    * within the timeout period. Keep it alive. */
                continue;
            }
        }

        /* Alert instance no longer matches query, remove it */ 
        flecs_alerts_remove_alert_from_src(world, e, parent);
        ecs_map_remove(&alert->instances, e);
        ecs_delete(world, ai);
    }

    ecs_script_vars_fini(vars);
}

ecs_entity_t ecs_alert_init(
    ecs_world_t *world,
    const ecs_alert_desc_t *desc)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_alert_desc_t was not initialized to zero");
    ecs_check(!desc->query.entity || desc->entity == desc->query.entity, 
        ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = desc->entity;
    if (!result) {
        result = ecs_new(world);
    }

    ecs_query_desc_t private_desc = desc->query;
    private_desc.entity = result;

    ecs_query_t *q = ecs_query_init(world, &private_desc);
    if (!q) {
        ecs_err("failed to create alert filter");
        return 0;
    }

    if (!(q->flags & EcsQueryMatchThis)) {
        ecs_err("alert filter must have at least one '$this' term");
        ecs_query_fini(q);
        return 0;
    }

    /* Initialize Alert component which identifiers entity as alert */
    EcsAlert *alert = ecs_ensure(world, result, EcsAlert);
    ecs_assert(alert != NULL, ECS_INTERNAL_ERROR, NULL);
    alert->message = ecs_os_strdup(desc->message);
    alert->retain_period = desc->retain_period;

    /* Initialize severity filters */
    int32_t i;
    for (i = 0; i < 4; i ++) {
        if (desc->severity_filters[i].with) {
            if (!desc->severity_filters[i].severity) {
                ecs_err("severity filter must have severity");
                goto error;
            }
            ecs_alert_severity_filter_t *sf = ecs_vec_append_t(NULL, 
                &alert->severity_filters, ecs_alert_severity_filter_t);
            *sf = desc->severity_filters[i];
            if (sf->var) {
                sf->_var_index = ecs_query_find_var(q, sf->var);
                if (sf->_var_index == -1) {
                    ecs_err("unresolved variable '%s' in alert severity filter", 
                        sf->var);
                    goto error;
                }
            }
        }
    }

    /* Fetch data for member monitoring */
    if (desc->member) {
        alert->member = desc->member;
        if (!desc->id) {
            alert->id = ecs_get_parent(world, desc->member);
            if (!alert->id) {
                ecs_err("ecs_alert_desc_t::member is not a member");
                goto error;
            }
            ecs_check(alert->id != 0, ECS_INVALID_PARAMETER, NULL);
        } else {
            alert->id = desc->id;
        }

        ecs_id_record_t *idr = flecs_id_record_ensure(world, alert->id);
        ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
        if (!idr->type_info) {
            ecs_err("ecs_alert_desc_t::id must be a component");
            goto error;
        }

        ecs_entity_t type = idr->type_info->component;
        if (type != ecs_get_parent(world, desc->member)) {
            char *type_name = ecs_get_path(world, type);
            ecs_err("member '%s' is not a member of '%s'", 
                ecs_get_name(world, desc->member), type_name);
            ecs_os_free(type_name);
            goto error;
        }

        const EcsMember *member = ecs_get(world, alert->member, EcsMember);
        if (!member) {
            ecs_err("ecs_alert_desc_t::member is not a member");
            goto error;
        }
        if (!member->type) {
            ecs_err("ecs_alert_desc_t::member must have a type");
            goto error;
        }
        
        const EcsPrimitive *pr = ecs_get(world, member->type, EcsPrimitive);
        if (!pr) {
            ecs_err("ecs_alert_desc_t::member must be of a primitive type");
            goto error;
        }

        if (!ecs_has(world, desc->member, EcsMemberRanges)) {
            ecs_err("ecs_alert_desc_t::member must have warning/error ranges");
            goto error;
        }

        int32_t var_id = 0;
        if (desc->var) {
            var_id = ecs_query_find_var(q, desc->var);
            if (var_id == -1) {
                ecs_err("unresolved variable '%s' in alert member", desc->var);
                goto error;
            }
        }

        alert->offset = member->offset;
        alert->size = idr->type_info->size;
        alert->kind = pr->kind;
        alert->ranges = ecs_ref_init(world, desc->member, EcsMemberRanges);
        alert->var_id = var_id;
    }

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

    if (desc->doc_name) {
#ifdef FLECS_DOC
        ecs_doc_set_name(world, result, desc->doc_name);
#else
        ecs_err("cannot set doc_name for alert, requires FLECS_DOC addon");
        goto error;
#endif
    }

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
    if (result) {
        ecs_delete(world, result);
    }
    return 0;
}

int32_t ecs_get_alert_count(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t alert)
{
    flecs_poly_assert(world, ecs_world_t);
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
    flecs_poly_assert(world, ecs_world_t);
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
    ECS_COMPONENT_DEFINE(world, EcsAlertsActive);

    ecs_set_name_prefix(world, "EcsAlert");
    ECS_COMPONENT_DEFINE(world, EcsAlertInstance);
    ECS_COMPONENT_DEFINE(world, EcsAlertTimeout);

    ECS_TAG_DEFINE(world, EcsAlertInfo);
    ECS_TAG_DEFINE(world, EcsAlertWarning);
    ECS_TAG_DEFINE(world, EcsAlertError);
    ECS_TAG_DEFINE(world, EcsAlertCritical);

    ecs_add_id(world, ecs_id(EcsAlert), EcsPairIsTag);
    ecs_add_id(world, ecs_id(EcsAlert), EcsExclusive);
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

    ecs_set_hooks(world, EcsAlertInstance, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsAlertInstance),
        .move = ecs_move(EcsAlertInstance),
        .copy = ecs_copy(EcsAlertInstance)
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsAlertsActive),
        .members = {
            { .name = "info_count", .type = ecs_id(ecs_i32_t) },
            { .name = "warning_count", .type = ecs_id(ecs_i32_t) },
            { .name = "error_count", .type = ecs_id(ecs_i32_t) }
        }
    });

    ECS_SYSTEM(world, MonitorAlerts, EcsPreStore, 
        Alert, 
        (Poly, Query));

    ECS_SYSTEM(world, MonitorAlertInstances, EcsOnStore, Instance, 
        flecs.metrics.Source, 
        flecs.metrics.Value, 
        ?Timeout,
        ?Disabled);

    ecs_system(world, {
        .entity = ecs_id(MonitorAlerts),
        .immediate = true,
        .interval = (ecs_ftime_t)0.5
    });

    ecs_system(world, {
        .entity = ecs_id(MonitorAlertInstances),
        .interval = (ecs_ftime_t)0.5
    });
}

#endif
