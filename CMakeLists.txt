cmake_minimum_required(VERSION 3.16)

project(IoTClassifier2025)
set(CMAKE_CXX_FLAGS_RELEASE "-O3") 
#set(CMAKE_BUILD_TYPE Debug)

find_package(Boost COMPONENTS serialization REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Armadillo REQUIRED)

find_package(Python REQUIRED COMPONENTS Interpreter Development)

set(CMAKE_CXX_STANDARD 17)    

add_subdirectory(pack)
add_subdirectory(test)
add_subdirectory(third-party/PcapPlusPlus)
add_subdirectory(third-party/Catch2-3.8.0)
add_subdirectory(third-party/range-v3-0.12.0)
add_subdirectory(third-party/pybind11-2.13.6)

# 第三方头文件路径
set(MAIN_INCLUDE "${CMAKE_SOURCE_DIR}/include")
set(PACK_INCLUDE "${CMAKE_SOURCE_DIR}/pack/header")

set(MAGIC_ENUM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/magic_enum-0.9.7/include/magic_enum")
set(RANGE_v3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/range-v3-0.12.0/include")
set(Packetpp_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/PcapPlusPlus/Packet++/header")
set(Commonpp_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/PcapPlusPlus/Common++/header")
set(Pcappp_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/PcapPlusPlus/Pcap++/header")
set(Mlpack_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/mlpack-4.6.0/src")
set(PYBIND_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/pybind11-2.13.6/include")
set(PYTHON_INCLUDE_DIR "/usr/include/python3.10")

# Include Path
include_directories(${MAIN_INCLUDE})
include_directories(${PACK_INCLUDE})
include_directories(${MAGIC_ENUM_INCLUDE_DIR})
include_directories(${Packetpp_INCLUDE_DIR})
include_directories(${Commonpp_INCLUDE_DIR})
include_directories(${Pcappp_INCLUDE_DIR})
include_directories(${Mlpack_INCLUDE_DIR})
include_directories(${RANGE_v3_INCLUDE_DIR})
include_directories(${PYBIND_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIR})

# add_compile_options(-Wunused-variable -Wall -Wextra -Werror)

add_executable(MainExe ${CMAKE_SOURCE_DIR}/src/main.cpp ${CMAKE_SOURCE_DIR}/src/labSetting.cpp ${CMAKE_SOURCE_DIR}/src/experiment.cpp )#${CMAKE_SOURCE_DIR}/src/labSetting_bindings.cpp)

target_link_libraries(MainExe pack Boost::serialization Packet++ Pcap++ Common++ range-v3::range-v3  pybind11::embed)

pybind11_add_module(labSetting_module ${CMAKE_SOURCE_DIR}/src/labSetting.cpp ${CMAKE_SOURCE_DIR}/src/labSetting_bindings.cpp)
add_dependencies(labSetting_module pack Boost::serialization Packet++ Pcap++ Common++ range-v3::range-v3 pybind11::embed)

set_target_properties(labSetting_module PROPERTIES CXX_STANDARD 17)
target_link_libraries(labSetting_module PRIVATE
    pack Boost::serialization Packet++ Pcap++ Common++ range-v3::range-v3 pybind11::embed
)
