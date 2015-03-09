find_library(LIB_BOOST_UNIT_TEST_FRAMEWORK boost_unit_test_framework)

if(${LIB_BOOST_UNIT_TEST_FRAMEWORK} STREQUAL "LIB_BOOST_UNIT_TEST_FRAMEWORK-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'libboost_unit_test_framework' library")
endif()

set(IMAGE_FROM_FILE_SRC ../image.cpp ../image_from_file.cpp)

add_executable(test_image ${IMAGE_FROM_FILE_SRC} image.cpp)
add_executable(test_box_char ${IMAGE_FROM_FILE_SRC} make_box_character.cpp)

target_link_libraries (test_image ${LIB_ASAN} ${LIB_BOOST_UNIT_TEST_FRAMEWORK})
target_link_libraries (test_box_char ${LIB_ASAN} ${LIB_BOOST_UNIT_TEST_FRAMEWORK})
