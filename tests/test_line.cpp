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
#define BOOST_TEST_MODULE TestAuthentifierNew
#include <boost/test/auto_unit_test.hpp>

#undef SHARE_PATH
#define SHARE_PATH FIXTURES_PATH

#define LOGNULL
//#define LOGPRINT

#include <fstream>
#include "filters/line.hpp"
#include "image/image_from_string.hpp"

BOOST_AUTO_TEST_CASE(TestLine)
{
    DataLoader loader;
    filters::line line;
    std::ifstream file("lines_info");
    BOOST_CHECK_EQUAL(true, file.is_open());
    file >> line;

    {
        Definition def{
            "d", "",
            image_from_string({6, 8}, "----xx----xx-xxxxxxx--xxxx--xxxx--xxxx--xx-xxxxx"),
            DataLoader::Datas()
        };
        filters::CharInfo char_info{{&def}, Box{{0, 0}, {6, 8}}, true};
        filters::line::data_line data_line = line.search({char_info});

        BOOST_CHECK_EQUAL(0, data_line.capline);
        BOOST_CHECK_EQUAL(0, data_line.ascentline);
    }

    filters::line::data_line data_line{5, 6, 8, 15};
    {
        Box box{{111, 6}, {1, 3}};
        Definition def{"'", "", image_from_string(box.bounds(), "xxx"), DataLoader::Datas()};
        filters::ptr_def_list definitions({&def});
        line(definitions, box, data_line);

        BOOST_CHECK_EQUAL(definitions.size(), 1);
    }
    {
        Box box{{111, 5}, {6, 11}};
        Definition def{"b", "", image_from_string(box.bounds(), "x-----x-----x-----xxxxx-xx--xxx----xx----xx----xx----xxx--xxxxxxx-"), DataLoader::Datas()};
        filters::ptr_def_list definitions({&def});
        line(definitions, box, data_line);

        BOOST_CHECK_EQUAL(definitions.size(), 1);
    }
    {
        Box box{{111, 8}, {6, 11}};
        Definition def{"p", "", image_from_string(box.bounds(), "x-----x-----x-----xxxxx-xx--xxx----xx----xx----xx----xxx--xxxxxxx-"), DataLoader::Datas()};
        filters::ptr_def_list definitions({&def});
        line(definitions, box, data_line);

        BOOST_CHECK_EQUAL(definitions.size(), 1);
    }
}
