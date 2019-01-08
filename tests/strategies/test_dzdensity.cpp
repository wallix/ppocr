/*
* Copyright (C) 2016 Wallix
* 
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
* 
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
* 
* You should have received a copy of the GNU Lesser General Public License along
* with this library; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#define BOOST_TEST_MODULE TestStratDDensity
#include <boost/test/auto_unit_test.hpp>

#include "ppocr/strategies/dzdensity.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/strategies/utils/diagonal_zone_density.hpp"
#include "ppocr/image/image.hpp"


using namespace ppocr;

using D = unsigned;

namespace {
    D to_ddensity_id(Bounds bnd, const char * data_text)
    {
        Image img = image_from_string(bnd, data_text);
        return strategies::dzdensity(img, img.rotate90()).value();
    }
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
