#include <dser_type.h>

#include <stdio.h>


int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    typedef struct {
        ecs_vec_t v1;
        ecs_vec_t v2;
    } T;
    ecs_entity_t F64Vector = ecs_vector(ecs, {.entity = ecs_entity(ecs, { .name = "F64Vector" }), .type = ecs_id(ecs_f64_t)});
    ecs_entity_t t = ecs_struct_init(ecs, &(ecs_struct_desc_t){
        .entity = ecs_entity(ecs, {.name = "T"}),
        .members = {
            {.name = "v1", .type = F64Vector,},
            {.name = "v2", .type = F64Vector,},
        }
    });


    // Collection in of bounds:
    {
        T value = {0};
        ecs_vec_init(NULL, &value.v1, sizeof(ecs_f64_t), 6);
        ecs_vec_init(NULL, &value.v2, sizeof(ecs_f64_t), 6);
        char const * json = "{\"v1\": [1.123456789, 2.123, 6.6], \"v2\": [5.123, 6.123]}\"";
        const char *ptr = ecs_ptr_from_json(ecs, t, &value, json, NULL);
        if(ptr)
        {
            char *expr = ecs_ptr_to_json(ecs, t, &value);
            printf("json: %s\n", json);
            printf("expr: %s\n", expr);
            /*
            json: {"v1": [1.123456789, 2.123], "v2": [5.123, 6.123]}"
            expr: {"v1":[1.123456789, 2.123], "v2":[5.123, 6.123]}
            */
        }
    }


    // Collection out of bounds when the ecs_vec_t is initilized too small
    {
        T value = {0};
        ecs_vec_init(NULL, &value.v1, sizeof(ecs_f64_t), 2);
        ecs_vec_init(NULL, &value.v2, sizeof(ecs_f64_t), 6);
        char const * json = "{\"v1\": [1.123456789, 2.123, 6.6], \"v2\": [5.123, 6.123]}\"";
        const char *ptr = ecs_ptr_from_json(ecs, t, &value, json, NULL);
        if(ptr)
        {
            char *expr = ecs_ptr_to_json(ecs, t, &value);
            printf("json: %s\n", json);
            printf("expr: %s\n", expr);
            /*
            json: {"v1": [1.123456789, 2.123], "v2": [5.123, 6.123]}"
            expr: {"v1":[1.123456789, 2.123], "v2":[5.123, 6.123]}
            error: cursor.c: 239: out of collection bounds (2)
            */
        }
    }


    ecs_fini(ecs);
}
