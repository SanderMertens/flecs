/**
 * @file addons/cpp/mixins/app/mixin.inl
 * @brief App world addon mixin.
 */

/**
 * @ingroup cpp_addons_app
 * @memberof flecs::world
 *
 * @{
 */

/** Return app builder.
 * The app builder is a convenience wrapper around a loop that runs 
 * world::progress. An app allows for writing platform agnostic code,
 * as it provides hooks to modules for overtaking the main loop which is 
 * required for frameworks like emscripten.
 */
flecs::app_builder app() {
    flecs::world_t *w = world_;
    world_ = nullptr; // Take ownership
    return flecs::app_builder(w);
}

/** @} */
