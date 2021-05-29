cmake_minimum_required(VERSION 3.4)
project(DingDingApi VERSION 0.1 DESCRIPTION "DingDing API" LANGUAGES CXX)

find_package(CryptoPP REQUIRED)
find_package(Boost REQUIRED system)
find_package(OpenSSL REQUIRED)

add_library(dingding STATIC dingding.cpp)
set_target_properties(dingding PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(dingding ${CryptoPP_LIBRARIES} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})