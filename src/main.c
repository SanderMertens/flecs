/* This is a managed file. Do not delete this comment. */

#include <reflecs/reflecs.h>
#include <reflecs/components.h>
#include <reflecs/vector.h>
#include <stdio.h>

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
    int nums[] = {12, 56, 32, 85, 81, 78, 69, 43, 41, 23, 51, 21, 48, 10, 16, 14, 33, 29, 37, 60, 96};
    int count = sizeof(nums) / sizeof(int);

    EcsVectorParams params = {
        .chunk_count = 1000 * 1000,
        .element_size = sizeof(int),
        .compare_action = compare_int
    };

    EcsVector *vec = ecs_vector_new(&params);

    int i;
    for (i = 0; i < 1000 * 1000 * 1000; i ++) {
        int *elem = ecs_vector_add(vec, &params);
        *elem = i;
    }

    EcsIter it = ecs_vector_iter(vec, &params);

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
