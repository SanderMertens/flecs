
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Reader for world/snapshot serialization
////////////////////////////////////////////////////////////////////////////////

class reader final {
public:
    explicit reader(world& world) {
        m_reader = ecs_reader_init(world.c_ptr());
    }

    reader(world& world, snapshot& snapshot) {
        (void)world;
        ecs_iter_t it = ecs_snapshot_iter(snapshot.c_ptr(), nullptr);
        m_reader = ecs_reader_init_w_iter(&it, ecs_snapshot_next);
    }

    int32_t read(char *buffer, int64_t size) {
        return ecs_reader_read(buffer, static_cast<int32_t>(size), &m_reader);
    }

private:
    ecs_reader_t m_reader;
};

////////////////////////////////////////////////////////////////////////////////
//// Writer for world deserialization
////////////////////////////////////////////////////////////////////////////////

class writer final {
public:
    explicit writer(world& world) {
        m_writer = ecs_writer_init(world.c_ptr());
    }

    int32_t write(const char *buffer, int64_t size) {
        return ecs_writer_write(buffer, static_cast<int32_t>(size), &m_writer);
    }

private:
    ecs_writer_t m_writer;
};

} // namespace flecs
