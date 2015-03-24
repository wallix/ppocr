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
#define BOOST_TEST_MODULE TestHorizontalDirection2
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include "strategies/hdirection2.hpp"
#include "image_from_string.hpp"
#include "image.hpp"
#include <sstream>

#define IMAGE_PATH "./images/"

using D = unsigned;

static D to_hdirection2_value(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return strategies::hdirection2(img, img.rotate90()).value();
}

BOOST_AUTO_TEST_CASE(TestHDirection2)
{
    D value;
    auto hInterval = strategies::hdirection2::traits::get_interval() / 2;

    value = to_hdirection2_value({3, 3},
        "xxx"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(value, (3 + 3 - 1)*100/6);

    value = to_hdirection2_value({3, 3},
        "-x-"
        "---"
        "xxx"
    );
    BOOST_CHECK_EQUAL(value, (3 + 1 - 3)*100/6);

    value = to_hdirection2_value({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hdirection2_value({3, 3},
        "--x"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hdirection2_value({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hdirection2_value({3, 5},
        "--x"
        "---"
        "---"
        "x-x"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, (6 + 1 - 3) *100/12);

    using strategies::hdirection2;

    BOOST_CHECK_EQUAL(100, hdirection2(hInterval).relationship(hInterval));
    BOOST_CHECK_EQUAL(50, hdirection2(hInterval*2).relationship(hInterval));
    BOOST_CHECK_EQUAL(75, hdirection2(hInterval+hInterval/2).relationship(hInterval));
    BOOST_CHECK_EQUAL(75, hdirection2(hInterval/2).relationship(hInterval));
    BOOST_CHECK_EQUAL(75,  hdirection2(hInterval/2).relationship(0));
    BOOST_CHECK_EQUAL(25,  hdirection2(hInterval/2).relationship(hInterval*2));
    BOOST_CHECK_EQUAL(91,  hdirection2(hInterval/2).relationship(hInterval/3));
    BOOST_CHECK_EQUAL(57,  hdirection2(88).relationship(45));
}
