#pragma once

namespace flecs {

#define flecs_me_ this->me()

// Rest mixin implementation
inline void rest_m_world::init() {
    flecs_me_.template component<rest::Rest>("flecs::rest::Rest");
}

#undef flecs_me_

}
