cmake_minimum_required(VERSION 2.6)
project(ppocr)

include(CMakeDefinitions.txt)

include_directories(.)
include_directories(/home/jpoelen/projects/falcon)
include_directories(/home/jpoelen/projects/redemption-wab.9.x/capture/ocr/)

add_definitions(-fsanitize=address)

find_library(LIB_ASAN asan)
# find_library(LIB_ASAN asan HINTS /usr/lib/gcc/x86_64-linux-gnu/4.9/)

if(${LIB_ASAN} STREQUAL "LIB_ASAN-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'libasan' library")
endif()

add_executable(ppocr image.cpp image_from_file.cpp main.cpp)
add_executable(learning learning.cpp)
add_executable(array_view array_view.cpp)

target_link_libraries (ppocr ${LIB_ASAN})
target_link_libraries (learning ${LIB_ASAN})
target_link_libraries (array_view ${LIB_ASAN})


# add_subdirectory(test)

# install(TARGETS ppocr RUNTIME DESTINATION bin)

add_subdirectory(tests)
