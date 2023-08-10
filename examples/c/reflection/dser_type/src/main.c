#include <dser_type.h>

#include <stdio.h>


typedef struct {
    ecs_vec_t vv_i32; // vector<vector<ecs_i32_t>>
} MiscellaneousVectors;



int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, MiscellaneousVectors);

    ecs_entity_t VectorI32 = ecs_vector(ecs, {.type = ecs_id(ecs_i32_t)});
    ecs_entity_t VectorVectorI32 = ecs_vector(ecs, {.type = VectorI32});
    ecs_entity_t t = ecs_struct_init(ecs, &(ecs_struct_desc_t){
        .entity = ecs_id(MiscellaneousVectors),
        .members = {
            {.name = "vv_i32", .type = VectorVectorI32,},
        }
    });

    MiscellaneousVectors value;
    char const * json_input = "{\"vv_i32\": [[1,2],[3,6]]}";
    printf("json_input: %s\n", json_input);
    const char *ptr = ecs_ptr_from_json(ecs, t, &value, json_input, NULL);

    // If json parse successful:
    if(ptr)
    {
        char *json_output = ecs_ptr_to_json(ecs, t, &value);
        printf("json_output: %s\n", json_output);
    }

    for(int i = 0; i < ecs_vec_count(&value.vv_i32); ++i) {
        ecs_vec_fini_t(NULL, ecs_vec_get_t(&value.vv_i32, ecs_vec_t, i), ecs_i32_t);
    }
    ecs_vec_fini_t(NULL, &value.vv_i32, ecs_vec_t);

    ecs_fini(ecs);

    // Output
    // json_input: {"vv_i32": [[1,2],[3,6]]}
    // json_output: {"vv_i32":[[1, 2], [3, 6]]}
}
