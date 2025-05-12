
cmake_minimum_required(VERSION 3.15...4.0)

project(
  InvictiExercise
  VERSION 1.0
  LANGUAGES CXX)

set(SOURCES
    src/main.cpp
    src/task.cpp
)

add_executable(main ${SOURCES})

target_include_directories(main
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

include_directories("cpp-httplib")
include_directories("myhtml")







add_library( myhtml SHARED IMPORTED )
set_target_properties( myhtml PROPERTIES IMPORTED_LOCATION /usr/local/lib/libmyhtml.so)

target_link_libraries(main PUBLIC myhtml)
