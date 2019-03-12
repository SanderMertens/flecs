#include <include/${id base}.h>

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    /* Create your entities, components and systems here! */

    return ecs_fini(world);
}
