
flecs::app_builder app() {
    m_owned = false; // App takes ownership of world
    return flecs::app_builder(m_world);
}
