#include <include/randomplacement.h>
#include <math.h>

#define NUM_ENTITIES 10

typedef struct Rect {
    int top;
    int left;
    int width;
    int height;
} Rect;

typedef struct Position {
    int x;
    int y;
} Position;

void RandomPlacement(EcsRows *rows) {
    void *row;

    printf("RandomPlacement\n");

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_column(rows, row, 0);
        Rect *r = ecs_column(rows, row, 1);

        p->x = r->left;
        p->y = r->top;

        //p->x = rand() % r->width + r->left;
        //p->y = rand() % r->height + r->top;
    }
}

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Rect);
    ECS_COMPONENT(world, Position);

    /* The RandomPlacement system is called when Position is added to an entity.
     * It places entities within a specified rectangle */
    ECS_SYSTEM(world, RandomPlacement, EcsOnAdd, Position, SYSTEM.Rect);

    /* Initialize values for the rectangle */
    ecs_set(world, RandomPlacement_h, Rect, {
        .top = 50,
        .left = 50,
        .width = 100,
        .height = 100
    });

    /* Create 10 entities with the Position component */
    EcsHandle handles[NUM_ENTITIES];
    ecs_new_w_count(world, Position_h, NUM_ENTITIES, handles);

    /* Print entities */
    int i;
    for (i = 0; i < NUM_ENTITIES; i ++) {
        Position p = ecs_get(world, handles[i], Position);
        printf("Entity %lld: x=%d, y=%d\n", handles[i], p.x, p.y);
    }


    return 0;
}
