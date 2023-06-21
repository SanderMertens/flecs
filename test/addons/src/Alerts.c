#include <addons.h>

void Alerts_one_active_alert() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity"
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_progress(world, 1.0);

    /* Verify there is still only one alert */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add(world, e2, Velocity);

    ecs_progress(world, 1.0);

    /* Verify that alert has cleared */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(!ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_two_active_alerts() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e1, Mass);
    ecs_add(world, e2, Position);

    ecs_entity_t alert_1 = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity"
    });

    ecs_entity_t alert_2 = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_mass"),
        .filter.expr = "Position, !Mass"
    });

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 2);
    {
        test_assert(ecs_get_alert_count(world, e2, alert_1) == 1);
        test_assert(ecs_get_alert_count(world, e2, alert_2) == 1);
        test_assert(ecs_get_alert_count(world, e2, 0) == 2);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_1);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        {
            const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
            test_assert(instance != NULL);
            const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_2);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        {
            const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
            test_assert(instance != NULL);
            const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_progress(world, 1.0);

    /* Verify there are still only two alerts */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 2);
    {
        test_assert(ecs_get_alert_count(world, e2, alert_1) == 1);
        test_assert(ecs_get_alert_count(world, e2, alert_2) == 1);
        test_assert(ecs_get_alert_count(world, e2, 0) == 2);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_1);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        {
            const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
            test_assert(instance != NULL);
            const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_2);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        {
            const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
            test_assert(instance != NULL);
            const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add(world, e2, Mass);

    ecs_progress(world, 1.0);

    /* Verify there is only one alert left */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert_1) == 1);
        test_assert(ecs_get_alert_count(world, e2, alert_2) == 0);
        test_assert(ecs_get_alert_count(world, e2, 0) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_1);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add(world, e2, Velocity);

    ecs_progress(world, 1.0);

    /* Verify that alert has cleared */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(!ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_alert_message() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .message = "missing velocity"
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "missing velocity");

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_alert_message_w_this_var() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .message = "$this: missing velocity"
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "e2: missing velocity");

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_alert_message_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_1 = ecs_new_entity(world, "p1");
    ecs_add(world, parent_1, Position);
    ecs_entity_t parent_2 = ecs_new_entity(world, "p2");
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_pair(world, e1, EcsChildOf, parent_1);
    ecs_add_pair(world, e2, EcsChildOf, parent_2);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position($this), ChildOf($this, $parent), !Position($parent)",
        .message = "$this: parent $parent does not have Position"
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "p2.e2: parent p2 does not have Position");

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_alert_message_w_changed_var() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_1 = ecs_new_entity(world, "p1");
    ecs_add(world, parent_1, Position);
    ecs_entity_t parent_2 = ecs_new_entity(world, "p2");
    ecs_entity_t parent_3 = ecs_new_entity(world, "p3");
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_pair(world, e1, EcsChildOf, parent_1);
    ecs_add_pair(world, e2, EcsChildOf, parent_2);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position($this), ChildOf($this, $parent), !Position($parent)",
        .message = "$this: parent $parent does not have Position"
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "p2.e2: parent p2 does not have Position");

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add_pair(world, e2, EcsChildOf, parent_3);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "p3.e2: parent p3 does not have Position");

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_set_brief() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "has_position"),
        .filter.expr = "Position",
        .brief = "Entity has Position"
    });
    test_assert(alert != 0);
    test_str(ecs_doc_get_brief(world, alert), "Entity has Position");

    ecs_fini(world);
}

void Alerts_set_doc_name() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "has_position"),
        .filter.expr = "Position",
        .doc_name = "Has Position"
    });
    test_assert(alert != 0);
    test_str(ecs_doc_get_name(world, alert), "Has Position");

    ecs_fini(world);
}

