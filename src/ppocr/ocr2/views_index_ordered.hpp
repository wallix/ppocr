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

#ifndef REDEMPTION_PPOCR_SRC_OCR2_VIEWS_INDEX_ORDERED_HPP
#define REDEMPTION_PPOCR_SRC_OCR2_VIEWS_INDEX_ORDERED_HPP

#include "glyphs.hpp"

#include <vector>
#include <functional>

namespace ppocr { namespace ocr2 {

inline std::vector<unsigned> get_views_indexes_ordered(Glyphs const & glyphs) {
    std::vector<unsigned> id_views;
    if (!glyphs.empty()) {
        std::vector<std::reference_wrapper<Views const>> glyph_refs(glyphs.begin(), glyphs.end());
        std::sort(glyph_refs.begin(), glyph_refs.end(), LtViews{});

        auto first = glyph_refs.begin() + 1;
        auto last = glyph_refs.end();
        // re-assign duplication
        for (; first != last; ++first) {
            if (EqViews{}(first->get(), (first-1)->get())) {
                *first = *(first-1);
            }
        }

        id_views.resize(glyphs.size());
        auto it = id_views.begin();
        for (Views const & g : glyph_refs) {
            *it = static_cast<unsigned>(&g - &glyphs[0]);
            ++it;
        }
    }
    return id_views;
}

} }

#endif
