#include "core.h"
#include <cassert>
#include <string>
int main() {
  Core core;

  typedef struct Dingus {
    uint16_t number;
    std::string str;
  } Dingus;

  typedef struct Singus {
    uint16_t number;
  } Singus;

  typedef struct Lingus {
    uint16_t number;
  } Lingus;
  typedef struct Ringus {

  } Ringus;

  entity_id e1 = core.create_entity();
  entity_id e2 = core.create_entity();

  std::ignore = core.add_component<Dingus>(e1, Dingus{1, "hi"});
  std::ignore = core.add_component<Singus>(e1, Singus{2});
  std::ignore = core.add_component<Singus>(e2, Singus{2});

  auto query_result = core.get_query<Dingus, Singus>();

  (query_result);

  auto query = query_result.value();

  (query.has_component<Dingus>());
  (query.has_component<Singus>());
  (query.has_component<Lingus>());
  (query.has_component<Ringus>());

  auto query_it = query.iterator();

  while (query_it.has_next()) {
    auto &d_c = query_it.get<Dingus>();
    auto s_c = query_it.get<Singus>();
    d_c.number += s_c.number;
    query_it.next();
  }

  (core.get_component<Dingus>(e1).value().get().number, 3);
}