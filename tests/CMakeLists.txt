find_library(LIB_BOOST_UNIT_TEST_FRAMEWORK boost_unit_test_framework)

if(${LIB_BOOST_UNIT_TEST_FRAMEWORK} STREQUAL "LIB_BOOST_UNIT_TEST_FRAMEWORK-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'libboost_unit_test_framework' library")
endif()

add_executable(test_image ../image.cpp ../image_from_file.cpp image.cpp)

target_link_libraries (test_image ${LIB_ASAN} ${LIB_BOOST_UNIT_TEST_FRAMEWORK})
