cmake_minimum_required(VERSION 3.4)
project(GridTrading VERSION 0.1 DESCRIPTION "Grid Trading API" LANGUAGES CXX)

find_package(Boost REQUIRED system)
find_package(VSqlite REQUIRED)

add_library(gridtrading STATIC trading.cpp)
set_target_properties(gridtrading PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(gridtrading  ${Boost_LIBRARIES} ${VSQLITE_LIBRARIES})