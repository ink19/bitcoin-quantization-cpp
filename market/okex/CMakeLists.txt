cmake_minimum_required(VERSION 3.4)
project(OkexApi VERSION 0.1 DESCRIPTION "Okex V5 API" LANGUAGES CXX)


add_library(okex STATIC okex.cpp)
set_target_properties(okex PROPERTIES VERSION ${PROJECT_VERSION})
