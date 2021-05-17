#include <dynamic_buffer.h>

typedef struct {
    double x, y;
} Position;

/* Non-POD component type with a dynamic buffer */
typedef struct {
    int *data;
    size_t count;
} DynamicBuffer;

/* Lifecycle callbacks for the DynamicBuffer. These ensure that when a component
 * is created, destructed, copied or moved no memory corruption or leakage
 * happens. */
ECS_CTOR(DynamicBuffer, ptr, {
    printf("DynamicBuffer::ctor\n");
    ptr->data = NULL;
    ptr->count = 0;
})

ECS_DTOR(DynamicBuffer, ptr, {
    printf("DynamicBuffer::dtor\n");
    free(ptr->data);
})

ECS_COPY(DynamicBuffer, dst, src, {
    printf("DynamicBuffer::copy\n");
    if (dst->data) {
        free(dst->data);
    }

    size_t size = sizeof(int) * src->count;
    dst->data = malloc(size);
    dst->count = src->count;
    memcpy(dst->data, src->data, size);
})

ECS_MOVE(DynamicBuffer, dst, src, {
    printf("DynamicBuffer::move\n");
    if (dst->data) {
        free(dst->data);
    }

    dst->data = src->data;
    dst->count = src->count;

    src->data = NULL;
    src->count = 0;
})

/* Forward declare component handles */
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(DynamicBuffer);

/* Add an element to a new or existing buffer */
void add_elem(ecs_world_t *ecs, ecs_entity_t e, int value) {
    DynamicBuffer *ptr = ecs_get_mut(ecs, e, DynamicBuffer, NULL);

    ptr->count ++;
    ptr->data = realloc(ptr->data, ptr->count * sizeof(int));
    ptr->data[ptr->count - 1] = value;
}

/* Remove element from buffer */
void remove_elem(ecs_world_t *ecs, ecs_entity_t e, size_t elem) {
    DynamicBuffer *ptr = ecs_get_mut(ecs, e, DynamicBuffer, NULL);

    size_t last = ptr->count - 1;

    if (last >= elem) {
        if (last - elem) {
            ptr->data[elem] = ptr->data[last];
        }
        
        ptr->count --;
    }
}

/* Get element from a buffer */
int* get_elem(ecs_world_t *ecs, ecs_entity_t e, size_t elem) {
    const DynamicBuffer *ptr = ecs_get(ecs, e, DynamicBuffer);

    if (ptr && (ptr->count > elem)) {
        return &ptr->data[elem];
    } else {
        return NULL;
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    /* Register ids for forward declared components */
    ECS_COMPONENT_DEFINE(ecs, Position);
    ECS_COMPONENT_DEFINE(ecs, DynamicBuffer);

    /* Register lifecycle functions for comopnent */
    ecs_set_component_actions(ecs, DynamicBuffer, {
        .ctor = ecs_ctor(DynamicBuffer),
        .dtor = ecs_dtor(DynamicBuffer),
        .copy = ecs_copy(DynamicBuffer),
        .move = ecs_move(DynamicBuffer)
    });

    ecs_entity_t e = ecs_new_id(ecs);

    /* Add 3 elements to the buffer. The first add will add the DynamicBuffer
     * element to the entity. */
    add_elem(ecs, e, 10);
    add_elem(ecs, e, 20);
    add_elem(ecs, e, 30);

    printf("Elem 1 = %d\n", *get_elem(ecs, e, 1));

    /* Remove element. This will move the last element from the buffer to the
     * removed element. */
    remove_elem(ecs, e, 1);

    printf("Elem 1 = %d (after remove)\n", *get_elem(ecs, e, 1));

    /* Add component. This causes the entity to move between tables, and will
     * invoke DynamicComponent::move to copy the component value from the src to
     * the dst table. This also invokes DynamicComponent::ctor to construct the
     * component in the dst table. */
    ecs_add(ecs, e, Position);

    /* This will invoke DynamicComponent::dtor. */
    return ecs_fini(ecs);
}
