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
#include "image_from_string.hpp"
#include "image.hpp"
#include <sstream>

#define IMAGE_PATH "./images/"

static int to_gravity_id(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return strategies::gravity(img, img.rotate90()).id();
}

BOOST_AUTO_TEST_CASE(TestGravity)
{
    int id;

    id = to_gravity_id({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_gravity_id({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 1);

    id = to_gravity_id({3, 3},
        "---"
        "--x"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 5);

    id = to_gravity_id({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(id, 3);

    id = to_gravity_id({3, 3},
        "---"
        "x--"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 15);

    id = to_gravity_id({3, 3},
        "x--"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 16);

    id = to_gravity_id({3, 3},
        "--x"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 6);

    id = to_gravity_id({3, 3},
        "---"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 8);

    id = to_gravity_id({3, 3},
        "---"
        "---"
        "x--"
    );
    BOOST_CHECK_EQUAL(id, 18);

    id = to_gravity_id({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_gravity_id({3, 3},
        "---"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 3);

    id = to_gravity_id({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 5);

    id = to_gravity_id({3, 5},
        "--x"
        "--x"
        "---"
        "--x"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 10);

    id = to_gravity_id({5, 7},
        "-----"
        "-----"
        "---xx"
        "-----"
        "-----"
        "-----"
        "---xx"
    );
    BOOST_CHECK_EQUAL(id, 13);
}
