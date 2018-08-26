#ifndef ECS_TYPES_H
#define ECS_TYPES_H

typedef struct EcsComponentElement {
    void *data_ptr;
    EcsEntity *type;
} EcsComponentElement;

struct EcsEntity {
    EcsEntity *prefab;
    EcsVector *components;
};

#endif
