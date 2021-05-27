cmake_minimum_required(VERSION 3.4)

project(BitCoinQuantization LANGUAGES CXX)

add_subdirectory("market/okex")
find_package(Boost REQUIRED json)

add_executable(BitCoinQuantization main.cpp)
target_link_libraries(BitCoinQuantization ${Boost_LIBRARIES} okex)