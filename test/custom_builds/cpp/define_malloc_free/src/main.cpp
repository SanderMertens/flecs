#include <stdlib.h>

namespace foo {
    void* my_malloc(size_t size) {
        return malloc(size);
    }

    void my_free(void* ptr) {
        free(ptr);
    }
}

#define malloc(size) foo::my_malloc(size)
#define free(ptr) foo::my_free(ptr)

#include <define_malloc_free.h>

int main(int, char *[]) {
    flecs::world world;

    void *ptr = malloc(10);
    free(ptr);

    return 0;
}
