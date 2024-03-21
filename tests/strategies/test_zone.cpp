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

#define BOOST_TEST_MODULE TestLoop
#include <boost/test/unit_test.hpp>

#include "ppocr/strategies/zone.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/image/image.hpp"


using namespace ppocr;

namespace {
    strategies::zone zone_;
    using A = strategies::zone::value_type;

    A mk_zone(Bounds bnd, const char * data_text)
    {
        Image img = image_from_string(bnd, data_text);
        strategies::zone::ctx_type ctx;
        return zone_.load(img, img/*.rotate90()*/, ctx);
    }
}

# define TEST_EQUAL_RANGES(...)                       \
    [](auto const & a_, auto const & b_) {            \
        using std::begin;                             \
        using std::end;                               \
        BOOST_CHECK_EQUAL_COLLECTIONS(                \
            (void(#__VA_ARGS__), begin(a_)), end(a_), \
            begin(b_), end(b_)                        \
        );                                            \
    }(__VA_ARGS__)

BOOST_AUTO_TEST_CASE(TestLoop)
{
    A zone;

    zone = mk_zone({3, 3},
        "xxx"
        "---"
        "-x-"
    );
    TEST_EQUAL_RANGES(zone, A{{1, 0, 0, 1, 1, 1, 0}});

    zone = mk_zone({3, 5},
        "--x"
        "---"
        "---"
        "xxx"
        "--x"
    );
    TEST_EQUAL_RANGES(zone, A{{0, 1, 1, 1, 1, 2, 0}});

    zone = mk_zone({3, 4},
        "-x-"
        "x-x"
        "xxx"
        "x-x"
    );
    TEST_EQUAL_RANGES(zone, A{{0, 1, 2, 1, 1, 1, 1}});

    zone = mk_zone({5, 6},
        "--x--"
        "-x-x-"
        "x---x"
        "xxxxx"
        "x---x"
        "x---x"
    );
    TEST_EQUAL_RANGES(zone, A{{0, 1, 2, 1, 1, 1, 1}});
}
