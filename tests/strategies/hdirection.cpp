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
#define BOOST_TEST_MODULE TestHorizontalDirection
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include "strategies/hdirection.hpp"
#include "image_from_string.hpp"
#include "image.hpp"
#include <sstream>

#define IMAGE_PATH "./images/"

using D = unsigned;

static D to_hdirection_value(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return strategies::hdirection(img, img.rotate90()).value();
}

BOOST_AUTO_TEST_CASE(TestHDirection)
{
    D value;
    auto hInterval = strategies::hdirection::traits::get_interval() / 2;

    value = to_hdirection_value({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hdirection_value({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(value, hInterval*2);

    value = to_hdirection_value({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(value, 0);

    value = to_hdirection_value({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hdirection_value({3, 3},
        "--x"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hdirection_value({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, 100);

    value = to_hdirection_value({3, 5},
        "--x"
        "---"
        "---"
        "x-x"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, (hInterval*2) / 4);

    using strategies::hdirection;

    BOOST_CHECK_EQUAL(100, hdirection(hInterval).relationship(hInterval));
    BOOST_CHECK_EQUAL(50, hdirection(hInterval*2).relationship(hInterval));
    BOOST_CHECK_EQUAL(75, hdirection(hInterval+hInterval/2).relationship(hInterval));
    BOOST_CHECK_EQUAL(75, hdirection(hInterval/2).relationship(hInterval));
    BOOST_CHECK_EQUAL(75,  hdirection(hInterval/2).relationship(0));
    BOOST_CHECK_EQUAL(25,  hdirection(hInterval/2).relationship(hInterval*2));
    BOOST_CHECK_EQUAL(92,  hdirection(hInterval/2).relationship(hInterval/3));
    BOOST_CHECK_EQUAL(79,  hdirection(88).relationship(45));
}
