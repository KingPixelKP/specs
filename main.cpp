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

  entity_id e1 = core.create_entity();
  entity_id e2 = core.create_entity();

  std::ignore = core.add_component<Dingus>(e1, Dingus{1, "hi"});
  std::ignore = core.add_component<Singus>(e2, Singus{2});

  (core.has_component<Dingus>(e1).value());
  (core.has_component<Singus>(e2).value());

  auto result = core.get_component<Dingus>(e1);
  assert(result.value().get().number == 1);
  result.value().get().number = 2;
  result = core.get_component<Dingus>(e1);
  assert(result.value().get().number == 2);

  auto query_result = core.get_querry<Dingus, Singus, Lingus>();

}