#include <custom_merge.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Create three stages. */
    ecs_set_stages(world, 3);

    /* By default stages are merged automatically when calling staging_end(). 
     * This behavior can be overridden so that an application can control when
     * each stage is merged, and in what order.
     *
     * The set_automerge() function enables or disables automatic merging for
     * all current stages in the world, and sets the default for new stages. */
    ecs_set_automerge(world, false);

    ecs_world_t *stage_1 = ecs_get_stage(world, 0);
    ecs_world_t *stage_2 = ecs_get_stage(world, 1);
    ecs_world_t *stage_3 = ecs_get_stage(world, 2);

    /* Enable automerging just for stage 1 */
    ecs_set_automerge(stage_1, true);

    /* Begin staging. Note that this also demonstrates a very minimal example of
     * staging, without frame_begin() and frame_end() functions. Also note that
     * we need to enable staging before we may write to a stage. */
    ecs_staging_begin(world);

    /* Create an entity in each stage */
    ecs_entity_t e1 = ecs_set_name(stage_1, 0, "e_stage_1");
    ecs_entity_t e2 = ecs_set_name(stage_2, 0, "e_stage_2");
    ecs_entity_t e3 = ecs_set_name(stage_3, 0, "e_stage_3");

    /* End staging. This will only merge the stages for which automerging has
     * been enabled, which in this case is just stage_1. */
    ecs_staging_end(world);

    /* Show that only e1 has a name */
    printf("e1: \"%s\", e2: %s, e3: %s\n",
        ecs_get_name(world, e1),
        ecs_get_name(world, e2),
        ecs_get_name(world, e3));

    /* Now merge the remaining stages. We could either call merge() on the world
     * which merges all stages, or we could merge each individual stage. The
     * latter gives us more control over the order in which stages are merged.
     * Note that merging is only allowed when staging is disabled. */
    ecs_merge(stage_3);
    ecs_merge(stage_2);

    /* All entities are now merged */
    printf("e1: \"%s\", e2: \"%s\", e3: \"%s\"\n",
        ecs_get_name(world, e1),
        ecs_get_name(world, e2),
        ecs_get_name(world, e3));
}
