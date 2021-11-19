#pragma once

namespace flecs {

#define me_ this->me()

// Rest mixin implementation
inline void rest_m_world::init() {
    me_.template component<rest::Rest>("flecs::rest::Rest");
}

#undef me_

}
