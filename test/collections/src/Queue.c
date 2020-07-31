#include <collections.h>

void Queue_setup() {
    ecs_os_set_api_defaults();
}

void Queue_free_empty() {
    ecs_queue_t *queue = ecs_queue_new(int, 3);
    test_assert(queue != NULL);
    ecs_queue_free(queue);
}

void Queue_push() {
    ecs_queue_t *queue = ecs_queue_new(int, 3);
    test_assert(queue != NULL);
    test_int(ecs_queue_count(queue), 0);
    test_int(ecs_queue_index(queue), 0);

    int *
    v = ecs_queue_push(queue, int);
    test_assert(v != NULL);
    *v = 10;

    test_int(ecs_queue_count(queue), 1);
    test_int(ecs_queue_index(queue), 1);
    test_int(*ecs_queue_get(queue, int, 0), 10);

    v = ecs_queue_push(queue, int);
    test_assert(v != NULL);
    *v = 20;

    test_int(ecs_queue_count(queue), 2);
    test_int(ecs_queue_index(queue), 2);
    test_int(*ecs_queue_get(queue, int, 0), 10);

    v = ecs_queue_push(queue, int);
    test_assert(v != NULL);
    *v = 30;

    test_int(ecs_queue_count(queue), 3);
    test_int(ecs_queue_index(queue), 0);
    test_int(*ecs_queue_get(queue, int, 0), 10);

    v = ecs_queue_push(queue, int);
    *v = 40;

    test_int(ecs_queue_count(queue), 3);
    test_int(ecs_queue_index(queue), 1);
    test_int(*ecs_queue_get(queue, int, 0), 20);    

    ecs_queue_free(queue);
}
