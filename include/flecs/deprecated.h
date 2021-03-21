/**
 * @file deprecated.h
 * @brief Deprecated operations.
 */

#ifndef FLECS_DEPRECATED_H
#define FLECS_DEPRECATED_H

#ifdef __cplusplus
extern "C" {
#endif

#define EcsSingleton (ECS_HI_COMPONENT_ID + 26)

#define ecs_trait(comp, trait) (ECS_PAIR | ecs_entity_t_comb(comp, trait))

/** Add a trait
 * This operation adds a trait from an entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity for which to remove the trait.
 * @param trait The trait to remove.
 */
#define ecs_add_trait(world, entity, component, trait)\
    ecs_add_entity(world, entity, ecs_trait(component, trait))

/** Remove a trait
 * This operation removes a trait from an entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity for which to remove the trait.
 * @param trait The trait to remove.
 */
#define ecs_remove_trait(world, entity, component, trait)\
    ecs_remove_entity(world, entity, ecs_trait(component, trait))

/** Test if an entity has a trait.
 * This operation returns true if the entity has the provided trait for the
 * specified component in its type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity.
 * @param trait The entity.
 * @return True if the entity has the trait, false if not.
 */
#define ecs_has_trait(world, entity, component, trait)\
    ecs_has_entity(world, entity, ecs_trait(component, trait))


#ifndef FLECS_LEGACY
/** Set trait for component. 
 * This operation adds a trait for an entity and component. Traits can be added
 * multiple times to the same entity, as long as it is for different components.
 *
 * Traits can be matched with systems by providing the PAIR role to the 
 * trait component in the system signature. A system will match multiple times
 * with the same entity if the trait is added for multiple components.
 *
 * * This operation can only be used with traits that are components.
 *
 * @param world The world.
 * @param e The entity.
 * @param component The component for which to add the trait.
 * @param trait The trait to add.
 */
#define ecs_set_trait(world, entity, component, trait, ...)\
    ecs_set_ptr_w_entity(world, entity, ecs_trait(ecs_typeid(component), ecs_typeid(trait)), sizeof(trait), &(trait)__VA_ARGS__)


/** Set tag trait for component. 
 * This operation is similar to ecs_set_trait, but is used for trait tags. When
 * a trait tag is set on an entity, the trait type is not used (tags have no
 * type) and instead the component type is used.
 *
 * This operation can only be used with traits that are not components.
 *
 * @param world The world.
 * @param e The entity.
 * @param component The component for which to add the trait.
 * @param trait The trait to add.
 */
#define ecs_set_trait_tag(world, entity, trait, component, ...)\
    ecs_set_ptr_w_entity(world, entity, ecs_trait(ecs_typeid(component), trait), sizeof(component), &(component)__VA_ARGS__)

#endif

/** Get trait for component. 
 * This operation obtains the value of a trait for a componetn that has been 
 * added by ecs_set_trait.
 *
 * @param world The world.
 * @param e The entity.
 * @param component The component to which the trait was added.
 * @param trait The trait that was added.
 */
#define ecs_get_trait(world, entity, component, trait)\
    ((trait*)ecs_get_w_entity(world, entity, ecs_trait(ecs_typeid(component), ecs_typeid(trait))))

/** Get trait tag for component. 
 * This operation obtains the value of a trait for a componetn that has been 
 * added by ecs_set_trait.
 *
 * @param world The world.
 * @param e The entity.
 * @param trait The trait that was added.
 * @param component The component to which the trait was added.
 */
#define ecs_get_trait_tag(world, entity, trait, component)\
    ((component*)ecs_get_w_entity(world, entity, ecs_trait(ecs_typeid(component), trait)))

#ifdef __cplusplus
}
#endif

#endif
