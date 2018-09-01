#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

typedef struct EcsComponentElement {
    void *data_ptr;
    EcsEntity *type;
} EcsComponentElement;

struct EcsEntity {
    EcsEntity *base;
    EcsVector *components;
};

#endif
