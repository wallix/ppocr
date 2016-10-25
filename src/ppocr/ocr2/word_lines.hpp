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

#ifndef PPOCR_CAPTURE_RDP_PPOCR_WORD_LINES_HPP
#define PPOCR_CAPTURE_RDP_PPOCR_WORD_LINES_HPP

#include <iosfwd>
#include <string>
#include <vector>

namespace ppocr { namespace ocr2 {

struct WordLines {
    enum {
        Unspecified = 0,
        Upper = 1,
        Below = 2,
        Above = 4
    };
    unsigned short ascentline:3;
    unsigned short capline:3;
    unsigned short meanline:3;
    unsigned short baseline:3;
};


struct WWordLines
{
    std::string s;
    WordLines wlines;
};

struct WWordsLines : std::vector<WWordLines>
{ using std::vector<WWordLines>::vector; };

std::istream & operator>>(std::istream & is, WWordsLines & wwords_lines);
std::istream & operator>>(std::istream & is, WordLines & wlines);

} }

#endif
