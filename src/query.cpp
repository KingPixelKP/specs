#include "query.h"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include "core.h"

Query::Query(Core* core, boost::dynamic_bitset<> bitset) : core(core), query_bitset(bitset) {};
