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

#ifndef REDEMPTION_CAPTURE_RDP_PPOCR_CACHE_HPP
#define REDEMPTION_CAPTURE_RDP_PPOCR_CACHE_HPP

#include "ppocr/ocr2/glyphs.hpp"

#include "ppocr/image/image.hpp"
#include "ppocr/utils/image_compare.hpp"

#include <map>
#include <vector>
#include <functional>

namespace ppocr { namespace ocr2 {

using view_ref = std::reference_wrapper<View const>;
using view_ref_list = std::vector<view_ref>;

struct def_img_compare {
    def_img_compare() {}
    bool operator()(ppocr::Image const & a, ppocr::Image const & b) const
    { return ppocr::image_compare(a, b) < 0; }
};
using image_cache_type_t = std::map<ppocr::Image, view_ref_list, def_img_compare>;

} }

#endif
