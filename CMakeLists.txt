cmake_minimum_required(VERSION 2.6)
project(ppocr)

include(CMakeDefinitions.txt)

include_directories(.)

if(NOT DEFINED MILENA_INCLUDE_PATH)
    set(MILENA_INCLUDE_PATH /home/jpoelen/projects/redemption-wab.9.x/capture/ocr/)
endif()
include_directories(${MILENA_INCLUDE_PATH})

if(NOT DEFINED NO_ASAN)
#     find_library(LIB_ASAN asan)
    find_library(LIB_ASAN asan HINTS /usr/lib/gcc/x86_64-linux-gnu/4.9/)

    if(${LIB_ASAN} STREQUAL "LIB_ASAN-NOTFOUND")
        message(FATAL_ERROR "Couldn't find the 'libasan' library")
    endif()

    add_definitions(-fsanitize=address)

    # this is a hack
    set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS ${LIB_ASAN})
endif()

set(COORDINATE_IO_SRC coordinate.cpp)
set(BOX_IO_SRC box.cpp ${COORDINATE_IO_SRC})
set(IMAGE_FROM_FILE_SRC image.cpp image_from_file.cpp)
file(GLOB ALL_STRATEGIES_SRC strategies/*.cpp)
# set(ALL_STRATEGIES_SRC strategies/alternation.cpp strategies/direction.cpp)
set(BASIC_OCR_SRC ${BOX_IO_SRC} ${IMAGE_FROM_FILE_SRC} make_box_character.cpp)
set(DATA_REGISTRY_SRC ${ALL_STRATEGIES_SRC} data_loader.cpp registry.cpp)
set(DEFINITION_SRC definition.cpp)
set(UTILS_DEFINITION_SRC ${DEFINITION_SRC} utils/unique_sort_definition.cpp utils/image_compare.cpp)
set(OCR_SRC ${BASIC_OCR_SRC} ${DATA_REGISTRY_SRC} ${DEFINITION_SRC})

add_executable(learning2 learning2.cpp ${OCR_SRC} ${UTILS_DEFINITION_SRC})
add_executable(extract_text2 extract_text2.cpp ${OCR_SRC})
add_executable(print_alternation print_alternation.cpp ${OCR_SRC})

# Put all variables
# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

# # target_link_libraries (ppocr ${LIB_ASAN})
# target_link_libraries (learning ${LIB_ASAN})
# target_link_libraries (learning2 ${LIB_ASAN})
# # target_link_libraries (array_view ${LIB_ASAN})
# target_link_libraries (print_alternation ${LIB_ASAN})
# target_link_libraries (extract_text ${LIB_ASAN})
# target_link_libraries (extract_text2 ${LIB_ASAN})


# foreach()
#     target_link_libraries (learning ${LIB_ASAN})
# endforeach()

# add_subdirectory(test)

# install(TARGETS ppocr RUNTIME DESTINATION bin)

add_subdirectory(tests)
add_subdirectory(utils)
add_subdirectory(math)
