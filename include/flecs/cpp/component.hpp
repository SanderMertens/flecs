////////////////////////////////////////////////////////////////////////////////
//// Register component, provide global access to component handles / metadata
////////////////////////////////////////////////////////////////////////////////

namespace flecs 
{

namespace _ 
{
    
    // Trick to obtain typename from type, as described here
    // https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
    //
    // The code from the link has been modified to work with more types, and across
    // multiple compilers.
    //
    struct name_util {
        /* Remove parts from typename that aren't needed for component name */
        static void trim_name(char *typeName) {
            ecs_size_t len = ecs_os_strlen(typeName);
            
            /* Remove 'const' */
            ecs_size_t const_len = ecs_os_strlen("const ");
            if ((len > const_len) && !ecs_os_strncmp(typeName, "const ", const_len)) {
                ecs_os_memmove(typeName, typeName + const_len, len - const_len);
                typeName[len - const_len] = '\0';
                len -= const_len;
            }

            /* Remove 'struct' */
            ecs_size_t struct_len = ecs_os_strlen("struct ");
            if ((len > struct_len) && !ecs_os_strncmp(typeName, "struct ", struct_len)) {
                ecs_os_memmove(typeName, typeName + struct_len, len - struct_len);
                typeName[len - struct_len] = '\0';
                len -= struct_len;
            }

            /* Remove 'class' */
            ecs_size_t class_len = ecs_os_strlen("class ");
            if ((len > class_len) && !ecs_os_strncmp(typeName, "class ", class_len)) {
                ecs_os_memmove(typeName, typeName + class_len, len - class_len);
                typeName[len - class_len] = '\0';
                len -= class_len;
            }            

            while (typeName[len - 1] == ' ' ||
                   typeName[len - 1] == '&' ||
                   typeName[len - 1] == '*') 
            {
                len --;
                typeName[len] = '\0';
            }

            /* Remove const at end of string */
            if (len > const_len) {
                if (!ecs_os_strncmp(&typeName[len - const_len], " const", const_len)) {
                    typeName[len - const_len] = '\0';
                }
            }
        }
    };

// Compiler-specific conversion from __PRETTY_FUNCTION__ to component name. 
// This code uses a trick that instantiates a function for the component type. 
// Then __PRETTY_FUNCTION__ is used to obtain the name of the function. Because
// the result of __PRETTY_FUNCTION__ is not standardized, there are different
// implementations for clang, gcc and msvc. Code that uses a different compiler
// needs to register component names explicitly.
#if defined(__clang__)
  static const unsigned int FRONT_SIZE = sizeof("static const char* flecs::_::name_helper<") - 1u;
  static const unsigned int BACK_SIZE = sizeof(">::name() [T = ]") - 1u;
 
  template <typename T>
  struct name_helper
  {
    static const char* name(void) {
      static const size_t size = (sizeof(__PRETTY_FUNCTION__) - FRONT_SIZE - BACK_SIZE) / 2 + 1u;
      static char typeName[size + 6] = {};
      memcpy(typeName, __PRETTY_FUNCTION__ + FRONT_SIZE, size - 1u);
      name_util::trim_name(typeName);
      return typeName;
    }
  };    
#elif defined(__GNUC__)
  static const unsigned int FRONT_SIZE = sizeof("static const char* flecs::_::name_helper<T>::name() [with T = ") - 1u;
  static const unsigned int BACK_SIZE = sizeof("]") - 1u;
 
  template <typename T>
  struct name_helper
  {
    static const char* name(void) {
      static const size_t size = sizeof(__PRETTY_FUNCTION__) - FRONT_SIZE - BACK_SIZE;
      static char typeName[size + 6] = {};
      memcpy(typeName, __PRETTY_FUNCTION__ + FRONT_SIZE, size - 1u);
      name_util::trim_name(typeName);
      return typeName;
    }
  };
#elif defined(_WIN32)
  static const unsigned int FRONT_SIZE = sizeof("flecs::_::name_helper<") - 1u;
  static const unsigned int BACK_SIZE = sizeof(">::name") - 1u;
 
