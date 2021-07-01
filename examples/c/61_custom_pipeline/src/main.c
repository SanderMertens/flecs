#include <custom_pipeline.h>

void PreFrameSystem(ecs_iter_t *it) {
    (void)it; /* Silence unused parameter warning */
    printf("- PreFrame\n");
}

void OnFrameSystem(ecs_iter_t *it) {
    (void)it;
    printf("- OnFrame\n");
}

void PostFrameSystem(ecs_iter_t *it) {
    (void)it;
    printf("- PostFrame\n");
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Create three custom phases for the pipeline */
    ECS_TAG(world, PreFrame);
    ECS_TAG(world, OnFrame);
    ECS_TAG(world, PostFrame);

    /* Create the pipeline with the three custom phases */
    ECS_PIPELINE(world, MyPipeline, PreFrame, OnFrame, PostFrame);

    /* Make the world use the custom pipeline. After this the pipeline should
     * no longer be modified. */
    ecs_set_pipeline(world, MyPipeline);

    /* Define a system for each of the phases. Define them in opposite order so
     * we can see the pipeline orders them correctly. */
    ECS_SYSTEM(world, PreFrameSystem, PreFrame, 0);
    ECS_SYSTEM(world, OnFrameSystem, OnFrame, 0);
    ECS_SYSTEM(world, PostFrameSystem, PostFrame, 0);

    /* Run the pipeline once */
    printf("Begin\n");
    ecs_progress(world, 0);
    printf("End\n");

    /* Cleanup */
    return ecs_fini(world);
}
