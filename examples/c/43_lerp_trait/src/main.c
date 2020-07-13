#include <lerp_trait.h>

typedef struct Position {
    float x;
    float y;
} Position;

typedef struct Lerp {
    float value;
    float lerp_time;
} Lerp;

void DoLerp(ecs_iter_t *it) {
    /* Get the Lerp trait column */
    Lerp *l = ecs_column(it, Lerp, 1);

    /* Obtain the identifier and colum index for the component to lerp */
    ecs_entity_t trait = ecs_column_entity(it, 1);
    ecs_entity_t comp_id = ecs_entity_t_lo(trait);
    int32_t column_index = ecs_table_component_index(it, comp_id);

    /* Get the component column and component size */
    float *cur = ecs_table_column(it, column_index);
    size_t size = ecs_table_column_size(it, column_index);

    /* These are the trait columns for LerpStart and LerpStop. Because these are
     * trait tags, the system does not know their types at compile time. */
    float *start = ecs_column_w_size(it, size, 2);
    float *stop = ecs_column_w_size(it, size, 3);

    /* Apply the lerp. Because we don't know the type of the component we'll
     * assume that the component consists out of only float values. We can then
     * iterate the component pointer with increments of sizeof(float) and apply
     * the lerp to each pointer.
     *
     * A more complete implementation of a lerp could add support for multiple
     * datatypes like double, which could be done by either adding a field to
     * the Lerp trait, or by introducing a Lerp4 and Lerp8 trait. */
    for (int32_t i = 0; i < it->count; i ++) {
        float lerp = l[i].value + it->delta_time / l[i].lerp_time;
        bool lerp_done = lerp >= 1.0;
        lerp = (lerp * !lerp_done) + (1.0 * lerp_done);

        /* Do the actual lerp */
        for (size_t s = 0; s < size; s += sizeof(float)) {
            *cur = *start * (1.0 - lerp) + *stop * lerp;

            cur ++;
            start ++;
            stop ++;
        }

        l[i].value = lerp;

        /* If the lerp is done, remove the traits so we don't keep executing the
         * lerp system. */
        if (lerp_done) {
            ecs_world_t *world = it->world;
            ecs_entity_t e = it->entities[i];
            ecs_entity_t trait_start = ecs_column_entity(it, 2);
            ecs_entity_t trait_stop = ecs_column_entity(it, 3);            
            
            ecs_remove_entity(world, e, trait);
            ecs_remove_entity(world, e, trait_start);
            ecs_remove_entity(world, e, trait_stop);
        }
    }
}

void PrintPosition(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);

    for (int32_t i = 0; i < it->count; i ++) {
        printf("{%f, %f}\n", p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();

    /* Register components and trait as a regular components and tags */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Lerp);
    ECS_TAG(world, LerpStart);
    ECS_TAG(world, LerpStop);

    /* Create a Lerp system that accepts a Lerp trait in addition to a LerpStart
     * and LerpStop trait between which the system will interpolate. This
     * system will only match with entities that have all the traits set with
     * the same cardinality. If for example an entity has two instances of Lerp
     * and LerpStart but three of LerpStop, the system will not match. */
    ECS_SYSTEM(world, DoLerp, EcsOnUpdate, TRAIT | Lerp, TRAIT | LerpStart, TRAIT | LerpStop);

    /* System that prints Position, so we can see the lerp in action */
    ECS_SYSTEM(world, PrintPosition, EcsOnUpdate, Position);

    /* Create an entity with Position and Velocity */
    ecs_entity_t e = ecs_new(world, Position);

    /* Add the Lerp trait to Position, apply lerp over 5 seconds */
    ecs_set_trait(world, e, Position, Lerp, {
        .value = 0,
        .lerp_time = 5
    });

    /* The LerpStart and LerpStop traits are not components but tags. When a
     * trait tag is added to an entity, the trait assumes the type of the
     * component to which it is added. This lets us add the Position 
     * component twice as trait, for the lerp start and lerp stop values. */
    ecs_set_trait_tag(world, e, LerpStart, Position, {
        .x = 1,
        .y = 2
    });

    ecs_set_trait_tag(world, e, LerpStop, Position, {
        .x = 10,
        .y = 20
    });

    /* The entity now has three instances of Position, the original component,
     * the LerpStart value and the LerpStop value. */

    ecs_set_target_fps(world, 4);

    while (ecs_progress(world, 0)) { }

    return ecs_fini(world);
}
