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

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestStratAGravity
#include "boost_unit_tests.hpp"

#include "ppocr/strategies/agravity.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/image/image.hpp"


using namespace ppocr;

using strategies::agravity;

static agravity to_gravity(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return agravity(img, img.rotate90());
}

BOOST_AUTO_TEST_CASE(TestAGravity)
{
    agravity ag;

    ag = to_gravity({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(ag, agravity(agravity::null_angle()));

    ag = to_gravity({3, 3},
        "-x-"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(ag, agravity(M_PI / 2));

    ag = to_gravity({3, 3},
        "---"
        "--x"
        "---"
    );
    BOOST_CHECK_EQUAL(ag, agravity(0));

    ag = to_gravity({3, 3},
        "---"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(ag, agravity(-M_PI / 2));

    ag = to_gravity({3, 3},
        "---"
        "x--"
        "---"
    );
    BOOST_CHECK_EQUAL(ag, agravity(0));

    ag = to_gravity({3, 3},
        "x--"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(ag, agravity(M_PI/4));

    ag = to_gravity({3, 3},
        "--x"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(ag, agravity(M_PI/2-M_PI/4));

    ag = to_gravity({3, 3},
        "---"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(ag, agravity(-M_PI/4));

    ag = to_gravity({3, 3},
        "---"
        "---"
        "x--"
    );
    BOOST_CHECK_EQUAL(ag, agravity(-M_PI/2+M_PI/4));

    ag = to_gravity({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(ag, agravity(agravity::null_angle()));

    ag = to_gravity({3, 3},
        "---"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(ag, agravity(-M_PI/2));

    ag = to_gravity({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(ag, agravity(0));

    ag = to_gravity({3, 5},
        "--x"
        "--x"
        "---"
        "--x"
        "--x"
    );
    BOOST_CHECK_EQUAL(ag, agravity(0));

    ag = to_gravity({5, 7},
        "-----"
        "-----"
        "---xx"
        "-----"
        "-----"
        "-----"
        "---xx"
    );
    {
        long const hx = 1 + 1 + 2 + 2;
        long const hy = 1 + 1 - 3 - 3;
        BOOST_CHECK_EQUAL(ag, agravity(std::asin(double(hy) / std::sqrt(hy*hy+hx*hx))));
    }

    agravity const nullg(agravity::null_angle());
    BOOST_CHECK_EQUAL(100, nullg.relationship(nullg));
    BOOST_CHECK_EQUAL(0, nullg.relationship(agravity(0)));
    BOOST_CHECK_EQUAL(0, nullg.relationship(agravity(M_PI/2)));
    BOOST_CHECK_EQUAL(100, agravity(10).relationship(agravity(10)));
    BOOST_CHECK_EQUAL(50, agravity(M_PI).relationship(agravity(M_PI/2)));
    BOOST_CHECK_EQUAL(50, agravity(M_PI/2).relationship(agravity(M_PI)));
}
