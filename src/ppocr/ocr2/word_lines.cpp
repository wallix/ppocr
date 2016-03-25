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

#include "ppocr/word_lines.hpp"

#include "ppocr/../utils/read_file.hpp"

#include <istream>

std::istream& ppocr::ocr2::operator>>(std::istream& is, WordLines & wlines) {
    unsigned ascentline;
    unsigned capline;
    unsigned meanline;
    unsigned baseline;

    if (is >> ascentline >> capline >> meanline >> baseline) {
        wlines.ascentline = ascentline;
        wlines.capline = capline;
        wlines.meanline = meanline;
        wlines.baseline = baseline;
    }
    return is;
}

std::istream& ppocr::ocr2::operator>>(std::istream& is, WWordsLines& wwords_lines)
{
    WWordLines wwl;
    while (is >> wwl.s >> wwl.wlines) {
        wwords_lines.push_back(wwl);
    }
    return is;
}
