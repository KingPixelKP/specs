#include "archetype.h"
#include "boost/dynamic_bitset_fwd.hpp"
#include "types.h"
#include <cassert>
#include <expected>
#include <memory>
#include <stdexcept>

Archetype Archetype::make_of_my_type() {
  Archetype cloned;
  for (auto pair : component_id_to_array) {
    cloned.component_id_to_array.insert(
        {pair.first, pair.second->make_of_my_type()});
  }
  return cloned;
}

std::expected<void, EcsError> Archetype::transfer(Archetype &other,
                                                  entity_id e_id) {
  entities.emplace(e_id);
  for (auto pair : component_id_to_array) {
    std::shared_ptr<IComponentArray> component_array =
        other.component_id_to_array.at(pair.first);
    if (!pair.second->transfer_entity(component_array, e_id)) {
      assert(false && "somthing went wrong");
    }
  }
  if (!other.remove_entity(e_id)) {
    assert(false && "somehting went wrong");
  }
  return {};
}

std::expected<void, EcsError> Archetype::remove_entity(entity_id e_id) {
  // if (!has_entity(e_id))
  //   return std::unexpected(NoEntity);
  for (auto pair : component_id_to_array) {
    if (!pair.second->remove_entity(e_id)) {
      assert(false && "somthing went wrong");
    }
  }

  entities.erase(e_id);
  return {};
}
bool Archetype::has_entity(entity_id e_id) { return entities.contains(e_id); }
bool Archetype::manages_component(component_id c_id) {
  return component_id_to_array.contains(c_id);
}

ArchetypeManager::ArchetypeManager() {}

ArchetypeManager::~ArchetypeManager() {}

std::expected<void, EcsError>
ArchetypeManager::remove_component_entity(entity_id e_id, component_id c_id,
                                          boost::dynamic_bitset<> old_bitset) {
  auto &old_archetype = get_archetype(old_bitset);
  // if (!old_archetype.has_entity(e_id))
  //   return std::unexpected(NoEntity);
  old_bitset.set(c_id, false);
  auto &new_archetype = get_archetype(old_bitset);

  if (!new_archetype.transfer(old_archetype, e_id)) {
    assert(false && "somthing went wrong");
  }

  return {};
}

std::expected<void, EcsError> ArchetypeManager::add_entity(entity_id e_id) {
  if (has_entity(e_id))
    return std::unexpected(DupEntity);
  if (e_id >= managed_entities.size()) {
    managed_entities.resize(e_id + managed_entities.size() + 1);
  }
  managed_entities.set(e_id);
  return {};
}
std::expected<void, EcsError>
ArchetypeManager::remove_entity(entity_id e_id,
                                boost::dynamic_bitset<> current_bitset) {
  if (!has_entity(e_id))
    return std::unexpected(NoEntity);
  auto &current_archetype = get_archetype(current_bitset);
  if (!current_archetype.remove_entity(e_id))
    assert(false && "this should never happen");
  managed_entities.set(e_id, false);
  return {};
}

bool ArchetypeManager::has_entity(entity_id e_id) {
  return e_id < managed_entities.size() && managed_entities.test(e_id);
}

Archetype &ArchetypeManager::get_archetype(boost::dynamic_bitset<> bitset) {
  try {
    return bitset_to_archetype.at(bitset);
  } catch (std::out_of_range) {
    Archetype new_archetype = Archetype();
    bitset_to_archetype.insert({bitset, new_archetype});
    return bitset_to_archetype.at(bitset);
  }
}