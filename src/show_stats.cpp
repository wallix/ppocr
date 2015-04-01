#include "image/image.hpp"
#include "factory/data_loader.hpp"
#include "box_char/make_box_character.hpp"
#include "factory/registry.hpp"
#include "factory/definition.hpp"
#include "box_char/box.hpp"
#include "strategies/utils/basic_proportionality.hpp"
#include "strategies/hdirection.hpp"
#include "strategies/hdirection2.hpp"
#include "strategies/hgravity.hpp"
#include "strategies/hgravity2.hpp"
#include "strategies/proportionality.hpp"
#include "strategies/dvdirection.hpp"
#include "strategies/dvdirection2.hpp"
#include "strategies/dvgravity.hpp"
#include "strategies/dvgravity2.hpp"
#include "strategies/density.hpp"
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
    Line line;
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
    for (size_t idata = 1; idata < sizeof(coef)/sizeof(coef[0]) + 1; ++idata) {
        grid.clear();
        for (auto & p : m) {
//             if (p.first.size() != 1) {
//                 continue;
//             }
            line.s = &p.first;
            line.l.fill('-');
            const auto divide = coef[idata-1]/100;
            assert(divide*100 == coef[idata-1]);
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
                    line.l[i] = char(count + '0');
                }
            }
            grid.push_back(line);
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
            )));
        }
        std::cout << loader.names()[idata] << "  [ " << min << " - " << max << "]\n";
        for (Line & line : grid) {
            std::cout.write(line.l.data(), line.l.size());
            std::cout << "  " << *line.s << "\n";
        }
    }
}
