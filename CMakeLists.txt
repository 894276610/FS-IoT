cmake_minimum_required(VERSION 3.16)

project(BurstIoT)
set(CMAKE_CXX_FLAGS_RELEASE "-O3") 
#set(CMAKE_BUILD_TYPE Debug)

find_package(Boost COMPONENTS serialization REQUIRED)


set(CMAKE_CXX_STANDARD 17)    

add_subdirectory(pack)
add_subdirectory(test)
add_subdirectory(third-party/PcapPlusPlus)
add_subdirectory(third-party/Catch2-3.8.0)

# 第三方头文件路径
set(MAIN_INCLUDE "${CMAKE_SOURCE_DIR}/include")
set(MAGIC_ENUM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/magic_enum-0.9.7/include/magic_enum")
set(Packetpp_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/PcapPlusPlus/Packet++/header")
set(Commonpp_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/PcapPlusPlus/Common++/header")
set(Pcappp_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third-party/PcapPlusPlus/Pcap++/header")

# Include Path
include_directories(${MAIN_INCLUDE})
include_directories(${MAGIC_ENUM_INCLUDE_DIR})
include_directories(${Packetpp_INCLUDE_DIR})
include_directories(${Commonpp_INCLUDE_DIR})
include_directories(${Pcappp_INCLUDE_DIR})

add_compile_options(-Wunused-variable)

add_executable(BurstIoTExe ${CMAKE_SOURCE_DIR}/src/burstIoTmain.cpp)
add_executable(ByteIoTExe ${CMAKE_SOURCE_DIR}/src/byteIoTmain.cpp)


# target_compile_options(MyExecutable PRIVATE -fsanitize=address -fno-omit-frame-pointer)
# target_link_options(MyExecutable PRIVATE -fsanitize=address)

target_link_libraries(BurstIoTExe pack Boost::serialization Packet++ Pcap++ Common++)
target_link_libraries(ByteIoTExe pack Boost::serialization Packet++ Pcap++ Common++)


## 添加外部库
#add_subdirectory(path_to_external_library)

# 在项目中引用外部库
#target_link_libraries(MyTarget ExternalLibrary)