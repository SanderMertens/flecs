/* This is a managed file. Do not delete this comment. */

#include <ecs/ecs.h>

int main(int argc, char *argv[]) {

    char *str = "Hello World\n";
    uint64_t hash = 0;
    ecs_hash(str, strlen(str), &hash);
    printf("hash = %lld\n", hash);

    str = "Hello World1\n";
    hash = 0;
    ecs_hash(str, strlen(str), &hash);
    printf("hash = %lld\n", hash);

    return 0;
}
