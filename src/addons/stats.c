
#include "../private_api.h"

#ifdef FLECS_STATS

static
void record_stat_int32(
    ecs_stat_int32_t *stat,
    int32_t t,
    int32_t value)
{
    stat->avg[t] = value;
    stat->min[t] = value;
    stat->max[t] = value;
}

static
void record_stat_float(
    ecs_stat_float_t *stat,
    int32_t t,
    float value)
{
    stat->avg[t] = value;
    stat->min[t] = value;
    stat->max[t] = value;    
}

static
void print_stat_int32(
    const char *name,
    int32_t t,
    const ecs_stat_int32_t *stat)
{
    ecs_size_t len = ecs_os_strlen(name);
    printf("%s: %*s %d\n", name, 32 - len, "", stat->avg[t]);
}

static
void print_val_float(
    const char *name,
    float value)
{
    ecs_size_t len = ecs_os_strlen(name);
    printf("%s: %*s %.2f\n", name, 32 - len, "", value);
}

static
void print_stat_float(
    const char *name,
    int32_t t,
    const ecs_stat_float_t *stat)
{
    print_val_float(name, stat->avg[t]);
}

void ecs_get_world_stats(
    ecs_world_t *world,
    ecs_world_stats_t *s)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(s != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t t = s->t = (s->t + 1) % ECS_STAT_WINDOW;

    /* Store time values from world info. These values monotonically increase.
     * To store the delta between samples, store the previous values as well */
    float prev_world_time_total_raw = s->prev_world_time_total_raw;
    float prev_world_time_total = s->prev_world_time_total;
    float prev_frame_time_total = s->prev_frame_time_total;
    float prev_system_time_total = s->prev_system_time_total;
    float prev_merge_time_total = s->prev_merge_time_total;

    record_stat_float(&s->world_time_total_raw, t, world->stats.world_time_total_raw - prev_world_time_total_raw);
    record_stat_float(&s->world_time_total, t, world->stats.world_time_total - prev_world_time_total);
    record_stat_float(&s->frame_time_total, t, world->stats.frame_time_total - prev_frame_time_total);
    record_stat_float(&s->system_time_total, t, world->stats.system_time_total - prev_system_time_total);
    record_stat_float(&s->merge_time_total, t, world->stats.merge_time_total - prev_merge_time_total);

    s->prev_world_time_total_raw = world->stats.world_time_total_raw;
    s->prev_world_time_total = world->stats.world_time_total;
    s->prev_frame_time_total = world->stats.frame_time_total;
    s->prev_system_time_total = world->stats.system_time_total;
    s->prev_merge_time_total = world->stats.merge_time_total;

    /* Store counters from world info. These values monotonically increase.
     * To store the delta between samples, store the previous values as well */
    int32_t prev_frame_count_total = s->prev_frame_count_total;
    int32_t prev_merge_count_total = s->prev_merge_count_total;
    int32_t prev_pipeline_build_count_total = s->prev_pipeline_build_count_total;
    int32_t prev_systems_ran_frame = s->prev_systems_ran_frame;

    record_stat_int32(&s->frame_count_total, t, world->stats.frame_count_total - prev_frame_count_total);
    record_stat_int32(&s->merge_count_total, t, world->stats.merge_count_total - prev_merge_count_total);
    record_stat_int32(&s->pipeline_build_count_total, t, world->stats.pipeline_build_count_total - prev_pipeline_build_count_total); 
    record_stat_int32(&s->systems_ran_frame, t, world->stats.systems_ran_frame - prev_systems_ran_frame);

    s->prev_frame_count_total = world->stats.frame_count_total;
    s->prev_merge_count_total = world->stats.merge_count_total;
    s->prev_pipeline_build_count_total = world->stats.pipeline_build_count_total;
    s->prev_systems_ran_frame = world->stats.systems_ran_frame;

    /* Compute frames per second from the delta for world time and frame */
    float delta_world_time = world->stats.world_time_total_raw - prev_world_time_total_raw;
    int32_t delta_frame = world->stats.frame_count_total - prev_frame_count_total;
    record_stat_float(&s->fps, t, 1.0f / (delta_world_time / delta_frame));    

    /* Compute world statistics */
    record_stat_int32(&s->entity_count, t, ecs_sparse_count(world->store.entity_index));
    record_stat_int32(&s->component_count, t, ecs_count_entity(world, ecs_typeid(EcsComponent)));
    record_stat_int32(&s->query_count, t, ecs_vector_count(world->queries));
    record_stat_int32(&s->system_count, t, ecs_count_entity(world, ecs_typeid(EcsSystem)));

    /* Compute deferred operation statistics. These values monotonically increase.
     * To store the delta between samples, store the previous values as well */
    int32_t prev_new_count = s->prev_new_count;
    int32_t prev_bulk_new_count = s->prev_bulk_new_count;
    int32_t prev_delete_count = s->prev_delete_count;
    int32_t prev_clear_count = s->prev_clear_count;
    int32_t prev_add_count = s->prev_add_count;
    int32_t prev_remove_count = s->prev_remove_count;
    int32_t prev_set_count = s->prev_set_count;
    int32_t prev_discard_count = s->prev_discard_count;

    s->prev_new_count = world->new_count;
    s->prev_bulk_new_count = world->bulk_new_count;
    s->prev_delete_count = world->delete_count;
    s->prev_clear_count = world->clear_count;
    s->prev_add_count = world->add_count;
    s->prev_remove_count = world->remove_count;
    s->prev_set_count = world->set_count;
    s->prev_discard_count = world->discard_count;

    record_stat_int32(&s->new_count, t, world->new_count - prev_new_count);
    record_stat_int32(&s->bulk_new_count, t, world->bulk_new_count - prev_bulk_new_count);
    record_stat_int32(&s->delete_count, t, world->delete_count - prev_delete_count);
    record_stat_int32(&s->clear_count, t, world->clear_count - prev_clear_count);
    record_stat_int32(&s->add_count, t, world->add_count - prev_add_count);
    record_stat_int32(&s->remove_count, t, world->remove_count - prev_remove_count);
    record_stat_int32(&s->set_count, t, world->set_count - prev_set_count);
    record_stat_int32(&s->discard_count, t, world->discard_count - prev_discard_count);

    /* Compute table statistics */
    int32_t empty_table_count = 0;
    int32_t singleton_table_count = 0;
    int32_t max_entities_per_table = 0;
    int32_t max_components_per_table = 0;
    int32_t max_columns_per_table = 0;
    int32_t max_matched_queries_per_table = 0;
    int32_t matched_table_count = 0, matched_entity_count = 0;

    int32_t i, count = ecs_sparse_count(world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(world->store.tables, ecs_table_t, i);
        int32_t entity_count = ecs_table_count(table);

        if (!entity_count) {
            empty_table_count ++;
        }

        if (entity_count > max_entities_per_table) {
            max_entities_per_table = entity_count;
        }

        int32_t component_count = ecs_vector_count(table->type);
        if (component_count > max_components_per_table) {
            max_components_per_table = component_count;
        }

        if (table->column_count > max_columns_per_table) {
            max_columns_per_table = table->column_count;
        }        

        /* Singleton tables are tables that have just one entity that also has
         * itself in the table type. */
        if (entity_count == 1) {
            ecs_data_t *data = ecs_table_get_data(table);
            ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
            if (ecs_type_has_entity(world, table->type, entities[0])) {
                singleton_table_count ++;
            }
        }

        int32_t queries_matched = ecs_vector_count(table->queries);
        if (queries_matched > max_matched_queries_per_table) {
            max_matched_queries_per_table = queries_matched;
        }

        /* If this table matches with queries and is not empty, increase the
         * matched table & matched entity count. These statistics can be used to
         * compute actual fragmentation ratio for queries. */
        if (queries_matched && entity_count) {
            matched_table_count ++;
            matched_entity_count += entity_count;
        }
    }

    record_stat_int32(&s->matched_table_count, t, matched_table_count);
    record_stat_int32(&s->matched_entity_count, t, matched_entity_count);
    record_stat_int32(&s->table_count, t, i);

    record_stat_int32(&s->empty_table_count, t, empty_table_count);
    record_stat_int32(&s->singleton_table_count, t, singleton_table_count);
    record_stat_int32(&s->max_entities_per_table, t, max_entities_per_table);
    record_stat_int32(&s->max_components_per_table, t, max_components_per_table);
    record_stat_int32(&s->max_columns_per_table, t, max_columns_per_table);
    record_stat_int32(&s->max_matched_queries_per_table, t, max_matched_queries_per_table);
}

