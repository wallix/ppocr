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
#define BOOST_TEST_MODULE TestReindex
#include <boost/test/auto_unit_test.hpp>

#include "ppocr/utils/reindex.hpp"

using ppocr::utils::reindex;

BOOST_AUTO_TEST_CASE(TestReindex)
{
    std::vector<unsigned> indexes{{0, 1, 2, 3, 4}};
    std::vector<unsigned> v{{0, 1, 2, 3, 4}};
    reindex(indexes, v);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), indexes.begin(), indexes.end());

    indexes = {4, 3, 2, 1, 0};
    reindex(indexes, v);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), indexes.begin(), indexes.end());

    indexes = {3, 0, 2, 4, 1};
    reindex(indexes, v);
    std::vector<unsigned> v2{{1, 4, 2, 0, 3}};
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), v2.begin(), v2.end());
}
