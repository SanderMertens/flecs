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
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
    }

    ecs_progress(world, 1.0);

    /* Verify there is still only one alert */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
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
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 2);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_1);
        {
            const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
            test_assert(instance != NULL);
            const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_2);
        {
            const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
            test_assert(instance != NULL);
            const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), false);
    }

    ecs_progress(world, 1.0);

    /* Verify there are still only two alerts */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 2);
    {
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 2);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_1);
        {
            const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
            test_assert(instance != NULL);
            const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_2);
        {
            const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
            test_assert(instance != NULL);
            const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
            test_assert(source != NULL);
            test_int(source->entity, e2);
        }

        test_bool(ecs_filter_next(&it), false);
    }

    ecs_add(world, e2, Mass);

    ecs_progress(world, 1.0);

    /* Verify there is only one alert left */
    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert_1);
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);

        const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
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
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "missing velocity");

        const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
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
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "e2: missing velocity");

        const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
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
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "p2.e2: parent p2 does not have Position");

        const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
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
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "p2.e2: parent p2 does not have Position");

        const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
    }

    ecs_add_pair(world, e2, EcsChildOf, parent_3);

    ecs_progress(world, 1.0);

    test_assert(!ecs_has(world, e1, EcsAlertsActive));
    test_assert(ecs_has(world, e2, EcsAlertsActive));
    test_int(ecs_count(world, EcsAlertInstance), 1);
    {
        const EcsAlertsActive *active = ecs_get(world, e2, EcsAlertsActive);
        test_assert(active != NULL);
        test_int(active->count, 1);

        ecs_filter_t *alerts = ecs_filter(world, { .expr = "flecs.alerts.AlertInstance" });
        ecs_iter_t it = ecs_filter_iter(world, alerts);
        test_bool(ecs_filter_next(&it), true);
        test_int(it.count, 1);
        
        test_assert(it.entities[0] != 0);
        test_assert(ecs_get_parent(world, it.entities[0]) == alert);
        const EcsAlertInstance *instance = ecs_get(world, it.entities[0], EcsAlertInstance);
        test_assert(instance != NULL);
        test_str(instance->message, "p3.e2: parent p3 does not have Position");

        const EcsAlertSource *source = ecs_get(world, it.entities[0], EcsAlertSource);
        test_assert(source != NULL);
        test_int(source->entity, e2);

        test_bool(ecs_filter_next(&it), false);
    }

    ecs_fini(world);
}
