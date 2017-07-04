cmake_minimum_required (VERSION 2.8)

include (utils.cmake)

disallow_intree_builds()

project (utf8proc C)

if (NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local)
endif()

set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation director for libraries.")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers.")
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" CACHE PATH "Installation dirctory for pkgconfig file.")

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

add_library (utf8proc
  utf8proc.c
  utf8proc.h
)

set(UTF8PROC_FULL_VERSION "${SO_MAJOR}.${SO_MINOR}.${SO_PATCH}")

set_target_properties (utf8proc PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  VERSION "${UTF8PROC_FULL_VERSION}"
  SOVERSION ${SO_MAJOR}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/utf8proc.pc.cmakein
               ${CMAKE_CURRENT_SOURCE_DIR}/utf8proc.pc @ONLY)
               
install(FILES utf8proc.pc DESTINATION "${INSTALL_PKGCONFIG_DIR}")
