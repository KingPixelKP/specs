#pragma once
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include "types.h"
#include <cassert>
#include <cstddef>
#include <expected>
#include <functional>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>
#ifndef ARCHETYPE_H
#define ARCHETYPE_H

class IComponentArray {
public:
  virtual ~IComponentArray() = default;
  /**
  Other should be another component array with the same type as this otherwise
  some bad stuff mat happen
   */
  virtual std::expected<void, EcsError>
  transfer_entity(std::shared_ptr<IComponentArray> other, entity_id e_id) = 0;
  virtual std::expected<void, EcsError> remove_entity(entity_id e_id) = 0;
  /**
  Creates another ComponentArray with the same template that the caller has */
  virtual std::shared_ptr<IComponentArray> make_of_my_type() = 0;
};

template <typename T> class ComponentArray : public IComponentArray {
public:
  ComponentArray();
  ~ComponentArray();
  std::expected<void, EcsError>
  transfer_entity(std::shared_ptr<IComponentArray> other,
                  entity_id e_id) override;
  std::expected<void, EcsError> remove_entity(entity_id e_id) override;
  std::shared_ptr<IComponentArray> make_of_my_type() override;
  std::expected<void, EcsError> add_component(entity_id e_id, T component);
  std::expected<std::reference_wrapper<T>, EcsError>
  get_component_entity(entity_id e_id);
  T &get_component_index(size_t index);

  std::unordered_map<entity_id, size_t> entity_to_index;
  std::vector<entity_id> index_to_entity;
  std::vector<T> data;
};

template <typename T> ComponentArray<T>::ComponentArray() {}
template <typename T> ComponentArray<T>::~ComponentArray() {}

template <typename T>
std::expected<void, EcsError>
ComponentArray<T>::transfer_entity(std::shared_ptr<IComponentArray> other,
                                   entity_id e_id) {
  std::shared_ptr<ComponentArray<T>> other_cmp =
      std::static_pointer_cast<ComponentArray<T>>(other);
  auto result = other_cmp->get_component_entity(e_id);
  if (!result)
    assert(false && "something went wrong");
  T trasnfered = result.value();
  if (!add_component(e_id, trasnfered))
    assert(false && "something bad happened");
  other_cmp->remove_entity(e_id);
  return {};
}
template <typename T>
std::expected<void, EcsError> ComponentArray<T>::remove_entity(entity_id e_id) {

  if (!entity_to_index.contains(e_id))
    return std::unexpected(NoEntity);
  size_t remove_index = entity_to_index.at(e_id);
  size_t last_index = data.size() - 1;
  entity_id last_entity_id = index_to_entity.at(last_index);

  data.at(remove_index) = data.at(last_index);
  index_to_entity.at(remove_index) = last_entity_id;
  entity_to_index.at(last_entity_id) = remove_index;

  data.pop_back();
  index_to_entity.pop_back();
  entity_to_index.erase(e_id);
  return {};
};
template <typename T>
std::shared_ptr<IComponentArray> ComponentArray<T>::make_of_my_type() {
  return std::make_shared<ComponentArray<T>>();
}
template <typename T>
std::expected<void, EcsError> ComponentArray<T>::add_component(entity_id e_id,
                                                               T component) {
  if (entity_to_index.contains(e_id))
    return std::unexpected(DupEntity);

  entity_to_index.insert({e_id, data.size()});
  index_to_entity.push_back(e_id);
  data.push_back(component);

  return {};
}
template <typename T>
std::expected<std::reference_wrapper<T>, EcsError>
ComponentArray<T>::get_component_entity(entity_id e_id) {
  if (!entity_to_index.contains(e_id))
    return std::unexpected(NoEntity);

  return data.at(entity_to_index.at(e_id));
}
template <typename T> T &ComponentArray<T>::get_component_index(size_t index) {
  return data.at(index);
}

class Archetype {
public:
  Archetype make_of_my_type();
  template <typename T>
  std::expected<void, EcsError> register_component(component_id c_id);
  template <typename T>
  std::expected<void, EcsError> add_component(entity_id e_id, component_id c_id,
                                              T component);
  /**Transfer an entity from other to this */
  std::expected<void, EcsError> transfer(Archetype &other, entity_id e_id);
  template <typename T>
  std::expected<std::reference_wrapper<T>, EcsError>
  get_component(entity_id e_id, component_id c_id);
  std::expected<void, EcsError> remove_entity(entity_id e_id);

