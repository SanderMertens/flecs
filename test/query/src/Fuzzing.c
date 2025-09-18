#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void Fuzzing_setup(void) {
    const char *cache_param = test_param("cache_kind");
    if (cache_param) {
        if (!strcmp(cache_param, "default")) {
            // already set to default
        } else if (!strcmp(cache_param, "auto")) {
            cache_kind = EcsQueryCacheAuto;
        } else {
            printf("unexpected value for cache_param '%s'\n", cache_param);
        }
    }
}

static
void fuzz(const char *expr) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);

    ecs_query_t *q = ecs_query(world, { 
        .expr = expr,
        .flags = EcsQueryAllowUnresolvedByName,
        .cache_kind = cache_kind
    });

    if (q) {
        ecs_query_fini(q);
    }

    test_assert(true);

    ecs_fini(world);
}

void Fuzzing_1(void) {
    fuzz("$is , *== $pe ||ee ||ethis  ");
}

void Fuzzing_2(void) {
    fuzz("$is$, *==sKike}�}}}}(Docked}}________y__LikeXe $this  ");
}

void Fuzzing_3(void) {
    fuzz("$$$m(), !_}, !{ ");
}

void Fuzzing_4(void) {
    fuzz("$$}}#9}$i}}#9}$is , *== $this\\...........C..");
}

void Fuzzing_5(void) {
    fuzz("$is == this,*");
}

void Fuzzing_6(void) {
    fuzz("\x24\x6e\x23\x36\x2c\x23\x32\x2e\x2e\x2e\x2e\x2e\x2e\x24\x74\x6b\x6b\x69\x6b\x2e\x7d\x7d\x24\x6e\x24\x69\x2a\x2c\x20\x2a\x2c\x20\x2a\x2c\x2a\x2c\x2a\x2c\x2a\x2c\x24\x69\x2a\x2c\x20\x2a\x2c\x20\x2a\x7d\x7b\x7b\x2a\x2c\x20\x2a\x2c\x2a\x2c\x2a\x2c\x2a\x2c\x24\x69\x2a\x2c\x20\x2a\x2c\x20\x2a\x7d\x7b\x7b\x2a\x2c\x20\x2a\x2c\x2a\x2c\x2a\x2c");
}
