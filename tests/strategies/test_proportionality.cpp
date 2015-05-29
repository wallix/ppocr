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
#define BOOST_TEST_MODULE TestProportionality
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include "strategies/proportionality.hpp"
#include "image/image_from_string.hpp"
#include "image/image.hpp"

#define IMAGE_PATH "./images/"

using namespace ppocr;

strategies::proportionality proportionality;

static strategies::proportionality::value_type to_proportion(Bounds bnd, const char * data_text)
{
    Image img = image_from_string(bnd, data_text);
    return proportionality.load(img, img/*.rotate90()*/);
}

BOOST_AUTO_TEST_CASE(TestProportionality)
{
    strategies::proportionality::value_type proportion;

    proportion = to_proportion({3, 3},
        "---"
        "---"
        "---"
    );
    BOOST_CHECK_EQUAL(proportion, 50);

    proportion = to_proportion({3, 5},
        "--x"
        "--x"
        "---"
        "--x"
        "--x"
    );
    BOOST_CHECK_EQUAL(proportion, 37);

    proportion = to_proportion({5, 7},
        "-----"
        "-----"
        "---xx"
        "-----"
        "-----"
        "-----"
        "---xx"
    );
    BOOST_CHECK_EQUAL(proportion, 41);

    proportion = to_proportion({7, 5},
        "-------"
        "-------"
        "-------"
        "-------"
        "-------"
    );
    BOOST_CHECK_EQUAL(proportion, 58);

    proportion = to_proportion({6, 4},
        "------"
        "------"
        "------"
        "------"
    );
    BOOST_CHECK_EQUAL(proportion, 60);

    auto const & relationship = proportionality.relationship();
    BOOST_CHECK_EQUAL(86, relationship(20, 6));
    BOOST_CHECK_EQUAL(86, relationship(6, 20));
}
