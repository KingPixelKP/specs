#include "core.h"
#include "query.h"
#include "types.h"
#include <cassert>
#include <chrono>
#include <iostream>

void core_speed_test() {

  constexpr int etities = 1000000;

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

  Core core;

  for (int i = 0; i < etities; i++) {
    entity_id e = core.create_entity();
    std::ignore = core.add_component<Dingus>(e, Dingus{12, ""});
    std::ignore = core.add_component<Singus>(e, Singus{12});
  }

  Query query = core.get_query<Dingus, Singus>().value();

  assert(query.size() == etities);

  QueryIterator query_iterator = query.iterator();


  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  int iters = 0;

  auto t1 = high_resolution_clock::now();
  while (query_iterator.has_next()) {
    auto &d_c = query_iterator.get<Dingus>();
    auto s_c = query_iterator.get<Singus>();
    iters++;
    query_iterator.next();
  }
  auto t2 = high_resolution_clock::now();
  auto ms_int = duration_cast<milliseconds>(t2 - t1);

  std::cout << "Iterations: "<< iters << " " << ms_int.count() << "ms\n";
}

int main() {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  auto t1 = high_resolution_clock::now();
  core_speed_test();
  auto t2 = high_resolution_clock::now();

  auto ms_int = duration_cast<milliseconds>(t2 - t1);

  std::cout << ms_int.count() << "ms\n";
}