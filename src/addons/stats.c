
#ifdef FLECS_STATS

#include "../private_api.h"

void ecs_get_world_stats(
    ecs_world_t *world,
    ecs_world_stats_t *stats)
{
    stats->entity_count = ecs_sparse_count(world->store.entity_index);
    stats->component_count = ecs_count_entity(world, ecs_typeid(EcsComponent));
    stats->query_count = ecs_vector_count(world->queries);
    stats->system_count = ecs_count_entity(world, ecs_typeid(EcsSystem));
    stats->empty_table_count = 0;
    stats->singleton_table_count = 0;
    stats->max_entities_per_table = 0;
    stats->max_components_per_table = 0;
    stats->max_columns_per_table = 0;
    stats->max_matched_queries_per_table = 0;

    stats->new_count = world->new_count;
    stats->bulk_new_count = world->bulk_new_count;
    stats->delete_count = world->delete_count;
    stats->clear_count = world->clear_count;
    stats->add_count = world->add_count;
    stats->remove_count = world->remove_count;
    stats->set_count = world->set_count;
    stats->discard_count = world->discard_count;

    int32_t i, count = ecs_sparse_count(world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *t = ecs_sparse_get(world->store.tables, ecs_table_t, i);
        int32_t entity_count = ecs_table_count(t);
        if (!entity_count) {
            stats->empty_table_count ++;
        }

        if (entity_count > stats->max_entities_per_table) {
            stats->max_entities_per_table = entity_count;
        }

        int32_t component_count = ecs_vector_count(t->type);
        if (component_count > stats->max_components_per_table) {
            stats->max_components_per_table = component_count;
        }

        if (entity_count == 1) {
            ecs_data_t *data = ecs_table_get_data(t);
            ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
            if (ecs_type_has_entity(world, t->type, entities[0])) {
                stats->singleton_table_count ++;
            }
        }

        if (t->column_count > stats->max_columns_per_table) {
            stats->max_columns_per_table = t->column_count;
        }

        int32_t queries_matched = ecs_vector_count(t->queries);
        if (queries_matched > stats->max_matched_queries_per_table) {
            stats->max_matched_queries_per_table = queries_matched;
        }
    }

    stats->table_count = i;
}

void ecs_dump_world_stats(
    ecs_world_t *world,
    const ecs_world_stats_t *stats)
{
    printf("Frame:                         %d\n", world->stats.frame_count_total);
    printf("-------------------------------------\n");
    printf("pipeline rebuilds:             %d\n", world->stats.pipeline_build_count_total);
    printf("systems ran last frame:        %d\n", world->stats.systems_ran_frame);
    printf("\n");
    printf("target FPS:                    %.2f\n", world->stats.target_fps);
    printf("time scale:                    %.2f\n", world->stats.time_scale);
    printf("\n");
    printf("actual FPS:                    %.2f\n", 1.0 / world->stats.delta_time);
    printf("frame time:                    %.2f\n", world->stats.frame_time_total);
    printf("system time:                   %.2f\n", world->stats.system_time_total);
    printf("merge time:                    %.2f\n", world->stats.merge_time_total);
    printf("simulation time elapsed:       %.2f\n", world->stats.world_time_total);
    printf("\n");
    printf("entity count:                  %d\n", stats->entity_count);
    printf("component count:               %d\n", stats->component_count);
    printf("query count:                   %d\n", stats->query_count);
    printf("system count:                  %d\n", stats->system_count);
    printf("table count:                   %d\n", stats->table_count);
    printf("singleton table count:         %d\n", stats->singleton_table_count);
    printf("empty table count:             %d\n", stats->empty_table_count);
    printf("max entities per table:        %d\n", stats->max_entities_per_table);
    printf("avg entities per table:        %.2f\n", (float)stats->entity_count / 
        (float)(stats->table_count - stats->empty_table_count - stats->singleton_table_count));
    printf("max components per table:      %d\n", stats->max_components_per_table);
    printf("max columns per table:         %d\n", stats->max_columns_per_table);
    printf("max matched queries per table: %d\n", 
        stats->max_matched_queries_per_table);
    printf("\n");
    printf("deferred new operations:       %d\n", world->new_count);
    printf("deferred bulk_new operations:  %d\n", world->bulk_new_count);
    printf("deferred delete operations:    %d\n", world->delete_count);
    printf("deferred clear operations:     %d\n", world->clear_count);
    printf("deferred add operations:       %d\n", world->add_count);
    printf("deferred remove operations:    %d\n", world->remove_count);
    printf("deferred set operations:       %d\n", world->set_count);
    printf("discarded operations:          %d\n", world->discard_count);
    printf("\n");
}

#endif
