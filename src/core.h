#pragma once
#include "archetype.h"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include "component.h"
#include "entity.h"
#include "types.h"
#include <expected>
#include <functional>
#include <vector>
#ifndef CORE_H
#define CORE_H

class Query;

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
  std::expected<std::reference_wrapper<T>, EcsError>
  get_component(entity_id e_id);

  template <typename T>
  std::expected<bool, EcsError> has_component(entity_id e_id);

  template <typename... T> std::expected<Query, EcsError> get_query();

  std::vector<Archetype*> get_archetypes(boost::dynamic_bitset<> query_bitset);

private:
  template <typename Head, typename... Tail>
  boost::dynamic_bitset<> component_bitset(boost::dynamic_bitset<> bitset);

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
  auto bitset_result = entity_manager.entity_bitset(e_id);
  if (!bitset_result)
    return std::unexpected(bitset_result.error());

  auto comp_result = archetype_manager.add_component_entity<T>(
      e_id, c_id, component, bitset_result.value());
  if (!comp_result)
    return comp_result;

  auto add_result = entity_manager.component_added(e_id, c_id);
  if (!add_result)
    return add_result;
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
std::expected<std::reference_wrapper<T>, EcsError>
Core::get_component(entity_id e_id) {
  component_id c_id = component_manager.get_component_id<T>();
  auto entity_fetch = entity_manager.entity_bitset(e_id);
  if (!entity_fetch)
    return std::unexpected(entity_fetch.error());
  return archetype_manager.get_component<T>(e_id, c_id, entity_fetch.value());
}

template <typename T>
std::expected<bool, EcsError> Core::has_component(entity_id e_id) {
  component_id c_id = component_manager.get_component_id<T>();
  return entity_manager.has_component(e_id, c_id);
}

#include "query.h"

template <typename... T> std::expected<Query, EcsError> Core::get_query() {
  boost::dynamic_bitset<> bitset;
  return Query(this, component_bitset<T...>(bitset));
}

template <typename Head, typename... Tail>
boost::dynamic_bitset<> Core::component_bitset(boost::dynamic_bitset<> bitset) {
  if (bitset.size() <= get_component_id<Head>())
    bitset.resize(get_component_id<Head>() + 1);
  bitset.set(get_component_id<Head>());
  if constexpr (sizeof...(Tail) > 0) {
    return component_bitset<Tail...>(bitset);
  } else {
    return bitset;
  }
}

#endif