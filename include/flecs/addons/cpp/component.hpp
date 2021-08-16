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

// If type is trivial, don't register lifecycle actions. While the functions
// that obtain the lifecycle callback do detect whether the callback is required
// adding a special case for trivial types eases the burden a bit on the
// compiler as it reduces the number of templates to evaluate.
template<typename T, enable_if_t<
    std::is_trivial<T>::value == true
        >* = nullptr>
void register_lifecycle_actions(ecs_world_t*, ecs_entity_t) { }

// If the component is non-trivial, register component lifecycle actions. 
// Depending on the type not all callbacks may be available.
template<typename T, enable_if_t<
    std::is_trivial<T>::value == false
        >* = nullptr>
void register_lifecycle_actions(
    ecs_world_t *world,
    ecs_entity_t component)
{
    if (!ecs_component_has_actions(world, component)) {
        EcsComponentLifecycle cl{};
        cl.ctor = ctor<T>();
        cl.dtor = dtor<T>();

        cl.copy = copy<T>();
        cl.copy_ctor = copy_ctor<T>();
        cl.move = move<T>();
        cl.move_ctor = move_ctor<T>();

        cl.ctor_move_dtor = ctor_move_dtor<T>();
        cl.move_dtor = move_dtor<T>();

        ecs_set_component_actions_w_id( world, component, &cl);
    }
}

// Class that manages component ids across worlds & binaries.
// The cpp_type class stores the component id for a C++ type in a static global
// variable that is shared between worlds. Whenever a component is used this
// class will check if it already has been registered (has the global id been
// set), and if not, register the component with the world.
//
// If the id has been set, the class will ensure it is known by the world. If it
// is not known the component has been registered by another world and will be
// registered with the world using the same id. If the id does exist, the class
// will register it as a component, and verify whether the input is consistent.
template <typename T>
class cpp_type_size {
public:
    static size_t size(bool allow_tag) {
        // C++ types that have no members still have a size. Use std::is_empty
        // to check if the type is empty. If so, use 0 for the component size.
        //
        // If s_allow_tag is set to false, the size returned by C++ is used.
        // This is useful in cases where class instances are still required, as
        // is the case with module classes.
        if (allow_tag && std::is_empty<T>::value) {
            return 0;
        } else {
            return sizeof(T);
        }
    }

    static size_t alignment(bool allow_tag) {
        if (size(allow_tag) == 0) {
            return 0;
        } else {
            return alignof(T);
        }        
    }
};

template <typename T>
class cpp_type_impl {
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

    // Names returned from the name_helper class do not start with ::
    // but are relative to the root. If the namespace of the type
    // overlaps with the namespace of the current module, strip it from
    // the implicit identifier.
    // This allows for registration of component types that are not in the 
    // module namespace to still be registered under the module scope.
    static const char* strip_module(world_t *world) {
        const char *name = _::name_helper<T>::name();
        entity_t scope = ecs_get_scope(world);
        if (!scope) {
            return name;
        }

        char *path = ecs_get_path_w_sep(world, 0, scope, "::", nullptr);
        if (path) {
            const char *ptr = strrchr(name, ':');
            ecs_assert(ptr != name, ECS_INTERNAL_ERROR, NULL);
            if (ptr) {
                ptr --;
                ecs_assert(ptr[0] == ':', ECS_INTERNAL_ERROR, NULL);
                ecs_size_t name_path_len = static_cast<ecs_size_t>(ptr - name);
                if (name_path_len <= ecs_os_strlen(path)) {
                    if (!ecs_os_strncmp(name, path, name_path_len)) {
                        name = &name[name_path_len + 2];
                    }
                }
            }
        }
        ecs_os_free(path);

        return name;
    }

    // Obtain a component identifier for explicit component registration.
    static entity_t id_explicit(world_t *world = nullptr, 
        const char *name = nullptr, bool allow_tag = true, flecs::id_t id = 0)
    {
        if (!s_id) {
            // If no world was provided the component cannot be registered
            ecs_assert(world != nullptr, ECS_COMPONENT_NOT_REGISTERED, name);            
            s_allow_tag = allow_tag;
        } else {
            ecs_assert(!id || s_id == id, ECS_INCONSISTENT_COMPONENT_ID, NULL);
            ecs_assert(s_allow_tag == allow_tag, ECS_INVALID_PARAMETER, NULL);
        }

        // If no id has been registered yet for the component (indicating the 
        // component has not yet been registered, or the component is used
        // across more than one binary), or if the id does not exists in the 
        // world (indicating a multi-world application), register it. */
        if (!s_id || (world && !ecs_exists(world, s_id))) {
            if (!s_id) {
                s_id = id;
            }

            // One type can only be associated with a single type
            ecs_assert(!id || s_id == id, ECS_INTERNAL_ERROR, NULL);

            char *symbol = nullptr;

            // If an explicit id is provided, it is possible that the symbol and
            // name differ from the actual type, as the application may alias
            // one type to another.
            if (!id) {
                symbol = symbol_helper<T>::symbol();
                if (!name) {
                    // If no name was provided, retrieve the name implicitly from
                    // the name_helper class.
                    name = strip_module(world);
                }
            } else {
                // If an explicit id is provided but it has no name, inherit
                // the name from the type.
                if (!ecs_get_name(world, id)) {
                    name = strip_module(world);
                }
            }

            ecs_component_desc_t desc = {};
            desc.entity.entity = s_id;
            desc.entity.name = name;
            desc.entity.sep = "::";
            desc.entity.root_sep = "::";
            desc.entity.symbol = symbol;
            desc.size = cpp_type_size<T>::size(allow_tag);
            desc.alignment = cpp_type_size<T>::alignment(allow_tag);

            ecs_entity_t entity = ecs_component_init(world, &desc);
            ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(!s_id || s_id == entity, ECS_INTERNAL_ERROR, NULL);
            ecs_os_free(symbol);
            
            init(world, s_id, allow_tag);
            s_id = entity;
        }

        // By now the identifier must be valid and known with the world.
        ecs_assert(s_id != 0 && ecs_exists(world, s_id), ECS_INTERNAL_ERROR, NULL);

        return s_id;
    }

