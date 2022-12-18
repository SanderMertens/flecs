/**
 * @file addons/cpp/mixins/app/mixin.inl
 * @brief App world addon mixin.
 */

flecs::app_builder app() {
    m_owned = false; // App takes ownership of world
    return flecs::app_builder(m_world);
}
