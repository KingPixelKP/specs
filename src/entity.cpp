#include "entity.h"
#include "boost/dynamic_bitset_fwd.hpp"
#include "types.h"
#include <cassert>
#include <expected>
#include <iostream>

void EntityManager::expand_entities(int start, int finish) {
  std::cout << "Expanded entities start: " << start << " finish: " << finish
            << std::endl;
  alive_entities.resize(start + finish, false);
  for (int i = finish - 1; i >= start; --i)
    available_entities.push(i);
}

EntityManager::EntityManager() : num_alive_entities(0) {
  expand_entities(num_alive_entities, INITIAL_ENTITIES);
}

EntityManager::~EntityManager() {}

entity_id EntityManager::create_entity() {
  if (available_entities.size() == 0)
    expand_entities(num_alive_entities, num_alive_entities + INITIAL_ENTITIES);
  entity_id ret = available_entities.top();
  available_entities.pop();
  num_alive_entities++;
  assert(ret < alive_entities.size());
  alive_entities.set(ret);
  boost::dynamic_bitset<> set;
  assert(set.size() == 0);
  entity_bitsets.insert({ret, set});
  return ret;
}

std::expected<void, EcsError> EntityManager::destroy_entity(entity_id e_id) {
  if (alive_entities.size() <= e_id || !alive_entities.test(e_id))
    return std::unexpected(NoEntity);
  available_entities.push(e_id);
  num_alive_entities--;
  assert(e_id < alive_entities.size());
  alive_entities.set(e_id, false);
  return {};
}

std::expected<void, EcsError>
EntityManager::component_added(entity_id e_id, component_id c_id) {
  if (alive_entities.size() <= e_id || !alive_entities.test(e_id))
    return std::unexpected(NoEntity);
  auto &entity_bitset = entity_bitsets.at(e_id);
  if (entity_bitset.size() <= c_id)
    entity_bitset.resize(c_id + 1, false);
  assert(c_id < entity_bitset.size());
  if (entity_bitset.test(c_id))
    return std::unexpected(DupComponent);
  entity_bitset.set(c_id);
  return {};
}

std::expected<void, EcsError>
EntityManager::component_removed(entity_id e_id, component_id c_id) {
  if (alive_entities.size() <= e_id || !alive_entities.test(e_id))
    return std::unexpected(NoEntity);
  auto &entity_bitset = entity_bitsets.at(e_id);
  if (entity_bitset.size() < c_id || !entity_bitset.test(c_id))
    return std::unexpected(NoComponent);
  entity_bitset.set(c_id, false);
  return {};
}

std::expected<bool, EcsError> EntityManager::has_component(entity_id e_id,
                                                           component_id c_id) {
  if (alive_entities.size() <= e_id || !alive_entities.test(e_id))
    return std::unexpected(NoEntity);
  return entity_bitsets.at(e_id).size() > c_id && entity_bitsets.at(e_id).test(c_id);
}