include(AddGoogleTest)

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")
message(STATUS "Test sources: ${TEST_SOURCES}")

add_executable(
    unit_tests
    ${TEST_SOURCES}
)

target_link_libraries(
  unit_tests PRIVATE src GTest::gtest_main fmt
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
