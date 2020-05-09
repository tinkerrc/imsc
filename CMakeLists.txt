cmake_minimum_required(VERSION 3.16)

project(LinuxScoringEngine)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 


find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)

link_directories(${GTK2_LIBRARY_DIRS})
add_definitions(${GTK2_CFLAGS_OTHER})

include_directories(/usr/include/glib-2.0)
include_directories(include)
include_directories(${GTK2_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)

include_directories(${Boost_INCLUDE_DIRS}) 
add_executable(imsc ${SOURCES} ${HEADERS}) 
target_link_libraries(imsc ${Boost_LIBRARIES} curlpp curl crypto++ notify ${GTK2_LIBRARIES})
target_compile_features(imsc PRIVATE cxx_std_17)
