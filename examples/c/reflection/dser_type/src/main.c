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

    {
        T value = {0};
        ecs_vec_init(NULL, &value.v1, sizeof(ecs_f64_t), 6);
        ecs_vec_init(NULL, &value.v2, sizeof(ecs_f64_t), 6);
        char const * test = "{\"v1\": [1.123, 2.123], \"v2\": [5.123, 6.123]}\"";
        const char *ptr = ecs_ptr_from_json(ecs, t, &value, test, NULL);
    }

    {
        T value = {0};
        ecs_vec_init(NULL, &value.v1, sizeof(ecs_f64_t), 6);
        ecs_vec_init(NULL, &value.v2, sizeof(ecs_f64_t), 6);
        ecs_vec_append_t(NULL, &value.v1, ecs_f64_t)[0] = 1.0001;
        ecs_vec_append_t(NULL, &value.v1, ecs_f64_t)[0] = 2.0002;
        ecs_vec_append_t(NULL, &value.v2, ecs_f64_t)[0] = 5.0003;
        ecs_vec_append_t(NULL, &value.v2, ecs_f64_t)[0] = 6.0007;

        char *expr = ecs_ptr_to_json(ecs, t, &value);
        printf("expr: %s\n", expr);
        //expr: {"v1":[1.0001, 2.0002], "v2":[5.0003, 6.0007]}
    }

    ecs_fini(ecs);
}
