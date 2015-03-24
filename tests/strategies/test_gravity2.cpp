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
#define BOOST_TEST_MODULE TestStratGravity
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include "strategies/gravity2.hpp"
#include "image/image_from_string.hpp"
#include "image/image.hpp"
#include <sstream>

#define IMAGE_PATH "./images/"

static int to_gravity2_id(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return strategies::gravity2(img, img.rotate90()).id();
}

BOOST_AUTO_TEST_CASE(TestGravity)
{
    int id;

    id = to_gravity2_id({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, (100 << 9) + 100);

    id = to_gravity2_id({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, (100 << 9) + 133);

    id = to_gravity2_id({3, 3},
        "---"
        "--x"
        "---"
    );
    BOOST_CHECK_EQUAL(id, (133 << 9) + 100);

    id = to_gravity2_id({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(id, (100 << 9) + 67);

    id = to_gravity2_id({3, 3},
        "---"
        "x--"
        "---"
    );
    BOOST_CHECK_EQUAL(id, (67 << 9) + 100);

    id = to_gravity2_id({3, 3},
        "x--"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, (67 << 9) + 133);

    id = to_gravity2_id({3, 3},
        "--x"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, (133 << 9) + 133);

    id = to_gravity2_id({3, 3},
        "---"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, (133 << 9) + 67);

    id = to_gravity2_id({3, 3},
        "---"
        "---"
        "x--"
    );
    BOOST_CHECK_EQUAL(id, (67 << 9) + 67);

    id = to_gravity2_id({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(id, (100 << 9) + 100);

    id = to_gravity2_id({3, 3},
        "---"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, (100 << 9) + 67);

    id = to_gravity2_id({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 85092);

    id = to_gravity2_id({3, 5},
        "--x"
        "--x"
        "---"
        "--x"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 92260);

    id = to_gravity2_id({5, 7},
        "-----"
        "-----"
        "---xx"
        "-----"
        "-----"
        "-----"
        "---xx"
    );
    BOOST_CHECK_EQUAL(id, 65623);

    using strategies::gravity2;

    BOOST_CHECK_EQUAL(100, gravity2(1).relationship(gravity2(1)));
    BOOST_CHECK_EQUAL(96,  gravity2((100 << 9) + 67).relationship(gravity2((100<< 9) + 100)));
}