  bool has_entity(entity_id e_id);
  bool manages_component(component_id c_id);

private:
  std::unordered_map<component_id, std::shared_ptr<IComponentArray>>
      component_id_to_array;
  std::set<entity_id> entities;
};

template <typename T>
std::expected<void, EcsError> Archetype::register_component(component_id c_id) {
  if (component_id_to_array.contains(c_id))
    return std::unexpected(DupComponent);

  std::shared_ptr<IComponentArray> ptr =
      std::static_pointer_cast<IComponentArray>(
          std::make_shared<ComponentArray<T>>());
  component_id_to_array.insert({c_id, ptr});
  return {};
}

template <typename T>
std::expected<void, EcsError>
Archetype::add_component(entity_id e_id, component_id c_id, T component) {
  entities.emplace(e_id);

  if (!manages_component(c_id))
    return std::unexpected(NoComponent);

  std::shared_ptr<ComponentArray<T>> component_array =
      std::static_pointer_cast<ComponentArray<T>>(
          component_id_to_array.at(c_id));

  component_array->add_component(e_id, component);

  return {};
}

template <typename T>
std::expected<std::reference_wrapper<T>, EcsError>
Archetype::get_component(entity_id e_id, component_id c_id) {
  // if (!has_entity(e_id))
  //   return std::unexpected(NoEntity);
  if (!manages_component(c_id))
    return std::unexpected(NoComponent);

  std::shared_ptr<ComponentArray<T>> component_array =
      std::static_pointer_cast<ComponentArray<T>>(
          component_id_to_array.at(c_id));

  return component_array->get_component_entity(e_id);
}

class ArchetypeManager {
public:
  ArchetypeManager();
  ~ArchetypeManager();

  template <typename T>
  std::expected<void, EcsError>
  add_component_entity(entity_id e_id, component_id c_id, T component,
                       boost::dynamic_bitset<> old_bitset);
  std::expected<void, EcsError>
  remove_component_entity(entity_id e_id, component_id c_id,
                          boost::dynamic_bitset<> old_bitset);

  template <typename T>
  std::expected<std::reference_wrapper<T>, EcsError>
  get_component(entity_id e_id, component_id c_id,
                boost::dynamic_bitset<> current_bitset);

private:
  std::expected<void, EcsError> add_entity(entity_id e_id);
  std::expected<void, EcsError>
  remove_entity(entity_id e_id, boost::dynamic_bitset<> current_bitset);
  bool has_entity(entity_id e_id);

  Archetype &get_archetype(boost::dynamic_bitset<> bitset);

  boost::dynamic_bitset<> managed_entities;

  std::unordered_map<boost::dynamic_bitset<>, Archetype> bitset_to_archetype;
};

template <typename T>
std::expected<void, EcsError>
ArchetypeManager::add_component_entity(entity_id e_id, component_id c_id,
                                       T component,
                                       boost::dynamic_bitset<> old_bitset) {
  std::ignore = add_entity(e_id); // Should not be a problem to ignore
  auto &old_archetype = get_archetype(old_bitset);
  if (old_archetype.manages_component(c_id))
    return std::unexpected(DupComponent);
  if (old_bitset.size() <= c_id)
    old_bitset.resize(c_id + 1);
  old_bitset.set(c_id); // new bitset
  auto &new_archetype = get_archetype(old_bitset);
  if (old_archetype.has_entity(e_id)) {
    if (!new_archetype.transfer(old_archetype, e_id)) {
      assert(false && "something went wrong");
    }
  }

  new_archetype.register_component<T>(c_id);

  if (!new_archetype.add_component<T>(e_id, c_id, component))
    assert(false && "Something went wrong");

  return {};
}

template <typename T>
std::expected<std::reference_wrapper<T>, EcsError>
ArchetypeManager::get_component(entity_id e_id, component_id c_id,
                                boost::dynamic_bitset<> current_bitset) {
  if (!has_entity(e_id))
    return std::unexpected(NoEntity);
  auto archetype = get_archetype(current_bitset);
  return archetype.get_component<T>(e_id, c_id);
}

#endif