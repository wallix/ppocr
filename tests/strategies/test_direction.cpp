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

#include "strategies/direction.hpp"
#include "strategies/utils/cardinal_direction_io.hpp"
#include "image/image_from_string.hpp"
#include "image/image.hpp"

#define IMAGE_PATH "./images/"

using D = strategies::direction::cardinal_direction;

static D to_directtion_id(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return strategies::direction(img, img.rotate90()).id();
}

BOOST_AUTO_TEST_CASE(TestDirection)
{
    using D = strategies::direction::cardinal_direction;
    D id;

    id = to_directtion_id({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NONE);

    id = to_directtion_id({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NORTH);

    id = to_directtion_id({3, 3},
        "---"
        "--x"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::EAST);

    id = to_directtion_id({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(id, D::SOUTH);

    id = to_directtion_id({3, 3},
        "---"
        "x--"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::WEST);

    id = to_directtion_id({3, 3},
        "x--"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NORTH_WEST);

    id = to_directtion_id({3, 3},
        "--x"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NORTH_EAST);

    id = to_directtion_id({3, 3},
        "---"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, D::SOUTH_EAST);

    id = to_directtion_id({3, 3},
        "---"
        "---"
        "x--"
    );
    BOOST_CHECK_EQUAL(id, D::SOUTH_WEST);

    id = to_directtion_id({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(id, D::NONE);

    id = to_directtion_id({3, 3},
        "---"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, D::SOUTH);

    id = to_directtion_id({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, D::EAST);

    id = to_directtion_id({3, 5},
        "--x"
        "--x"
        "---"
        "--x"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, D::EAST2);

    using strategies::direction;

    BOOST_CHECK_EQUAL(100, direction(D::EAST).relationship(direction(D::EAST)));
    BOOST_CHECK_EQUAL(88,  direction(D::EAST).relationship(direction(D::NONE)));
    BOOST_CHECK_EQUAL(88,  direction(D::NORTH_EAST).relationship(direction(D::EAST)));
    BOOST_CHECK_EQUAL(75,  direction(D::NORTH2_EAST).relationship(direction(D::EAST)));
    BOOST_CHECK_EQUAL(63,  direction(D::NORTH2_EAST).relationship(direction(D::NONE)));
    BOOST_CHECK_EQUAL(50,  direction(D::NORTH2_EAST).relationship(direction(D::WEST)));
    BOOST_CHECK_EQUAL(75,  direction(D::NORTH).relationship(direction(D::SOUTH)));
    BOOST_CHECK_EQUAL(0,   direction(D::NORTH2_EAST2).relationship(direction(D::SOUTH2_WEST2)));
    BOOST_CHECK_EQUAL(50,  direction(D::NORTH2).relationship(direction(D::SOUTH2)));
    BOOST_CHECK_EQUAL(50,  direction(D::NORTH_EAST).relationship(direction(D::SOUTH_WEST)));
}
