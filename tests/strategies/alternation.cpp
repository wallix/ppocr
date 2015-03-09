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

#include "strategies/alternation.hpp"
#include <sstream>

#define IMAGE_PATH "./images/"

BOOST_AUTO_TEST_CASE(TestAlternation)
{
    char const * text_img =
        "-xxxx-"
        "xx--xx"
        "xx--xx"
        "xxxxxx"
        "xx----"
        "xx--xx"
        "-xxxx-"
    ;

    Bounds const bnd(6, 7);
    assert(text_img[bnd.area()] == 0 && text_img[bnd.area()-1] == '-');

    Image const img(bnd, [text_img, &bnd]{
        size_t const area = bnd.area();
        PtrImageData data(new Pixel[area]);
        std::copy(text_img, text_img+area, data.get());
        return data;
    }());

    {
        std::ostringstream oss;
        oss << img;
        BOOST_CHECK_EQUAL(oss.str(),
            "::::::::\n"
            ":-xxxx-:\n"
            ":xx--xx:\n"
            ":xx--xx:\n"
            ":xxxxxx:\n"
            ":xx----:\n"
            ":xx--xx:\n"
            ":-xxxx-:\n"
            "::::::::\n"
        );
    }

    auto alternations = strategies::all_sequence_alternation(img, img.rotate90());
    using seq_t = strategies::alternation_seq_t;
    BOOST_CHECK(alternations[0] == seq_t({1, 0, 1}));
    BOOST_CHECK(alternations[1] == seq_t({1, 0}));
    BOOST_CHECK(alternations[2] == seq_t({1}));
    BOOST_CHECK(alternations[3] == seq_t({1}));
    BOOST_CHECK(alternations[4] == seq_t({1, 0, 1, 0, 1}));
    BOOST_CHECK(alternations[5] == seq_t({1}));
    BOOST_CHECK(alternations[6] == seq_t({1, 0, 1}));
}
