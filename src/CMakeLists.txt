cmake_minimum_required(VERSION 3.0)

project(Filepattern)

option(ARROW_LINK_SHARED "Link to the Arrow shared library" ON)

find_package(Arrow REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

message(STATUS "Arrow version: ${ARROW_VERSION}")
message(STATUS "Arrow SO version: ${ARROW_FULL_SO_VERSION}")

add_executable(filepattern functions.h functions.cpp filepattern.cpp)

if (ARROW_LINK_SHARED)
  target_link_libraries(filepattern PRIVATE arrow_shared)
else()
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(filepattern PRIVATE arrow_static Threads::Threads)
endif()
