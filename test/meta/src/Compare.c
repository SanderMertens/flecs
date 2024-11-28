#include "flecs.h"
#include <meta.h>
#include <stdint.h>
#include <stdlib.h>




const ecs_type_info_t *sort_ti = NULL;

int compare_element(const void *a, const void *b) {
    return sort_ti->hooks.comp(a, b, sort_ti);
}

int compare_array(const void *a, const void *b, ecs_size_t num_elements) {
    int i;
    for(i = 0; i<num_elements; i++){
        const void *el_a = ECS_ELEM(a, sort_ti->size, i);
        const void *el_b = ECS_ELEM(b, sort_ti->size, i);
        int c = compare_element(el_a, el_b);
        if(c != 0) {
            return c;
        }
    }
    return 0;
}

void sort_array(const ecs_world_t* world, ecs_entity_t component, void *arr, ecs_size_t num_elements) {
    sort_ti = ecs_get_type_info(world, component);
    qsort(arr, num_elements, sort_ti->size, compare_element);
}

void Compare_u8(void) {
    ecs_world_t *world = ecs_init();
    
    ecs_u8_t arr[] = {1, 79, 12, 3, 255, 79, 0, 14 };
    ecs_u8_t expected[] = {0, 1, 3, 12, 14, 79, 79, 255 };

    sort_array(world, ecs_id(ecs_u8_t), arr, 8);

    test_assert(compare_array(arr, expected, 8) == 0);

    ecs_fini(world);
}