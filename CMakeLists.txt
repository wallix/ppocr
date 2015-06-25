cmake_minimum_required(VERSION 2.6)
project(PpOcr)

set(SRC_DIR src)

include(CMakeDefinitions.txt)

include_directories(include)
include_directories(${SRC_DIR})

add_definitions(-g)

if(NOT DEFINED NO_ASAN)
    if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-fsanitize=address)
        #set(CMAKE_LDD_FLAGS "${CMAKE_LDD_FLAGS} -lasan")
        set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -lasan")
    endif()
endif()

set(coordinate_SRC        ${SRC_DIR}/image/coordinate.cpp)
set(box_SRC               ${SRC_DIR}/box_char/box.cpp)
set(image_SRC             ${SRC_DIR}/image/image.cpp)
set(image_from_file_SRC   ${SRC_DIR}/image/image_from_file.cpp)
set(image_from_string_SRC ${SRC_DIR}/image/image_from_string.cpp)
set(dictionary_SRC        ${SRC_DIR}/spell/dictionary.cpp)
set(glyphs_loader_SRC     ${SRC_DIR}/loader2/glyphs_loader.cpp)
set(image_compare_SRC     ${SRC_DIR}/utils/image_compare.cpp)
set(box_character_SRC     ${SRC_DIR}/box_char/make_box_character.cpp)
set(loader2_image_io_SRC  ${SRC_DIR}/loader2/image_io.cpp)
set(loader2_glyphs_loader_SRC  ${SRC_DIR}/loader2/glyphs_loader.cpp)

set(strategies_SRC
    ${SRC_DIR}/strategies/alternation.cpp
    ${SRC_DIR}/strategies/hdirection.cpp
    ${SRC_DIR}/strategies/hdirection2.cpp
    ${SRC_DIR}/strategies/hgravity.cpp
    ${SRC_DIR}/strategies/hgravity2.cpp
    ${SRC_DIR}/strategies/dvdirection.cpp
    ${SRC_DIR}/strategies/dvdirection2.cpp
    ${SRC_DIR}/strategies/dvgravity.cpp
    ${SRC_DIR}/strategies/dvgravity2.cpp
    ${SRC_DIR}/strategies/dzdensity.cpp
    ${SRC_DIR}/strategies/density.cpp
    ${SRC_DIR}/strategies/proportionality.cpp
    ${SRC_DIR}/strategies/proportionality_zone.cpp
    ${SRC_DIR}/strategies/hbar.cpp
    ${SRC_DIR}/strategies/zone.cpp
    ${SRC_DIR}/strategies/utils/basic_proportionality.cpp
)

set(strategies_basic_proportionality_SRC ${SRC_DIR}/strategies/utils/basic_proportionality.cpp)

add_library(StrategiesBase STATIC ${strategies_SRC})
add_library(Strategies STATIC ${strategies_basic_proportionality_SRC})
add_library(ImageCompare STATIC ${image_compare_SRC})
# add_library(Filters STATIC ${filters_SRC})
add_library(Image STATIC ${image_SRC})
add_library(ImageFromFile STATIC ${image_from_file_SRC})
add_library(Coordinate STATIC ${coordinate_SRC})
add_library(Box STATIC ${box_SRC})
add_library(BoxChar STATIC ${box_character_SRC})
add_library(Dictionary STATIC ${dictionary_SRC})
add_library(Loader2ImageIo STATIC ${loader2_image_io_SRC})
add_library(Loader2GlyphsLoader STATIC ${loader2_glyphs_loader_SRC})

add_library(ImageIO STATIC)

target_link_libraries(BoxChar Box)
target_link_libraries(ImageIO Image Coordinate)
target_link_libraries(ImageFromFile ImageIO)
target_link_libraries(Strategies StrategiesBase)
target_link_libraries(Loader2ImageIo ImageIO)
target_link_libraries(Loader2GlyphsLoader Loader2ImageIo)

foreach(Name
    write_glyphs
    normalize_glyphs
    write_datas
    write_datas2
    extract_text6
    extract_text8
    ocr2_extract_text
)
    add_executable(${Name} ${SRC_DIR}/${Name}.cpp)
    target_link_libraries(${Name} Loader2ImageIo Strategies Image BoxChar ImageFromFile ImageCompare)
endforeach()

foreach(Name write_glyphs write_datas2)
    target_link_libraries(${Name} Loader2GlyphsLoader)
endforeach()

foreach(Name normalize_glyphs extract_text6 extract_text8 ocr2_extract_text)
    target_link_libraries(${Name} Loader2GlyphsLoader Dictionary)
endforeach()

