cmake_minimum_required (VERSION 2.8.12)

include (utils.cmake)

disallow_intree_builds()

project (utf8proc C)

# This is the ABI version number, which may differ from the
# API version number (defined in utf8proc.h).
# Be sure to also update these in Makefile and MANIFEST!
set(SO_MAJOR 2)
set(SO_MINOR 3)
set(SO_PATCH 2)

option(UTF8PROC_INSTALL "Enable installation of utf8proc" On)
option(UTF8PROC_ENABLE_TESTING "Enable testing of utf8proc" Off)

add_library (utf8proc
  utf8proc.c
  utf8proc.h
)

# expose header path, for when this is part of a larger cmake project
target_include_directories(utf8proc PUBLIC ../utf8proc)

if (BUILD_SHARED_LIBS)
  # Building shared library
else()
  # Building static library
  target_compile_definitions(utf8proc PUBLIC "UTF8PROC_STATIC")
  if (MSVC)
    set_target_properties(utf8proc PROPERTIES OUTPUT_NAME "utf8proc_static")
  endif()
endif()

target_compile_definitions(utf8proc PRIVATE "UTF8PROC_EXPORTS")

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

if (UTF8PROC_INSTALL)
  install(TARGETS utf8proc
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

  install(
    FILES
      "${PROJECT_SOURCE_DIR}/utf8proc.h"
    DESTINATION include)
endif()

if(UTF8PROC_ENABLE_TESTING)
  enable_testing()

  if (WIN32)
    set(curl_cmd "c:/Windows/System32/curl.exe")
  else()
    set(curl_cmd "/usr/bin/curl")
  endif()

  set(curl_param --retry 5 --location -\#)
  set(UNICODE_VERSION 12.1.0)
  set(UNICODE_EMOJI_VERSION 12.0)

  execute_process(COMMAND bash -c "mkdir -p data")
  execute_process(
    COMMAND ${curl_cmd} ${curl_param} -o data/emoji-data.txt -O http://unicode.org/Public/emoji/${UNICODE_EMOJI_VERSION}/emoji-data.txt
    COMMAND ${curl_cmd} ${curl_param} -o data/UnicodeData.txt -O http://www.unicode.org/Public/${UNICODE_VERSION}/ucd/UnicodeData.txt
    COMMAND ${curl_cmd} ${curl_param} -o data/EastAsianWidth.txt -O http://www.unicode.org/Public/${UNICODE_VERSION}/ucd/EastAsianWidth.txt
    COMMAND ${curl_cmd} ${curl_param} -o data/GraphemeBreakProperty.txt -O http://www.unicode.org/Public/${UNICODE_VERSION}/ucd/auxiliary/GraphemeBreakProperty.txt
    COMMAND ${curl_cmd} ${curl_param} -o data/DerivedCoreProperties.txt -O http://www.unicode.org/Public/${UNICODE_VERSION}/ucd/DerivedCoreProperties.txt
    COMMAND ${curl_cmd} ${curl_param} -o data/CompositionExclusions.txt -O http://www.unicode.org/Public/${UNICODE_VERSION}/ucd/CompositionExclusions.txt
    COMMAND ${curl_cmd} ${curl_param} -o data/CaseFolding.txt -O http://www.unicode.org/Public/${UNICODE_VERSION}/ucd/CaseFolding.txt
    COMMAND ${curl_cmd} ${curl_param} -o data/NormalizationTest.txt -O http://www.unicode.org/Public/${UNICODE_VERSION}/ucd/NormalizationTest.txt
    COMMAND ${curl_cmd} ${curl_param} -o data/GraphemeBreakTestOrg.txt -O http://www.unicode.org/Public/${UNICODE_VERSION}/ucd/auxiliary/GraphemeBreakTest.txt
  )
  execute_process(COMMAND bash -c "cat data/GraphemeBreakTestOrg.txt | /usr/bin/perl -pe 's,÷,/,g;s,×,+,g'" OUTPUT_FILE data/GraphemeBreakTest.txt)
  execute_process(COMMAND bash -c "rm -f data/GraphemeBreakTestOrg.txt")

  add_executable(case test/tests.h test/tests.c utf8proc.h test/case.c)
  target_link_libraries(case utf8proc)
  add_executable(custom test/tests.h test/tests.c utf8proc.h test/custom.c)
  target_link_libraries(custom utf8proc)
  add_executable(iterate test/tests.h test/tests.c utf8proc.h test/iterate.c)
  target_link_libraries(iterate utf8proc)
  add_executable(misc test/tests.h test/tests.c utf8proc.h test/misc.c)
  target_link_libraries(misc utf8proc)
  add_executable(printproperty test/tests.h test/tests.c utf8proc.h test/printproperty.c)
  target_link_libraries(printproperty utf8proc)
  add_executable(valid test/tests.h test/tests.c utf8proc.h test/valid.c)
  target_link_libraries(valid utf8proc)
  add_test(utf8proc.testcase case)
  add_test(utf8proc.testcustom custom)
  add_test(utf8proc.testiterate iterate)
  add_test(utf8proc.testmisc misc)
  add_test(utf8proc.testprintproperty printproperty)
  add_test(utf8proc.testvalid valid)

  if (NOT WIN32)
    # no wcwidth function on Windows
    add_executable(charwidth test/tests.h test/tests.c utf8proc.h test/charwidth.c)
    target_link_libraries(charwidth utf8proc)
    add_test(utf8proc.testcharwidth charwidth)
  endif()
  add_executable(graphemetest test/tests.h test/tests.c utf8proc.h test/graphemetest.c)
  target_link_libraries(graphemetest utf8proc)
  add_executable(normtest test/tests.h test/tests.c utf8proc.h test/normtest.c)
  target_link_libraries(normtest utf8proc)
  #add_test(utf8proc.testgraphemetest graphemetest data/GraphemeBreakTest.txt)
  #add_test(utf8proc.testnormtest normtest data/NormalizationTest.txt)
endif()
