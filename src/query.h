#pragma once
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#ifndef QUERY_H
#define QUERY_H

class Query {
public:
  Query(boost::dynamic_bitset<> bitset);

private:
  boost::dynamic_bitset<> query_bitset;
};

#endif