void Alerts_alert_instance_has_doc_name() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity"
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_str(ecs_doc_get_name(world, it.entities[0]), "e2");

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_reraise_alert() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_add(world, e1, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity"
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    /* Clear the alert */
    ecs_add(world, e1, Velocity);

    ecs_progress(world, 1.0);

    /* Verify the alert has cleared */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));

    /* Reraise the alert */
    ecs_remove(world, e1, Velocity);

    ecs_progress(world, 1.0);

    /* Verify the alert has been raised again */
    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_info_severity() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .severity = EcsAlertInfo
    });
    test_assert(alert != 0);
    ecs_has_pair(world, alert, ecs_id(EcsAlert), EcsAlertInfo);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertInfo));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_warning_severity() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .severity = EcsAlertWarning
    });
    test_assert(alert != 0);
    ecs_has_pair(world, alert, ecs_id(EcsAlert), EcsAlertWarning);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_error_severity() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .severity = EcsAlertError
    });
    test_assert(alert != 0);
    ecs_has_pair(world, alert, ecs_id(EcsAlert), EcsAlertError);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_expire_after_retain() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .retain_period = 1.0
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    ecs_entity_t ai = 0;
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        ai = it.entities[0];
        test_assert(ecs_has(world, ai, EcsMetricValue));
        test_int(ecs_get(world, ai, EcsMetricValue)->value, 1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_progress(world, 1.0);

    /* Verify there is still only one alert */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    test_assert(ecs_is_alive(world, ai));
    test_assert(ecs_has(world, ai, EcsMetricValue));
    test_int(ecs_get(world, ai, EcsMetricValue)->value, 2);
    test_assert(ecs_get_alert_count(world, e2, alert) == 1);
    test_assert(!ecs_has_id(world, ai, EcsDisabled));

    ecs_add(world, e2, Velocity);

    ecs_progress(world, 1.0);

    /* Verify alert hasn't cleared yet and is disabled */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(!ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    test_assert(ecs_is_alive(world, ai));
    test_assert(ecs_has(world, ai, EcsMetricValue));
    test_int(ecs_get(world, ai, EcsMetricValue)->value, 2);
    test_assert(ecs_get_alert_count(world, e2, alert) == 0);
    test_assert(ecs_has_id(world, ai, EcsDisabled));

    ecs_progress(world, 1.0);

    /* Verify that alert has cleared */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(!ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);
    test_assert(ecs_get_alert_count(world, e2, alert) == 0);
    test_assert(!ecs_is_alive(world, ai));

    ecs_fini(world);
}

void Alerts_revive_w_retain() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .retain_period = 1.0
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    ecs_entity_t ai = 0;
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        ai = it.entities[0];
        test_assert(ecs_has(world, ai, EcsMetricValue));
        test_int(ecs_get(world, ai, EcsMetricValue)->value, 1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_progress(world, 1.0);

    /* Verify there is still only one alert */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    test_assert(ecs_is_alive(world, ai));
    test_assert(ecs_has(world, ai, EcsMetricValue));
    test_int(ecs_get(world, ai, EcsMetricValue)->value, 2);
    test_assert(ecs_get_alert_count(world, e2, alert) == 1);
    test_assert(!ecs_has_id(world, ai, EcsDisabled));

    ecs_add(world, e2, Velocity);

    ecs_progress(world, 1.0);

    /* Verify alert hasn't cleared yet and is disabled */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(!ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    test_assert(ecs_is_alive(world, ai));
    test_assert(ecs_has(world, ai, EcsMetricValue));
    test_int(ecs_get(world, ai, EcsMetricValue)->value, 2);
    test_assert(ecs_get_alert_count(world, e2, alert) == 0);
    test_assert(ecs_has_id(world, ai, EcsDisabled));

    ecs_remove(world, e2, Velocity);

    ecs_progress(world, 1.0);

    /* Verify that alert is active again */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    test_assert(ecs_is_alive(world, ai));
    test_assert(ecs_has(world, ai, EcsMetricValue));
    test_int(ecs_get(world, ai, EcsMetricValue)->value, 3);
    test_assert(ecs_get_alert_count(world, e2, alert) == 1);
    test_assert(!ecs_has_id(world, ai, EcsDisabled));

    ecs_progress(world, 1.0);

    /* Verify that alert is still active */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    test_assert(ecs_is_alive(world, ai));
    test_assert(ecs_has(world, ai, EcsMetricValue));
    test_int(ecs_get(world, ai, EcsMetricValue)->value, 4);
    test_assert(ecs_get_alert_count(world, e2, alert) == 1);
    test_assert(!ecs_has_id(world, ai, EcsDisabled));

    ecs_fini(world);
}

void Alerts_severity_filter() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .severity_filters[0] = {
            .severity = EcsAlertWarning,
            .with = ecs_id(Mass)
        }
    });
    test_assert(alert != 0);
    ecs_has_pair(world, alert, ecs_id(EcsAlert), EcsAlertError);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add(world, e1, Mass);
    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_remove(world, e1, Mass);
    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_two_severity_filters() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .severity_filters[0] = {
            .severity = EcsAlertWarning,
            .with = ecs_id(Mass)
        },
        .severity_filters[1] = {
            .severity = EcsAlertInfo,
            .with = ecs_id(Rotation)
        }
    });
    test_assert(alert != 0);
    ecs_has_pair(world, alert, ecs_id(EcsAlert), EcsAlertError);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add(world, e1, Mass);
    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_remove(world, e1, Mass);
    ecs_add(world, e1, Rotation);
    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertInfo));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_remove(world, e1, Rotation);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_severity_filter_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t parent = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add(world, e1, Position);
    ecs_add(world, e1, Mass);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity, (ChildOf, $parent)",
        .severity_filters[0] = {
            .severity = EcsAlertWarning,
            .with = ecs_id(Mass),
            .var = "parent"
        }
    });
    test_assert(alert != 0);
    ecs_has_pair(world, alert, ecs_id(EcsAlert), EcsAlertError);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);

        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add(world, parent, Mass);
    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_remove(world, parent, Mass);
    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_severity_filter_w_var_change_var() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t parent_1 = ecs_new_id(world);
    ecs_entity_t parent_2 = ecs_new_id(world);
    ecs_add(world, parent_2, Mass);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, EcsChildOf, parent_1);
    ecs_add(world, e1, Position);
    ecs_add(world, e1, Mass);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity, (ChildOf, $parent)",
        .severity_filters[0] = {
            .severity = EcsAlertWarning,
            .with = ecs_id(Mass),
            .var = "parent"
        }
    });
    test_assert(alert != 0);
    ecs_has_pair(world, alert, ecs_id(EcsAlert), EcsAlertError);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);

        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add_pair(world, e1, EcsChildOf, parent_2);
    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add_pair(world, e1, EcsChildOf, parent_1);
    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_member_range_warning() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .warning_range = { 0, 100 }}}
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Mass, {50});

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = member
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set(world, e1, Mass, {150});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_set(world, e1, Mass, {25});

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_member_range_error() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Mass, {50});

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = member
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set(world, e1, Mass, {150});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_set(world, e1, Mass, {25});

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_member_range_warning_error() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .warning_range = {0, 50}, .error_range = { 0, 100 }}}
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Mass, {25});

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = member
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set(world, e1, Mass, {75});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_set(world, e1, Mass, {125});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_set(world, e1, Mass, {25});

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_member_range_error_w_warning_severity() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Mass, {50});

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .severity = EcsAlertWarning,
        .member = member
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set(world, e1, Mass, {150});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);

        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_set(world, e1, Mass, {25});

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_member_range_error_w_severity_filter() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Mass, {50});

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .severity = EcsAlertWarning,
        .severity_filters[0] = {
            .with = Tag,
            .severity = EcsAlertError
        },
        .member = member
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set(world, e1, Mass, {150});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);

        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add(world, e1, Tag);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);

        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_member_range_warning_w_severity_filter() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .warning_range = { 0, 100 }}}
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Mass, {50});

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .severity = EcsAlertWarning,
        .severity_filters[0] = {
            .with = Tag,
            .severity = EcsAlertError
        },
        .member = member
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set(world, e1, Mass, {150});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);

        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add(world, e1, Tag);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);

        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertWarning));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_fini(world);
}

