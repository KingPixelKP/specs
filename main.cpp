#include "core.h"
int main() {

  Core core;

  typedef struct Dingus {
    uint16_t number;
  } Dingus;
  typedef struct Singus {
    uint16_t number;
  } Singus;

  entity_id e1 = core.create_entity();
  entity_id e2 = core.create_entity();

  std::ignore = core.add_component<Dingus>(e1, Dingus{1});
  std::ignore = core.add_component<Singus>(e2, Singus{2});

  core.has_component<Dingus>(e1);
  core.has_component<Singus>(e2);
  !core.has_component<Singus>(e1);
  !core.has_component<Dingus>(e2);
  return 0;
}