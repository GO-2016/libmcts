cmake_minimum_required(VERSION 3.1)
project(libmcts)

#option(libgoboard_build_tests "Build libgoboard's own tests" OFF)
option(libmcts_build_tests "Build libmcts' test" ON)

set(CMAKE_CXX_STANDARD 11)
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -pg")
#SET(LINK_FLAGS "-pg")
#CC=g++ -g -pg
##################################
# pthread
##################################
find_package(Threads)


##################################
# libgoboard
##################################
if (NOT TARGET goboard)
    add_subdirectory(vendor/libgoboard)
    set(libgoboard_SOURCE_DIR vendor/libgoboard)
    set(libgoboard_LIBS goboard)
    include_directories(${libgoboard_INCLUDE_DIR})
endif ()
#target_link_libraries(mcts goboard)


##################################
# libmcts
##################################
include_directories(src/)
add_library(mcts STATIC src/mct.cpp)
target_link_libraries(mcts goboard)
set(libmcts_INCLUDE_DIR ${libmcts_SOURCE_DIR}/src ${libgoboard_INCLUDE_DIR} PARENT_SCOPE)

#################################
# tests
################################
if (libmcts_build_tests)
#    enable_testing()
#    ################################
#    # GTest
#    ################################
#    ADD_SUBDIRECTORY (vendor/googletest/googletest)
#    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
#    ###############################
#    # board-test
#    ###############################
#    add_executable(uct-test src/uct_test.cpp)
#    target_link_libraries(uct-test uct gtest gtest_main)
#    add_test(uct_test uct-test)
    ###############################
    # mct-test
    ###############################
    add_executable(mct src/mct.cpp)
    target_link_libraries(mct mcts)
    target_link_libraries(mct ${CMAKE_THREAD_LIBS_INIT})
endif()
