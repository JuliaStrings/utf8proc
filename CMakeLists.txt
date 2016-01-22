cmake_minimum_required (VERSION 2.8)

include (utils.cmake)

disallow_intree_builds()

project (utf8proc C)

# Be sure to also update these in Makefile!
set(SO_MAJOR 2)
set(SO_MINOR 0)
set(SO_PATCH 0)

add_definitions (
  -DUTF8PROC_EXPORTS
)

add_library (utf8proc
  utf8proc.c
  utf8proc.h
)

if (NOT MSVC)
  set_target_properties(
    utf8proc PROPERTIES
    COMPILE_FLAGS "-O2 -std=c99 -pedantic -Wall"
  )
endif ()

set_target_properties (utf8proc PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  VERSION "${SO_MAJOR}.${SO_MINOR}.${SO_PATCH}"
  SOVERSION ${SO_MAJOR}
)
