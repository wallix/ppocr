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
*   Product name: redemption, a FLOSS RDP proxy
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#ifndef REDEMPTION_PPOCR_SRC_OCR2_INSERT_VIEWS_HPP
#define REDEMPTION_PPOCR_SRC_OCR2_INSERT_VIEWS_HPP

#include "probabilities.hpp"
#include "glyphs.hpp"
#include "cache.hpp"

namespace ppocr { namespace ocr2 {

inline void insert_views(
    ppocr::ocr2::view_ref_list & cache_element,
    ppocr::ocr2::Probabilities const & probabilities,
    ppocr::ocr2::Glyphs const & glyphs,
    double limit
) {
    for (auto & prob : probabilities) {
        if (!(prob.prob >= limit)) {
            break;
        }
        auto & views = glyphs[prob.i];
        cache_element.insert(cache_element.end(), views.begin(), views.end());
    }
}

} }

#endif
