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
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#include "defined_loader.hpp"
#include "loader2/image_io.hpp"
#include "image/image.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <cstring>
#include <cerrno>

using namespace ppocr;

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cerr << av[0] << "glyph_file\n";
        return 1;
    }

    PpOcrDatas datas;
    {
        std::ifstream file(av[1]);
        if (!file) {
            std::cerr << strerror(errno) << '\n';
            return 2;
        }

        Image img;
        unsigned n;
        while (loader2::read_img(file, img) >> n) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            while (n--) {
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            datas.load(img);
        }

        if (!file.eof()) {
            std::cout << '[' << file.rdbuf() << "]\n";
            std::cerr << av[1] << ": read error\n";
            return 5;
        }
    }

    std::cout << datas;
}
