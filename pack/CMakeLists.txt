file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(CMAKE_CXX_FLAGS_RELEASE "-O3") 
add_library(pack ${SRC_FILES})

set(Mlpack_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/mlpack-4.6.0/src")

target_include_directories(pack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header ${Mlpack_INCLUDE_DIR})
target_link_libraries(pack Boost::serialization Packet++ Pcap++ Common++ OpenMP::OpenMP_CXX armadillo)