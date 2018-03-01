cmake_minimum_required (VERSION 2.8)

include (utils.cmake)

set(HASCMAKEHELPERS EXISTS "${CMAKE_SOURCE_DIR}/../CMakeHelpers/generate_product_version.cmake")

if(${HASCMAKEHELPERS})
  include ("${CMAKE_SOURCE_DIR}/../CMakeHelpers/generate_product_version.cmake")
endif ()

disallow_intree_builds()

project (utf8proc C)

# This is the ABI version number, which may differ from the
# API version number (defined in utf8proc.h).
# Be sure to also update these in Makefile and MANIFEST!
set(SO_MAJOR 2)
set(SO_MINOR 1)
set(SO_PATCH 0)

add_definitions (
  -DUTF8PROC_EXPORTS
)

if (NOT MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=c99 -pedantic -Wall")
endif ()

if(${HASCMAKEHELPERS})
  generate_product_version(ProductVersionFiles
    NAME utf8proc
    VERSION_MAJOR ${SO_MAJOR}
    VERSION_MINOR ${SO_MINOR}
    VERSION_PATCH ${SO_PATCH}
    COMPANY_NAME julialang.org)
endif ()

add_library (utf8proc
  utf8proc.c
  utf8proc.h
  ${ProductVersionFiles}
)

set_target_properties (utf8proc PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  VERSION "${SO_MAJOR}.${SO_MINOR}.${SO_PATCH}"
  SOVERSION ${SO_MAJOR}
)
