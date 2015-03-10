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

set(COORDINATE_IO_SRC coordinate.cpp)
set(BOX_IO_SRC box.cpp ${COORDINATE_IO_SRC})
set(IMAGE_FROM_FILE_SRC image.cpp image_from_file.cpp)

# add_executable(ppocr ${BOX_IO_SRC} ${IMAGE_FROM_FILE_SRC} main.cpp)
add_executable(learning ${BOX_IO_SRC} ${IMAGE_FROM_FILE_SRC} strategies/alternation.cpp make_box_character.cpp learning.cpp)
add_executable(learning2 ${BOX_IO_SRC} ${IMAGE_FROM_FILE_SRC} strategies/alternation.cpp strategies/direction.cpp data_loader.cpp make_box_character.cpp learning2.cpp)
# add_executable(array_view array_view.cpp)
add_executable(print_alternation ${BOX_IO_SRC} ${IMAGE_FROM_FILE_SRC} strategies/alternation.cpp make_box_character.cpp print_alternation.cpp)
add_executable(extract_text ${IMAGE_FROM_FILE_SRC} strategies/alternation.cpp strategies/direction.cpp make_box_character.cpp extract_text.cpp)

# target_link_libraries (ppocr ${LIB_ASAN})
target_link_libraries (learning ${LIB_ASAN})
target_link_libraries (learning2 ${LIB_ASAN})
# target_link_libraries (array_view ${LIB_ASAN})
target_link_libraries (print_alternation ${LIB_ASAN})
target_link_libraries (extract_text ${LIB_ASAN})


# add_subdirectory(test)

# install(TARGETS ppocr RUNTIME DESTINATION bin)

add_subdirectory(tests)
