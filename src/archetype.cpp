#include "archetype.h"
#include "boost/dynamic_bitset_fwd.hpp"
#include "types.h"
#include <cassert>
#include <cstddef>
#include <expected>
#include <memory>
#include <stdexcept>
#include <vector>

Archetype::Archetype(boost::dynamic_bitset<> bitset)
    : managed_components(bitset) {}

Archetype *Archetype::make_of_my_type() {
  Archetype *cloned = new Archetype(managed_components);
  for (auto pair : component_id_to_array) {
    cloned->component_id_to_array.insert(
        {pair.first, pair.second->make_of_my_type()});
  }
  return cloned;
}

std::expected<void, EcsError> Archetype::transfer(Archetype *other,
                                                  entity_id e_id) {
  entities.emplace(e_id);
  for (auto pair : component_id_to_array) {
    if(!other->component_id_to_array.contains(pair.first)) continue;
    std::shared_ptr<IComponentArray> component_array =
        other->component_id_to_array.at(pair.first);
    if (!pair.second->transfer_entity(component_array, e_id)) {
      assert(false && "somthing went wrong");
    }
  }
  other->entities.erase(e_id);
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
  return managed_components.size() > c_id && managed_components.test(c_id);
}
size_t Archetype::size() { return entities.size(); }

ArchetypeManager::ArchetypeManager() {}

ArchetypeManager::~ArchetypeManager() {}

std::expected<void, EcsError>
ArchetypeManager::remove_component_entity(entity_id e_id, component_id c_id,
                                          boost::dynamic_bitset<> old_bitset) {
  auto old_result = get_archetype(old_bitset);
  if (!old_result)
    assert(false && "Why is there no bitset");
  // if (!old_archetype.has_entity(e_id))
  //   return std::unexpected(NoEntity);
  old_bitset.set(c_id, false);
  auto new_result = get_archetype(old_bitset);
  if (!new_result) {
    if (!old_result.value()->remove_entity(e_id))
      assert(false && "what?");
    return {};
  }

  if (!new_result.value()->transfer(old_result.value(), e_id)) {
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
  auto current_result = get_archetype(current_bitset);
  if(!current_result) assert(false && "bad?");

  if (!current_result.value()->remove_entity(e_id))
    assert(false && "this should never happen");
  managed_entities.set(e_id, false);
  return {};
}

bool ArchetypeManager::has_entity(entity_id e_id) {
  return e_id < managed_entities.size() && managed_entities.test(e_id);
}

std::expected<Archetype *, EcsError>
ArchetypeManager::get_archetype(boost::dynamic_bitset<> bitset) {
  try {
    return bitset_to_archetype.at(bitset);
  } catch (std::out_of_range) {
    return std::unexpected(NoArchetype);
  }
}

bool satisfies_query(boost::dynamic_bitset<> b1, boost::dynamic_bitset<> qs) {
  if (b1.size() < qs.size())
    return false;
  for (size_t i = 0; i < qs.size(); i++) {
    if (b1.test(i) != qs.test(i))
      return false;
  }
  return true;
}

std::vector<Archetype *>
ArchetypeManager::get_archetypes(boost::dynamic_bitset<> query_bitset) {
  std::vector<Archetype *> ret;
  for (auto pair : bitset_to_archetype) {
    if (satisfies_query(pair.first, query_bitset)) { // TODO
      ret.push_back(pair.second);
    }
  }
  return ret;
}