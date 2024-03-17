cmake_minimum_required(VERSION 3.17)

set(SAM_RESEARCH "$ENV{HOME}/SAMResearch"  CACHE PATH "SAM RESEARCH FOLDER")
include(${SAM_RESEARCH}/shared_header.cmake)
set(SR_LIBTORCH_CACHE "${SR_CACHE}/torch"  CACHE PATH "libtorch cache path")

rapids_cuda_init_architectures(DeepLearning)
project(DeepLearning LANGUAGES C CXX CUDA)
option(BUILD_EXAMPLES ON)

set(CMAKE_CXX_STANDARD 20)

#add_subdirectory(tensorboard_logger)

add_subdirectory(common)
add_subdirectory(basic)
#add_subdirectory(vision)
#add_subdirectory(memory)
#add_subdirectory(distribution)

#
#if(${BUILD_EXAMPLES})
#    file(GLOB Examples examples/*.cpp)
#    foreach(SRC_FILE IN LISTS Examples)
#        string(REPLACE .cpp "" executable_name ${SRC_FILE})
#        string(REPLACE ${CMAKE_SOURCE_DIR}/examples/ "" executable_name ${executable_name})
#        add_executable(${executable_name} ${SRC_FILE} vision/impala_residual_block.h vision_debugger.h)
#        target_link_libraries(${executable_name} PRIVATE ${PROJECT_NAME} ${TORCH_LIBRARIES} yaml-cpp tensorboard_logger)
#        target_include_directories( ${executable_name} PRIVATE . tensorboard_logger)
#    endforeach()
#endif()