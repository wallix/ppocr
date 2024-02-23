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
#include "ppocr/factory/data_loader.hpp"
#include "ppocr/box_char/make_box_character.hpp"
#include "ppocr/factory/registry.hpp"
#include "ppocr/factory/definition.hpp"
#include "ppocr/box_char/box.hpp"
#include "ppocr/strategies/utils/basic_proportionality.hpp"
#include "ppocr/strategies/hdirection.hpp"
#include "ppocr/strategies/hdirection2.hpp"
#include "ppocr/strategies/hgravity.hpp"
#include "ppocr/strategies/hgravity2.hpp"
#include "ppocr/strategies/proportionality.hpp"
#include "ppocr/strategies/dvdirection.hpp"
#include "ppocr/strategies/dvdirection2.hpp"
#include "ppocr/strategies/dvgravity.hpp"
#include "ppocr/strategies/dvgravity2.hpp"
#include "ppocr/strategies/density.hpp"
// #include "utils/unique_sort_definition.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <chrono>

#include <cstring>
#include <cerrno>
#include <cassert>

using namespace ppocr;

using std::size_t;

int main(int ac, char **av)
{
    if (ac < 1) {
        //std::cerr << av[0] << " datas images [-i]\n";
        std::cerr << av[0] << " datas\n";
        return 1;
    }

    std::ifstream file(av[1]);
    if (!file) {
        std::cerr << strerror(errno) << '\n';
        return 2;
    }

    DataLoader loader;
    all_registy(loader);

    std::vector<Definition> definitions = read_definitions(file, loader);

    if (!file.eof()) {
        std::cerr << "read error\n";
        return 5;
    }

    std::map<std::string, std::vector<std::reference_wrapper<DataLoader::Datas>>> m;
    for (Definition & def : definitions) {
        m[def.c].push_back(def.datas);
    }

    struct Line { std::string const * s; std::array<char, 101> l; };
    std::vector<Line> grid;
    Line tmpline;
    unsigned coef[] = {
        strategies::hdirection::traits::get_interval(),
        strategies::hdirection90::traits::get_interval(),
        strategies::hdirection2::traits::get_interval(),
        strategies::hdirection290::traits::get_interval(),
        strategies::hgravity::traits::get_interval(),
        strategies::hgravity90::traits::get_interval(),
        strategies::hgravity2::traits::get_interval(),
        strategies::hgravity290::traits::get_interval(),
        strategies::proportionality::traits::get_interval(),
        strategies::dvdirection::traits::get_interval(),
        strategies::dvdirection90::traits::get_interval(),
        strategies::dvdirection2::traits::get_interval(),
        strategies::dvdirection290::traits::get_interval(),
        strategies::dvgravity::traits::get_interval(),
        strategies::dvgravity90::traits::get_interval(),
        strategies::dvgravity2::traits::get_interval(),
        strategies::dvgravity290::traits::get_interval(),
        strategies::density::traits::get_interval(),
    };
    for (size_t idata = 0; idata < sizeof(coef)/sizeof(coef[0]); ++idata) {
        grid.clear();
        for (auto & p : m) {
//             if (p.first.size() != 1) {
//                 continue;
//             }
            tmpline.s = &p.first;
            tmpline.l.fill('-');
            const auto divide = coef[idata]/100;
            assert(divide*100 == coef[idata]);
            for (size_t i = 0; i <= 100; ++i) {
                if (size_t const count = std::count_if(
                    p.second.begin(), p.second.end(),
                    [i, idata, divide] (DataLoader::Datas const & datas) {
                        using Data = DataLoader::data_base;
                        using Base = strategies::proportionality_base const;
                        // BEGIN Big hack
                        auto value = reinterpret_cast<Base *>(
                            reinterpret_cast<unsigned char const *>(&datas[idata]) + sizeof(Data)
                        )->value();
                        // END
                        return value / divide == i;
                    }
                )) {
                    tmpline.l[i] = char(count + '0');
                }
            }
            grid.push_back(tmpline);
        }

        std::sort(grid.begin(), grid.end(), [](Line const & a, Line const & b) { return a.l < b.l; });
        unsigned const min = std::find_if(
            grid.back().l.begin(), grid.back().l.end(),
            [](char c) { return c != '-'; }
        ) - grid.back().l.begin();
        unsigned max = 0;
        for (Line & line : grid) {
            max = std::max(max, unsigned(line.l.rend() - std::find_if(
                line.l.rbegin(), line.l.rend(),
                [](char c) { return c != '-'; }
            ) - 1));
        }
        std::cout << loader.names()[idata] << "  [ " << min << " - " << max << "]\n";
        for (Line & line : grid) {
            std::cout.write(line.l.data(), line.l.size());
            std::cout << "  " << *line.s << "\n";
        }
    }
}