  template <typename T>
  struct name_helper
  {
    static const char* name(void) {
      static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
      static char typeName[size + 6] = {};
      memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);
      name_util::trim_name(typeName);
      return typeName;
    }
  };
#else
#error "implicit component registration not supported"
#endif

// Translate a typename into a language-agnostic identifier. This allows for
// registration of components/modules across language boundaries.
template <typename T>
struct symbol_helper
{
    static char* symbol(void) {
        const char *name = name_helper<T>::name();

        // Symbol is same as name, but with '::' replaced with '.'
        char *ptr, *sym = ecs_os_strdup(name);
        ecs_size_t i, len = ecs_os_strlen(sym);
        ptr = sym;
        for (i = 0, ptr = sym; i < len && *ptr; i ++, ptr ++) {
            if (*ptr == ':') {
                sym[i] = '.';
                ptr ++;
            } else {
                sym[i] = *ptr;
            }
        }

        sym[i] = '\0';

        return sym;
    }
};

// The following functions are lifecycle callbacks that are automatically
// registered with flecs to ensure component lifecycle is handled correctly. Not
// all types require this, yet callbacks are registered by default, which
// introduces some overhead when working with components.
//
// An application can optimize this by explicitly registering a component as a
// plain old datatype, with world.pod_component<T>().

template <typename T>
void component_ctor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)entity_ptr;
    (void)size;
    (void)ctx;

    ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *t_ptr = static_cast<T*>(ptr);
    
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&t_ptr[i], T);
    }
} 

template <typename T>
void component_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)entity_ptr;
    (void)size;
    (void)ctx;

    ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *t_ptr = static_cast<T*>(ptr);
    
    for (int i = 0; i < count; i ++) {
        t_ptr[i].~T();
    }
}

template <typename T>
void component_copy(
    ecs_world_t *world,
    ecs_entity_t component,    
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    const void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)dst_entity;
    (void)src_entity;
    (void)size;
    (void)ctx;

    ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *t_dst_ptr = static_cast<T*>(dst_ptr);
    const T *t_src_ptr = static_cast<const T*>(src_ptr);
    
    for (int i = 0; i < count; i ++) {
        t_dst_ptr[i] = t_src_ptr[i];
    }
}

template <typename T>
void component_move(
    ecs_world_t *world,
    ecs_entity_t component,    
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)dst_entity;
    (void)src_entity;
    (void)size;
    (void)ctx;

    ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *t_dst_ptr = static_cast<T*>(dst_ptr);
    T *t_src_ptr = static_cast<T*>(src_ptr);
    
    for (int i = 0; i < count; i ++) {
        t_dst_ptr[i] = std::move(t_src_ptr[i]);
    }
}

// Register component lifecycle callbacks with flecs.
template<typename T>
void register_lifecycle_actions(
    ecs_world_t *world,
    ecs_entity_t component,
    bool ctor,
    bool dtor,
    bool copy,
    bool move)
{
    if (!ecs_component_has_actions(world, component)) {
        EcsComponentLifecycle cl{};
        if (ctor) {
            cl.ctor = _::component_ctor<typename base_type<T>::type>;
        }
        if (dtor) {
            cl.dtor = _::component_dtor<typename base_type<T>::type>;
        }
        if (copy) {
            cl.copy = _::component_copy<typename base_type<T>::type>;
        }
        if (move) {
            cl.move = _::component_move<typename base_type<T>::type>;
        }

        ecs_set_component_actions_w_entity( world, component, &cl);
    }
}

// Class that holds component identifier and name.
// This class is used by most component operations to obtain the component id in
// a transparent way for the application. The component id is stored in a static
// member of a class, and is shared by all worlds in an application. As such,
// each world in an application must (and will) have the same id.
//
// Because of how global (templated) variables are instantiated, it is possible
// that different instances for the same component exist across different
// translation units. This is handled transparently by flecs. When a component
// id is requested from the cpp_type class, but the id is uninitialized, a 
// lookup by name will be performed for the component on the world, which will 
// return the id with which the component was already registered. This means 
// component identifiers are eventually consistent across translation units.
//
// When a component id is requested for a world that had not yet registered the
// component, it will be registered with the existing identifier. This ensures
// that multiple worlds in the same application use the same component ids.
//
// There are a few limitations of this approach.
//
// 1) When two worlds register components in different orders, it is possible
//    that different components receive the same identifier. When a world 
//    attempts to re-register a component with a different identifier, an error
//    will be thrown. To prevent this from happening, worlds should register
//    components in the same order.
//
// 2) Component names are used to ensure that different translation units use
//    the same identifiers. If a component is registered under different names
//    in the same application, id conflicts can occur.
//
// Known issues:
//
// It seems like component registration does not always work correctly in Unreal
// engine when recreating a world. A plausible cause for this is the hot 
// reloading of dynamic libraries by the engine. A workaround for this issue is
// to call flecs::_::cpp_type<T>::reset() before recreating the world.
// This will reset the global component state and avoids conflicts. The exact
// cause of the issue is investigated here: 
//   https://github.com/SanderMertens/flecs/issues/293

