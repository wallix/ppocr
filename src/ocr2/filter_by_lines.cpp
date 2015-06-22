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

#include "filter_by_lines.hpp"
#include "words_infos.hpp"

#include "../filters/best_baseline.hpp"

#include <cassert>


void ppocr::ocr2::filter_by_lines(
    ambiguous_t & ambiguous,
    WordsInfos const & words_infos,
    std::vector<ppocr::Box> const & boxes
) {
    using iterator_base = std::vector<Box>::const_iterator;
    struct iterator_baseline : iterator_base
    {
        using value_type = std::size_t;

        iterator_baseline(iterator_base base)
        : iterator_base(base)
        {}

        std::size_t operator*() const {
            return iterator_base::operator*().bottom();
        }
    };

    auto const baseline = filters::best_baseline(
        iterator_baseline(boxes.begin()),
        iterator_baseline(boxes.end())
    );

    std::map<size_t, size_t> meanline_map;

    //for (rdp_ppocr::view_ref_list & vec : ambiguous) {
    //    for (ppocr::ppocr::loader2::View const & view : vec) {
    //        std::cout << view.word << "(" << view.word.size() << ") ";
    //    }
    //    std::cout << "----\n";
    //}
    //std::cout << "#######\n";

    auto it = boxes.cbegin();
    assert(boxes.size() == ambiguous.size());
    for (ppocr::ocr2::view_ref_list & vec : ambiguous) {
        if (vec.size() >= 2) {
            vec.erase(std::remove_if(
                vec.begin(),
                vec.end(),
                [&](View const & view) -> bool {
                    if (auto p = words_infos.get(view.word)) {
                        switch (p->lines.baseline) {
                            case WordLines::Upper:
                                return it->bottom() + 1 >= baseline;
                            case WordLines::Below:
                                return (it->bottom() < baseline ? baseline - it->bottom() : it->bottom() - baseline) > 1u;
                            case WordLines::Above:
                                return it->bottom() <= baseline + 1;
                            case WordLines::Upper | WordLines::Below:
                                return it->bottom() + 1 > baseline;
                            case WordLines::Below | WordLines::Above:
                                return it->bottom() < baseline + 1;
                        }
                    }
                    return false;
                }
            ), vec.end());
        }

        if (vec.size() == 1) {
            if (auto p = words_infos.get(vec[0].get().word)) {
                auto const & lines = p->lines;
                if (lines.baseline == WordLines::Below && lines.meanline == WordLines::Below) {
                    ++meanline_map[it->top()];
                }
            }
        }

        ++it;
    }

    if (!meanline_map.empty()) {
        using cP = decltype(meanline_map)::value_type const;
        auto meanline = std::max_element(meanline_map.begin(), meanline_map.end(), [](cP & a, cP & b) {
            return a.second < b.second;
        })->first;

        it = boxes.cbegin();
        for (view_ref_list & vec : ambiguous) {
            if (vec.size() >= 2) {
                vec.erase(std::remove_if(
                    vec.begin(),
                    vec.end(),
                    [&](View const & view) -> bool {
                        if (auto p = words_infos.get(view.word)) {
                            switch (p->lines.meanline) {
                                case WordLines::Upper:
                                    return it->top() + 1 >= meanline;
                                case WordLines::Below:
                                    return (it->top() < meanline ? meanline - it->top() : it->top() - meanline) > 1u;
                                case WordLines::Above:
                                    return it->top() <= meanline + 1;
                                case WordLines::Upper | WordLines::Below:
                                    return it->top() + 1 > meanline;
                                case WordLines::Below | WordLines::Above:
                                    return it->top() < meanline + 1;
                            }
                        }
                        return false;
                    }
                ), vec.end());
            }
            ++it;
        }
    }
}
