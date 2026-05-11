cmake_minimum_required(VERSION 3.14)

find_package(GTest REQUIRED)

enable_testing()

set(SNIFFER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Config.cpp
    ${CMAKE_SOURCE_DIR}/src/StatisticsCollector.cpp
    ${CMAKE_SOURCE_DIR}/src/OutputRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/ProtocolParserFactory.cpp
    ${CMAKE_SOURCE_DIR}/src/ProtocolParser.cpp
    ${CMAKE_SOURCE_DIR}/src/EthernetHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/IPHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/TCPHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/UDPHandler.cpp
)

set(SNIFFER_INCLUDES
    ${CMAKE_SOURCE_DIR}/include
    ${PCAP_INCLUDE_DIRS}
)

add_executable(unit_tests
    test_statistics_collector.cpp
    test_protocol_parser_factory.cpp
    test_handlers.cpp
    test_config.cpp
    ${SNIFFER_SOURCES}
)
target_include_directories(unit_tests PRIVATE ${SNIFFER_INCLUDES})
target_link_libraries(unit_tests PRIVATE GTest::GTest GTest::Main ${PCAP_LIBRARIES})
target_compile_features(unit_tests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(unit_tests)

foreach(scenario 01_pipeline 02_malformed 03_load)
    add_executable(scenario_${scenario}
        scenario_${scenario}.cpp
        ${SNIFFER_SOURCES}
    )
    target_include_directories(scenario_${scenario} PRIVATE ${SNIFFER_INCLUDES})
    target_link_libraries(scenario_${scenario} PRIVATE ${PCAP_LIBRARIES})
    target_compile_features(scenario_${scenario} PRIVATE cxx_std_17)
endforeach()
