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
#define BOOST_TEST_MODULE TestStratDCompass
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include "ppocr/strategies/dcompass.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/strategies/utils/cardinal_direction_io.hpp"
#include "ppocr/image/image.hpp"

#define IMAGE_PATH "./images/"

using namespace ppocr;

using CardinalDirection = strategies::dcompass::cardinal_direction;

static CardinalDirection to_dcompass_d(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return strategies::dcompass(img, img.rotate90()).direction();
}

BOOST_AUTO_TEST_CASE(TestDCompass)
{
    CardinalDirection id;

    id = to_dcompass_d({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::NONE);

    id = to_dcompass_d({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::NORTH);

    id = to_dcompass_d({3, 3},
        "---"
        "--x"
        "---"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::EAST);

    id = to_dcompass_d({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::SOUTH);

    id = to_dcompass_d({3, 3},
        "---"
        "x--"
        "---"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::WEST);

    id = to_dcompass_d({3, 3},
        "x--"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::NORTH_WEST);

    id = to_dcompass_d({3, 3},
        "--x"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::NORTH_EAST);

    id = to_dcompass_d({3, 3},
        "---"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::SOUTH_EAST);

    id = to_dcompass_d({3, 3},
        "---"
        "---"
        "x--"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::SOUTH_WEST);

    id = to_dcompass_d({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::NONE);

    id = to_dcompass_d({3, 3},
        "---"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::SOUTH);

    id = to_dcompass_d({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::EAST);

    id = to_dcompass_d({3, 5},
        "--x"
        "--x"
        "---"
        "--x"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::EAST);

    id = to_dcompass_d({5, 7},
        "-----"
        "-----"
        "---xx"
        "-----"
        "-----"
        "-----"
        "---xx"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::SOUTH_EAST);

    id = to_dcompass_d({5, 7},
        "--xx-"
        "-----"
        "-----"
        "--x--"
        "-----"
        "-----"
        "-----"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::NORTH);

    id = to_dcompass_d({5, 6},
        "-----"
        "--x--"
        "-----"
        "-----"
        "-----"
        "-----"
    );
    BOOST_CHECK_EQUAL(id, CardinalDirection::NORTH);

    using strategies::dcompass;
    using D = CardinalDirection;

    BOOST_CHECK_EQUAL(75, dcompass(D::NORTH_EAST).relationship(dcompass(D::EAST)));
    BOOST_CHECK_EQUAL(50, dcompass(D::NORTH_EAST).relationship(dcompass(D::SOUTH_EAST)));
    BOOST_CHECK_EQUAL(25, dcompass(D::NORTH_EAST).relationship(dcompass(D::SOUTH)));
    BOOST_CHECK_EQUAL(25, dcompass(D::NORTH_EAST).relationship(dcompass(D::WEST)));
    BOOST_CHECK_EQUAL(00, dcompass(D::NORTH_EAST).relationship(dcompass(D::SOUTH_WEST)));
    BOOST_CHECK_EQUAL(50, dcompass(D::NORTH_EAST).relationship(dcompass(D::NONE)));
    BOOST_CHECK_EQUAL(75, dcompass(D::EAST).relationship(dcompass(D::NONE)));

    BOOST_CHECK_EQUAL(50, dcompass(D::EAST).relationship(dcompass(D::WEST)));
    BOOST_CHECK_EQUAL(25, dcompass(D::NORTH).relationship(dcompass(D::SOUTH_EAST)));
}
