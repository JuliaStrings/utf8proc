cmake_minimum_required (VERSION 2.8)

include (utils.cmake)

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

# https://cmake.org/Wiki/BuildingWinDLL
# Allow the developer to select if Dynamic or Static libraries are built, default is static.
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
# Set the LIB_TYPE variable to STATIC
SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
  # User wants to build Dynamic Libraries, so change the LIB_TYPE variable to CMake keyword 'SHARED'
  SET (LIB_TYPE SHARED)
ENDIF (BUILD_SHARED_LIBS)

add_library (utf8proc
  ${LIB_TYPE}
  utf8proc.c
  utf8proc.h
)

set_target_properties (utf8proc PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  VERSION "${SO_MAJOR}.${SO_MINOR}.${SO_PATCH}"
  SOVERSION ${SO_MAJOR}
)
