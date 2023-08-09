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



    T value = {{ 0 }};
    ecs_vec_init(NULL, &value.v1, sizeof(ecs_f64_t), 0);
    ecs_vec_init(NULL, &value.v2, sizeof(ecs_f64_t), 0);
    printf("v1: %i of %i\n", ecs_vec_count(&value.v1), ecs_vec_size(&value.v1));
    printf("v2: %i of %i\n", ecs_vec_count(&value.v2), ecs_vec_size(&value.v2));
    char const * json_input = "{\"v1\": [], \"v2\": [5.123, 6.123, 1, 3, 6, 7, 5, 8, 3, 7, 4]}";
    const char *ptr = ecs_ptr_from_json(ecs, t, &value, json_input, NULL);
    printf("v1: %i of %i\n", ecs_vec_count(&value.v1), ecs_vec_size(&value.v1));
    printf("v2: %i of %i\n", ecs_vec_count(&value.v2), ecs_vec_size(&value.v2));
    if(ptr)
    {
        char *json_output = ecs_ptr_to_json(ecs, t, &value);
        printf("json_input: %s\n", json_input);
        //json_input: {"v1": [], "v2": [5.123, 6.123, 1, 3, 6, 7, 5, 8, 3, 7, 4]}
        printf("json_output: %s\n", json_output);
        //json_output: {"v1":[], "v2":[5.123, 6.123, 1, 3, 6, 7, 5, 8, 3, 7, 4]}
    }
    

    ecs_fini(ecs);
}
