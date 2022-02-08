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

#define BOOST_TEST_MODULE TestProportionalityZone
#include <boost/test/unit_test.hpp>

#include "ppocr/strategies/proportionality_zone.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/image/image.hpp"

using namespace ppocr;

namespace {
    strategies::proportionality_zone proportionality_zone;

    strategies::proportionality_zone::value_type
    to_proportion(Bounds bnd, const char * data_text)
    {
        Image img = image_from_string(bnd, data_text);
        return proportionality_zone.load(img, img/*.rotate90()*/);
    }
}

BOOST_AUTO_TEST_CASE(TestProportionalityZone)
{
    using T = strategies::proportionality_zone::value_type;
    T proportion;
    T proportion2;

    proportion2 = {100};
    proportion = to_proportion({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL_COLLECTIONS(proportion.begin(), proportion.end(), proportion2.begin(), proportion2.end());

    proportion = to_proportion({3, 5},
        "--x"
        "--x"
        "---"
        "--x"
        "--x"
    );
    BOOST_CHECK_EQUAL_COLLECTIONS(proportion.begin(), proportion.end(), proportion2.begin(), proportion2.end());

    proportion = to_proportion({5, 7},
        "-----"
        "-----"
        "xxxxx"
        "-----"
        "-----"
        "-----"
        "---xx"
    );
    proportion2 = {37, 62};
    BOOST_CHECK_EQUAL_COLLECTIONS(proportion.begin(), proportion.end(), proportion2.begin(), proportion2.end());

    proportion = to_proportion({7, 5},
        "-------"
        "--xxx--"
        "--x-x--"
        "--xxx--"
        "-------"
    );
    proportion2 = {100};
    BOOST_CHECK_EQUAL_COLLECTIONS(proportion.begin(), proportion.end(), proportion2.begin(), proportion2.end());

    proportion = to_proportion({7, 5},
        "----x--"
        "--xxx--"
        "--xxx--"
        "-xxxx--"
        "-x---x-"
    );
    proportion2 = {22, 33, 44};
    BOOST_CHECK_EQUAL_COLLECTIONS(proportion.begin(), proportion.end(), proportion2.begin(), proportion2.end());

    proportion = to_proportion({6, 4},
        "x-x-x-"
        "-X-X-x"
        "x-x-x-"
        "-X-x-x"
    );
    proportion2 = {20, 20, 20, 40};
    BOOST_CHECK_EQUAL_COLLECTIONS(proportion.begin(), proportion.end(), proportion2.begin(), proportion2.end());

//     auto const & relationship = proportionality_zone.relationship();
//     BOOST_CHECK_EQUAL_COLLECTIONS(86, relationship(20, 6));
//     BOOST_CHECK_EQUAL_COLLECTIONS(86, relationship(6, 20));
}
