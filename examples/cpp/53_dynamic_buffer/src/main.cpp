#include <dynamic_buffer.h>
#include <iostream>

struct Position {
    float x, y;
};

/* Non-POD type. Don't use std::vector on purpose to demonstrate how a type
 * can register its own lifecycle actions. */
struct DynamicBuffer {
    DynamicBuffer() : data(nullptr), count(0) { 
        std::cout << "DynamicBuffer::ctor" << std::endl;
    }

    ~DynamicBuffer() {
        std::cout << "DynamicBuffer::dtor" << std::endl;
        free(data);
    }

    DynamicBuffer(const DynamicBuffer& obj) {
        *this = obj;
    }

    DynamicBuffer(DynamicBuffer&& obj) {
        *this = std::move(obj);
    }    

    DynamicBuffer& operator=(const DynamicBuffer& src) {
        std::cout << "DynamicBuffer::copy" << std::endl;
        if (data) {
            free(data);
        }

        size_t size = sizeof(int) * src.count;
        data = static_cast<int*>(malloc(size));
        count = src.count;
        memcpy(data, src.data, size);
        
        return *this;
    }

    DynamicBuffer& operator=(DynamicBuffer&& src) {
        std::cout << "DynamicBuffer::move" << std::endl;
        if (data) {
            free(data);
        }

        data = src.data;
        count = src.count;

        src.data = nullptr;
        src.count = 0;     

        return *this;   
    }    

    int *data;
    size_t count;
};

/* Add an element to a new or existing buffer */
void add_elem(flecs::entity e, int value) {
    DynamicBuffer *ptr = e.get_mut<DynamicBuffer>();

    ptr->count ++;
    ptr->data = static_cast<int*>(realloc(ptr->data, ptr->count * sizeof(int)));
    ptr->data[ptr->count - 1] = value;
}

/* Remove element from buffer */
void remove_elem(flecs::entity e, size_t elem) {
    DynamicBuffer *ptr = e.get_mut<DynamicBuffer>();

    size_t last = ptr->count - 1;

    if (last >= elem) {
        if (last - elem) {
            ptr->data[elem] = ptr->data[last];
        }
        
        ptr->count --;
    }
}

/* Get element from a buffer */
int* get_elem(flecs::entity e, size_t elem) {
    const DynamicBuffer *ptr = e.get<DynamicBuffer>();

    if (ptr && (ptr->count > elem)) {
        return &ptr->data[elem];
    } else {
        return NULL;
    }
}

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    auto e = ecs.entity();
    
    /* Add 3 elements to the buffer. The first add will add the DynamicBuffer
     * element to the entity. */
    add_elem(e, 10);
    add_elem(e, 20);
    add_elem(e, 30);

    std::cout << "Elem 1 = " << *get_elem(e, 1) << std::endl;

    /* Remove element. This will move the last element from the buffer to the
     * removed element. */
    remove_elem(e, 1);

    std::cout << "Elem 1 = " << *get_elem(e, 1) << " (after remove)" << std::endl;

    /* Add component. This causes the entity to move between tables, and will
     * invoke DynamicComponent::move to copy the component value from the src to
     * the dst table. This also invokes DynamicComponent::ctor to construct the
     * component in the dst table, and DynamicComponent::dtor to destruct the
     * element in the src table. */
    e.add<Position>();

    /* World gets cleaned up, which invokes DynamicComponent::dtor. */
}
