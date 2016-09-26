cmake_minimum_required (VERSION 2.8)

include (utils.cmake)

disallow_intree_builds()

project (utf8proc C)

# This is the ABI version number, which may differ from the
# API version number (defined in utf8proc.h).
# Be sure to also update these in Makefile and MANIFEST!
set(SO_MAJOR 2)
set(SO_MINOR 0)
set(SO_PATCH 2)

# configure build type

set(_utf8proc_is_set_build_shared_libs TRUE)
if(DEFINED BUILD_SHARED_LIBS)
  set(_utf8proc_is_set_build_shared_libs ${BUILD_SHARED_LIBS})
endif(DEFINED BUILD_SHARED_LIBS)

set(UTF8PROC_BUILD_AS_SHARED ${_utf8proc_is_set_build_shared_libs} CACHE BOOL "If present and true, this will cause utf8proc to be built shared unless the utf8proc was explicitly added as a static library.")

# set additional defined

if(NOT DEFINED _utf8proc_additional_defined)
  set(_utf8proc_additional_defined )
endif(NOT DEFINED _utf8proc_additional_defined)

if(UTF8PROC_BUILD_AS_SHARED)
  list(APPEND _utf8proc_additional_defined 
    "-DUTF8PROC_EXPORTS"
  )
  set(_utf8proc_build_type "SHARED")
else(UTF8PROC_BUILD_AS_SHARED)
  list(APPEND _utf8proc_additional_defined 
    "-DUTF8PROC_BUILD_AS_LIB"
  )
  set(_utf8proc_build_type "STATIC")
endif(UTF8PROC_BUILD_AS_SHARED)
list(REMOVE_DUPLICATES _utf8proc_additional_defined)
add_definitions("${_utf8proc_additional_defined}")

# other config

if(NOT MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=c99 -pedantic -Wall")
endif()

add_library (utf8proc ${_utf8proc_build_type}
  utf8proc.c
  utf8proc.h
)

if(MSVC)
  get_target_property(_utf8proc_msvc_build_prop_val utf8proc COMPILE_FLAGS)
  if(${_utf8proc_msvc_build_prop_val} STREQUAL "_utf8proc_msvc_build_prop_val-NOTFOUND")
    set_target_properties(utf8proc PROPERTIES COMPILE_FLAGS "/Wall")
  else(${_utf8proc_msvc_build_prop_val} STREQUAL "_utf8proc_msvc_build_prop_val-NOTFOUND")
    set_target_properties(utf8proc PROPERTIES COMPILE_FLAGS "${_utf8proc_msvc_build_prop_val} /Wall")
  endif(${_utf8proc_msvc_build_prop_val} STREQUAL "_utf8proc_msvc_build_prop_val-NOTFOUND")
endif()

set_target_properties (utf8proc PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  VERSION "${SO_MAJOR}.${SO_MINOR}.${SO_PATCH}"
  SOVERSION ${SO_MAJOR}
)
