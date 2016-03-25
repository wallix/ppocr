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
#define BOOST_TEST_MODULE TestBestBaseline
#include <boost/test/auto_unit_test.hpp>

#include "ppocr/filters/best_baseline.hpp"
#include <sstream>

using namespace ppocr;

BOOST_AUTO_TEST_CASE(TestBestBaseline)
{
   unsigned baselines[] {7, 15, 15, 18, 15, 7, 15, 7};

   using std::begin;
   using std::end;

   BOOST_CHECK_EQUAL(filters::best_baseline(begin(baselines), end(baselines)), 15);
}
