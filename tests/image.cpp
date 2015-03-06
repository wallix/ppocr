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
#include <sstream>

#define IMAGE_PATH "./images/"

BOOST_AUTO_TEST_CASE(TestImage)
{
    Image img = image_from_file(IMAGE_PATH "image1.pbm");
    std::ostringstream oss;
    oss << img;
    BOOST_CHECK_EQUAL(oss.str(),
    "::::::::::\n"
    ":--------:\n"
    ":--------:\n"
    ":--------:\n"
    ":--xxxx--:\n"
    ":-----xx-:\n"
    ":--xxxxx-:\n"
    ":-xx--xx-:\n"
    ":-xx--xx-:\n"
    ":--xxxxx-:\n"
    ":--------:\n"
    ":--------:\n"
    "::::::::::\n"
    );

    img = image_from_file(IMAGE_PATH "image2.pnm");
    oss.str("");
    oss << img;
    BOOST_CHECK_EQUAL(oss.str(),
    "::::::::::\n"
    ":--------:\n"
    ":--------:\n"
    ":--------:\n"
    ":--------:\n"
    ":--xxxx--:\n"
    ":-xx--xx-:\n"
    ":-xx--xx-:\n"
    ":-xxxxxx-:\n"
    ":-xx-----:\n"
    ":-xx--xx-:\n"
    ":--xxxx--:\n"
    ":--------:\n"
    ":--------:\n"
    ":--------:\n"
    "::::::::::\n"
    );

    Image img_rotate = img.rotate90();
    oss.str("");
    oss << img_rotate;
    BOOST_CHECK_EQUAL(oss.str(),
    "::::::::::::::::\n"
    ":--------------:\n"
    ":-----xxxxx----:\n"
    ":----xxxxxxx---:\n"
    ":----x--x--x---:\n"
    ":----x--x--x---:\n"
    ":----xxxx-xx---:\n"
    ":-----xxx-x----:\n"
    ":--------------:\n"
    "::::::::::::::::\n"
    );
}
