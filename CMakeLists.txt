cmake_minimum_required(VERSION 3.15)
#set(CMAKE_CXX_SCAN_FOR_MODULES ON)
#set(BUILD_SHARED_LIBS OFF)

# set the project template
project(template)

include(CTest)
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(src)

# Add and link executable with internal libraries
add_executable(${PROJECT_NAME} main.cpp)
#target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE src)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt)

# add Unit tests
add_subdirectory(test)