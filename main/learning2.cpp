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

#include "ppocr/image/image.hpp"
#include "ppocr/image/image_from_file.hpp"
#include "ppocr/factory/data_loader.hpp"
#include "ppocr/box_char/make_box_character.hpp"
#include "ppocr/factory/registry.hpp"
#include "ppocr/factory/definition.hpp"

#include <iostream>
#include <fstream>

#include <cstring>
#include <cerrno>
#include <cassert>
#include <stdexcept>

using namespace ppocr;

static void load_file(
    std::vector<Definition> & definitions, DataLoader const & loader,
    const char * imagefilename, const char * textfilename, const char * fontname,
    unsigned luminance
) {
    std::ifstream file(textfilename);
    if (!file) {
        throw std::runtime_error(strerror(errno));
    }

    Image img = image_from_file(imagefilename, luminance);
    //std::cerr << img << '\n';

    size_t x = 0;
    Bounds const & bounds = img.bounds();

    std::string s;

    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //std::cerr << "\nbox(" << cbox << ")\n";

        if (!(file >> s)) {
            throw std::runtime_error("definition not found");
        }

        auto newimg = img.section(cbox.index(), cbox.bounds());
        //std::cerr << newimg << s << '\n';

        definitions.emplace_back(s, fontname, std::move(newimg), loader);
        assert(definitions.back().datas == definitions.back().datas);

        x = cbox.x() + cbox.w();
    }

    if (file >> s) {
        throw std::runtime_error("box character not found");
    }
}

int main(int ac, char **av)
{
    int i_ac = 0;
    unsigned luminance = 128;
    if (ac > 3) {
        if (av[1][0] == '-' && av[1][1] == 'l' && av[1][2] == '\0') {
            char * p;
            luminance = std::strtoul(av[2], &p, 10);
            if (p == av[2] || *p != '\0' || errno) {
                return 2;
            }
            i_ac += 2;
        }
    }
    if ((ac - i_ac) < 4 || (ac - i_ac - 1) % 3) {
        std::cerr << av[0] << " [-l luminance] image_file text_file font_name [image_file text_file font_name ...]\n";
        return 1;
    }

    DataLoader loader;
    all_registy(loader);

    std::vector<Definition> definitions;

    for (++i_ac; i_ac < ac; i_ac += 3) {
        load_file(definitions, loader, av[i_ac], av[i_ac+1], av[i_ac+2], luminance);
    }

    for (auto & def : definitions) {
        write_definition(std::cout, def, loader);
    }
}
