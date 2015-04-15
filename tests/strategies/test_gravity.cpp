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

#include "strategies/gravity.hpp"
#include "image/image_from_string.hpp"
#include "strategies/utils/cardinal_direction_io.hpp"
#include "image/image.hpp"

#define IMAGE_PATH "./images/"

using D = strategies::gravity::cardinal_direction;

static D to_gravity_id(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return strategies::gravity(img, img.rotate90()).id();
}

BOOST_AUTO_TEST_CASE(TestGravity)
{
    D id;

    id = to_gravity_id({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NONE);

    id = to_gravity_id({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NORTH);

    id = to_gravity_id({3, 3},
        "---"
        "--x"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::EAST);

    id = to_gravity_id({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(id, D::SOUTH);

    id = to_gravity_id({3, 3},
        "---"
        "x--"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::WEST);

    id = to_gravity_id({3, 3},
        "x--"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NORTH_WEST);

    id = to_gravity_id({3, 3},
        "--x"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NORTH_EAST);

    id = to_gravity_id({3, 3},
        "---"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, D::SOUTH_EAST);

    id = to_gravity_id({3, 3},
        "---"
        "---"
        "x--"
    );
    BOOST_CHECK_EQUAL(id, D::SOUTH_WEST);

    id = to_gravity_id({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NONE);

    id = to_gravity_id({3, 3},
        "---"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, D::SOUTH);

    id = to_gravity_id({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, D::EAST);

    id = to_gravity_id({3, 5},
        "--x"
        "--x"
        "---"
        "--x"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, D::EAST2);

    id = to_gravity_id({5, 7},
        "-----"
        "-----"
        "---xx"
        "-----"
        "-----"
        "-----"
        "---xx"
    );
    BOOST_CHECK_EQUAL(id, D::SOUTH_EAST2);

    using strategies::gravity;

    BOOST_CHECK_EQUAL(100, gravity(D::EAST).relationship(gravity(D::EAST)));
    BOOST_CHECK_EQUAL(88,  gravity(D::EAST).relationship(gravity(D::NONE)));
    BOOST_CHECK_EQUAL(88,  gravity(D::NORTH_EAST).relationship(gravity(D::EAST)));
    BOOST_CHECK_EQUAL(75,  gravity(D::NORTH2_EAST).relationship(gravity(D::EAST)));
    BOOST_CHECK_EQUAL(63,  gravity(D::NORTH2_EAST).relationship(gravity(D::NONE)));
    BOOST_CHECK_EQUAL(50,  gravity(D::NORTH2_EAST).relationship(gravity(D::WEST)));
    BOOST_CHECK_EQUAL(75,  gravity(D::NORTH).relationship(gravity(D::SOUTH)));
    BOOST_CHECK_EQUAL(0,   gravity(D::NORTH2_EAST2).relationship(gravity(D::SOUTH2_WEST2)));
    BOOST_CHECK_EQUAL(50,  gravity(D::NORTH2).relationship(gravity(D::SOUTH2)));
    BOOST_CHECK_EQUAL(50,  gravity(D::NORTH_EAST).relationship(gravity(D::SOUTH_WEST)));
}
