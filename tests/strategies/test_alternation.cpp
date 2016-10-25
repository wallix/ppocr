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
#define BOOST_TEST_MODULE TestAuthentifierNew
#include "boost_unit_tests.hpp"

#include "ppocr/strategies/alternation.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/image/image.hpp"

#include <sstream>

using namespace ppocr;

BOOST_AUTO_TEST_CASE(TestAlternation)
{
    Image img = image_from_string({6, 7},
        "-xxxx-"
        "xx--xx"
        "xx--xx"
        "xxxxxx"
        "xx----"
        "xx--xx"
        "-xxxx-"
    );

    strategies::alternations alternations;
    auto relationship = alternations.relationship();

    using alternations_t = strategies::alternations::alternations_type;
    auto const a = alternations.load(img, img.rotate90());
    BOOST_CHECK(a[0] == alternations_t({1, 3}));
    BOOST_CHECK(a[1] == alternations_t({1, 2}));
    BOOST_CHECK(a[2] == alternations_t({1, 1}));
    BOOST_CHECK(a[3] == alternations_t({1, 1}));
    BOOST_CHECK(a[4] == alternations_t({1, 5}));
    BOOST_CHECK(a[5] == alternations_t({1, 1}));
    BOOST_CHECK(a[6] == alternations_t({1, 3}));
    BOOST_CHECK_EQUAL(100, relationship(a, a));

    img = image_from_string({6, 7},
        "-x--x-"
        "xx--xx"
        "xx--xx"
        "xxxxxx"
        "xx----"
        "xx--xx"
        "-xxxx-"
    );

    auto const a2 = alternations.load(img, img.rotate90());
    BOOST_CHECK(a2[0] == alternations_t({1, 3}));
    BOOST_CHECK(a2[1] == alternations_t({1, 2}));
    BOOST_CHECK(a2[2] == alternations_t({1, 3}));
    BOOST_CHECK(a2[3] == alternations_t({1, 1}));
    BOOST_CHECK(a2[4] == alternations_t({0, 4}));
    BOOST_CHECK(a2[5] == alternations_t({1, 1}));
    BOOST_CHECK(a2[6] == alternations_t({1, 3}));
    BOOST_CHECK_EQUAL(5*100/7, relationship(a, a2));
    BOOST_CHECK_EQUAL(5*100/7, relationship(a2, a));

    std::stringstream ss;
    for (auto & x : a2) {
        ss << x << ' ';
    }
    strategies::alternations::value_type a3;
    for (auto & x : a3) {
        ss >> x;
    }
    BOOST_CHECK(a2 == a3);
}
