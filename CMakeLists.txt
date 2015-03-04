cmake_minimum_required(VERSION 2.6)
project(ppocr)

include(CMakeDefinitions.txt)

include_directories(.)
include_directories(/home/jpoelen/projects/falcon)
include_directories(/home/jpoelen/projects/redemption-wab.9.x/capture/ocr/)

add_definitions(-fsanitize=address)

find_library(LIB_ASAN asan)

if(${LIB_ASAN} STREQUAL "LIB_ASAN-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'libasan' library")
endif()

add_executable(ppocr main.cpp)

target_link_libraries (ppocr ${LIB_ASAN})


# add_subdirectory(test)

# install(TARGETS ppocr RUNTIME DESTINATION bin)
