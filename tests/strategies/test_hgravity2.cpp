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

#define BOOST_TEST_MODULE TestHorizontalGravity2
#include <boost/test/unit_test.hpp>

#include "ppocr/strategies/hgravity2.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/image/image.hpp"


using namespace ppocr;

using D = strategies::hgravity2::value_type;

namespace {
    strategies::hgravity2 hgravity2;

    D to_hgravity2_value(Bounds bnd, const char * data_text)
    {
        Image img = image_from_string(bnd, data_text);
        strategies::hgravity2::ctx_type ctx;
        return hgravity2.load(img, img/*.rotate90()*/, ctx);
    }
}

BOOST_AUTO_TEST_CASE(TestHGravity2)
{
    D value;
    auto hInterval = hgravity2.relationship().count() / 2;

    value = to_hgravity2_value({3, 3},
        "xxx"
        "---"
        "-x-"
    );
    BOOST_CHECK_EQUAL(value, (3 + 3 - 1)*100/6);

    value = to_hgravity2_value({3, 3},
        "-x-"
        "---"
        "xxx"
    );
    BOOST_CHECK_EQUAL(value, (3 + 1 - 3)*100/6);

    value = to_hgravity2_value({3, 3},
        "---"
        "-x-"
        "---"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hgravity2_value({3, 3},
        "--x"
        "x--"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hgravity2_value({3, 3},
        "--x"
        "---"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, hInterval);

    value = to_hgravity2_value({3, 5},
        "--x"
        "---"
        "---"
        "x-x"
        "--x"
    );
    BOOST_CHECK_EQUAL(value, (9 + 2 - 4) *100/18);

    auto const & relationship = hgravity2.relationship();

    BOOST_CHECK_EQUAL(100, relationship(hInterval, hInterval));
    BOOST_CHECK_EQUAL(50,  relationship(hInterval*2, hInterval));
    BOOST_CHECK_EQUAL(75,  relationship(hInterval+hInterval/2, hInterval));
    BOOST_CHECK_EQUAL(75,  relationship(hInterval/2, hInterval));
    BOOST_CHECK_EQUAL(75,  relationship(hInterval/2, 0));
    BOOST_CHECK_EQUAL(25,  relationship(hInterval/2, hInterval*2));
    BOOST_CHECK_EQUAL(91,  relationship(hInterval/2, hInterval/3));
    BOOST_CHECK_EQUAL(57,  relationship(88, 45));
}
