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

#include "factory/data_loader.hpp"
#include "factory/definition.hpp"
#include "factory/registry.hpp"

#include "image/image.hpp"

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
#include "strategies/dzdensity.hpp"

#include "utils/read_definitions_file_and_normalize.hpp"

#include <iostream>
#include <array>
#include <iomanip>
#include <fstream>
// #include <string>
#include <algorithm>
// #include <set>
// #include <map>

#include <cstring>
#include <cerrno>

using namespace ppocr;

unsigned get_value(DataLoader::data_base const & data) {
    return *reinterpret_cast<unsigned const *>(
        reinterpret_cast<unsigned char const *>(&data) + sizeof(DataLoader::data_base)
    );
}

int main(int ac, char **av)
{
    if (ac < 2) {
        std::cerr << av[0] << " datas images\n";
        return 1;
    }

    DataLoader loader;
    all_registy(loader);

    std::vector<Definition> const definitions = read_definitions_file_and_normalize(av[1], loader, &std::cout);

    unsigned const intervals[] = {
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
        strategies::dzdensity::traits::get_interval(),
        strategies::dzdensity90::traits::get_interval(),
    };
    constexpr size_t count_interval = sizeof(intervals) / sizeof(intervals[0]);

    constexpr char const * colors[] {
        "\033[38;5;226m",
        "\033[38;5;190m",
        "\033[38;5;154m",
        "\033[38;5;112m",
        "\033[38;5;76m",
        "\033[38;5;70m",
        "\033[38;5;106m",
        "\033[38;5;136m",
        "\033[38;5;130m",
        "\033[38;5;124m",
    };
    constexpr size_t nb_colors = sizeof(colors) / sizeof(colors[0]);
    constexpr char const * best_color = "\033[38;5;220m";
    constexpr char const * fail_color = "\033[38;5;124m";
    constexpr char const * reset_color = "\033[0m";

    std::array<
        std::array<
            std::vector<
                std::reference_wrapper<Definition const>
            >,
            10
        >,
        count_interval
    > arr;

    std::ios::sync_with_stdio(false);

    auto print_value = [&colors](size_t sz, size_t n) {
        std::cout
            << (sz <= n ? fail_color :
                0 == n ? best_color
                : colors[n * nb_colors / sz]
            )
            << std::setw(6) << n << reset_color
        ;
    };
    auto print_name = [&loader](size_t i) {
        std::cout << std::setw(15) << loader.names()[i] << std::setw(3) << i;
    };

    size_t i = 0;
    for (auto & a : arr) {
        for (Definition const & def : definitions) {
            a[get_value(def.datas[i]) * a.size() / (intervals[i] + 1)].emplace_back(def);
        }
        print_name(i);
        for (auto & vec : a) {
            print_value(definitions.size() / 2, vec.size());
        }
        std::cout << "\n";
        ++i;
    }
    std::cout << "\n";

    i = 0;
    for (auto & a : arr) {
        for (auto & v : a) {
            v.clear();
        }
        unsigned const d = intervals[i]/10u;
        for (Definition const & def : definitions) {
            auto const value = get_value(def.datas[i]);
            auto min = (value > d ? (value - d) * a.size() / (intervals[i] + 1) : 0);
            auto max = std::min(value + d, intervals[i]) * a.size() / (intervals[i] + 1);
            for (; min <= max; ++min) {
                a[min].emplace_back(def);
            }
        }
        print_name(i);
        for (auto & vec : a) {
            print_value(definitions.size() - definitions.size()/4, vec.size());
        }
        std::cout << "\n";
        ++i;
    }

    std::cout << "\n---------\n\n";

    using idx_for_value = std::pair<unsigned, unsigned>;
    std::array<idx_for_value, count_interval> column_values;

    for (size_t i = 0; i < arr.size(); ++i) {
        size_t x = 0;
        std::cerr << "{{\n";
        for (auto & vec : arr[i]) {
            std::cout << std::setw(2) << i << " " << loader.names()[i];
            std::cout << "\n" << std::setw(15 + 3 + x*6 + 6) << vec.size() << "\n";
            auto it_val = column_values.begin();
            for (size_t ii = 0; ii < arr.size(); ++ii) {
                print_name(ii);
                size_t xx = 0;
                for (auto & vec2 : arr[ii]) {
                    unsigned n = 0;
                    if (vec.empty() || vec2.empty()) {
                        std::cout << best_color << std::setw(6) << "0" << reset_color;
                    }
                    else {
                        struct Counter : std::iterator<std::output_iterator_tag, unsigned>{
                            unsigned & i;
                            Counter(unsigned & n) : i(n) {}
                            Counter & operator ++ () { return *this; }
                            Counter & operator * () { return *this; }
                            Counter & operator = (Definition const &) { ++i; return *this; }
                        };
                        std::set_difference(
                            vec.begin(), vec.end(), vec2.begin(), vec2.end(), Counter{n},
                            [](Definition const & a, Definition const & b) { return &a < &b; }
                        );
                        n = vec.size() - n;
                        print_value(vec.size(), n);
                    }
                    if (xx == x) {
                        *it_val++ = {ii, n};
                    }
                    ++xx;
                }
                std::cout << "\n";
            }
            std::sort(
                column_values.begin(), column_values.end(),
                [](idx_for_value const & p1, idx_for_value const & p2) {
                    return p1.second < p2.second;
                }
            );
            std::cout << "\n";
            std::cerr << "    {";
            for (auto & p : column_values) {
                std::cerr << p.first << ", ";
            }
            std::cerr << "}, \n";
            std::cout << "\n";
            ++x;
        }
        std::cerr << "}},";
        std::cout << "\n";
    }

    //for (auto s : colors) {
    //    std::cout << s << "plop" << reset_color << "\n";
    //}
    //std::cout << best_color << "plop" << reset_color << "\n";
    //std::cout << fail_color << "plop" << reset_color << "\n";
}
