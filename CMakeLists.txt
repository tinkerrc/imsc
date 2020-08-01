cmake_minimum_required(VERSION 3.1.3) # target_compile_features

project(LinuxScoringEngine)
set (IMSC_VERSION_MAJOR 0)
set (IMSC_VERSION_MINOR 1)
set (IMSC_VERSION_PATCH 0)
set (IMSC_VERSION "${IMSC_VERSION_MAJOR}.${IMSC_VERSION_MINOR}.${IMSC_VERSION_PATCH}")

if (NOT IMSC_URL)
    message("Please specify imsc server URL. A dummy URL was set by default.")
    set(IMSC_URL "https://imsc.example.com")
endif()
if (NOT IMSC_KEY)
    message(FATAL_ERROR "Please specify an encryption / decryption key using -DIMSC_KEY=")
endif()
if (NOT IMSC_LOG_FILE)
    message("Using default logging path")
    set(IMSC_LOG_FILE "/var/log/imsc.log")
endif()

# needs it in include/ for ccls
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h @ONLY)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/json/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(imsc ${SOURCES} ${HEADERS})
target_link_libraries(imsc curl curlpp cryptopp)
target_compile_features(imsc PRIVATE cxx_std_17)
