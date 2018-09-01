# Copyright 2018 by Cliff Green
#
# https://github.com/cliffg-softwarelibre/utility-rack
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.8 )

project ( test LANGUAGES CXX )

set ( MAIN_SRC main_test.cpp )
set ( TEST_SOURCES 
    queue/wait_queue_test.cpp
    timer/periodic_timer_test.cpp
    utility/erase_where_test.cpp
    utility/make_byte_array_test.cpp
    utility/repeat_test.cpp
    utility/shared_buffer_test.cpp )

set ( HDR_DIR
    ${PROJECT_SOURCE_DIR}/include
    ${BUILD_INTERFACE}/boost_1_68_0
    ${BUILD_INTERFACE}/Catch2/include
    ${BUILD_INTERFACE}/expected-lite/include
    ${BUILD_INTERFACE}/networking_ts_impl/include
    ${BUILD_INTERFACE}/ring-span-lite/include )

    
set ( OPTIONS "" )
set ( DEFINITIONS "" )

function ( make_target target src )
    add_executable            ( ${target} ${MAIN_SRC} ${src} )
    message ( "Test executable created: ${target}" )
    target_compile_features   ( ${target} PRIVATE cxx_std_17 )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} )
    target_include_directories( ${target} PRIVATE ${HDRDIR} )
endfunction()

foreach ( test_src IN LISTS ${TEST_SOURCES} )
    get_filename_component ( targ ${test_src} NAME_WE )
    message ( "Calling make_target with: ${targ}" )
    make_target ( ${targ} ${test_src} )
endforeach()

# TODO testing invocation
# enable_testing()

# add_test ( something )
