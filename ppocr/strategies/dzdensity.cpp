/*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Copyright (C) Wallix 2015
*   Author(s): Jonathan Poelen
*/

#include "dzdensity.hpp"

#include "image/image.hpp"

#include <algorithm>

namespace ppocr { namespace strategies {

namespace {
    template<class Fn>
    void dzdensity_load_impl(Image const & img, Fn f) {
        int const area = int(img.area());
        int const h3 = int(img.height() * 3);
        int const w3 = int(img.width() * 3);
        int yp = 0;
        for (size_t y = 0; y < img.height(); ++y, yp += w3) {
            //double const d = double(img.width())/3.;
            //double const xs = std::ceil(double(y+1)*w/h-d);
            //double const xe = std::floor(double(y )*w/h+d);
            int const xs_ = (w3 + yp - area + h3-1) / h3;
            int const xe_ = (     yp + area       ) / h3;

            int const xs = std::max(0, xs_);
            int const xe = std::min(int(img.width()), xe_);

            f(xs, xe);
        }
    }
}

dzdensity::value_type dzdensity::load(Image const & img, Image const & /*img90*/) const
{
    if (img.width() < 3 || img.height() < 3) {
        return 0;
    }

    int res = 0;
    int area = 0;
    auto data = img.data();
    dzdensity_load_impl(img, [&](int xs, int xe) {
        xs = std::max(0, xs);
        xe = std::min(int(img.width()), xe);
        res += std::count_if(data+xs, data+xe, is_pix_letter_fn());
        area += xe-xs;
        data += img.width();
    });

    return dzdensity::value_type(res * 100 / area);
}

dzdensity::relationship_type dzdensity::relationship() const
{ return {100}; }

unsigned dzdensity::best_difference() const
{ return {5}; }

} }
