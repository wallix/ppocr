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

  Author(s): Jonathan Poelen
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
    Image img = image_from_file(IMAGE_PATH "image_a1.pbm");
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

    img = image_from_file(IMAGE_PATH "image_e1.pbm");
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
    ":-----xxx-x----:\n"
    ":----xxxx-xx---:\n"
    ":----x--x--x---:\n"
    ":----x--x--x---:\n"
    ":----xxxxxxx---:\n"
    ":-----xxxxx----:\n"
    ":--------------:\n"
    "::::::::::::::::\n"
    );

    {
        auto range = hrange(img, {0, 5}, 8);
        BOOST_CHECK_EQUAL(range.size(), 8);

        std::vector<bool> v;
        std::copy(range.begin(), range.end(), std::back_inserter(v));
        std::string s; for (auto b : v) s += b  ? 'x' : '-';
        BOOST_CHECK_EQUAL(s, "-xx--xx-");
    }

    {
        auto range = hrange(img, {0, 5}, {8, 3});
        BOOST_CHECK_EQUAL(range.size(), 8);

        std::vector<bool> v;
        std::copy(range.begin(), range.end(), std::back_inserter(v));
        std::string s; for (auto b : v) s += b  ? 'x' : '-';
        BOOST_CHECK_EQUAL(s, "-xxxxxx-");
    }

    BOOST_CHECK_EQUAL(false, rng::any(hrange(img, {0, 0}, 8)));
    BOOST_CHECK_EQUAL(false, rng::all(hrange(img, {0, 0}, 8)));
    BOOST_CHECK_EQUAL(true, rng::none(hrange(img, {0, 0}, 8)));

    BOOST_CHECK_EQUAL(true, rng::any(hrange(img, {0, 5}, 8)));
    BOOST_CHECK_EQUAL(false, rng::all(hrange(img, {0, 5}, 8)));
    BOOST_CHECK_EQUAL(false, rng::none(hrange(img, {0, 5}, 8)));

    {
        auto range = hrange(img, {0, 5}, 8);
        auto it = range.begin();
        auto last = range.end();
        BOOST_CHECK_EQUAL(true, rng::next_alternation(it, last));
        BOOST_CHECK_EQUAL(true, rng::next_alternation(it, last));
        BOOST_CHECK_EQUAL(true, rng::next_alternation(it, last));
        BOOST_CHECK_EQUAL(true, rng::next_alternation(it, last));
        BOOST_CHECK_EQUAL(false, rng::next_alternation(it, last));
    }
}
