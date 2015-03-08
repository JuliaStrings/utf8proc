cmake_minimum_required (VERSION 2.8)

include (utils.cmake)

disallow_intree_builds()

project (utf8proc C)

add_definitions (
  -DUTF8PROC_EXPORTS
)

if (NOT MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=c99 -pedantic -Wall")
endif ()

add_library (utf8proc
  utf8proc.c
  utf8proc.h
)

set_property (TARGET utf8proc PROPERTY POSITION_INDEPENDENT_CODE ON)
