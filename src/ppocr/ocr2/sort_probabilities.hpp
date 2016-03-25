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

#ifndef PPOCR_SRC_OCR2_SORT_PROBABILITIES_HPP
#define PPOCR_SRC_OCR2_SORT_PROBABILITIES_HPP

#include "probabilities.hpp"

#include <algorithm>


namespace ppocr { namespace ocr2 {

struct EqProbByViews {
    std::vector<unsigned> const & id_views;
    bool operator()(ppocr::ocr2::Probability const & a, ppocr::ocr2::Probability const & b) const {
        return id_views[a.i] == id_views[b.i];
    }
};

struct LtProbByViews {
    std::vector<unsigned> const & id_views;
    bool operator()(ppocr::ocr2::Probability const & a, ppocr::ocr2::Probability const & b) const {
        if (id_views[a.i] == id_views[b.i]) {
            return a.prob > b.prob;
        }
        return id_views[a.i] < id_views[b.i];
    }
};

void unique_copy_by_views(
    ppocr::ocr2::Probabilities & out,
    ppocr::ocr2::Probabilities const & probabilities,
    std::vector<unsigned> const & id_views
) {
    out.resize(std::unique_copy(
        probabilities.begin(),
        probabilities.end(),
        out.begin(),
        ppocr::ocr2::EqProbByViews{id_views}
    ) - out.begin());
}

void unique_by_views(ppocr::ocr2::Probabilities & probabilities, std::vector<unsigned> const & id_views
) {
    probabilities.resize(
        std::unique(probabilities.begin(), probabilities.end(), EqProbByViews{id_views})
      - probabilities.begin()
    );
}

void sort_by_views(ppocr::ocr2::Probabilities & probabilities, std::vector<unsigned> const & id_views) {
    std::sort(probabilities.begin(), probabilities.end(), LtProbByViews{id_views});
}

void sort_by_prop(ppocr::ocr2::Probabilities & probabilities) {
    std::sort(probabilities.begin(), probabilities.end(), ppocr::ocr2::GtProb{});
}

} }

#endif
