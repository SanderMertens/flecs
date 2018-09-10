/* This is a managed file. Do not delete this comment. */

#include <reflecs/reflecs.h>
#include <reflecs/components.h>
#include <reflecs/vector.h>
#include <stdio.h>

#include <unistd.h>

int compare_int(void *p1, void *p2) {
    int v1 = *(int*)p1;
    int v2 = *(int*)p2;
    return v1 > v2
      ? 1
      : v2 < v1
        ? -1
        : 0
        ;
}

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_world_new(0);

    int i;
    for (i = 0; i < 1000 * 1000; i ++) {
        ecs_new(world, NULL);
    }

    sleep(10000);

    /*while (ecs_iter_hasnext(&it)) {
        int *elem = ecs_iter_next(&it);
        printf("%d ", *elem);
    }
    printf("\n");*/

    /*ecs_vector_sort(vec, &params);

    it = ecs_vector_iter(vec, &params);
    while (ecs_iter_hasnext(&it)) {
        int *elem = ecs_iter_next(&it);
        printf("%d ", *elem);
    }
    printf("\n");*/

    return 0;
}
