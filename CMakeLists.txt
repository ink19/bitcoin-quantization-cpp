cmake_minimum_required(VERSION 3.4)

project(BitCoinQuantization LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    #option (ALGODEBUG "Debug Mode" ON)
    message("Debug Mode")
    add_definitions(-DALGODEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
message(${CMAKE_MODULE_PATH})

add_subdirectory("market")
add_subdirectory("notifier")
add_subdirectory("strategy/grid_trading")
find_package(Boost REQUIRED json thread program_options)

add_executable(BitCoinQuantization main.cpp comm/config.cpp utils.cpp)
target_link_libraries(BitCoinQuantization ${Boost_LIBRARIES} market notifier gridtrading)