#pragma once
#include <cstdint>
#ifndef TYPES_H
#define TYPES_H

typedef uint16_t entity_id;
typedef uint16_t component_id;

enum EcsError {
    NoEntity,
    NoComponent,
    DupComponent,
    DupEntity,
    NotImplemented,
};



#endif