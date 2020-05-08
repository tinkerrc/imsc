cmake_minimum_required(VERSION 3.16)

project(LinuxScoringEngine)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED) 

include_directories(${Boost_INCLUDE_DIRS}) 
add_executable(imsc engine.cpp) 
target_link_libraries(imsc ${Boost_LIBRARIES} curlpp curl crypto++)
target_compile_features(imsc PRIVATE cxx_std_17)
