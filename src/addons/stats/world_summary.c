/**
 * @file addons/world_summary.c
 * @brief Monitor addon.
 */

#include "flecs.h"
#include "stats.h"

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(EcsWorldSummary);

static
void UpdateWorldSummary(ecs_iter_t *it) {
    EcsWorldSummary *summary = ecs_field(it, EcsWorldSummary, 0);

    const ecs_world_info_t *info = ecs_get_world_info(it->world);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        summary[i].target_fps = (double)info->target_fps;

        summary[i].frame_time_last = (double)info->frame_time_total - summary[i].frame_time_total;
        summary[i].system_time_last = (double)info->system_time_total - summary[i].system_time_total;
        summary[i].merge_time_last = (double)info->merge_time_total - summary[i].merge_time_total;

        summary[i].frame_time_total = (double)info->frame_time_total;
        summary[i].system_time_total = (double)info->system_time_total;
        summary[i].merge_time_total = (double)info->merge_time_total;

        summary[i].frame_count ++;
        summary[i].command_count +=
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

        summary[i].build_info = *ecs_get_build_info();
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

    ecs_system(world, {
        .entity = ecs_entity(world, { 
            .name = "UpdateWorldSummary", 
            .add = ecs_ids(ecs_dependson(EcsPreFrame)) 
        }),
        .query.terms[0] = { .id = ecs_id(EcsWorldSummary) },
        .callback = UpdateWorldSummary
    });

    ECS_SYSTEM(world, UpdateWorldSummary, EcsPreFrame, WorldSummary);
    ecs_set(world, EcsWorld, EcsWorldSummary, {0});
}

#endif
