#include <api.h>

static
void MonitorEntityDummy(ecs_rows_t* rows) {

}

static
void MonitorEntityInstance(ecs_rows_t* rows) {

}

void Set_w_data_monitored_1_column_3_rows_w_entities() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add(world, 5000, Position);
    //ecs_set(world, 5000, EcsId, { "e_1" });
    
    ecs_add(world, 5001, Position, Velocity);
    ecs_inherit(world, 5001, 5000);
    ecs_adopt(world, 5001, 5000);
    //ECS_ENTITY(world, e_2, Position);
    //ECS_ENTITY(world, e_3, INSTANCEOF | e_2);
    ECS_SYSTEM(world, MonitorEntityInstance, EcsOnUpdate, Position);

    //ECS_SYSTEM(world, MonitorEntityDummy, EcsOnUpdate, e_1.Position);



    ecs_entity_t e = ecs_set_w_data(world, &(ecs_table_data_t){
        .column_count = 1,
            .row_count = 3,
            .entities = (ecs_entity_t[]){ 5000, 5001, 5002 },
            .components = (ecs_entity_t[]){ ecs_entity(Position) },
            .columns = (ecs_table_columns_t[]){
                (Position[]) {
                    {10, 20},
                    {11, 21},
                    {12, 22}
                }
        }
    });

    test_assert(e != 0);
    test_int(ecs_count(world, Position), 3);

    int i;
    for (i = 0; i < 3; i++) {
        ecs_entity_t e = 5000 + i;
        test_assert(ecs_has(world, e, Position));
        Position* p = ecs_get_ptr(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10 + i);
        test_int(p->y, 20 + i);
    }

    ecs_fini(world);
}
