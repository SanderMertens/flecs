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
    auto earth = ecs.entity("Earth")
        .add<Planet>();

    // Continents
    auto north_america = ecs.entity("NorthAmerica")
        .add<Continent>()
        .add<LocatedIn>(earth);

    auto europe = ecs.entity("Europe")
        .add<Continent>()
        .add<LocatedIn>(earth);

    // Countries
    auto united_states = ecs.entity("UnitedStates")
        .add<Country>()
        .add<LocatedIn>(north_america);

    auto netherlands = ecs.entity("Netherlands")
        .add<Country>()
        .add<LocatedIn>(europe);

    // States
    auto california = ecs.entity("California")
        .add<State>()
        .add<LocatedIn>(united_states);

    auto washington = ecs.entity("Washington")
        .add<State>()
        .add<LocatedIn>(united_states);

    auto noord_holland = ecs.entity("NoordHolland")
        .add<State>()
        .add<LocatedIn>(netherlands);

    // Cities
    auto san_francisco = ecs.entity("SanFrancisco")
        .add<City>()
        .add<LocatedIn>(california);

    auto seattle = ecs.entity("Seattle")
        .add<City>()
        .add<LocatedIn>(washington);

    auto amsterdam = ecs.entity("Amsterdam")
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
    // LocatedIn is transitive, the rule engine will traverse the relationship
    // until it found something that is a country.
    // 
    // The equivalent of this query in the DSL is:
    //   Person, (LocatedIn, $Location), Country($Location)
    auto r = ecs.rule_builder()
        .term<Person>()
        .term<LocatedIn>().obj().var("Location")
        .term<Country>().subj().var("Location")
        .build();
    
    // Lookup the index of the variable. This will let us quickly lookup its
    // value while we're iterating.
    int location_var = r.find_var("Location");

    // Iterate the rule
    r.each([&](flecs::iter& it, size_t index) {
        std::cout 
            << it.entity(index).name()
            << " lives in " << it.get_var(location_var).name()
            << "\n";
    });

    // Rules need to be explicitly deleted.
    r.destruct();

    // Output:
    //   Bob lives in UnitedStates
    //   Alice lives in UnitedStates
    //   Job lives in Netherlands
}
