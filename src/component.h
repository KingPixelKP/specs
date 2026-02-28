#pragma once
#include "types.h"
#include <cstddef>
#include <sys/stat.h>
#ifndef COMPONENT_H
#define COMPONENT_H

class ComponentManager {
public:
  ComponentManager();
  ~ComponentManager();

  template <typename T>
  component_id get_component_id();

private:
  template <typename T> component_id template_to_component_id();
  component_id registered_components;
};

template <typename T>
component_id ComponentManager::template_to_component_id() {
  static component_id c_id = static_cast<component_id>(registered_components++);
  return c_id;
}

template <typename T>
component_id ComponentManager::get_component_id() {
  return template_to_component_id<T>();
}

#endif