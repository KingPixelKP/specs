#pragma once
#include "archetype.h"
#include "component.h"
#include "entity.h"
#include "types.h"
#include <expected>
#ifndef CORE_H
#define CORE_H

class Core {
public:
  Core();
  ~Core();

  entity_id create_entity();
  std::expected<void, EcsError> destroy_entity(entity_id e_id);

  template <typename T> component_id get_component_id();

  template <typename T>
  std::expected<void, EcsError> add_component(entity_id e_id, T component);
  template <typename T>
  std::expected<void, EcsError> remove_component(entity_id e_id);

  template <typename T>
  std::expected<bool, EcsError> has_component(entity_id e_id);

private:
  EntityManager entity_manager;
  ComponentManager component_manager;
  ArchetypeManager archetype_manager;
};

template <typename T> component_id Core::get_component_id() {
  return component_manager.get_component_id<T>();
}

template <typename T>
std::expected<void, EcsError> Core::add_component(entity_id e_id, T component) {
  component_id c_id = get_component_id<T>();
  auto add_result = entity_manager.component_added(e_id, c_id);
  if (!add_result)
    return add_result;
  // Add to archetype
  return {};
}

template <typename T>
std::expected<void, EcsError> Core::remove_component(entity_id e_id) {
  component_id c_id = component_manager.get_component_id<T>();
  auto remove_result = entity_manager.component_removed(e_id, c_id);
  if (!remove_result)
    return remove_result;
  // Remove from archetype
  return {};
}

template <typename T>
std::expected<bool, EcsError> Core::has_component(entity_id e_id) {
  component_id c_id = component_manager.get_component_id<T>();
  return entity_manager.has_component(e_id, c_id);
}

#endif