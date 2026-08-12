#include <http.h>

static bool dummy_reply(
    const ecs_http_request_t *request,
    ecs_http_reply_t *reply,
    void *ctx)
{
    (void)request;
    (void)reply;
    (void)ctx;
    return false;
}

int main(int, char *[]) {
    flecs::world ecs;

    ecs_http_server_desc_t desc = {};
    desc.callback = dummy_reply;
    desc.port = 27761;

    ecs_http_server_t *server = ecs_http_server_init(&desc);
    assert(server != NULL);

    ecs_http_server_fini(server);

    return 0;
}
