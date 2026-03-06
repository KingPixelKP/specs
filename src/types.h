#pragma once
#include <cstddef>
#ifndef TYPES_H
#define TYPES_H

typedef size_t entity_id;
typedef size_t component_id;

enum EcsError {
    NoEntity,
    NoComponent,
    NoArchetype,
    DupComponent,
    DupEntity,
    NotImplemented,
};



#endif