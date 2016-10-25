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

#ifndef PPOCR_SRC_STRATEGIES_UTILS_COUNT_ZONE_HPP
#define PPOCR_SRC_STRATEGIES_UTILS_COUNT_ZONE_HPP

#include "ppocr/image/image.hpp"

#include <vector>
#include <map>

namespace ppocr { namespace strategies { namespace utils {

    inline bool propagation_zone(
        std::vector<unsigned> & mirror, Image const & img,
        unsigned current, Index const & idx
    ) {
        {
            auto & n = mirror[img.to_size_t(idx)];
            if (n || is_pix_letter(img[idx])) {
                return false;
            }
            n = current;
        }

        if (idx.x() != 0) {
            propagation_zone(mirror, img, current, {idx.x()-1, idx.y()});
        }
        if (idx.x() + 1 != img.width()) {
            propagation_zone(mirror, img, current, {idx.x()+1, idx.y()});
        }
        if (idx.y() != 0) {
            propagation_zone(mirror, img, current, {idx.x(), idx.y()-1});
        }
        if (idx.y() + 1 != img.height()) {
            propagation_zone(mirror, img, current, {idx.x(), idx.y()+1});
        }

        return true;
    }

    struct ZoneInfo {
        std::map<unsigned, unsigned> top;
        std::map<unsigned, unsigned> right;
        std::map<unsigned, unsigned> bottom;
        std::map<unsigned, unsigned> left;
        unsigned count_zone = 1;
    };

    inline ZoneInfo count_zone(const Image& img) {
        ZoneInfo zone;
        std::vector<unsigned> mirror(img.area(), 0);

        for (size_t x = 0; x < img.width(); ++x) {
            for (size_t y = 0; y < img.height(); ++y) {
                if (utils::propagation_zone(mirror, img, zone.count_zone, {x, y})) {
                    ++zone.count_zone;
                }
            }
        }

        auto insert = [&](std::map<unsigned, unsigned> & m, size_t x, size_t y) {
            auto i = img.to_size_t({x, y});
            if (mirror[i]) {
                ++m[mirror[i]];
            }
        };

        for (size_t x = 0; x < img.width(); ++x) {
            insert(zone.top, x, 0);
            insert(zone.bottom, x, img.height()-1);
        }

        for (size_t y = 0; y < img.height(); ++y) {
            insert(zone.left, 0, y);
            insert(zone.right, img.width()-1, y);
        }

        return zone;
    }


} } }

#endif
