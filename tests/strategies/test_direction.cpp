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

#define RED_TEST_MODULE TestAuthentifierNew
#include "boost_unit_tests.hpp"

#include "ppocr/strategies/direction.hpp"
#include "ppocr/strategies/utils/cardinal_direction_io.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/image/image.hpp"


using namespace ppocr;

using D = strategies::direction::cardinal_direction;

namespace {
    D to_directtion_id(Bounds bnd, const char * data_text)
    {
        Image img = image_from_string(bnd, data_text);
        return strategies::direction(img, img.rotate90()).id();
    }
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
