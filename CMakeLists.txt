
cmake_minimum_required(VERSION 3.15...4.0)

project(
  InvictiExercise
  VERSION 1.0
  LANGUAGES CXX)

add_executable(main main.cpp)

include_directories("headers")
include_directories("/home/jhc/myhtml")

add_library( myhtml SHARED IMPORTED )
set_target_properties( myhtml PROPERTIES IMPORTED_LOCATION /usr/local/lib/libmyhtml.so)

target_link_libraries(main PUBLIC myhtml)
