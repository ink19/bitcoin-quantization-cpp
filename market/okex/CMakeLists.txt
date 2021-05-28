cmake_minimum_required(VERSION 3.4)
project(OkexApi VERSION 0.1 DESCRIPTION "Okex V5 API" LANGUAGES CXX)

find_package(CryptoPP REQUIRED)
find_package(Boost REQUIRED system)
find_package(OpenSSL REQUIRED)

add_library(okex STATIC okex.cpp)
set_target_properties(okex PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(okex ${CryptoPP_LIBRARIES} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})