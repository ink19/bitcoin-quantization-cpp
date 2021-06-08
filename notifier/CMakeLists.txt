cmake_minimum_required(VERSION 3.4)
project(NotifierApi VERSION 0.1 DESCRIPTION "MarketApi API" LANGUAGES CXX)

add_subdirectory("dingding")
add_subdirectory("wework")

file(GLOB SOURCE_FILES ./*.cpp)


add_library(notifier STATIC ${SOURCE_FILES})
set_target_properties(notifier PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(notifier dingding wework)