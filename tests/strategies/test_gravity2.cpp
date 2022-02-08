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

#define BOOST_TEST_MODULE TestStratGravity
#include <boost/test/unit_test.hpp>

#include "ppocr/strategies/gravity2.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/image/image.hpp"


using namespace ppocr;

namespace {
    int to_gravity2_id(Bounds bnd, const char * data_text)
    {
        Image img = image_from_string(bnd, data_text);
        return strategies::gravity2(img, img.rotate90()).id();
    }
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