void Alerts_member_range_pair_id() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Mass, {50});

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = member,
        .id = ecs_pair_t(Mass, Tag)
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set_pair(world, e1, Mass, Tag, {150});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_set_pair(world, e1, Mass, Tag, {25});

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_member_range_invalid_member() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_log_set_level(-4);
    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = Tag,
    });
    test_assert(alert == 0);

    ecs_fini(world);
}

void Alerts_member_range_invalid_member_child() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, ecs_id(Mass));

    ecs_log_set_level(-4);
    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = child,
    });
    test_assert(alert == 0);

    ecs_fini(world);
}

void Alerts_member_range_invalid_type() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_log_set_level(-4);
    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = member,
        .id = Tag
    });
    test_assert(alert == 0);

    ecs_fini(world);
}

void Alerts_member_range_invalid_member_type() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t) }}
    });

    ecs_struct(world, {
        .entity = ecs_new_entity(world, "Foo"),
        .members = {{ "value", ecs_id(Mass) }}
    });

    ecs_entity_t member = ecs_lookup_fullpath(world, "Foo.value");
    test_assert(member != 0);

    ecs_log_set_level(-4);
    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = member,
    });
    test_assert(alert == 0);

    ecs_fini(world);
}

void Alerts_member_range_no_range() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t) }}
    });

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_log_set_level(-4);
    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = member,
    });
    test_assert(alert == 0);

    ecs_fini(world);
}

