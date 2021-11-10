// Utility for adding features (mixins) to a class without modifying its 
// definition.

namespace flecs {

// Macro for mixin template type so we don't go cross-eyed
#define FLECS_MIXIN template<typename Self> class

// Template that can store a list of mixin templates
template< FLECS_MIXIN ... Mixin>
struct mixin_list { };

// Used for last element in mixins list (see next)
template <typename T, typename Mixin>
struct extendable_impl { 
    void init_mixins() { }
};

// Recursively unpack the elements in the mixin list. Add current mixin as base
// base class, wrap remaining mixins in a new list & pass to self.
template <typename T, FLECS_MIXIN Mixin, FLECS_MIXIN ... Mixins>
struct extendable_impl<T, mixin_list<Mixin, Mixins...> > : Mixin<T>, extendable_impl<T, mixin_list< Mixins... >> {
    using Base = extendable_impl<T, mixin_list< Mixins... >>;

    void init_mixins() {
        Mixin<T>::init();
        Base::init_mixins();
    }
};

// Base class for mixin implementations. The virtual method provides access to
// the instance of the top-level type.
template <typename Self>
struct mixin {
protected:
    virtual Self& me() const = 0;
    virtual ~mixin() { }
};

// Base for extendable class. Accepts own type and list of mixins
template <typename T, typename Mixins>
struct extendable : extendable_impl<T, Mixins> {
    T& me() const {
        return *const_cast<T*>(static_cast<const T*>(this));
    }
};

}
