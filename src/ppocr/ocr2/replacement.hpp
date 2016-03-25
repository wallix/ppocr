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

#ifndef PPOCR_CAPTURE_RDP_PPOCR_REPLACEMENT_HPP
#define REDEMPTION_CAPTURE_RDP_PPOCR_REPLACEMENT_HPP

#include <iosfwd>
#include <string>
#include <vector>

namespace ppocr { namespace ocr2 {

struct Replacement {
    std::string pattern;
    std::string replace;
};

struct Replacements : std::vector<Replacement>
{ using std::vector<Replacement>::vector; };

std::istream & operator >> (std::istream & is, Replacements & replacements);


inline void replace_words(std::string & result, Replacements const & replacements) {
    for (Replacement const & rep : replacements) {
        std::string::size_type pos = 0;
        while ((pos = result.find(rep.pattern, pos)) != std::string::npos) {
            result.replace(pos, rep.pattern.size(), rep.replace);
        }
    }
}

} }

#endif
