cmake_minimum_required(VERSION 4.1.2)
project(specs)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -g")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cpp)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(AddGoogleTest)
include(AddFmt)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

set(AddBoost.cmake_VERSION 3.7.3)
CPMAddPackage(
  NAME AddBoost.cmake
  VERSION "${AddBoost.cmake_VERSION}"
  URL "https://github.com/Arniiiii/AddBoost.cmake/archive/refs/tags/${AddBoost.cmake_VERSION}.tar.gz"
)

set(TRY_BOOST_VERSION "1.88.0.beta1")
set(BOOST_MY_OPTIONS "BOOST_ENABLE_PYTHON ON;")
set(BOOST_NOT_HEADER_ONLY_COMPONENTS_THAT_YOU_NEED "thread;system")
set(BOOST_HEADER_ONLY_COMPONENTS_THAT_YOU_NEED "dynamic_bitset;asio;beast;uuid;container;cobalt")

add_boost(
  TRY_BOOST_VERSION BOOST_HEADER_ONLY_COMPONENTS_THAT_YOU_NEED
  BOOST_NOT_HEADER_ONLY_COMPONENTS_THAT_YOU_NEED ${PROJECT_NAME} src
)


target_link_libraries(${PROJECT_NAME} PRIVATE src)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt)

add_subdirectory(test)