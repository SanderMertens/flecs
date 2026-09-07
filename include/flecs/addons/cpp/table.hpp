/**
 * @file addons/cpp/table.hpp
 * @brief Direct access to table data.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_tables Tables
 * @ingroup cpp_core
 * Table operations.
 *
 * @{
 */

/** Table.
 * A table stores entities with the same set of components.
 *
 * @ingroup cpp_tables
 */
struct table {
    /** Default constructor. */
    table() : world_(nullptr), table_(nullptr) { }

    /** Construct a table from a world and C table pointer.
     *
     * @param world The world.
     * @param t Pointer to the C table.
     */
    table(world_t *world, table_t *t)
        : world_(world)
        , table_(t) { }

    /** Destructor. */
    virtual ~table() { }

    /** Convert the table type to a string. */
    flecs::string str() const {
        return flecs::string(ecs_table_str(world_, table_));
    }

    /** Get the table type. */
    flecs::type type() const {
        return flecs::type(world_, ecs_table_get_type(table_));
    }

    /** Get the table count. */
    int32_t count() const {
        return ecs_table_count(table_);
    }

    /** Get the number of allocated elements in the table. */
    int32_t size() const {
        return ecs_table_size(table_);
    }

    /** Get the array of entity IDs. */
    const flecs::entity_t* entities() const {
        return ecs_table_entities(table_);
    }

    /** Delete entities in the table. */
    void clear_entities() const {
        ecs_table_clear_entities(world_, table_);
    }

    template <typename... T, typename... Args>
    int32_t type_index(Args... args) const {
        return ecs_table_get_type_index(world_, table_, _::make_id<T...>(world_, args...).id);
    }

    template <typename... T, typename... Args>
    int32_t column_index(Args... args) const {
        return ecs_table_get_column_index(world_, table_, _::make_id<T...>(world_, args...).id);
    }

    template <typename... T, typename... Args>
    bool has(Args... args) const {
        return type_index<T...>(args...) != -1;
    }

    /** Get a pointer to the component array by column index.
     *
     * @param index The column index.
     * @return Pointer to the column, nullptr if not a component.
     */
    virtual void* get_column(int32_t index) const {
        return ecs_table_get_column(table_, index, 0);
    }


    template <typename... T, typename... Args>
    auto try_get(Args... args) const {
        return get_ptr<false>(_::make_id<T...>(world_, args...));
    }

    template <typename... T, typename... Args>
    auto get(Args... args) const {
        return get_ptr<true>(_::make_id<T...>(world_, args...));
    }

    /** Get the column size.
     *
     * @param index The column index.
     * @return The size of the column's component type.
     */
    size_t column_size(int32_t index) const {
        return ecs_table_get_column_size(table_, index);
    }

    /** Get the depth for a given relationship.
     *
     * @param rel The relationship.
     * @return The depth.
     */
    int32_t depth(flecs::entity_t rel) const  {
        return ecs_table_get_depth(world_, table_, rel);
    }

    /** Get the depth for a given relationship.
     *
     * @tparam Rel The relationship.
     * @return The depth.
     */
    template <typename Rel>
    int32_t depth() const {
        return depth(_::type<Rel>::id(world_));
    }

    /** Get the table records array.
     *
     * @return The table records.
     */
    ecs_table_records_t records() const {
        return flecs_table_records(table_);
    }

    /** Get the table ID.
     *
     * @return The table ID.
     */
    uint64_t id() const {
        return flecs_table_id(table_);
    }

    /** Lock the table. */
    void lock() const {
        ecs_table_lock(world_, table_);
    }

    /** Unlock the table. */
    void unlock() const {
        ecs_table_unlock(world_, table_);
    }

    /** Check if the table has flags.
     *
     * @param flags The flags to check for.
     * @return True if the table has the specified flags.
     */
    bool has_flags(ecs_flags32_t flags) const {
        return ecs_table_has_flags(table_, flags);
    }

    /** Get the table.
     *
     * @return The table.
     */
    table_t* get_table() const {
        return table_;
    }

    /** Implicit conversion to table_t*. */
    operator table_t*() const {
        return table_;
    }

private:
    template <bool Required, typename Id>
    typename Id::type* get_ptr(Id id) const {
        int32_t index = column_index(id.id);
        if (index == -1) {
            return nullptr;
        }
        auto ptr = static_cast<typename Id::type*>(get_column(index));
        if constexpr (Required) {
            ecs_assert(ptr != nullptr, ECS_INVALID_OPERATION,
                "invalid get: table does not have component (use try_get())");
        }
        return ptr;
    }

protected:
    world_t *world_;
    table_t *table_;
};

/** Table range.
 * A table range represents a contiguous range of entities in a table.
 *
 * @ingroup cpp_tables
 */
struct table_range : table {
    /** Default constructor. */
    table_range()
        : table()
        , offset_(0)
        , count_(0) { }

    /** Construct a table range from a world, table, offset, and count.
     *
     * @param world The world.
     * @param t Pointer to the C table.
     * @param offset The starting row offset.
     * @param count The number of rows in the range.
     */
    table_range(world_t *world, table_t *t, int32_t offset, int32_t count)
        : table(world, t)
        , offset_(offset)
        , count_(count) { }

    /** Get the offset of the range. */
    int32_t offset() const {
        return offset_;
    }

    /** Get the number of entities in the range. */
    int32_t count() const {
        return count_;
    }

    /** Get a pointer to the component array by column index.
     *
     * @param index The column index.
     * @return Pointer to the column, nullptr if not a component.
     */
    void* get_column(int32_t index) const override {
        return ecs_table_get_column(table_, index, offset_);
    }

private:
    int32_t offset_ = 0;
    int32_t count_ = 0;
};

/** @} */

}
