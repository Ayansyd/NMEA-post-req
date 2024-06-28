cmake_minimum_required(VERSION 3.0)
project(NMEA_Parser)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# Add executable target
add_executable(nmea_parser
    nmea_parser.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nmea.txt ${CMAKE_CURRENT_BINARY_DIR}/nmea.txt COPYONLY)

# Link libcurl
target_link_libraries(nmea_parser ${CURL_LIBRARIES})
