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

#include "image/image.hpp"
#include "factory/data_loader.hpp"
#include "box_char/make_box_character.hpp"
#include "factory/registry.hpp"
#include "factory/definition.hpp"
#include "box_char/box.hpp"
#include "filters/line.hpp"

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

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <map>

#include <cstring>
#include <cerrno>
#include <cassert>

using std::size_t;

using string_ref_t = std::reference_wrapper<std::string const>;
using definition_ref_t = std::reference_wrapper<Definition const>;

struct Probability {
    definition_ref_t refdef;
    double prop;

    Probability(Definition const & def, double prop)
    : refdef(def)
    , prop(prop)
    {}
};

using probability_by_characters_t = std::map<string_ref_t, std::vector<Probability>, std::less<std::string>>;


unsigned get_value(DataLoader::data_base const & data) {
    return *reinterpret_cast<unsigned const *>(
        reinterpret_cast<unsigned char const *>(&data) + sizeof(DataLoader::data_base)
    );
}


probability_by_characters_t reduce_univers(
    probability_by_characters_t const & probability_by_characters,
    size_t sig_idx, unsigned value, unsigned interval
) {
    auto d = interval/10;

    auto approximate_get_value = [d, sig_idx, value, interval](Definition const & def) {
        unsigned const sig_value = get_value(def.datas[sig_idx]);
        return value < sig_value
            ? (sig_value <= value + d ? (interval - (sig_value - value)) * 100 / interval : 0u)
            : (value <= sig_value + d ? (interval - (value - sig_value)) * 100 / interval : 0u);
    };

    probability_by_characters_t new_probability_by_characters;

    for (auto const & sref_prob : probability_by_characters) {
        // P(x and Sn=value)
        for (Probability const & prop : sref_prob.second) {
            if (unsigned x = approximate_get_value(prop.refdef)) {
                auto & props = new_probability_by_characters[sref_prob.first];
                props.emplace_back(prop.refdef, prop.prop + x);
            }
        }
    }

    return new_probability_by_characters;
}


int main(int ac, char **av)
{
    if (ac < 2) {
        std::cerr << av[0] << " datas images\n";
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

    Image img = image_from_file(av[2]);
    Bounds const bounds(img.width(), img.height());
    size_t x = 0;

    probability_by_characters_t probability_by_characters;
    for (auto const & def : definitions) {
        probability_by_characters[def.c].emplace_back(def, 0u);
    }


    struct { bool enable; unsigned interval; } const intervals[] = {
        {01, strategies::hdirection::traits::get_interval()},
        {01, strategies::hdirection90::traits::get_interval()},
        {01, strategies::hdirection2::traits::get_interval()},
        {01, strategies::hdirection290::traits::get_interval()},
        {01, strategies::hgravity::traits::get_interval()},
        {01, strategies::hgravity90::traits::get_interval()},
        {01, strategies::hgravity2::traits::get_interval()},
        {01, strategies::hgravity290::traits::get_interval()},
        {01, strategies::proportionality::traits::get_interval()},
        {01, strategies::dvdirection::traits::get_interval()},
        {01, strategies::dvdirection90::traits::get_interval()},
        {01, strategies::dvdirection2::traits::get_interval()},
        {01, strategies::dvdirection290::traits::get_interval()},
        {01, strategies::dvgravity::traits::get_interval()},
        {01, strategies::dvgravity90::traits::get_interval()},
        {01, strategies::dvgravity2::traits::get_interval()},
        {01, strategies::dvgravity290::traits::get_interval()},
        {01, strategies::density::traits::get_interval()},
    };

    using resolution_clock = std::chrono::high_resolution_clock;
    auto t1 = resolution_clock::now();
    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //min_y = std::min(cbox.y(), min_y);
        //bounds_y = std::max(cbox.y() + cbox.h(), bounds_y);
        //bounds_x = cbox.x() + cbox.w();
        //std::cerr << "\nbox(" << cbox << ")\n";

        Image const img_word = img.section(cbox.index(), cbox.bounds());
        std::cerr << img_word;

        auto data = loader.new_datas(img_word, img_word.rotate90());

        auto new_probability_by_characters = probability_by_characters;
        for (size_t i = 0; i < sizeof(intervals)/sizeof(intervals[0]); ++i) {
            if (!intervals[i].enable) {
                continue;
            }
            new_probability_by_characters = reduce_univers(
                new_probability_by_characters, i, get_value(data[i]), intervals[i].interval
            );
        }
        for (auto & p : new_probability_by_characters) {
            std::cout
                << p.first.get() << " ["
                << std::accumulate(
                    p.second.begin(), p.second.end(), 0u,
                    [](unsigned n, Probability const & prop) { return n+prop.prop; }
                )
                << "]  "
            ;
        }
        std::cout << "\n\n";

        x = cbox.x() + cbox.w();
    }

    {
        auto t2 = resolution_clock::now();
        std::cout << std::chrono::duration<double>(t2-t1).count() << "s\n";
    }
}
