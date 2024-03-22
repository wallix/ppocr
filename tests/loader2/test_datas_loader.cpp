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

#define BOOST_TEST_MODULE TestLoader2
#include <boost/test/unit_test.hpp>



#include "ppocr/image/image.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/loader2/datas_loader.hpp"

#include "ppocr/strategies/hgravity.hpp"
#include "ppocr/strategies/hdirection.hpp"

using namespace ppocr;

BOOST_AUTO_TEST_CASE(TestLoader2)
{
    using strat1 = loader2::Strategy<strategies::hgravity, loader2::PolicyLoader::img>;
    using strat2 = loader2::Strategy<strategies::hgravity, loader2::PolicyLoader::img90>;
    using strat3 = loader2::Strategy<strategies::hdirection, loader2::PolicyLoader::img>;
    using Loader = loader2::Datas<strat1, strat2, strat3>;

    Loader loader {};

    Image img1 = image_from_string(
        {3, 3},
        "x--"
        "xxx"
        "x-x"
    );

    Image img2 = image_from_string(
        {3, 3},
        "x-x"
        "xxx"
        "x-x"
    );

    loader.load(img1);
    loader.load(img1);
    loader.load(img2);

    {
        auto & data = loader.get<strat1>();
        BOOST_CHECK_EQUAL(strat1::relationship_type::compute(data[0], data[1]), 100);
        BOOST_CHECK_EQUAL(strat1::relationship_type::compute(data[1], data[2]), 83);
    }
    {
        auto & data = loader.get<strat2>();
        BOOST_CHECK_EQUAL(strat1::relationship_type::compute(data[0], data[1]), 100);
        BOOST_CHECK_EQUAL(strat1::relationship_type::compute(data[1], data[2]), 90);
    }
    {
        auto & data = loader.get<strat3>();
        BOOST_CHECK_EQUAL(strat1::relationship_type::compute(data[0], data[1]), 100);
        BOOST_CHECK_EQUAL(strat1::relationship_type::compute(data[1], data[2]), 83);
    }
}
