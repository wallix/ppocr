/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  Product name: redemption, a FLOSS RDP proxy
  Copyright (C) Wallix 2013
  Author(s): Christophe Grosjean, Meng Tan
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestAuthentifierNew
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include "image.hpp"
#include "make_box_character.hpp"
#include <type_traits>
#include <string>

#define IMAGE_PATH "./images/"

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
