#include "core.h"
#include "types.h"
#include <cstdint>
#include <gtest/gtest.h>
#include <tuple>

TEST(Core, CreateEntity1) {
  Core core;
  EXPECT_EQ(core.create_entity(), 0);
  EXPECT_EQ(core.create_entity(), 1);
  EXPECT_EQ(core.create_entity(), 2);
}

TEST(Core, CreateEntity2) {
  Core core;
  for (int i = 0; i < 10000; i++) {
    EXPECT_EQ(core.create_entity(), i);
  }
}

TEST(Core, ComponentId) {
  Core core;

  typedef struct Dingus {
    uint16_t number;
  } Dingus;
  typedef struct Singus {
    uint16_t number;
  } Singus;
  typedef struct Lingus {
    uint16_t number;
  } Lingus;

  EXPECT_EQ(core.get_component_id<Dingus>(), 0);
  EXPECT_EQ(core.get_component_id<Singus>(), 1);
  EXPECT_EQ(core.get_component_id<Lingus>(), 2);
  EXPECT_EQ(core.get_component_id<Dingus>(), 0);
  EXPECT_EQ(core.get_component_id<Singus>(), 1);
  EXPECT_EQ(core.get_component_id<Lingus>(), 2);
}

TEST(Core, CoponentAdd) {
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

  EXPECT_TRUE(core.has_component<Dingus>(e1).value());
  EXPECT_TRUE(core.has_component<Singus>(e2).value());
  EXPECT_TRUE(!core.has_component<Singus>(e1).value());
  EXPECT_TRUE(!core.has_component<Dingus>(e2).value());

  // Duplicate components expects an error
  EXPECT_TRUE(!core.add_component<Dingus>(e1, Dingus{1}));
}

TEST(Core, ComponentRemove) {
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

  EXPECT_TRUE(core.has_component<Dingus>(e1).value());
  EXPECT_TRUE(core.has_component<Singus>(e2).value());
  EXPECT_TRUE(!core.has_component<Singus>(e1).value());
  EXPECT_TRUE(!core.has_component<Dingus>(e2).value());

  std::ignore = core.remove_component<Dingus>(e1);

  EXPECT_TRUE(!core.has_component<Dingus>(e1).value());

  EXPECT_TRUE(!core.remove_component<Dingus>(e1));
  EXPECT_TRUE(!core.remove_component<Dingus>(e2));
}

TEST(Core, GetComponent) {

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

  EXPECT_TRUE(core.has_component<Dingus>(e1).value());
  EXPECT_TRUE(core.has_component<Singus>(e2).value());

  auto result = core.get_component<Dingus>(e1).value();
  EXPECT_EQ(result.get().number, 1);
  result.get().number = 2;
  result = core.get_component<Dingus>(e1).value();
  EXPECT_EQ(result.get().number, 2);
}

TEST(Core, QueryCreation) {
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
  std::ignore = core.add_component<Singus>(e2, Singus{2});

  auto query_result = core.get_querry<Dingus, Singus, Lingus>();

  EXPECT_TRUE(query_result);

  auto query = query_result.value();

  EXPECT_TRUE(query.has_component<Dingus>());
  EXPECT_TRUE(query.has_component<Singus>());
  EXPECT_TRUE(query.has_component<Lingus>());
  EXPECT_FALSE(query.has_component<Ringus>());
}