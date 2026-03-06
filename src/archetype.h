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
   * Transfers an entity's component from other into this
   * @param other another component array with the same type as this
   * @param e_id an entity to transfer
   * @return An error if the entity is not tracked by the array
   */
  virtual std::expected<void, EcsError>
  transfer_entity(std::shared_ptr<IComponentArray> other, entity_id e_id) = 0;

  /**
   * Removes an entity from the component array
   * @param e_id Entity id to remove
   * @return An error if the entity is not tracked by the array
   */
  virtual std::expected<void, EcsError> remove_entity(entity_id e_id) = 0;

  /**
   * Creates another component array tied to the same template as this
   * @return A shared pointer to the just created array
   */
  virtual std::shared_ptr<IComponentArray> make_of_my_type() = 0;
};

/**
 * Stores a mapping of entities to their components
 * @tparam T Component type to be stored
 */
template <typename T> class ComponentArray : public IComponentArray {
public:
  ComponentArray();
  ~ComponentArray();
  std::expected<void, EcsError>
  transfer_entity(std::shared_ptr<IComponentArray> other,
                  entity_id e_id) override;
  std::expected<void, EcsError> remove_entity(entity_id e_id) override;
  std::shared_ptr<IComponentArray> make_of_my_type() override;

  /**
   * Adds a component to an entity
   * @param e_id Entity to add component to
   * @param component Component to add
   * @return Error if the entity was already in the array
   */
  std::expected<void, EcsError> add_component(entity_id e_id, T component);

  /**
   * Retrieves the component tied to the entity
   * @param e_id Entity to get component from
   * @return A reference wrapper to the component, or an error if the entity is
   * not in the array
   */
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

/**
 * Class used to store entities and its components that have a signature equal
 * to it
 */
class Archetype {
public:
  Archetype(boost::dynamic_bitset<> bitset);

  /**
   * Creates a new archetype with the same signature and component arrays as
   * this
   * @return An archetype
   */
  Archetype *make_of_my_type();

  template <typename T> static Archetype *make_of_component(component_id c_id);

  /**
   * Registers a component to this archetyp
   * @tparam T Component to register
   * @param c_id Component id of the registered component
   * @return An error if the component was already registered
   */
  template <typename T>
  std::expected<void, EcsError> register_component(component_id c_id);

  /**
   * Ties a component to an entity
   * @tparam T Component type
   * @param e_id entity to add component to
   * @param c_id component id tied to the type
   * @param component component to add
   * @return An error if the archetype does not manage that component, or if the
   * entity is already in the archetype
   */
  template <typename T>
  std::expected<void, EcsError> add_component(entity_id e_id, component_id c_id,
                                              T component);

  /**
   * Transfers all the components tied to an entity in other into this
   * @param other Archetype to transfer the entity from
   * @param e_id entity to be transferred
   * @return An error if the entity is not tracked by other
   */
  std::expected<void, EcsError> transfer(Archetype *other, entity_id e_id);

  /**
   * Gets a component from an entity
   * @tparam T Component type to get
   * @param e_id entity to get component from
   * @param c_id component id tied to that type
   * @return Component tied to an entity, or an error if the archetype does not
   * manage the component or does not have the entity
   */
  template <typename T>
  std::expected<std::reference_wrapper<T>, EcsError>
  get_component(entity_id e_id, component_id c_id);

  template <typename T> T &get_component_index(size_t index, component_id c_id);

  /**
   * Removes an entity and all its components from the archetype
   * @param e_id Entity to remove
   * @return An error if the entity did not exist in the archetype
   */
  std::expected<void, EcsError> remove_entity(entity_id e_id);

  bool has_entity(entity_id e_id);
  bool manages_component(component_id c_id);
  size_t size();

private:
  template <typename T>
  std::shared_ptr<ComponentArray<T>>
  get_component_array_fast(component_id c_id);

  std::unordered_map<component_id, std::shared_ptr<IComponentArray>>
      component_id_to_array;
  std::set<entity_id> entities;
  boost::dynamic_bitset<> managed_components;
};

template <typename T>
Archetype *Archetype::make_of_component(component_id c_id) {
  auto bitset = boost::dynamic_bitset<>();
  Archetype *new_archetype = new Archetype(bitset);
  new_archetype->register_component<T>(c_id);
  return new_archetype;
}

template <typename T>
std::expected<void, EcsError> Archetype::register_component(component_id c_id) {
  if (component_id_to_array.contains(c_id))
    return std::unexpected(DupComponent);

  std::shared_ptr<IComponentArray> ptr =
      std::static_pointer_cast<IComponentArray>(
          std::make_shared<ComponentArray<T>>());

  component_id_to_array.insert({c_id, ptr});
  if (managed_components.size() <= c_id)
    managed_components.resize(c_id + 1);
  managed_components.set(c_id);
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

  return component_array->add_component(e_id, component);
}

template <typename T>
std::expected<std::reference_wrapper<T>, EcsError>
Archetype::get_component(entity_id e_id, component_id c_id) {
  // if (!has_entity(e_id))
  //   return std::unexpected(NoEntity);
  if (!manages_component(c_id))
    return std::unexpected(NoComponent);

  std::shared_ptr<ComponentArray<T>> component_array =
      get_component_array_fast<T>(c_id);

  return component_array->get_component_entity(e_id);
}

template <typename T>
T &Archetype::get_component_index(size_t index, component_id c_id) {
  assert(manages_component(c_id));

  std::shared_ptr<ComponentArray<T>> component_array =
      get_component_array_fast<T>(c_id);

  return component_array->get_component_index(index);
}

template <typename T>
std::shared_ptr<ComponentArray<T>>
Archetype::get_component_array_fast(component_id c_id) {
  static std::shared_ptr<ComponentArray<T>> component_array =
      std::static_pointer_cast<ComponentArray<T>>(
          component_id_to_array.at(c_id));
  return component_array;
}

class ArchetypeManager {
public:
  ArchetypeManager();
  ~ArchetypeManager();

  /**
   * Adds a component to an entity
   * @tparam T Type of component to get
   * @param e_id Entity to add component from
   * @param c_id Component id tied to the component type
   * @param component Component to add
   * @param old_bitset Old bitset of the entity (before this component was
   * added)
   * @return An error if the entity already had that component
   */
  template <typename T>
  std::expected<void, EcsError>
  add_component_entity(entity_id e_id, component_id c_id, T component,
                       boost::dynamic_bitset<> old_bitset);

  /**
   * Removes a component from an entity
   * @param e_id Entity to remove component from
   * @param c_id Component id of the component to remove
   * @param old_bitset Old bitset of the entity (before this component was
   * removed)
   * @return An error if the entity does not exist or does not have that
   * component
   */
  std::expected<void, EcsError>
  remove_component_entity(entity_id e_id, component_id c_id,
                          boost::dynamic_bitset<> old_bitset);

  /**
   * Retrieves a component of an entity
   * @tparam T Component type to get
   * @param e_id Entity to get component from
   * @param c_id Component id tied to the component type
   * @param current_bitset Current bitset (signature) of the entity
   * @return The component of that entity, or an error if the entity does not
   * exist or if it does not have that component
   */
  template <typename T>
  std::expected<std::reference_wrapper<T>, EcsError>
  get_component(entity_id e_id, component_id c_id,
                boost::dynamic_bitset<> current_bitset);

  std::vector<Archetype *> get_archetypes(boost::dynamic_bitset<> query_bitset);

private:
  /**
   *
   * @param e_id Entity to start tracking
   * @return An error if the entity is already being tracked
   */
  std::expected<void, EcsError> add_entity(entity_id e_id);
  std::expected<void, EcsError>
  remove_entity(entity_id e_id, boost::dynamic_bitset<> current_bitset);
  bool has_entity(entity_id e_id);

  std::expected<Archetype *, EcsError>
  get_archetype(boost::dynamic_bitset<> bitset);

  boost::dynamic_bitset<> managed_entities;

  std::unordered_map<boost::dynamic_bitset<>, Archetype *> bitset_to_archetype;
};

template <typename T>
std::expected<void, EcsError>
ArchetypeManager::add_component_entity(entity_id e_id, component_id c_id,
                                       T component,
                                       boost::dynamic_bitset<> old_bitset) {
  std::ignore = add_entity(e_id); // Should not be a problem to ignore
  auto old_result = get_archetype(old_bitset);
  if (old_bitset.size() <= c_id)
    old_bitset.resize(c_id + 1);
  old_bitset.set(c_id); // new bitset
  auto new_result = get_archetype(old_bitset);
  Archetype *new_archetype;
  if (old_result) {
    auto old_archetype = old_result.value();
    if (old_archetype->manages_component(c_id))
      return std::unexpected(DupComponent);

    if (!new_result) {
      new_archetype = old_archetype->make_of_my_type();
      new_archetype->register_component<T>(c_id);
      bitset_to_archetype.insert({old_bitset, new_archetype});
    }

    if (old_archetype->has_entity(e_id)) {
      if (!new_archetype->transfer(old_archetype, e_id)) {
        assert(false && "something went wrong");
      }
    }
  } else {
    new_archetype = Archetype::make_of_component<T>(c_id);
    bitset_to_archetype.insert({old_bitset, new_archetype});
  }
  if (!new_archetype->add_component<T>(e_id, c_id, component))
    assert(false && "Something went wrong");
  return {};
}

template <typename T>
std::expected<std::reference_wrapper<T>, EcsError>
ArchetypeManager::get_component(entity_id e_id, component_id c_id,
                                boost::dynamic_bitset<> current_bitset) {
  if (!has_entity(e_id))
    return std::unexpected(NoEntity);
  auto result = get_archetype(current_bitset);
  if (!result)
    assert(false && "Why is there no archetype?");
  return result.value()->get_component<T>(e_id, c_id);
}
#endif