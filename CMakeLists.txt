cmake_minimum_required(VERSION 3.10)
project(PacketSniffer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(PCAP REQUIRED)

set(SOURCES
    src/main.cpp
    src/Config.cpp
    src/PacketSniffer.cpp
    src/CaptureEngine.cpp
    src/ProtocolParserFactory.cpp
    src/ProtocolParser.cpp
    src/StatisticsCollector.cpp
    src/OutputRenderer.cpp
    src/EthernetHandler.cpp
    src/IPHandler.cpp
    src/TCPHandler.cpp
    src/UDPHandler.cpp
)

add_executable(sniffer ${SOURCES})

target_include_directories(sniffer
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${PCAP_INCLUDE_DIRS}
)

target_link_libraries(sniffer
    PRIVATE
        ${PCAP_LIBRARIES}
)

install(TARGETS sniffer DESTINATION bin)

add_subdirectory(tests)
