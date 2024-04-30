#include <transitive_queries.h>
#include <iostream>

// Transitive relationships make it possible to tell the ECS that if an entity
// has a relationship (R, X) and X has relationship (R, Y), the entity should be
// treated as if it also has (R, Y). In practice this is useful for expressing
// things like:
//
// Bob lives in SanFrancisco
// San Francisco is located in the United States
// Therefore Bob also lives in the United States.
//
// An example of transitivity can be seen in the component_inheritance example.
// This example uses the builtin IsA relationship, which is transitive. This
// example shows how to achieve similar behavior with a user-defined relationship.

struct LocatedIn { };
struct Planet { };
struct Continent { };
struct Country { };
struct State { };
struct City { };
struct Person { };

int main(int, char *[]) {
    flecs::world ecs;

    // Register the LocatedIn relationship as transitive
    ecs.component<LocatedIn>().add(flecs::Transitive);

    // Populate the store with locations
    flecs::entity earth = ecs.entity("Earth")
        .add<Planet>();

    // Continents
    flecs::entity north_america = ecs.entity("NorthAmerica")
        .add<Continent>()
        .add<LocatedIn>(earth);

    flecs::entity europe = ecs.entity("Europe")
        .add<Continent>()
        .add<LocatedIn>(earth);

    // Countries
    flecs::entity united_states = ecs.entity("UnitedStates")
        .add<Country>()
        .add<LocatedIn>(north_america);

    flecs::entity netherlands = ecs.entity("Netherlands")
        .add<Country>()
        .add<LocatedIn>(europe);

    // States
    flecs::entity california = ecs.entity("California")
        .add<State>()
        .add<LocatedIn>(united_states);

    flecs::entity washington = ecs.entity("Washington")
        .add<State>()
        .add<LocatedIn>(united_states);

    flecs::entity noord_holland = ecs.entity("NoordHolland")
        .add<State>()
        .add<LocatedIn>(netherlands);

    // Cities
    flecs::entity san_francisco = ecs.entity("SanFrancisco")
        .add<City>()
        .add<LocatedIn>(california);

    flecs::entity seattle = ecs.entity("Seattle")
        .add<City>()
        .add<LocatedIn>(washington);

    flecs::entity amsterdam = ecs.entity("Amsterdam")
        .add<City>()
        .add<LocatedIn>(noord_holland);

    // Inhabitants
    ecs.entity("Bob")
        .add<Person>()
        .add<LocatedIn>(san_francisco);

    ecs.entity("Alice")
        .add<Person>()
        .add<LocatedIn>(seattle);

    ecs.entity("Job")
        .add<Person>()
        .add<LocatedIn>(amsterdam);

    // Create a query that finds the countries persons live in. Note that these
    // have not been explicitly added to the Person entities, but because the
    // LocatedIn is transitive, the query engine will traverse the relationship
    // until it found something that is a country.
    // 
    // The equivalent of this query in the DSL is:
    //   Person, (LocatedIn, $location), Country($location)
    flecs::query<> q = ecs.query_builder()
        .with<Person>()
        .with<LocatedIn>("$location")
        .with<Country>().src("$location")
        .build();
    
    // Lookup the index of the variable. This will let us quickly lookup its
    // value while we're iterating.
    int location_var = q.find_var("location");

    // Iterate the query
    q.each([&](flecs::iter& it, size_t index) {
        std::cout 
            << it.entity(index).name()
            << " lives in " << it.get_var(location_var).name()
            << "\n";
    });

    // Output:
    //   Bob lives in UnitedStates
    //   Alice lives in UnitedStates
    //   Job lives in Netherlands
}
