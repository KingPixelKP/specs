#pragma once
#include "archetype.h"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include <vector>
#ifndef QUERY_H
#define QUERY_H

class Core;

class QueryIterator {
public:
  QueryIterator(Core *core, boost::dynamic_bitset<> bitset,
                std::vector<Archetype *>);

  bool has_next();
  void next();
  template <typename T> T &get();

private:
  size_t current_archetype_index;
  size_t current_entity_index;

  Core *core;
  boost::dynamic_bitset<> query_bitset;
  std::vector<Archetype *> archetypes;
};

class Query {
public:
  Query(Core *core, boost::dynamic_bitset<> bitset);

  template <typename T> bool has_component();

  QueryIterator iterator();

private:
  Core *core;
  boost::dynamic_bitset<> query_bitset;
};

#include "core.h"

template <typename T> T &QueryIterator::get() {
  return archetypes.at(current_archetype_index)
      ->get_component_index<T>(current_entity_index,
                               core->get_component_id<T>());
}

template <typename T> bool Query::has_component() {
  return query_bitset.size() > core->get_component_id<T>() &&
         query_bitset.test(core->get_component_id<T>());
}

#endif
