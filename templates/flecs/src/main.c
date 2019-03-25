#include <include/${id base}.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Create your entities, components and systems here! */

    return ecs_fini(world);
}
