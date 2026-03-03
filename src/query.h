#pragma once
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#ifndef QUERY_H
#define QUERY_H

class Core;

class Query {
public:
  Query(Core *core, boost::dynamic_bitset<> bitset);
  template <typename T> bool has_component();

private:
  Core *core;
  boost::dynamic_bitset<> query_bitset;
};

#include "core.h"

template <typename T> bool Query::has_component() {
  return query_bitset.size() > core->get_component_id<T>() &&
         query_bitset.test(core->get_component_id<T>());
}

#endif