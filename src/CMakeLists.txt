file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_library(src ${PROJECT_SOURCES})
target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})