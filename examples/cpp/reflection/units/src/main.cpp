#include <units.h>
#include <iostream>

struct WeatherStation {
    double temperature;
    double pressure;
    double precipitation;
};

void print_value(const flecs::meta::cursor& cur) {
    // Get unit entity and component
    flecs::entity u = cur.get_unit();
    const flecs::Unit *u_data = u.get<flecs::Unit>();

    // Print value with unit symbol
    std::cout << cur.get_member() << ": " << cur.get_float() 
        << " " << u_data->symbol << "\n";
}

int main(int, char *[]) {
    flecs::world ecs;

    // Import units module.
    ecs.import<flecs::units>();

    // Register reflection data with units. This can improve the way information
    // is visualized in tools, such as the explorer.
    ecs.component<WeatherStation>()
        .member<double, flecs::units::temperature::Celsius>("temperature")
        .member<double, flecs::units::pressure::Bar>("pressure")
        .member<double, flecs::units::length::MilliMeters>("precipitation");

    auto e = ecs.entity().set<WeatherStation>({24, 1.2, 0.5});

    // Use cursor API to print values with units
    WeatherStation *ptr = e.get_mut<WeatherStation>();
    auto cur = ecs.cursor<WeatherStation>(ptr);
    cur.push();
    print_value(cur);
    cur.next();
    print_value(cur);
    cur.next();
    print_value(cur);
    cur.pop();

    // Output:
    //  temperature: 24 °C
    //  pressure: 1.2 bar
    //  precipitation: 0.5 mm
}
