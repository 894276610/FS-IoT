file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(pack ${SRC_FILES})

target_include_directories(pack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
target_link_libraries(pack Boost::serialization Packet++ Pcap++ Common++)



