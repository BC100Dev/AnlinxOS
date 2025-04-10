cmake_minimum_required(VERSION 3.25)
project(AnlinOS)

set(CMAKE_CXX_STANDARD 20)

file(GLOB CM_CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CM_Config/*.cmake")
foreach (CM_CONFIG_FILE ${CM_CONFIG_FILES})
    include("${CM_CONFIG_FILE}")
endforeach ()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")

file(GLOB Private_CM_CONFIG_Modules "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*")
foreach (Private_CM_CONFIG_Module ${Private_CM_CONFIG_Modules})
    if (EXISTS "${Private_CM_CONFIG_Module}/SourceInfo.cmake")
        set(PROJECT_MODULE_ROOT "${Private_CM_CONFIG_Module}")
        include("${PROJECT_MODULE_ROOT}/SourceInfo.cmake")

        message(STATUS "Project Index successful: ${PROJECT_MODULE_ROOT}")
    endif ()
endforeach ()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Targets")
    file(GLOB CUSTOM_TARGETS "${CMAKE_CURRENT_SOURCE_DIR}/Targets/*.cmake")
    foreach (CUSTOM_TARGET ${CUSTOM_TARGETS})
        include("${CUSTOM_TARGET}")
    endforeach ()
endif ()