    // Obtain a component identifier for implicit component registration. This
    // is almost the same as id_explicit, except that this operation 
    // automatically registers lifecycle callbacks.
    // Additionally, implicit registration temporarily resets the scope & with
    // state of the world, so that the component is not implicitly created with
    // the scope/with of the code it happens to be first used by.
    static id_t id(world_t *world = nullptr, const char *name = nullptr, 
        bool allow_tag = true) 
    {
        // If no id has been registered yet, do it now.
        if (!s_id || (world && !ecs_exists(world, s_id))) {
            ecs_entity_t prev_scope = 0;
            ecs_id_t prev_with = 0;

            if (world) {
                prev_scope = ecs_set_scope(world, 0);
                prev_with = ecs_set_with(world, 0);
            }
            
            // This will register a component id, but will not register 
            // lifecycle callbacks.
            id_explicit(world, name, allow_tag);

            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's. */
            if (size()) {
                register_lifecycle_actions<T>(world, s_id);
            }
            
            if (prev_with) {
                ecs_set_with(world, prev_with);
            }
            if (prev_scope) {
                ecs_set_scope(world, prev_scope);
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
    // first calling id_explicit() and then name(), this function ensures
    // that the lifecycle callback templates are not instantiated. This allows
    // some types (such as module classes) to be created without a default
    // constructor.
    static const char* name_no_lifecycle(world_t *world = nullptr) {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            id_explicit(world);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        // Return 
        return s_name.c_str();
    }    

    // Return the size of a component.
    static size_t size() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);
        return cpp_type_size<T>::size(s_allow_tag);
    }

    // Return the alignment of a component.
    static size_t alignment() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);
        return cpp_type_size<T>::alignment(s_allow_tag);
    }

    // Was the component already registered.
    static bool registered() {
        return s_id != 0;
    }

    // This function is only used to test cross-translation unit features. No
    // code other than test cases should invoke this function.
    static void reset() {
        s_id = 0;
        s_name.clear();
    }

private:
    static entity_t s_id;
    static flecs::string s_name;
    static flecs::string s_symbol;
    static bool s_allow_tag;
};

// Global templated variables that hold component identifier and other info
template <typename T> entity_t      cpp_type_impl<T>::s_id( 0 );
template <typename T> flecs::string cpp_type_impl<T>::s_name;
template <typename T> bool          cpp_type_impl<T>::s_allow_tag( true );

// Front facing class for implicitly registering a component & obtaining 
// static component data

// Regular type
template <typename T>
class cpp_type<T, if_not_t< is_pair<T>::value >> 
    : public cpp_type_impl<base_type_t<T>> { };

// Pair type
template <typename T>
class cpp_type<T, if_t< is_pair<T>::value >>
{
public:
    // Override id method to return id of pair
    static id_t id(world_t *world = nullptr) {
        return ecs_pair(
            cpp_type< pair_relation_t<T> >::id(world),
            cpp_type< pair_object_t<T> >::id(world));
    }
};

} // namespace _

////////////////////////////////////////////////////////////////////////////////
//// Register a component with flecs
////////////////////////////////////////////////////////////////////////////////

