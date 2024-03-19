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

#include "ppocr/strategies/alternations.hpp"
#include "ppocr/image/image.hpp"

#include <ostream>
#include <istream>
#include <cassert>

#ifdef DEBUG_ALTERNATION
#include <iostream>
#define MAKE_SEQUENCE_ALTERNATION(name, img, x, y, B)                                          \
    (void(std::cout << name ":[" << img.bounds() << "][" << Index x, y << "] + [" << B << "]\n"), \
    make_alternations(img, x, y, B))
#else
#define MAKE_SEQUENCE_ALTERNATION(name, img, x, y, B) make_alternations(img, x, y, B)
#endif

namespace ppocr { namespace strategies {

namespace {
    template<class T>
    alternations::alternations_type
    make_alternations(const Image & img, Index const & idx, T const & bounds)
    {
        auto range = hrange(img, idx, bounds);
        auto it = range.begin();
        auto last = range.end();

        alternations::alternations_type alternations{*it, 1};
        while (rng::next_alternation(it, last)) {
            ++alternations.count;
        }

        return alternations;
    }
}

alternations::value_type alternations::load(const Image& img, const Image& img90) const
{
    alternations::value_type seq_alternations;
    auto it = seq_alternations.begin();

    {
        Bounds const & bnd = img.bounds();

        if (bnd.height() < 2) {
            *it++ = alternations_type{0, 0};
        } else {
            *it++ = MAKE_SEQUENCE_ALTERNATION("Hl1", img, {0, (bnd.height()-2)/3}, bnd.width());
        }
        *it++ = MAKE_SEQUENCE_ALTERNATION("Hl2", img, {0, (bnd.height()*2-1)/3}, bnd.width());
        Bounds const bnd_mask(bnd.width(), bnd.height()/3);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Hm1", img, {0, 0}, bnd_mask);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Hm2", img, {0, bnd.height()-bnd_mask.height()}, bnd_mask);
    }

    {
        Bounds const & bnd = img90.bounds();

        *it++ = MAKE_SEQUENCE_ALTERNATION("Vl1", img90, {0, (bnd.height()-1)/2}, bnd.width());
        Bounds const bnd_mask(bnd.width(), bnd.height()/3);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Vm1", img90, {0, bnd.height()-bnd_mask.height()}, bnd_mask);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Vm2", img90, {0, 0}, bnd_mask);
    }

    assert(it == seq_alternations.end());
    return seq_alternations;
}

unsigned alternations::best_difference() const
{ return 10; }

std::ostream & operator<<(std::ostream & os, alternations::alternations_type const & x) {
    return os << x.start_contains_letter << ' ' << x.count;
}

std::istream & operator>>(std::istream & is, alternations::alternations_type & x) {
    return is >> x.start_contains_letter >> x.count;
}

} }
