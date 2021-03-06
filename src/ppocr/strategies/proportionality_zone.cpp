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

#include "ppocr/strategies/proportionality_zone.hpp"
#include "ppocr/image/image.hpp"
#include "ppocr/strategies/utils/count_zone.hpp"
#include "ppocr/strategies/utils/relationship.hpp"

#include <algorithm>
#include <functional>
#include <numeric>

namespace ppocr { namespace strategies {

proportionality_zone::value_type proportionality_zone::load(Image const & img, Image const & /*img90*/) const
{
    proportionality_zone::value_type ret;

    utils::ZoneInfo zone_info = utils::count_zone(img);

    for (auto & x : zone_info.right) zone_info.top.insert(x);
    for (auto & x : zone_info.bottom) zone_info.top.insert(x);
    for (auto & x : zone_info.left) zone_info.top.insert(x);

    unsigned area = 0;
    for (auto & x : zone_info.top) {
        area += x.second;
    }

    for (auto & x : zone_info.top) {
        ret.push_back(x.second * 100 / area);
    }

    std::sort(ret.begin(), ret.end());

    return ret;
}

unsigned proportionality_zone::relationship_type::operator()(const value_type& a, const value_type& b) const
{
    if (a.size() != b.size()) {
        return 0;
    }
    if (a.empty()) {
        return 100;
    }
    unsigned const total = std::inner_product(
        a.begin(), a.end(), b.begin(), 0u
      , std::plus<unsigned>()
      , [](unsigned a, unsigned b) { return a*100/(a+b); }
    ) / a.size();
    return utils::compute_relationship(total, 50u, 100u);
}

std::size_t proportionality_zone::relationship_type::count() const
{ return 101; }


double proportionality_zone::relationship_type::dist(
    proportionality_zone::relationship_type::value_type const & a,
    proportionality_zone::relationship_type::value_type const & b
) const {
    return static_cast<double>(operator()(a, b)) / 100.;
}

bool proportionality_zone::relationship_type::in_dist(
    proportionality_zone::relationship_type::value_type const & a,
    proportionality_zone::relationship_type::value_type const & b,
    unsigned d
) const {
    return operator()(a, b) >= d;
}

} }