void Alerts_member_range_alert_two_instances() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Mass, {50});
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Mass, {50});

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_mass"),
        .filter.expr = "Mass",
        .member = member,
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(!ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set(world, e1, Mass, {150});
    ecs_set(world, e2, Mass, {150});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 2);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 2);

        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        test_assert(ecs_get(world, it.entities[0], EcsAlertInstance) != NULL);
        test_assert(ecs_get(world, it.entities[1], EcsAlertInstance) != NULL);

        {
            const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
            test_assert(source != NULL);
            test_int(source->entity, e1);
        }
        {
            const EcsMetricSource *source = ecs_get(world, it.entities[1], EcsMetricSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 2);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);
        test_assert(ecs_get_alert_count(world, e2, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 2);

        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        test_assert(ecs_get(world, it.entities[0], EcsAlertInstance) != NULL);
        test_assert(ecs_get(world, it.entities[1], EcsAlertInstance) != NULL);

        {
            const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
            test_assert(source != NULL);
            test_int(source->entity, e1);
        }
        {
            const EcsMetricSource *source = ecs_get(world, it.entities[1], EcsMetricSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_set(world, e1, Mass, {25});
    ecs_set(world, e2, Mass, {25});

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_member_range_from_var() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_set(world, p, Mass, {50});
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_parent_mass"),
        .filter.expr = "(ChildOf, $parent), Mass($parent)",
        .member = member,
        .var = "parent"
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set(world, p, Mass, {150});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_set(world, p, Mass, {25});

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_member_range_from_var_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {{ "value", ecs_id(ecs_f32_t), .error_range = { 0, 100 }}}
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_set(world, p, Mass, {50});
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_entity_t member = ecs_lookup_fullpath(world, "Mass.value");
    test_assert(member != 0);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "high_parent_mass"),
        .filter.expr = "(ChildOf, $parent), Mass($parent)",
        .member = member,
        .var = "parent"
    });
    test_assert(alert != 0);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_set(world, p, Mass, {150});

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_remove(world, p, Mass);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}

void Alerts_retained_alert_w_dead_source() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .severity = EcsAlertError,
        .retain_period = 10
    });
    test_assert(alert != 0);
    ecs_has_pair(world, alert, ecs_id(EcsAlert), EcsAlertError);

    ecs_progress(world, 1.0);

    test_assert(ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    ecs_entity_t ai = 0;
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);

        ai = it.entities[0];
        
        test_assert(ai != 0);
        test_assert(ecs_get_parent(world, ai) == alert);
        test_assert(ecs_has_pair(world, ai, ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, ai, EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, ai, EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_add(world, e1, Velocity);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        test_assert(ecs_get_alert_count(world, e1, alert) == 0);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.Instance, ?Disabled" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] == ai);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        test_assert(ecs_has_pair(world, it.entities[0], ecs_id(EcsAlert), EcsAlertError));
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsMetricSource *source = ecs_get(world, it.entities[0], EcsMetricSource);
        test_assert(source != NULL);
        test_int(source->entity, e1);

        test_bool(ecs_filter_next(&it), false);
        ecs_filter_fini(alerts);
    }

    ecs_delete(world, e1);

    ecs_progress(world, 1.0);

    test_int(ecs_count(world, EcsAlertInstance), 0);

    ecs_fini(world);
}
