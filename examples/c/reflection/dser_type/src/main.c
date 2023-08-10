#include <dser_type.h>

#include <stdio.h>


typedef struct {
    ecs_vec_t v_f64_0; // vector<ecs_f64_t>
    ecs_vec_t v_f64_1; // vector<ecs_f64_t>
    ecs_vec_t v_i32_0; // vector<ecs_i32_t>
    ecs_vec_t v_i32_1; // vector<ecs_i32_t>
} MiscellaneousVectors;

void MiscellaneousVectors_print_status(MiscellaneousVectors * v) {
    printf("v_f64_0: %i of %i\n", ecs_vec_count(&v->v_f64_0), ecs_vec_size(&v->v_f64_0));
    printf("v_f64_1: %i of %i\n", ecs_vec_count(&v->v_f64_1), ecs_vec_size(&v->v_f64_1));
    printf("v_i32_0: %i of %i\n", ecs_vec_count(&v->v_i32_0), ecs_vec_size(&v->v_i32_0));
    printf("v_i32_1: %i of %i\n", ecs_vec_count(&v->v_i32_1), ecs_vec_size(&v->v_i32_1));
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, MiscellaneousVectors);

    ecs_entity_t VectorF64 = ecs_vector(ecs, {.entity = ecs_entity(ecs, { .name = "VectorF64" }), .type = ecs_id(ecs_f64_t)});
    ecs_entity_t VectorI32 = ecs_vector(ecs, {.entity = ecs_entity(ecs, { .name = "VectorI32" }), .type = ecs_id(ecs_i32_t)});
    ecs_entity_t t = ecs_struct_init(ecs, &(ecs_struct_desc_t){
        .entity = ecs_id(MiscellaneousVectors),
        .members = {
            {.name = "v_f64_0", .type = VectorF64,},
            {.name = "v_f64_1", .type = VectorF64,},
            {.name = "v_i32_0", .type = VectorI32,},
            {.name = "v_i32_1", .type = VectorI32,},
        }
    });

    MiscellaneousVectors value;
    // ecs_vec_t needs to be initilized before calling ecs_ptr_from_json:
    ecs_vec_init_t(NULL, &value.v_f64_0, ecs_f64_t, 0);
    ecs_vec_init_t(NULL, &value.v_f64_1, ecs_f64_t, 100);
    ecs_vec_init_t(NULL, &value.v_i32_0, ecs_i32_t, 200);
    ecs_vec_init_t(NULL, &value.v_i32_1, ecs_i32_t, 0);
    MiscellaneousVectors_print_status(&value);
    char const * json_input = "{\"v_f64_1\": [0.00000000042], \"v_i32_0\":[42.42], \"v_i32_1\": [5.123, 6.123, 1, 3, 6, 7, 5, 8, 3, 7, 4], \"v_f64_0\":[]}";
    printf("json_input: %s\n", json_input);
    const char *ptr = ecs_ptr_from_json(ecs, t, &value, json_input, NULL);
    MiscellaneousVectors_print_status(&value);

    // If json parse successful:
    if(ptr)
    {
        char *json_output = ecs_ptr_to_json(ecs, t, &value);
        printf("json_output: %s\n", json_output);
        //json_output: {"v_f64_0":[], "v_f64_1":[0.0000000004], "v_i32_0":[42], "v_i32_1":[5, 6, 1, 3, 6, 7, 5, 8, 3, 7, 4]}
    }

    ecs_vec_fini_t(NULL, &value.v_f64_0, ecs_f64_t);
    ecs_vec_fini_t(NULL, &value.v_f64_1, ecs_f64_t);
    ecs_vec_fini_t(NULL, &value.v_i32_0, ecs_i32_t);
    ecs_vec_fini_t(NULL, &value.v_i32_1, ecs_i32_t);
    MiscellaneousVectors_print_status(&value);

    ecs_fini(ecs);
}
