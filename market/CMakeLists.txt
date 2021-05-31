cmake_minimum_required(VERSION 3.4)
project(MarketApi VERSION 0.1 DESCRIPTION "MarketApi API" LANGUAGES CXX)

add_subdirectory("okex")

file(GLOB SOURCE_FILES ./*.cpp)


add_library(market STATIC ${SOURCE_FILES})
set_target_properties(market PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(market okex)