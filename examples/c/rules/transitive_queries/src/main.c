#include <transitive_queries.h>
#include <stdio.h>

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

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // LocatedIn relationship
    ecs_entity_t LocatedIn = ecs_new_id(ecs);
    ecs_add_id(ecs, LocatedIn, EcsTransitive);

    // Tags
    ECS_TAG(ecs, Planet);
    ECS_TAG(ecs, Continent);
    ECS_TAG(ecs, Country);
    ECS_TAG(ecs, State);
    ECS_TAG(ecs, City);
    ECS_TAG(ecs, Person);

    // Populate the store with locations
    ecs_entity_t earth = ecs_new_entity(ecs, "Earth");
    ecs_add(ecs, earth, Planet);

    // Continents
    ecs_entity_t north_america = ecs_new_entity(ecs, "NorthAmerica");
    ecs_add(ecs, north_america, Continent);
    ecs_add_pair(ecs, north_america, LocatedIn, earth);

    ecs_entity_t europe = ecs_new_entity(ecs, "Europe");
    ecs_add(ecs, europe, Continent);
    ecs_add_pair(ecs, europe, LocatedIn, earth);

    // Countries
    ecs_entity_t united_states = ecs_new_entity(ecs, "UnitedStates");
    ecs_add(ecs, united_states, Country);
    ecs_add_pair(ecs, united_states, LocatedIn, north_america);

    ecs_entity_t netherlands = ecs_new_entity(ecs, "Netherlands");
    ecs_add(ecs, netherlands, Country);
    ecs_add_pair(ecs, netherlands, LocatedIn, europe);

    // States
    ecs_entity_t california = ecs_new_entity(ecs, "California");
    ecs_add(ecs, california, State);
    ecs_add_pair(ecs, california, LocatedIn, united_states);

    ecs_entity_t washington = ecs_new_entity(ecs, "Washington");
    ecs_add(ecs, washington, State);
    ecs_add_pair(ecs, washington, LocatedIn, united_states);

    ecs_entity_t noord_holland = ecs_new_entity(ecs, "NoordHolland");
    ecs_add(ecs, noord_holland, State);
    ecs_add_pair(ecs, noord_holland, LocatedIn, netherlands);

    // Cities
    ecs_entity_t san_francisco = ecs_new_entity(ecs, "SanFrancisco");
    ecs_add(ecs, san_francisco, City);
    ecs_add_pair(ecs, san_francisco, LocatedIn, california);

    ecs_entity_t seattle = ecs_new_entity(ecs, "Seattle");
    ecs_add(ecs, seattle, City);
    ecs_add_pair(ecs, seattle, LocatedIn, washington);

    ecs_entity_t amsterdam = ecs_new_entity(ecs, "Amsterdam");
    ecs_add(ecs, amsterdam, City);
    ecs_add_pair(ecs, amsterdam, LocatedIn, noord_holland);

    // Inhabitants
    ecs_entity_t bob = ecs_new_entity(ecs, "Bob");
    ecs_add(ecs, bob, Person);
    ecs_add_pair(ecs, bob, LocatedIn, san_francisco);

    ecs_entity_t alice = ecs_new_entity(ecs, "Alice");
    ecs_add(ecs, alice, Person);
    ecs_add_pair(ecs, alice, LocatedIn, seattle);

    ecs_entity_t job = ecs_new_entity(ecs, "Job");
    ecs_add(ecs, job, Person);
    ecs_add_pair(ecs, job, LocatedIn, amsterdam);

    // Create a query that finds the countries persons live in. Note that these
    // have not been explicitly added to the Person entities, but because the
    // LocatedIn is transitive, the rule engine will traverse the relationship
    // until it found something that is a country.
    // 
    // The equivalent of this query in the DSL is:
    //   Person, (LocatedIn, $Location), Country($Location)
    ecs_rule_t *r = ecs_rule_init(ecs, &(ecs_filter_desc_t){
        .terms = {
            { .id = Person },
            { 
                .pred.entity = LocatedIn, 
                .obj = { .name = (char*)"Location", .var = EcsVarIsVariable },
            },
            {
                .pred.entity = Country, 
                .subj = { .name = (char*)"Location", .var = EcsVarIsVariable },
            },
        }
    });

    // Lookup the index of the variable. This will let us quickly lookup its
    // value while we're iterating.
    int location_var = ecs_rule_find_var(r, "Location");

    // Iterate the rule
    ecs_iter_t it = ecs_rule_iter(ecs, r);
    while (ecs_rule_next(&it)) {
        ecs_entity_t location = ecs_iter_get_var(&it, location_var);
        for (int i = 0; i < it.count; i ++) {
            printf("%s lives in %s\n",
                ecs_get_name(ecs, it.entities[i]),
                ecs_get_name(ecs, location));
        }
    }

    ecs_rule_fini(r);

    // Output:
    //   Bob lives in UnitedStates
    //   Alice lives in UnitedStates
    //   Job lives in Netherlands

    return ecs_fini(ecs);
}
