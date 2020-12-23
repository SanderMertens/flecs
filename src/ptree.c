#include "private_api.h"

#define PAGE_SIZE (65536)

typedef struct addr_t {
    uint16_t value[4];
} addr_t;

typedef struct array_t {
    void *data;
    uint16_t offset;
    int32_t length;
} array_t;

typedef struct page_t {
    array_t data;
    array_t pages;
} page_t;

struct ecs_ptree_t {
    page_t root;
    void *first_65k;
    uint16_t min_65k;
    uint16_t max_65k;
    uint8_t elem_size;
};

static
addr_t to_addr(
    uint64_t id) 
{
    union {
        addr_t addr;
        uint64_t id;
    } u = {.id = id};
    return u.addr;
}

static
void* array_ensure(
    array_t *array,
    ecs_size_t elem_size,
    uint16_t index)
{
    int32_t offset = (int32_t)array->offset;
    int32_t length = array->length;

    ecs_assert(offset >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(length >= 0, ECS_INTERNAL_ERROR, NULL);

    if (!length) {
        array->length = 1;
        array->offset = index;
        array->data = ecs_os_calloc(elem_size);
        ecs_assert(array->data != NULL, ECS_OUT_OF_MEMORY, NULL);
        return array->data; 

    } else if (index < offset) {
        int32_t new_offset = ecs_next_pow_of_2(index) >> 1;
        int32_t dif_offset = offset - new_offset;
        int32_t new_length = ecs_next_pow_of_2(length + dif_offset);
        
        if ((new_offset + new_length) > PAGE_SIZE) {
            new_offset = 0;
            new_length = PAGE_SIZE;
            dif_offset = offset;
        }

        ecs_assert(length != new_length, ECS_INTERNAL_ERROR, NULL);
        array->data = ecs_os_realloc(array->data, new_length * elem_size);
        ecs_assert(array->data != NULL, ECS_OUT_OF_MEMORY, NULL);

        ecs_os_memset(ECS_OFFSET(array->data, length * elem_size), 0, (new_length - length) * elem_size);
        ecs_os_memmove(ECS_OFFSET(array->data, dif_offset * elem_size), array->data, 
            length * elem_size);
        ecs_os_memset(array->data, 0, dif_offset * elem_size);            

        ecs_assert(new_offset >= 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(new_offset < 65536, ECS_INTERNAL_ERROR, NULL);

        array->offset = (uint16_t)new_offset;
        array->length = new_length;

    } else if (index >= (offset + length)) {
        int32_t new_length = ecs_next_pow_of_2(index + offset + 1);
        if ((new_length + offset) > PAGE_SIZE) {
            int32_t new_offset = ecs_next_pow_of_2(offset - ((new_length + offset) - PAGE_SIZE)) >> 1;
            int32_t dif_offset = offset - new_offset;
            new_length = ecs_next_pow_of_2(new_offset + index);
            
            ecs_assert(length != new_length, ECS_INTERNAL_ERROR, NULL);
            array->data = ecs_os_realloc(array->data, new_length * elem_size);
            ecs_assert(array->data != NULL, ECS_OUT_OF_MEMORY, NULL);

            ecs_os_memset(ECS_OFFSET(array->data, length * elem_size), 0, (new_length - length) * elem_size);
            ecs_os_memmove(ECS_OFFSET(array->data, dif_offset * elem_size), array->data, 
                length * elem_size);
            ecs_os_memset(array->data, 0, dif_offset * elem_size);

            ecs_assert(new_offset >= 0, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(new_offset < 65536, ECS_INTERNAL_ERROR, NULL);

            array->offset = (uint16_t)new_offset;
        } else {
            ecs_assert(length != new_length, ECS_INTERNAL_ERROR, NULL);
            array->data = ecs_os_realloc(array->data, new_length * elem_size);
            ecs_assert(array->data != NULL, ECS_OUT_OF_MEMORY, NULL);

            ecs_os_memset(ECS_OFFSET(array->data, length * elem_size), 0, 
                (new_length - length) * elem_size);
        }

        array->length = new_length;
    }

    ecs_assert((array->offset + array->length) <= PAGE_SIZE, 
        ECS_INTERNAL_ERROR, NULL);
    
    return ECS_OFFSET(array->data, (index - array->offset) * elem_size);
}

static
void* array_get(
    array_t *array,
    ecs_size_t elem_size,
    int32_t index)
{
    uint16_t offset = array->offset;
    int32_t length = array->length;

    if (index < offset) {
        return NULL;
    }

    index -= offset;
    if (index >= length) {
        return NULL;
    }

    return ECS_OFFSET(array->data, index * elem_size);
}

static
page_t* get_page(
    page_t *p,
    uint16_t *addr,
    int count) 
{
    int32_t i;
    for (i = count; i > 0; i --) {
        p = array_get(&p->pages, ECS_SIZEOF(page_t), addr[i]);
        if (!p) {
            return NULL;
        }
    }

    return p;
}

static
page_t* get_or_create_page(
    page_t *p,
    uint16_t *addr,
    int count)
{
    int32_t i;
    for (i = count; i > 0; i --) {
        p = array_ensure(&p->pages, ECS_SIZEOF(page_t), addr[i]);
        ecs_assert(p != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    return p;
}

static
int8_t page_count(
    uint64_t index)
{
    return (int8_t)((index > 65536) +
        (index > 0x00000000FFFF0000) +
        (index > 0x0000FFFF00000000));
}

static
int32_t page_free(
    ecs_ptree_t *ptree,
    page_t *p)
{
    int32_t result = sizeof(page_t);

    if (p->data.data) {
        result += p->data.length * ECS_SIZEOF(ptree->elem_size);
        ecs_os_free(p->data.data);
    }

    if (p->pages.data) {
        int32_t i;
        for (i = 0; i < p->pages.length; i ++) {
            result += page_free(ptree, array_get(&p->pages, ECS_SIZEOF(page_t), 
                i + p->pages.offset));
        }

        ecs_os_free(p->pages.data);
    }

    return result;
}

static
uint64_t index_from_it(
    ecs_ptree_iter_t *it,
    page_t *page)
{
    ecs_assert(it->cur_elem >= 0, ECS_INTERNAL_ERROR, NULL);
    
    uint64_t result = page->data.offset + (uint32_t)it->cur_elem;
    int8_t sp = it->sp;

    int8_t i;
    for (i = 0; i <= sp; i ++) {
        result += (uint64_t)((it->cur_page[i] + 
            ((page_t*)it->frames[i])->pages.offset)) << (16 * (1 + sp - i));
    }

    return result;
}

ecs_ptree_t* _ecs_ptiny_new(
    ecs_size_t elem_size)
{
    ecs_ptree_t *result = ecs_os_calloc(sizeof(ecs_ptree_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    ecs_assert(elem_size < 256, ECS_INVALID_PARAMETER, NULL);
    result->elem_size = (uint8_t)elem_size;
    result->min_65k = 65535;
    return result;
}

ecs_ptree_t* _ecs_ptree_new(
    ecs_size_t elem_size)
{
    ecs_ptree_t *result = _ecs_ptiny_new(elem_size);
    result->first_65k = ecs_os_calloc(elem_size * 65536);
    return result;
}

int32_t ecs_ptree_free(
    ecs_ptree_t *ptree)
{
    if (!ptree) {
        return 0;
    }

    int32_t result = page_free(ptree, &ptree->root);
    result += ECS_SIZEOF(int) * 65536;
    result += ECS_SIZEOF(ecs_ptree_t);

    ecs_os_free(ptree->first_65k);
    ecs_os_free(ptree);
    return result;
}

ecs_ptree_iter_t ecs_ptiny_iter(
    ecs_ptree_t *ptree)
{
    return (ecs_ptree_iter_t){
        .ptree = ptree,
        .frames[0] = &ptree->root,
        .index = (uint64_t)ptree->root.data.offset - 1
    };
}

ecs_ptree_iter_t ecs_ptree_iter(
    ecs_ptree_t *ptree)
{
    return (ecs_ptree_iter_t){
        .ptree = ptree,
        .frames[0] = &ptree->root,
        .index = (uint64_t)ptree->min_65k - 1
    };
}

void* _ecs_ptiny_next(
    ecs_ptree_iter_t *it,
    ecs_size_t elem_size)
{
    int8_t sp = it->sp;
    uint16_t cur_page = it->cur_page[sp];
    int32_t cur_elem = it->cur_elem;

    ecs_ptree_t *pt = it->ptree;
    if ((it->index == (uint64_t)-1) || (it->index < 65536)) {
        array_t *root_data = &pt->root.data;
        uint16_t max = root_data->offset + root_data->length;
        uint64_t index = ++ it->index;
        if (index < max) {
            return array_get(&pt->root.data, elem_size, index);
        }
    }

    do {
        page_t *frame = it->frames[sp];

        if (cur_page < frame->pages.length) {
            page_t *page = ECS_OFFSET(frame->pages.data, 
                cur_page * sizeof(page_t));

            if (cur_elem < page->data.length) {
                void *result = ECS_OFFSET(page->data.data, cur_elem * elem_size);
                ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
                it->index = index_from_it(it, page);
                it->cur_elem ++;
                return result;
            }

            if (page->pages.length) {
                sp = ++ it->sp;
                ecs_assert(sp < 3, ECS_INTERNAL_ERROR, NULL);
                cur_page = it->cur_page[sp] = 0;
                it->frames[sp] = page;
                cur_elem = it->cur_elem = 0;
                continue;
            }

            if (cur_page < (frame->pages.length - 1)) {
                cur_page = ++ it->cur_page[sp];
                cur_elem = it->cur_elem = 0;
                continue;
            }
        }

        if (sp) {
            sp = -- it->sp;
            cur_page = ++ it->cur_page[sp];
            continue;
        }

        return NULL;
    } while (1);
}

void* _ecs_ptree_next(
    ecs_ptree_iter_t *it,
    ecs_size_t elem_size)
{
    ecs_ptree_t *pt = it->ptree;
    if ((it->index == (uint64_t)-1) || (it->index < pt->max_65k)) {
        uint64_t index = ++ it->index;
        return ECS_OFFSET(pt->first_65k, index * pt->elem_size);
    } else {
        return _ecs_ptiny_next(it, elem_size);
    }
}

void* _ecs_ptiny_ensure(
    ecs_ptree_t *ptree,
    ecs_size_t elem_size,
    uint64_t index)
{
    ecs_assert(elem_size == (ecs_size_t)ptree->elem_size, ECS_INVALID_PARAMETER, NULL);

    addr_t addr = to_addr(index);
    page_t *p = get_or_create_page(&ptree->root, addr.value, page_count(index));

    ecs_assert(p != NULL, ECS_INTERNAL_ERROR, NULL);
    void *data = array_ensure(&p->data, elem_size, addr.value[0]);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    return data;
}

void* _ecs_ptree_ensure(
    ecs_ptree_t *ptree,
    ecs_size_t elem_size,
    uint64_t index)
{
    ecs_assert(elem_size == (ecs_size_t)ptree->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ptree->first_65k != NULL, ECS_INVALID_PARAMETER, NULL);

    if (index < 65536) {
        if (index > ptree->max_65k) {
            ptree->max_65k = (uint16_t)index;
        }
        if (index < ptree->min_65k) {
            ptree->min_65k = (uint16_t)index;
        }
        return ECS_OFFSET(ptree->first_65k, (uint8_t)elem_size * index);
    } else {
       return _ecs_ptiny_ensure(ptree, (uint8_t)elem_size, index);
    }
}

void* _ecs_ptiny_get(
    ecs_ptree_t *ptree,
    ecs_size_t elem_size,
    uint64_t index)
{
    ecs_assert(elem_size == (ecs_size_t)ptree->elem_size, ECS_INVALID_PARAMETER, NULL);

    int32_t pcount = page_count(index);
    addr_t addr = to_addr(index);
    page_t *p = get_page(&ptree->root, addr.value, pcount);
    if (!p) {
        return NULL;
    }
    return array_get(&p->data, elem_size, addr.value[0]);
}

void* _ecs_ptree_get(
    ecs_ptree_t *ptree,
    ecs_size_t elem_size,
    uint64_t index)
{
    ecs_assert(elem_size == (ecs_size_t)ptree->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ptree->first_65k != NULL, ECS_INTERNAL_ERROR, NULL);

    if (index < 65536) {
        return ECS_OFFSET(ptree->first_65k, (uint8_t)elem_size * index);
    } else {
        return _ecs_ptiny_get(ptree, (uint8_t)elem_size, index);
    }
}
