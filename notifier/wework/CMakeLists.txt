cmake_minimum_required(VERSION 3.4)
project(WeworkApi VERSION 0.1 DESCRIPTION "Wework API" LANGUAGES CXX)

find_package(CryptoPP REQUIRED)
find_package(Boost REQUIRED system)
find_package(OpenSSL REQUIRED)

add_library(wework STATIC wework.cpp)
set_target_properties(wework PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(wework ${CryptoPP_LIBRARIES} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})