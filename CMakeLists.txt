cmake_minimum_required(VERSION 2.6)
project(Kaguya)

# If you have a C++11 compiler, comment out these lines
find_package( Boost REQUIRED )
    INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
    LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )
    set( Boost_USE_STATIC_LIBS        OFF )
    set( Boost_USE_MULTITHREADED      ON )
    set( Boost_USE_STATIC_RUNTIME     OFF )
    set( BOOST_ALL_DYN_LINK           ON )

set( LUA_LIBRARIES lua )
set( LUA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdParty/lua ) # lua5.3
include_directories( ${LUA_INCLUDE_DIR} )
# link_directories(${LUA_LIBRARY_DIRS})
add_subdirectory( 3rdParty )

include_directories( "include" )

file(GLOB KAGUYA_HEADER RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    include/kaguya/*.hpp
    include/kaguya/detail/*.hpp)
include_directories( ${CMAKE_SOURCE_DIR} )

if(ADDITIONAL_INCLUDE_PATH)
include_directories(${ADDITIONAL_INCLUDE_PATH})
endif()

if(NOT MSVC)#-Wall nonsense on MSVC
add_definitions("-Wall")
#add_definitions("-std=c++11")
endif(NOT MSVC)

#check for address sanitizer support
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 4.8)
      set(HAVE_FLAG_SANITIZE_ADDRESS TRUE)
    endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 3.2)
      set(HAVE_FLAG_SANITIZE_ADDRESS TRUE)
    endif()
    #detect stack-buffer-overflow at clang++3.4(on ubuntu 12.04) with luajit but don't know reason
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_EQUAL 3.4)
      if(${LUA_SEARCH_LIB_NAME} STREQUAL "luajit")
        unset(HAVE_FLAG_SANITIZE_ADDRESS)
      endif()
    endif()
endif()
if(CYGWIN OR WIN32)
      unset(HAVE_FLAG_SANITIZE_ADDRESS)
endif()

file(GLOB TEST_SRCS files RELATIVE ${CMAKE_SOURCE_DIR} test/*.cpp)

add_executable(test_runner
				${TEST_SRCS} ${KAGUYA_HEADER})
target_link_libraries(test_runner ${LUA_LIBRARIES})

if(HAVE_FLAG_SANITIZE_ADDRESS)
message(STATUS "test_runner: address sanitizer enabled.")
SET_TARGET_PROPERTIES(test_runner  PROPERTIES COMPILE_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
SET_TARGET_PROPERTIES(test_runner  PROPERTIES LINK_FLAGS "-fsanitize=address")
endif(HAVE_FLAG_SANITIZE_ADDRESS)

set(BENCHMARK_SRCS benchmark/benchmark.cpp benchmark/benchmark_function.cpp benchmark/benchmark_function.hpp)

add_executable(benchmark ${BENCHMARK_SRCS} ${KAGUYA_HEADER})
target_link_libraries(benchmark ${LUA_LIBRARIES})

enable_testing()
add_test(kaguya_test test_runner)

if(COVERAGE)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage -coverage")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage -coverage")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -coverage")
 add_executable(test_coverage ${TEST_SRCS})
target_link_libraries(test_coverage ${LUA_LIBRARIES} gcov)
add_test(NAME kaguya_test_coverage COMMAND test_coverage)
endif(COVERAGE)
