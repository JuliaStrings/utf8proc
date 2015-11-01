cmake_minimum_required (VERSION 2.8)

include (utils.cmake)

disallow_intree_builds()

project (utf8proc C)

# Be sure to also update these in Makefile!
set(SO_MAJOR 1)
set(SO_MINOR 3)
set(SO_PATCH 1)

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

set_target_properties (utf8proc PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  VERSION "${SO_MAJOR}.${SO_MINOR}.${SO_PATCH}"
  SOVERSION ${SO_MAJOR}
)
