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