/** Plain old datatype, no lifecycle actions are registered */
template <typename T>
flecs::entity pod_component(
    flecs::world_t *world, 
    const char *name = nullptr, 
    bool allow_tag = true, 
    flecs::id_t id = 0) 
{
    const char *n = name;
    bool implicit_name = false;
    if (!n) {
        n = _::name_helper<T>::name();

        /* Keep track of whether name was explicitly set. If not, and the 
         * component was already registered, just use the registered name.
         *
         * The registered name may differ from the typename as the registered
         * name includes the flecs scope. This can in theory be different from
         * the C++ namespace though it is good practice to keep them the same */
        implicit_name = true;
    }

    if (_::cpp_type<T>::registered()) {
        /* Obtain component id. Because the component is already registered,
         * this operation does nothing besides returning the existing id */
        id = _::cpp_type<T>::id_explicit(world, name, allow_tag, id);

        /* If entity has a name check if it matches */
        if (ecs_get_name(world, id) != nullptr) {
            if (!implicit_name && id >= EcsFirstUserComponentId) {
                char *path = ecs_get_path_w_sep(
                    world, 0, id, "::", nullptr);
                ecs_assert(!strcmp(path, n), 
                    ECS_INCONSISTENT_NAME, name);
                ecs_os_free(path);
            }
        } else {
            /* Register name with entity, so that when the entity is created the
             * correct id will be resolved from the name. Only do this when the
             * entity is empty.*/
            ecs_add_path_w_sep(world, id, 0, n, "::", "::");
        }

        /* If a component was already registered with this id but with a 
         * different size, the ecs_component_init function will fail. */

        /* We need to explicitly call ecs_component_init here again. Even though
         * the component was already registered, it may have been registered
         * with a different world. This ensures that the component is registered
         * with the same id for the current world. 
         * If the component was registered already, nothing will change. */
        ecs_component_desc_t desc = {};
        desc.entity.entity = id;
        desc.size = _::cpp_type<T>::size();
        desc.alignment = _::cpp_type<T>::alignment();
        ecs_entity_t entity = ecs_component_init(world, &desc);
        (void)entity;
        
        ecs_assert(entity == id, ECS_INTERNAL_ERROR, NULL);

        /* This functionality could have been put in id_explicit, but since
         * this code happens when a component is registered, and the entire API
         * calls id_explicit, this would add a lot of overhead to each call.
         * This is why when using multiple worlds, components should be 
         * registered explicitly. */
    } else {
        /* If the component is not yet registered, ensure no other component
         * or entity has been registered with this name. Ensure component is 
         * looked up from root. */
        ecs_entity_t prev_scope = ecs_set_scope(world, 0);
        ecs_entity_t entity;
        if (id) {
            entity = id;
        } else {
            entity = ecs_lookup_path_w_sep(world, 0, n, "::", "::", false);
        }

        ecs_set_scope(world, prev_scope);

        /* If entity exists, compare symbol name to ensure that the component
         * we are trying to register under this name is the same */
        if (entity) {
            if (!id) {
                const char *sym = ecs_get_symbol(world, entity);
                ecs_assert(sym != NULL, ECS_INTERNAL_ERROR, NULL);
                (void)sym;

                char *symbol = _::symbol_helper<T>::symbol();
                ecs_assert(!ecs_os_strcmp(sym, symbol), ECS_NAME_IN_USE, n);
                ecs_os_free(symbol);

            /* If an existing id was provided, it's possible that this id was
             * registered with another type. Make sure that in this case at
             * least the component size/alignment matches.
             * This allows applications to alias two different types to the same
             * id, which enables things like redefining a C type in C++ by
             * inheriting from it & adding utility functions etc. */
            } else {
                const EcsComponent *comp = ecs_get(world, entity, EcsComponent);
                if (comp) {
                    ecs_assert(comp->size == ECS_SIZEOF(T),
                        ECS_INVALID_COMPONENT_SIZE, NULL);
                    ecs_assert(comp->alignment == ECS_ALIGNOF(T),
                        ECS_INVALID_COMPONENT_ALIGNMENT, NULL);
                } else {
                    /* If the existing id is not a component, no checking is
                     * needed. */
                }
            }

        /* If no entity is found, lookup symbol to check if the component was
         * registered under a different name. */
        } else {
            char *symbol = _::symbol_helper<T>::symbol();
            entity = ecs_lookup_symbol(world, symbol, false);
            ecs_assert(entity == 0, ECS_INCONSISTENT_COMPONENT_ID, symbol);
            ecs_os_free(symbol);
        }

        /* Register id as usual */
        id = _::cpp_type<T>::id_explicit(world, name, allow_tag, id);
    }
    
    return flecs::entity(world, id);
}

/** Register component */
template <typename T>
flecs::entity component(flecs::world_t *world, const char *name = nullptr) {
    flecs::entity result = pod_component<T>(world, name);

    if (_::cpp_type<T>::size()) {
        _::register_lifecycle_actions<T>(world, result);
    }

    return result;
}

/* Register component with existing entity id */
template <typename T>
void component_for_id(flecs::world_t *world, flecs::id_t id) {
    flecs::entity result = pod_component<T>(world, nullptr, true, id);

    ecs_assert(result.id() == id, ECS_INTERNAL_ERROR, NULL);

    if (_::cpp_type<T>::size()) {
        _::register_lifecycle_actions<T>(world, result);
    }
}

ECS_DEPRECATED("API detects automatically whether type is trivial")
template <typename T>
flecs::entity relocatable_component(const flecs::world& world, const char *name = nullptr) {
    flecs::entity result = pod_component<T>(world, name);

    _::register_lifecycle_actions<T>(world.c_ptr(), result.id());

    return result;
}

template <typename T>
flecs::entity_t type_id() {
    return _::cpp_type<T>::id();
}

} // namespace flecs

