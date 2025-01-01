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
    const ecs_world_info_t *info = ecs_get_world_info(world);

    dst->target_fps = (double)info->target_fps;
    dst->time_scale = (double)info->time_scale;

    dst->frame_time_last = (double)info->frame_time_total - dst->frame_time_total;
    dst->system_time_last = (double)info->system_time_total - dst->system_time_total;
    dst->merge_time_last = (double)info->merge_time_total - dst->merge_time_total;

    dst->frame_time_total = (double)info->frame_time_total;
    dst->system_time_total = (double)info->system_time_total;
    dst->merge_time_total = (double)info->merge_time_total;

    dst->frame_count ++;
    dst->command_count +=
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

    dst->build_info = *ecs_get_build_info();
}

static
void UpdateWorldSummary(ecs_iter_t *it) {
    EcsWorldSummary *summary = ecs_field(it, EcsWorldSummary, 0);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        flecs_copy_world_summary(it->world, &summary[i]);
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
    ecs_entity_t build_info = ecs_lookup(world, "flecs.core.build_info_t");
    ecs_struct(world, {
        .entity = ecs_id(EcsWorldSummary),
        .members = {
            { .name = "target_fps", .type = ecs_id(ecs_f64_t), .unit = EcsHertz },
            { .name = "time_scale", .type = ecs_id(ecs_f64_t) },
            { .name = "frame_time_total", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds },
            { .name = "system_time_total", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },
            { .name = "merge_time_total", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },
            { .name = "frame_time_last", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },
            { .name = "system_time_last", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },
            { .name = "merge_time_last", .type = ecs_id(ecs_f64_t), .unit = EcsSeconds  },
            { .name = "frame_count", .type = ecs_id(ecs_u64_t) },
            { .name = "command_count", .type = ecs_id(ecs_u64_t) },
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
