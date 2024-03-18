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

#define BOOST_TEST_MODULE TestBestBaseline
#include <boost/test/unit_test.hpp>

#include "ppocr/image/image_from_string.hpp"
#include "ppocr/defined_loader.hpp"

#include "ppocr/ocr2/views_index_ordered.hpp"
#include "ppocr/ocr2/probabilities.hpp"
#include "ppocr/ocr2/compute_image.hpp"
#include "ppocr/ocr2/glyphs.hpp"

using namespace ppocr;

BOOST_AUTO_TEST_CASE(TestComputeImage)
{
    using S1 = loader2::Strategy<strategies::hdirection, loader2::PolicyLoader::img>;
    using S2 = loader2::Strategy<strategies::hdirection, loader2::PolicyLoader::img90>;
    using list_t = mp_list<S1, S2>;
    loader2::Datas<S1, S2> datas {};

    auto img1 = image_from_string(
        {5, 5},
        "xxxxx"
        "--x--"
        "--x--"
        "--x--"
        "--x--"
    );
    auto img2 = image_from_string(
        {5, 5},
        "x---x"
        "-x-x-"
        "-x-x-"
        "--x--"
        "--x--"
    );

    datas.load(img1);
    datas.load(img2);

    ocr2::Glyphs glyphs;
    glyphs.push_back({{0, 0}});
    glyphs.push_back({{1, 0}});

    auto const id_views = ocr2::get_views_indexes_ordered(glyphs);

    ocr2::DataIndexesOrdered<S1> const first_strategy_ordered(datas);
    ocr2::Probabilities probabilities(glyphs.size());
    ocr2::Probabilities tmp_probabilities(glyphs.size());

    ocr2::compute_simple_universe(list_t{}, probabilities, datas, first_strategy_ordered, img1, img1.rotate90());

    {
        double prob[]{1., 0.92};
        BOOST_CHECK_EQUAL(probabilities.size(), 2);
        BOOST_CHECK_EQUAL(probabilities[0].prob, prob[probabilities[0].i]);
        BOOST_CHECK_EQUAL(probabilities[1].prob, prob[probabilities[1].i]);
    }

    ocr2::compute_simple_universe(list_t{}, probabilities, datas, first_strategy_ordered, img2, img2.rotate90());

    {
        double prob[]{0.92, 1.};
        BOOST_CHECK_EQUAL(probabilities.size(), 2);
        BOOST_CHECK_EQUAL(probabilities[0].prob, prob[probabilities[0].i]);
        BOOST_CHECK_EQUAL(probabilities[1].prob, prob[probabilities[1].i]);
    }

    ocr2::reduce_complexe_universe(list_t{}, probabilities, datas, img1, img1.rotate90());

    {
        double prob[]{0.92, 0.92};
        BOOST_CHECK_EQUAL(probabilities.size(), 2);
        BOOST_CHECK_EQUAL(probabilities[0].prob, prob[probabilities[0].i]);
        BOOST_CHECK_EQUAL(probabilities[1].prob, prob[probabilities[1].i]);
    }

    ocr2::reduce_complexe_universe(list_t{}, probabilities, datas, img2, img2.rotate90());

    {
        double prob[]{0.8464, 0.92};
        BOOST_CHECK_EQUAL(probabilities.size(), 2);
        BOOST_CHECK_EQUAL(probabilities[0].prob, prob[probabilities[0].i]);
        BOOST_CHECK_EQUAL(probabilities[1].prob, prob[probabilities[1].i]);
    }

    ocr2::DataIndexesByWords const data_indexes_by_words(glyphs);

    ocr2::reduce_exclusive_universe(list_t{}, probabilities, datas, img1, img1.rotate90(), data_indexes_by_words);

    {
        double prob[]{0.8464, 0.92};
        BOOST_CHECK_EQUAL(probabilities.size(), 2);
        BOOST_CHECK_EQUAL(probabilities[0].prob, prob[probabilities[0].i]);
        BOOST_CHECK_EQUAL(probabilities[1].prob, prob[probabilities[1].i]);
    }
}
