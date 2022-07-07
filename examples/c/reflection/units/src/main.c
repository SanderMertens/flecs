#include <units.h>
#include <stdio.h>

typedef struct {
    double temperature;
    double pressure;
    double precipitation;
} WeatherStation;

void print_value(ecs_world_t *world, const ecs_meta_cursor_t *cur) {
    // Get unit entity and component
    ecs_entity_t u = ecs_meta_get_unit(cur);
    const EcsUnit *u_data = ecs_get(world, u, EcsUnit);
    
    // Print value with unit symbol
    printf("%s: %.1f %s\n", ecs_meta_get_member(cur), ecs_meta_get_float(cur),
        u_data->symbol);
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Import units module.
    ECS_IMPORT(ecs, FlecsUnits);

    ECS_COMPONENT(ecs, WeatherStation);

    // Register reflection data with units. This can improve the way information
    // is visualized in tools, such as the explorer.
    ecs_struct_init(ecs, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(WeatherStation),
        .members = {
            {
                .name = "temperature", 
                .type = ecs_id(ecs_f64_t), 
                .unit = EcsCelsius 
            },
            { 
                .name = "pressure", 
                .type = ecs_id(ecs_f64_t), 
                .unit = EcsBar 
            },
            {
                .name = "precipitation", 
                .type = ecs_id(ecs_f64_t), 
                .unit = EcsMilliMeters 
            },
        }
    });

    ecs_entity_t e = ecs_set(ecs, 0, WeatherStation, {24, 1.2, 0.5});

    // Use cursor API to print values with units
    WeatherStation *ptr = ecs_get_mut(ecs, e, WeatherStation);
    ecs_meta_cursor_t cur = ecs_meta_cursor(ecs, ecs_id(WeatherStation), ptr);

    ecs_meta_push(&cur);
    print_value(ecs, &cur);
    ecs_meta_next(&cur);
    print_value(ecs, &cur);
    ecs_meta_next(&cur);
    print_value(ecs, &cur);
    ecs_meta_pop(&cur);

    return ecs_fini(ecs);

    // Output:
    //  temperature: 24.0 °C
    //  pressure: 1.2 bar
    //  precipitation: 0.5 mm
}
