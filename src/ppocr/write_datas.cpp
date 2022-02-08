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

#include "ppocr/defined_loader.hpp"
#include "ppocr/loader2/image_io.hpp"
#include "ppocr/image/image.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>

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