template <typename T>
class cpp_type final {
public:
    // Initialize component identifier
    static void init(world_t* world, entity_t entity, bool allow_tag = true) {
        // If an identifier was already set, check for consistency
        if (s_id) {
            // If an identifier was registered, a name should've been registered
            // as well.
            ecs_assert(s_name.c_str() != nullptr, ECS_INTERNAL_ERROR, NULL);

            // A component cannot be registered using a different identifier.
            ecs_assert(s_id == entity, ECS_INCONSISTENT_COMPONENT_ID, 
                _::name_helper<T>::name());

            ecs_assert(allow_tag == s_allow_tag, ECS_INTERNAL_ERROR, NULL);

            // Component was already registered and data is consistent with new
            // identifier, so nothing else to be done.
            return;
        }

        // Component wasn't registered yet, set the values. Register component
        // name as the fully qualified flecs path.
        char *path = ecs_get_fullpath(world, entity);
        s_id = entity;
        s_name = flecs::string(path);
        s_allow_tag = allow_tag;
    }

    // Obtain a component identifier without registering lifecycle callbacks.
    static entity_t id_no_lifecycle(world_t *world = nullptr, 
        const char *name = nullptr, bool allow_tag = true) 
    {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            const char *n = _::name_helper<T>::name();
            
            if (!name) {
                // If no name was provided, retrieve the name implicitly from
                // the name_helper class.
                name = n;
            }

            s_allow_tag = allow_tag;

            // If no world was provided, we can't implicitly register the 
            // component. While there are a few cases where calling this 
            // function without a world is OK, in general functions should 
            // always provide a world to enable implicit registration.
            ecs_assert(world != nullptr, ECS_COMPONENT_NOT_REGISTERED, name);

            // Create entity with name first. This ensurs that the component is
            // created in the right location in the hierarchy, in case a scoped
            // name is used. This allows registering components with the same
            // name in different namespaces.
            //
            // If the component was already registered for this world, this will 
            // resolve the existing component identifier. This enables 
            // transparent component registeration across multiple translation 
            // units, as long as the same world is used.
            //
            // The last parameter ('true') ensures that when the component was
            // not yet registered, a new component identifier is created. 
            // Component identifiers and entity identifiers are equivalent,
            // though a pool of identifiers at the start of the id range is 
            // reserved for components. This is a performance optimization, as
            // low ids in some parts of the code allow for direct indexing.
            flecs::world w(world);
            flecs::entity result = entity(w, name, true);
            
            // Initialize types with identifier
            cpp_type<typename base_type<T>::type>::init(world, result.id(), allow_tag);
            cpp_type<const typename base_type<T>::type>::init(world, result.id(), allow_tag);
            cpp_type<typename base_type<T>::type*>::init(world, result.id(), allow_tag);
            cpp_type<typename base_type<T>::type&>::init(world, result.id(), allow_tag);

            // Now use the resulting identifier to register the component. Note
            // that the name is not passed into this function, as the entity was
            // already created with the correct name.
            ecs_entity_t entity = ecs_new_component(
                world, result.id(), nullptr, 
                size(), 
                alignment());

            ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);

            // Set the symbol in the Name component to the actual C++ name.
            // Comparing symbols allows for verifying whether a different 
            // component is being registered under the same name. We can't use
            // the name used for registration, because it is possible that a
            // user (erroneously) attempts to register the same datatype with
            // the same name. Without verifying that the actual C++ type name
            // matches, that scenario would go undetected.
            EcsName *name_comp = static_cast<EcsName*>(ecs_get_mut_w_id(
                world, entity, ecs_id(EcsName), NULL));
            char *symbol = symbol_helper<T>::symbol();

