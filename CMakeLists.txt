cmake_minimum_required(VERSION 3.10)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12 CACHE STRING "Build for 10.12" )
project(synth-formula-evaluator VERSION 0.1.0.0 LANGUAGES CXX)
message( STATUS "Configuring synth-formula-evaluationr with compiler ${CMAKE_CXX_COMPILER_VERSION}" )

add_library(synth-formula-evaluator
  SHARED
  src/Parser.cpp
  src/Evaluator.cpp
  )

target_compile_features( synth-formula-evaluator PRIVATE cxx_std_17 )
target_include_directories( synth-formula-evaluator PRIVATE include PEGTL/include )
# Expand to no warnings on lin and win too one day
if( APPLE )
  target_compile_options( synth-formula-evaluator PRIVATE -Wall -Werror )
endif()

add_executable(synth-formula-test
  test/main.cpp
  test/simple.cpp
  test/eval.cpp
  )

target_compile_features( synth-formula-test PRIVATE cxx_std_17 )
target_link_libraries( synth-formula-test PRIVATE synth-formula-evaluator )
target_include_directories( synth-formula-test PRIVATE catch2 include )
