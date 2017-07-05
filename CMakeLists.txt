cmake_minimum_required (VERSION 2.8)

include (utils.cmake)

disallow_intree_builds()

project (utf8proc C)

set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries.")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers.")
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" CACHE PATH "Installation directory for pkgconfig file.")

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

add_library (utf8proc SHARED
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
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake.cmakein
               ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config-version.cmake.cmakein
               ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config-version.cmake @ONLY)

install(FILES utf8proc.pc DESTINATION "${INSTALL_PKGCONFIG_DIR}")
install(FILES utf8proc.h DESTINATION "${INSTALL_INC_DIR}")
install(TARGETS utf8proc
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}")
  
install(FILES ${PROJECT_NAME}-config.cmake ${PROJECT_NAME}-config-version.cmake
        DESTINATION "${INSTALL_LIB_DIR}/${PROJECT_NAME}") 
