find_path(PCAP_INCLUDE_DIR
    NAMES pcap.h pcap/pcap.h
    PATHS /usr/include /usr/local/include
)

find_library(PCAP_LIBRARY
    NAMES pcap
    PATHS /usr/lib /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PCAP
    REQUIRED_VARS PCAP_LIBRARY PCAP_INCLUDE_DIR
)

if(PCAP_FOUND)
    set(PCAP_LIBRARIES    ${PCAP_LIBRARY})
    set(PCAP_INCLUDE_DIRS ${PCAP_INCLUDE_DIR})
endif()

mark_as_advanced(PCAP_INCLUDE_DIR PCAP_LIBRARY)
