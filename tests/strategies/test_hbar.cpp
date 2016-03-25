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
#define BOOST_TEST_MODULE TestHorizontalBar
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include "ppocr/strategies/hbar.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/image/image.hpp"

#define IMAGE_PATH "./images/"

using namespace ppocr;

strategies::hbar hbar;

static strategies::hbar::value_type to_hbar_value(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return hbar.load(img, img);
}

BOOST_AUTO_TEST_CASE(TestHBar)
{
    strategies::hbar::value_type value;

    value = to_hbar_value({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(value, 0);

    value = to_hbar_value({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(value, 0);

    value = to_hbar_value({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(value, 0);

    value = to_hbar_value({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(value, 0);

    value = to_hbar_value({3, 3},
        "--x"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, 0);

    value = to_hbar_value({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, 0);

    value = to_hbar_value({3, 5},
        "--x"
        "---"
        "---"
        "x-x"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, 0);

    value = to_hbar_value({3, 3},
        "xxx"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, 1);

    value = to_hbar_value({3, 3},
        "xxx"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, 1);

    value = to_hbar_value({3, 3},
        "xxx"
        "xxx"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, 1);

    value = to_hbar_value({3, 5},
        "xxx"
        "xxx"
        "--x"
        "xxx"
        "xxx"
    );
    BOOST_CHECK_EQUAL(value, 2);

    value = to_hbar_value({3, 5},
        "xx-"
        "xxx"
        "--x"
        "xxx"
        "x-x"
    );
    BOOST_CHECK_EQUAL(value, 2);


    auto const & relationship = hbar.relationship();

    BOOST_CHECK_EQUAL(true, relationship(0, 0));
    BOOST_CHECK_EQUAL(true, relationship(2, 2));
    BOOST_CHECK_EQUAL(false, relationship(2, 1));
}
