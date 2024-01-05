/**
 * @file addons/cpp/mixins/app/mixin.inl
 * @brief App world addon mixin.
 */

/** Return app builder.
 * The app builder is a convenience wrapper around a loop that runs 
 * world::progress. An app allows for writing platform agnostic code,
 * as it provides hooks to modules for overtaking the main loop which is 
 * required for frameworks like emscripten.
 * 
 * \ingroup cpp_addons_app
 * \memberof flecs::world
 */
flecs::app_builder app() {
    m_owned = false; // App takes ownership of world
    return flecs::app_builder(m_world);
}

/** @} */
