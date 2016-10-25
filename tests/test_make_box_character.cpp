/*
* Copyright (C) 2016 Wallix
* 
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
* 
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
* 
* You should have received a copy of the GNU Lesser General Public License along
* with this library; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestAuthentifierNew
#include "boost_unit_tests.hpp"



#include "ppocr/image/image_from_file.hpp"
#include "ppocr/box_char/make_box_character.hpp"
#include <type_traits>
#include <string>


using namespace ppocr;

BOOST_AUTO_TEST_CASE(TestImage)
{
    Image img = image_from_file(IMAGE_PATH "computer.pnm");

    char const * characters[]{
        "::::::::\n"
        ":-xxxxx:\n"
        ":xx----:\n"
        ":xx----:\n"
        ":xx----:\n"
        ":xx----:\n"
        ":xx----:\n"
        ":xx----:\n"
        ":-xxxxx:\n"
        "::::::::\n"
    ,
        "::::::::\n"
        ":-xxxx-:\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":-xxxx-:\n"
        "::::::::\n"
    ,
        "::::::::::::\n"
        ":xxxxx-xxx-:\n"
        ":xx--xx--xx:\n"
        ":xx--xx--xx:\n"
        ":xx--xx--xx:\n"
        ":xx--xx--xx:\n"
        ":xx--xx--xx:\n"
        "::::::::::::\n"
    ,
        "::::::::\n"
        ":xxxxx-:\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":xxxxx-:\n"
        ":xx----:\n"
        ":xx----:\n"
        "::::::::\n"
    ,
        "::::::::\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":xx--xx:\n"
        ":-xxxxx:\n"
        "::::::::\n"
    ,
        "::::::\n"
        ":xx--:\n"
        ":xx--:\n"
        ":xxxx:\n"
        ":xx--:\n"
        ":xx--:\n"
        ":xx--:\n"
        ":xx--:\n"
        ":-xxx:\n"
        "::::::\n"
    ,
        "::::::::\n"
        ":-xxxx-:\n"
        ":xx--xx:\n"
        ":xxxxxx:\n"
        ":xx----:\n"
        ":xx----:\n"
        ":-xxxxx:\n"
        "::::::::\n"
    ,
        "::::::\n"
        ":xx-x:\n"
        ":xxxx:\n"
        ":xx--:\n"
        ":xx--:\n"
        ":xx--:\n"
        ":xx--:\n"
        "::::::\n"
    };

    Index idx;
    std::ostringstream oss;
    for (auto s : characters) {
        auto const box = make_box_character(img, idx, img.bounds());
        BOOST_CHECK(bool(box));
        oss << img.section(box.index(), box.bounds());
        BOOST_CHECK_EQUAL(oss.str(), s);
        oss.str("");
        idx = Index(box.x()+box.w(), idx.y());
    }

    BOOST_CHECK(!bool(make_box_character(img, idx, img.bounds())));
}
