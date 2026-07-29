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

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_http_server_t *server = ecs_http_server_init(
        &(ecs_http_server_desc_t){
            .callback = dummy_reply,
            .port = 27760
        });
    assert(server != NULL);

    ecs_http_server_fini(server);

    return ecs_fini(world);
}
