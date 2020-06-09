#include <save_to_file.h>

#define BUFFER_SIZE (36)
#define FILENAME "savegame.flecs"

typedef struct Position {
    float x;
    float y;
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

void Move(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Velocity, v, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        printf("Moved %s {%f, %f}\n", 
            ecs_get_name(it->world, it->entities[i]), p[i].x, p[i].y);
    }
}

void save_to_file(
    ecs_world_t *world,
    const char *filename)
{
    ecs_reader_t reader = ecs_reader_init(world);
    FILE *file = fopen(filename, "wb");

    /* Read data from flecs in BUFFER_SIZE chunks */
    char buffer[BUFFER_SIZE];
    size_t read;
    while ((read = ecs_reader_read(buffer, BUFFER_SIZE, &reader))) {
        fwrite(buffer, 1, read, file);
    }

    fclose(file);
}

void load_from_file(
    ecs_world_t *world,
    const char *filename)
{
    ecs_writer_t writer = ecs_writer_init(world);
    FILE* file = fopen(filename, "r");

    /* Write data to flecs in BUFFER_SIZE chunks */
    char buffer[BUFFER_SIZE];
    size_t read;    
    while ((read = fread(buffer, 1, BUFFER_SIZE, file))) {
        if (ecs_writer_write(buffer, read, &writer)) {
            printf("error: %s\n", ecs_strerror(writer.error));
            break;
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E1, Position, Velocity);
    ecs_set(world, E1, Position, {1, 2});
    ecs_set(world, E1, Velocity, {1, 1});

    ECS_ENTITY(world, E2, Position, Velocity);
    ecs_set(world, E2, Position, {3, 4});
    ecs_set(world, E2, Velocity, {1, 1});

    save_to_file(world, FILENAME);

    /* Destroy old world, create new empty world */
    ecs_fini(world);
    world = ecs_init();

    load_from_file(world, FILENAME);

    /* Note that components don't have to be redefined, they are restored by the
     * writer. */

    /* Create and run a system on the loaded data */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
    ecs_progress(world, 0);

    /* Cleanup */
    return ecs_fini(world);
}
