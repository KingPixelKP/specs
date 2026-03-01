#pragma once
#include "boost/dynamic_bitset_fwd.hpp"
#include "types.h"
#include <boost/dynamic_bitset.hpp>
#include <cstddef>
#include <expected>
#include <stack>
#include <unordered_map>
#ifndef ENTITY_H
#define ENTITY_H

#ifndef INITIAL_ENTITIES
#define INITIAL_ENTITIES 1024
#endif

class EntityManager {
public:
  EntityManager();
  ~EntityManager();

  entity_id create_entity();
  std::expected<void, EcsError> destroy_entity(entity_id e_id);

  std::expected<void, EcsError> component_added(entity_id e_id,
                                                component_id c_id);
  std::expected<void, EcsError> component_removed(entity_id e_id,
                                                  component_id c_id);
  std::expected<boost::dynamic_bitset<>, EcsError> entity_bitset(entity_id e_id);
  std::expected<bool, EcsError> has_component(entity_id e_id,
                                              component_id c_id);

private:
  void expand_entities(int start, int finish);

  std::stack<entity_id> available_entities;

  boost::dynamic_bitset<> alive_entities;
  std::unordered_map<entity_id, boost::dynamic_bitset<>> entity_bitsets;

  size_t num_alive_entities;
};

#endif