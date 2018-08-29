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

#define RED_TEST_MODULE TestLoader2
#include "boost_unit_tests.hpp"



#include "ppocr/image/image.hpp"
#include "ppocr/image/image_from_string.hpp"
#include "ppocr/loader2/glyphs_loader.hpp"

using namespace ppocr;

BOOST_AUTO_TEST_CASE(TestGlyphLoader)
{
    loader2::Glyphs glyphs;
    glyphs.push_back({
        image_from_string(
            {3, 3},
            "x--"
            "xxx"
            "x-x"
        ),
        {{"h", "imaginary", 0}}
    });
    glyphs.push_back({
        image_from_string(
            {3, 3},
            "x-X"
            "xxx"
            "x-x"
        ),
        {{"H", "imaginary", 0}}
    });

    std::stringstream ss;
    ss << glyphs;

    BOOST_CHECK_EQUAL(ss.str(),
        "3 3 x--xxxx-x\n"
        "1\n"
        "h imaginary 0\n"
        "3 3 x-Xxxxx-x\n"
        "1\n"
        "H imaginary 0\n"
    );


    loader2::Glyphs glyphs2;
    ss >> glyphs2;

    BOOST_CHECK_EQUAL(glyphs, glyphs2);
}
