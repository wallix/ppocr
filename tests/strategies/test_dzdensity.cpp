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
#define BOOST_TEST_MODULE TestStratDDensity
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include "strategies/dzdensity.hpp"
#include "image/image_from_string.hpp"
#include "strategies/utils/diagonal_zone_density.hpp"
#include "image/image.hpp"

#define IMAGE_PATH "./images/"

using namespace ppocr;

using D = unsigned;

static D to_ddensity_id(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return strategies::dzdensity(img, img.rotate90()).value();
}

BOOST_AUTO_TEST_CASE(TestDigonalDensity)
{
    D id;

    id = to_ddensity_id({1, 1},
        "-"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({2, 2},
        "--"
        "--"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({2, 4},
        "--"
        "--"
        "--"
        "--"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({1, 4},
        "-"
        "-"
        "-"
        "-"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({3, 3},
        "---"
        "--x"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({3, 3},
        "---"
        "x--"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({3, 3},
        "x--"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 1*100/3);

    id = to_ddensity_id({3, 3},
        "--x"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({3, 3},
        "---"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 1*100/3);

    id = to_ddensity_id({3, 3},
        "---"
        "---"
        "x--"
    );
    BOOST_CHECK_EQUAL(id, 0);

    id = to_ddensity_id({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(id, 1*100/3);

    id = to_ddensity_id({3, 3},
        "---"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 1*100/3);

    id = to_ddensity_id({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 1*100/3);

    id = to_ddensity_id({3, 5},
        "--x"
        "--x"
        "---"
        "--x"
        "--x"
    );
    BOOST_CHECK_EQUAL(id, 1*100/3);

    id = to_ddensity_id({5, 7},
        "-----"
        "-----"
        "---xx"
        "-----"
        "-----"
        "-----"
        "---xx"
    );
    BOOST_CHECK_EQUAL(id, 1*100/11);

    id = to_ddensity_id({6, 6},
        "xx----"
        "xxx---"
        "-xxx--"
        "--xxx-"
        "---xxx"
        "----xx"
    );
    BOOST_CHECK_EQUAL(id, 16*100/16);

    id = to_ddensity_id({6, 6},
        "xxxxxx"
        "xxxxxx"
        "xxxxxx"
        "xxxxxx"
        "xxxxxx"
        "xxxxxx"
    );
    BOOST_CHECK_EQUAL(id, 16*100/16);

    id = to_ddensity_id({6, 3},
        "xx----"
        "--xx--"
        "----xx"
    );
    BOOST_CHECK_EQUAL(id, 6*100/6);

    id = to_ddensity_id({6, 3},
        "xxxxxx"
        "xxxxxx"
        "xxxxxx"
    );
    BOOST_CHECK_EQUAL(id, 6*100/6);
}