add_library(OCR2 STATIC
    ${SRC_DIR}/ocr2_extract_text.cpp
    ${SRC_DIR}/ocr2/glyphs.cpp
    ${SRC_DIR}/ocr2/word_lines.cpp
    ${SRC_DIR}/ocr2/data_indexes_by_words.cpp
    ${SRC_DIR}/ocr2/filter_by_lines.cpp
    ${SRC_DIR}/ocr2/filter_by_font.cpp
    ${SRC_DIR}/ocr2/disambiguous_with_dict.cpp
)
target_link_libraries(ocr2_extract_text OCR2)


# Put all variables
# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

# foreach()
#     target_link_libraries (learning ${LIB_ASAN})
# endforeach()

# add_subdirectory(test)

# install(TARGETS ppocr RUNTIME DESTINATION bin)

# add_subdirectory(tests)
# add_subdirectory(utils)
# add_subdirectory(math)

find_library(LIB_BOOST_UNIT_TEST_FRAMEWORK boost_unit_test_framework)

if(${LIB_BOOST_UNIT_TEST_FRAMEWORK} STREQUAL "LIB_BOOST_UNIT_TEST_FRAMEWORK-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'libboost_unit_test_framework' library")
endif()

set(TESTS_DIR tests)

set(image_from_string_SRC ${SRC_DIR}/image/image_from_string.cpp)
add_library(ImageFromString STATIC ${image_from_string_SRC})
target_link_libraries(ImageFromString Image)

enable_testing()

add_executable(test_image ${TESTS_DIR}/test_image.cpp)
target_link_libraries(test_image ImageFromFile Box ${LIB_BOOST_UNIT_TEST_FRAMEWORK})
add_test(test_image test_image)

add_executable(test_make_box_character ${TESTS_DIR}/test_make_box_character.cpp)
target_link_libraries(test_make_box_character ImageFromFile BoxChar ${LIB_BOOST_UNIT_TEST_FRAMEWORK})
add_test(test_make_box_character test_make_box_character)

add_executable(test_trie ${TESTS_DIR}/container/test_trie.cpp)
target_link_libraries(test_trie ${LIB_BOOST_UNIT_TEST_FRAMEWORK})
add_test(test_trie test_trie)

add_executable(test_word_disambiguouser ${TESTS_DIR}/spell/test_word_disambiguouser.cpp)
target_link_libraries(test_word_disambiguouser Dictionary ${LIB_BOOST_UNIT_TEST_FRAMEWORK})
add_test(test_trie test_word_disambiguouser)


# file(GLOB StrategyNames ${TESTS_DIR}/strategies/*.cpp)
foreach(Name
    ${TESTS_DIR}/strategies/test_agravity.cpp
    ${TESTS_DIR}/strategies/test_alternation.cpp
    ${TESTS_DIR}/strategies/test_dcompass.cpp
    ${TESTS_DIR}/strategies/test_direction.cpp
#     ${TESTS_DIR}/strategies/test_dzdensity.cpp
    ${TESTS_DIR}/strategies/test_gravity2.cpp
    ${TESTS_DIR}/strategies/test_gravity.cpp
    ${TESTS_DIR}/strategies/test_hbar.cpp
    ${TESTS_DIR}/strategies/test_hdirection2.cpp
    ${TESTS_DIR}/strategies/test_hdirection.cpp
    ${TESTS_DIR}/strategies/test_hgravity2.cpp
    ${TESTS_DIR}/strategies/test_hgravity.cpp
    ${TESTS_DIR}/strategies/test_proportionality.cpp
    ${TESTS_DIR}/strategies/test_proportionality_zone.cpp
    ${TESTS_DIR}/strategies/test_zone.cpp
)
    STRING(REGEX REPLACE ".*/(.*)\\.cpp" "\\1" Filename ${Name})
    STRING(REGEX REPLACE "(.*)test_(.*)" "\\2" src_strategy ${Name})
    add_executable(
        ${Filename}_strategy
        ${SRC_DIR}/strategies/${src_strategy}
        ${Name}
    )
    target_link_libraries(
        ${Filename}_strategy
        ${LIB_BOOST_UNIT_TEST_FRAMEWORK}
        ImageFromString Box StrategiesBase
    )
    add_test(${Filename}_strategy ${Filename}_strategy)
endforeach()

# set(
#     CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS
#     "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} ${LIB_BOOST_UNIT_TEST_FRAMEWORK}"
# )

# target_link_libraries (test_image ${LIB_ASAN} ${LIB_BOOST_UNIT_TEST_FRAMEWORK})

# add_subdirectory(strategies)
