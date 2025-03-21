set(CMAKE_BUILD_TYPE Debug)

add_executable(test-kpacket test-kpacket.cpp)
add_executable(test-kdevice test-kdevice.cpp)
add_executable(test-kburst test-kburst.cpp)
add_executable(test-packetDataset test-packetDataset.cpp)
add_executable(test-burstMaker test-burstMaker.cpp)
add_executable(test-burstDataset test-burstDataset.cpp)
add_executable(test-metric test-metric.cpp)
add_executable(test-burstClassifier test-burstClassifier.cpp)
add_executable(test-boClassifier test-boClassifier.cpp)
add_executable(test-byteIoTDataset test-byteIoTDataset.cpp)
add_executable(test-byteIoTClassifier test-byteIoTClassifier.cpp)



target_link_libraries(test-kpacket PRIVATE Catch2::Catch2WithMain
                                   PRIVATE pack)

target_link_libraries(test-kdevice PRIVATE Catch2::Catch2WithMain
PRIVATE pack)

target_link_libraries(test-kburst PRIVATE Catch2::Catch2WithMain
PRIVATE pack)

target_link_libraries(test-packetDataset PRIVATE Catch2::Catch2WithMain
PRIVATE pack Packet++ Pcap++ Common++)

target_link_libraries(test-burstMaker PRIVATE Catch2::Catch2WithMain
PRIVATE pack)

target_link_libraries(test-burstDataset PRIVATE Catch2::Catch2WithMain
PRIVATE pack)

target_link_libraries(test-metric PRIVATE Catch2::Catch2WithMain PRIVATE pack)

target_link_libraries(test-burstClassifier PRIVATE Catch2::Catch2WithMain PRIVATE pack)

target_link_libraries(test-boClassifier PRIVATE Catch2::Catch2WithMain PRIVATE pack)
target_link_libraries(test-byteIoTDataset PRIVATE Catch2::Catch2WithMain PRIVATE pack)
target_link_libraries(test-byteIoTClassifier PRIVATE Catch2::Catch2WithMain PRIVATE pack)

