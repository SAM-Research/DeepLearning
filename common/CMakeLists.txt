find_package(yaml-cpp REQUIRED)
include(${SR_CMAKE}/get_libtorch.cmake)

add_library(DeepLearning STATIC common.cpp builder.cpp)

target_include_directories(DeepLearning PRIVATE ..)

target_link_libraries(DeepLearning PRIVATE torch yaml-cpp::yaml-cpp tensorboard_logger)