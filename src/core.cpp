#include "core.h"
#include "types.h"
#include <expected>

Core::Core() {

}
Core::~Core() {
    
}
entity_id Core::create_entity() {
    return entity_manager.create_entity();
}
std::expected<void, EcsError> Core::destroy_entity(entity_id e_id) {
    return entity_manager.destroy_entity(e_id);
}