void ecs_dump_world_stats(
    ecs_world_t *world,
    const ecs_world_stats_t *s)
{
    int32_t t = s->t;
    
    print_stat_int32("Frame", t, &s->frame_count_total);
    printf("-------------------------------------\n");
    print_stat_int32("pipeline rebuilds", t, &s->pipeline_build_count_total);
    print_stat_int32("systems ran last frame", t, &s->systems_ran_frame);
    printf("\n");
    print_val_float("target FPS", world->stats.target_fps);
    print_val_float("time scale", world->stats.time_scale);
    printf("\n");
    print_stat_float("actual FPS", t, &s->fps);
    print_stat_float("frame time", t, &s->frame_time_total);
    print_stat_float("system time", t, &s->system_time_total);
    print_stat_float("merge time", t, &s->merge_time_total);
    print_stat_float("simulation time elapsed", t, &s->world_time_total);
    printf("\n");
    print_stat_int32("entity count", t, &s->entity_count);
    print_stat_int32("component count", t, &s->component_count);
    print_stat_int32("query count", t, &s->query_count);
    print_stat_int32("system count", t, &s->system_count);
    print_stat_int32("table count", t, &s->table_count);
    print_stat_int32("singleton table count", t, &s->singleton_table_count);
    print_stat_int32("empty table count", t, &s->empty_table_count);
    print_stat_int32("max entities per table", t, &s->max_entities_per_table);
    print_stat_int32("max components per table", t, &s->max_components_per_table);
    print_stat_int32("max columns per table", t, &s->max_columns_per_table);
    print_stat_int32("max matched queries per table", t, &s->max_matched_queries_per_table);
    printf("\n");
    print_stat_int32("deferred new operations", t, &s->new_count);
    print_stat_int32("deferred bulk_new operations", t, &s->bulk_new_count);
    print_stat_int32("deferred delete operations", t, &s->delete_count);
    print_stat_int32("deferred clear operations", t, &s->clear_count);
    print_stat_int32("deferred add operations", t, &s->add_count);
    print_stat_int32("deferred remove operations", t, &s->remove_count);
    print_stat_int32("deferred set operations", t, &s->set_count);
    print_stat_int32("discarded operations", t, &s->discard_count);
    printf("\n");
}

#endif
