#include "query.h"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"

Query::Query(boost::dynamic_bitset<> bitset) : query_bitset(bitset) {};
