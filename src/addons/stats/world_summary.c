/**
 * @file addons/world_summary.c
 * @brief Monitor addon.
 */

#include "flecs.h"
#include "stats.h"

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(EcsWorldSummary);

static 
void flecs_copy_world_summary(
    ecs_world_t *world,
    EcsWorldSummary *dst)
{
    flecs_poly_assert(world, ecs_world_t);
    const ecs_world_info_t *info = ecs_get_world_info(world);

    dst->target_fps = (double)info->target_fps;
    dst->time_scale = (double)info->time_scale;
    dst->fps = 1.0 / (double)info->delta_time_raw;

    dst->frame_time_frame = (double)info->frame_time_total - dst->frame_time_total;
    dst->system_time_frame = (double)info->system_time_total - dst->system_time_total;
    dst->merge_time_frame = (double)info->merge_time_total - dst->merge_time_total;

    dst->merge_count_frame = info->merge_count_total - dst->merge_count;
    dst->systems_ran_frame = info->systems_ran_total - dst->systems_ran_total;
    dst->observers_ran_frame = info->observers_ran_total - dst->observers_ran_total;
    dst->queries_ran_frame = info->queries_ran_total - dst->queries_ran_total;

    int64_t command_count = 
        info->cmd.add_count +
        info->cmd.remove_count +
        info->cmd.delete_count +
        info->cmd.clear_count +
        info->cmd.set_count +
        info->cmd.ensure_count +
        info->cmd.modified_count +
        info->cmd.discard_count +
        info->cmd.event_count +
        info->cmd.other_count;

    dst->command_count_frame = command_count - dst->command_count;

    dst->frame_time_total = (double)info->frame_time_total;
    dst->system_time_total = (double)info->system_time_total;
    dst->merge_time_total = (double)info->merge_time_total;

    dst->entity_count = flecs_entities_count(world);
    dst->table_count = flecs_sparse_count(&world->store.tables);
    dst->frame_count = info->frame_count_total;
    dst->command_count = command_count;
    dst->merge_count = info->merge_count_total;
    dst->systems_ran_total = info->systems_ran_total;
    dst->observers_ran_total = info->observers_ran_total;
    dst->queries_ran_total = info->queries_ran_total;
    dst->tag_count = info->tag_id_count;
    dst->component_count = info->component_id_count;
    dst->pair_count = info->pair_id_count;

    dst->simulation_time += (double)info->delta_time;

    ecs_time_t now;
    ecs_os_get_time(&now);
    dst->uptime = now.sec - info->creation_time;

    dst->build_info = *ecs_get_build_info();
}

static
void UpdateWorldSummary(ecs_iter_t *it) {
    EcsWorldSummary *summary = ecs_field(it, EcsWorldSummary, 0);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        flecs_copy_world_summary(it->real_world, &summary[i]);
    }
}

static
void OnSetWorldSummary(ecs_iter_t *it) {
    EcsWorldSummary *summary = ecs_field(it, EcsWorldSummary, 0);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_set_target_fps(it->world, (ecs_ftime_t)summary[i].target_fps);
        ecs_set_time_scale(it->world, (ecs_ftime_t)summary[i].time_scale);
    }
}

void FlecsWorldSummaryImport(
    ecs_world_t *world) 
{
    ECS_COMPONENT_DEFINE(world, EcsWorldSummary);

#if defined(FLECS_META) && defined(FLECS_UNITS) 
    ecs_entity_t build_info = ecs_lookup(world, "flecs.core.BuildInfo");
    ecs_struct(world, {
        .entity = ecs_id(EcsWorldSummary),
        .members = {
            /* Time */
            { .name = "target_fps", .type = ecs_id(ecs_f64_t), .unit = EcsHertz },
            { .name = "time_scale", .type = ecs_id(ecs_f64_t) },
            { .name = "fps", .type = ecs_id(ecs_f64_t), .unit = EcsHertz },

            /* Totals */
            { .name = "frame_time_total", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds },
            { .name = "system_time_total", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },
            { .name = "merge_time_total", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },

            { .name = "entity_count", .type = ecs_id(ecs_i64_t) },
            { .name = "table_count", .type = ecs_id(ecs_i64_t) },
            { .name = "frame_count", .type = ecs_id(ecs_i64_t) },
            { .name = "command_count", .type = ecs_id(ecs_i64_t) },
            { .name = "merge_count", .type = ecs_id(ecs_i64_t) },
            { .name = "systems_ran_total", .type = ecs_id(ecs_i64_t) },
            { .name = "observers_ran_total", .type = ecs_id(ecs_i64_t) },
            { .name = "queries_ran_total", .type = ecs_id(ecs_i64_t) },

            { .name = "tag_count", .type = ecs_id(ecs_i32_t) },
            { .name = "component_count", .type = ecs_id(ecs_i32_t) },
            { .name = "pair_count", .type = ecs_id(ecs_i32_t) },

            /* Per frame */
            { .name = "frame_time_frame", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },
            { .name = "system_time_frame", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },
            { .name = "merge_time_frame", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },

            { .name = "merge_count_frame", .type = ecs_id(ecs_i64_t) },
            { .name = "systems_ran_frame", .type = ecs_id(ecs_i64_t) },
            { .name = "observers_ran_frame", .type = ecs_id(ecs_i64_t) },
            { .name = "queries_ran_frame", .type = ecs_id(ecs_i64_t) },
            { .name = "command_count_frame", .type = ecs_id(ecs_i64_t) },

            { .name = "simulation_time", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds },
            { .name = "uptime", .type = ecs_id(ecs_u32_t), .unit = EcsSeconds },

            /* Build info */
            { .name = "build_info", .type = build_info }
        }
    });
#endif
    const ecs_world_info_t *info = ecs_get_world_info(world);

    ecs_system(world, {
        .entity = ecs_entity(world, { 
            .name = "UpdateWorldSummary",
            .add = ecs_ids(ecs_pair(EcsDependsOn, EcsPreFrame))
        }),
        .query.terms = {{ .id = ecs_id(EcsWorldSummary) }},
        .callback = UpdateWorldSummary
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { 
            .name = "OnSetWorldSummary"
        }),
        .events = { EcsOnSet },
        .query.terms = {{ .id = ecs_id(EcsWorldSummary) }},
        .callback = OnSetWorldSummary
    });

    ecs_set(world, EcsWorld, EcsWorldSummary, {
        .target_fps = (double)info->target_fps,
        .time_scale = (double)info->time_scale
    });

    EcsWorldSummary *summary = ecs_ensure(world, EcsWorld, EcsWorldSummary);
    flecs_copy_world_summary(world, summary);
    ecs_modified(world, EcsWorld, EcsWorldSummary);    
}

#endif
