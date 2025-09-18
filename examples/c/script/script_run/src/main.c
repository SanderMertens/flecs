#include <script_run.h>
#include <stdio.h>

/* This example shows how to run a script once. To see how to parse a script and
 * run it multiple times, see the script_parse_eval example. */

const char *script = 
    "Sun {\n"
    "  Mercury {}\n"
    "  Venus {}\n"
    "  Earth {}\n"
    "  Mars {}\n"
    "  Jupiter {}\n"
    "  Saturn {}\n"
    "  Neptune {}\n"
    "  Uranus {}\n"
    "}\n"
    ;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    if (ecs_script_run(world, "My script", script, NULL)) {
        printf("script failed to run\n");
    }

    ecs_entity_t sun = ecs_lookup(world, "Sun");
    ecs_iter_t it = ecs_children(world, sun);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            printf(" - %s\n", ecs_get_name(world, it.entities[i]));
        }
    }

    return ecs_fini(world);

    // Output
    //  - Mercury
    //  - Venus
    //  - Earth
    //  - Mars
    //  - Jupiter
    //  - Saturn
    //  - Neptune
    //  - Uranus
}