            if (name_comp->symbol) {
                ecs_assert( !strcmp(name_comp->symbol, symbol), 
                    ECS_COMPONENT_NAME_IN_USE, name);
                ecs_os_free(symbol);
            } else {
                name_comp->symbol = symbol;
            }
            
            // The identifier returned by the function should be the same as the
            // identifier that was passed in.
            ecs_assert(entity == result.id(), ECS_INTERNAL_ERROR, NULL);

        } else if (world && !ecs_exists(world, s_id)) {
            const char *n = _::name_helper<T>::name();
            
            if (!name) {
                // If no name was provided, retrieve the name implicitly from
                // the name_helper class.
                name = n;
            }

            ecs_entity_t entity = ecs_new_component(
                world, s_id, name,
                size(), 
                alignment());
                
            (void)entity;

            ecs_assert(entity == s_id, ECS_INTERNAL_ERROR, NULL);

            init(world, s_id, allow_tag);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        return s_id;
    }

    // Obtain a component identifier, register lifecycle callbacks if this is
    // the first time the component is used.
    static entity_t id(world_t *world = nullptr, const char *name = nullptr, 
        bool allow_tag = true) 
    {
        // If no id has been registered yet, do it now.
        if (!s_id || (world && !ecs_exists(world, s_id))) {
            // This will register a component id, but will not register 
            // lifecycle callbacks.
            id_no_lifecycle(world, name, allow_tag);

            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's. */
            if (size()) {
                register_lifecycle_actions<T>(world, s_id,
                    true, true, true, true);
            }
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        return s_id;
    }

    // Obtain a component name
    static const char* name(world_t *world = nullptr) {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            id(world);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        // If the id is set, the name should also have been set
        return s_name.c_str();
    }

    // Obtain a component name, don't register lifecycle if the component hadn't
    // been registered yet. While functionally the same could be achieved by
    // first calling id_no_lifecycle() and then name(), this function ensures
    // that the lifecycle callback templates are not instantiated. This allows
    // some types (such as module classes) to be created without a default
    // constructor.
    static const char* name_no_lifecycle(world_t *world = nullptr) {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            id_no_lifecycle(world);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        // Return 
        return s_name.c_str();
    }    

    // Return the type of a component.
    // The type is a vector of component ids. This will return a type with just
    // the current component id.
    static type_t type(world_t *world = nullptr) {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            id(world);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);        

        // Create a type from the component id.
        if (!s_type) {
            s_type = ecs_type_from_entity(world, s_id);
        }

        ecs_assert(s_type != nullptr, ECS_INTERNAL_ERROR, NULL);

        return s_type;
    }

    // Return the size of a component.
    static size_t size() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);
        
        // C++ types that have no members still have a size. Use std::is_empty
        // to check if the type is empty. If so, use 0 for the component size.
        //
        // If s_allow_tag is set to false, the size returned by C++ is used.
        // This is useful in cases where class instances are still required, as
        // is the case with module classes.
        if (s_allow_tag && std::is_empty<T>::value) {
            return 0;
        } else {
            return sizeof(typename base_type<T>::type);
        }
    }

    // Return the alignment of a component.
    static size_t alignment() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        if (size() == 0) {
            return 0;
        } else {
            return alignof(typename base_type<T>::type);
        }
    }

    // Was the component already registered.
    static bool registered() {
        return s_id != 0;
    }

    // This function is only used to test cross-translation unit features. No
    // code other than test cases should invoke this function.
    static void reset() {
        s_id = 0;
        s_type = NULL;
        s_name.clear();
    }

private:
    static entity_t s_id;
    static type_t s_type;
    static flecs::string s_name;
    static flecs::string s_symbol;
    static bool s_allow_tag;
};

// Global templated variables that hold component identifier and other info
template <typename T> entity_t cpp_type<T>::s_id( 0 );
template <typename T> type_t cpp_type<T>::s_type( nullptr );
template <typename T> flecs::string cpp_type<T>::s_name;
template <typename T> bool cpp_type<T>::s_allow_tag( true );

} // namespace _

