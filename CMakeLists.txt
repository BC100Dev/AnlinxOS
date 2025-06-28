cmake_minimum_required(VERSION 3.25)
project(AnlinxOS)

set(CMAKE_CXX_STANDARD 20)

file(GLOB CM_CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CM_Config/*.cmake")
foreach (CM_CONFIG_FILE ${CM_CONFIG_FILES})
    include("${CM_CONFIG_FILE}")
endforeach ()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")

file(GLOB EXT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ExtSources/*")
foreach (EXT_MODULE ${EXT_SRC_DIR})
    if (EXISTS "${EXT_MODULE}/SourceInfo.cmake")
        set(PROJECT_MODULE_ROOT "${EXT_MODULE}")
        include("${EXT_MODULE}/SourceInfo.cmake")

        message(STATUS "External module indexed: ${EXT_MODULE}")
    endif ()
endforeach ()

file(GLOB Private_CM_CONFIG_Modules "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*")
foreach (Private_CM_CONFIG_Module ${Private_CM_CONFIG_Modules})
    if (EXISTS "${Private_CM_CONFIG_Module}/SourceInfo.cmake")
        set(PROJECT_MODULE_ROOT "${Private_CM_CONFIG_Module}")
        include("${PROJECT_MODULE_ROOT}/SourceInfo.cmake")

        message(STATUS "Project Index successful: ${PROJECT_MODULE_ROOT}")
    endif ()
endforeach ()

file(GLOB TEST_MODULE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/Tests/*")
foreach (TEST_MODULE ${TEST_MODULE_LIST})
    if (EXISTS "${TEST_MODULE}/SourceInfo.cmake")
        set(PROJECT_MODULE_ROOT "${TEST_MODULE}")
        include("${PROJECT_MODULE_ROOT}/SourceInfo.cmake")

        message(STATUS "Project Index successful: ${PROJECT_MODULE_ROOT}")
    endif ()
endforeach ()

unset(PROJECT_MODULE_ROOT)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Targets")
    file(GLOB CUSTOM_TARGETS "${CMAKE_CURRENT_SOURCE_DIR}/Targets/*.cmake")
    foreach (CUSTOM_TARGET ${CUSTOM_TARGETS})
        include("${CUSTOM_TARGET}")
    endforeach ()
endif ()