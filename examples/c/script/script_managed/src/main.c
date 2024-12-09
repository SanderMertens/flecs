#include <script_managed.h>
#include <stdio.h>

/* This example shows how to create a managed script. A managed script remains
 * in memory after it is ran, and makes it possible to inspect and modify the
 * script. Managed scripts can be inspected and modified from the explorer.
 * 
 * Updating a script will delete any entities or components that are no longer
 * described by the script.
 */

void print_planets(ecs_world_t *world) {
    ecs_entity_t sun = ecs_lookup(world, "Sun");
    ecs_iter_t it = ecs_children(world, sun);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            printf(" - %s\n", ecs_get_name(world, it.entities[i]));
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_entity_t script = ecs_script(world, {
        .code = 
            "Sun {\n"
            "  Mercury {}\n"
            "  Venus {}\n"
            "  Earth {}\n"
            "  Mars {}\n"
            "  Jupiter {}\n"
            "  Saturn {}\n"
            "  Neptune {}\n"
            "  Uranus {}\n"
            "  Pluto {}\n"
            "}\n"
    });

    if (!script) {
        printf("script failed to run\n");
    }

    printf("Before updating script:\n");
    print_planets(world);

    ecs_script_update(world, script, 0,
        "Sun {\n"
        "  Mercury {}\n"
        "  Venus {}\n"
        "  Earth {}\n"
        "  Mars {}\n"
        "  Jupiter {}\n"
        "  Saturn {}\n"
        "  Neptune {}\n"
        "  Uranus {}\n"
        // Pluto {}\n" not a planet :(
        "}\n"
    );

    printf("\nAfter updating script:\n");
    print_planets(world);

    return ecs_fini(world);

    // Output
    //  Before updating script:
    //   - Mercury
    //   - Venus
    //   - Earth
    //   - Mars
    //   - Jupiter
    //   - Saturn
    //   - Neptune
    //   - Uranus
    //   - Pluto
    //  
    //  After updating script:
    //   - Mercury
    //   - Venus
    //   - Earth
    //   - Mars
    //   - Jupiter
    //   - Saturn
    //   - Neptune
    //   - Uranus
}