////////////////////////////////////////////////////////////////////////////////
//// Register a component with flecs
////////////////////////////////////////////////////////////////////////////////

/** Plain old datatype, no lifecycle actions are registered */
template <typename T>
flecs::entity pod_component(const flecs::world& world, const char *name = nullptr, bool allow_tag = true) {
    if (!name) {
        name = _::name_helper<T>::name();
    }

    world_t *world_ptr = world.c_ptr();
    entity_t id = 0;

    if (_::cpp_type<T>::registered()) {
        /* Obtain component id. Because the component is already registered,
         * this operation does nothing besides returning the existing id */
        id = _::cpp_type<T>::id_no_lifecycle(world_ptr, name, allow_tag);

        /* If entity is not empty check if the name matches */
        if (ecs_get_type(world_ptr, id) != nullptr) {
            if (id >= EcsFirstUserComponentId) {
                char *path = ecs_get_path_w_sep(
                    world_ptr, 0, id, 0, "::", nullptr);
                ecs_assert(!strcmp(path, name), 
                    ECS_INCONSISTENT_COMPONENT_NAME, name);
                ecs_os_free(path);
            }
        }

        /* Register name with entity, so that when the entity is created the
         * correct id will be resolved from the name. */
        ecs_add_path_w_sep(world_ptr, id, 0, name, "::", "::");

        /* If a component was already registered with this id but with a 
         * different size, the ecs_new_component function will fail. */

        /* We need to explicitly call ecs_new_component here again. Even though
         * the component was already registered, it may have been registered
         * with a different world. This ensures that the component is registered
         * with the same id for the current world. 
         * If the component was registered already, nothing will change. */
        ecs_entity_t entity = ecs_new_component(
            world.c_ptr(), id, nullptr, 
            _::cpp_type<T>::size(), 
            _::cpp_type<T>::alignment());
        
        (void)entity;
        
        ecs_assert(entity == id, ECS_INTERNAL_ERROR, NULL);

        /* This functionality could have been put in id_no_lifecycle, but since
         * this code happens when a component is registered, and the entire API
         * calls id_no_lifecycle, this would add a lot of overhead to each call.
         * This is why when using multiple worlds, components should be 
         * registered explicitly. */
    } else {
        /* If the component is not yet registered, ensure no other component
         * or entity has been registered with this name */
        ecs_entity_t entity = ecs_lookup_fullpath(world_ptr, name);

        /* If entity exists, compare symbol name to ensure that the component
         * we are trying to register under this name is the same */
        if (entity) {
            const EcsName *name_comp = static_cast<EcsName*>(ecs_get_mut_w_id(
                world.c_ptr(), entity, ecs_id(EcsName), NULL));
            ecs_assert(name_comp != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(name_comp->symbol != NULL, ECS_INTERNAL_ERROR, NULL);

            const char *symbol = _::name_helper<T>::name();

            ecs_assert(!strcmp(name_comp->symbol, symbol), 
                ECS_COMPONENT_NAME_IN_USE, name);

            (void)name_comp;
            (void)symbol;
        }

        /* Register id as usual */
        id = _::cpp_type<T>::id_no_lifecycle(world_ptr, name, allow_tag);
    }
    
    return world.entity(id);
}

/** Regular component with ctor, dtor copy and move actions */
template <typename T>
flecs::entity component(const flecs::world& world, const char *name = nullptr) {
    flecs::entity result = pod_component<T>(world, name);

    if (_::cpp_type<T>::size()) {
        _::register_lifecycle_actions<T>(world.c_ptr(), result.id(),
            true, true, true, true);
    }

    return result;
}

/** Trivially relocatable component that can be memcpy'd. */
template <typename T>
flecs::entity relocatable_component(const flecs::world& world, const char *name = nullptr) {
    flecs::entity result = pod_component<T>(world, name);

    _::register_lifecycle_actions<T>(world.c_ptr(), result.id(),
        true, true, true, false);

    return result;
}

template <typename T>
flecs::entity_t type_id() {
    return _::cpp_type<T>::id();
}

} // namespace flecs
