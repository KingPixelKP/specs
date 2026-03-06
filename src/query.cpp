#include "query.h"
#include "archetype.h"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include "core.h"
#include <vector>

QueryIterator::QueryIterator(Core *core, boost::dynamic_bitset<> bitset,
                             std::vector<Archetype *> archetypes)
    : current_archetype_index(0), current_entity_index(0), core(core),
      query_bitset(bitset), archetypes(archetypes) {};

bool QueryIterator::has_next() {
  return current_archetype_index < archetypes.size() &&
         current_entity_index < archetypes.at(current_archetype_index)->size();
}

void QueryIterator::next() {//TODO make better with modulus
  if (current_entity_index >= archetypes.at(current_archetype_index)->size()) {
    current_archetype_index++;
    current_entity_index = 0;
  } else {
    current_entity_index++;
  }
}

Query::Query(Core *core, boost::dynamic_bitset<> bitset)
    : core(core), query_bitset(bitset) {};

QueryIterator Query::iterator() {
  return QueryIterator(core, query_bitset, core->get_archetypes(query_bitset));
}