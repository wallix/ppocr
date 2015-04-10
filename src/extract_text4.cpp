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
#include "strategies/dzdensity.hpp"

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
    double prob;

    Probability(Definition const & def, double prob)
    : refdef(def)
    , prob(prob)
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
        for (Probability const & prob : sref_prob.second) {
            if (unsigned x = approximate_get_value(prob.refdef)) {
                auto & probs = new_probability_by_characters[sref_prob.first];
                probs.emplace_back(prob.refdef, prob.prob * x / 100);
            }
        }
    }

    return new_probability_by_characters;
}


double get_probability(std::vector<Probability> const & probs) {
    return std::accumulate(
        probs.begin(), probs.end(), 0.,
        [](double n, Probability const & prob) { return n+prob.prob; }
        //[](double n, Probability const & prob) { return std::max(n, prob.prob); }
    ) / probs.size();
}


struct Info { std::string const * s_p; unsigned percent; size_t sz; };

void init_and_show_infos(
    std::vector<Info> & infos,
    probability_by_characters_t const & probability_by_characters
) {
    infos.resize(probability_by_characters.size());
    {
        auto it = infos.begin();
        for (auto & p : probability_by_characters) {
            it->s_p = &p.first.get();
            it->percent = std::round(get_probability(p.second) * 100);
            it->sz = p.second.size();
            ++it;
        }
    }
    std::sort(
        infos.begin(), infos.end(),
        [](Info const & a, Info const & b) { return a.percent > b.percent; }
    );

    for (auto & info : infos) {
        std::cout
            << "\033[00;31m" << *info.s_p << "\033[0m [\033[00;32m"
            << std::setw(3) << info.percent
            << "\033[0m/" << info.sz << "]  "
        ;
    }
    std::cout << "\n\n";
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

    probability_by_characters_t const probability_by_characters = [&definitions]() {
        probability_by_characters_t ret;
        for (auto const & def : definitions) {
            ret[def.c].emplace_back(def, 1.);
        }
        return ret;
    }();


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
        {0/*1*/, strategies::dvdirection2::traits::get_interval()},
        {01, strategies::dvdirection290::traits::get_interval()},
        {01, strategies::dvgravity::traits::get_interval()},
        {01, strategies::dvgravity90::traits::get_interval()},
        {0/*1*/, strategies::dvgravity2::traits::get_interval()},
        {01, strategies::dvgravity290::traits::get_interval()},
        {01, strategies::density::traits::get_interval()},
        {01, strategies::dzdensity::traits::get_interval()},
        {01, strategies::dzdensity90::traits::get_interval()},
    };

    std::string result1;
    std::string result2;

    using resolution_clock = std::chrono::high_resolution_clock;
    auto t1 = resolution_clock::now();
    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //min_y = std::min(cbox.y(), min_y);
        //bounds_y = std::max(cbox.y() + cbox.h(), bounds_y);
        //bounds_x = cbox.x() + cbox.w();
        //std::cerr << "\nbox(" << cbox << ")\n";

        Image const img_word = img.section(cbox.index(), cbox.bounds());
        std::cerr << img_word;

        auto datas = loader.new_datas(img_word, img_word.rotate90());

        auto new_probability_by_characters = probability_by_characters;
        for (size_t i = 0; i < sizeof(intervals)/sizeof(intervals[0]); ++i) {
            if (!intervals[i].enable) {
                continue;
            }
            new_probability_by_characters = reduce_univers(
                new_probability_by_characters, i, get_value(datas[i]), intervals[i].interval
            );
        }

        std::vector<Info> infos;
        init_and_show_infos(infos, new_probability_by_characters);
        if (new_probability_by_characters.empty()) {
            result1 += '_';
        }
        else {
            result1 += *infos.front().s_p;
        }

        if (new_probability_by_characters.empty()) {
            result2 += '_';
        }
        else if (infos.front().percent >= 95) {
            result2 += *infos.front().s_p;
        }
        else {
            {
                probability_by_characters_t result_probability_by_characters;
                for (size_t i = sizeof(intervals)/sizeof(intervals[0]), e = i + 3/*5*/; i < e; ++i) {
                    result_probability_by_characters.clear();
                    for (auto const & sref_prob : new_probability_by_characters) {
                        // P(x and Sn=value)
                        for (Probability const & prob : sref_prob.second) {
                            unsigned const x = prob.refdef.get().datas[i].relationship(datas[i]);
                            if (x >= 50) {
                                auto & probs = result_probability_by_characters[sref_prob.first];
                                probs.emplace_back(prob.refdef, prob.prob * x / 100);
                            }
                        }
                    }
                    swap(new_probability_by_characters, result_probability_by_characters);
                }

                for (size_t i = sizeof(intervals)/sizeof(intervals[0]) + 3, e = i + 2; i < e; ++i) {
                    result_probability_by_characters.clear();
                    for (auto const & sref_prob : new_probability_by_characters) {
                        // P(x and Sn=value)
                        if ([&]() -> bool {
                            for (Probability const & prob : probability_by_characters.find(sref_prob.first)->second) {
                                if (prob.refdef.get().datas[i].relationship(datas[i]) >= 50) {
                                    return true;
                                }
                            }
                            return false;
                        }()) {
                            result_probability_by_characters[sref_prob.first] = sref_prob.second;
                        }
                    }
                    swap(new_probability_by_characters, result_probability_by_characters);
                }
            }

            init_and_show_infos(infos, new_probability_by_characters);

            if (!new_probability_by_characters.empty()) {
                using pair_type = probability_by_characters_t::value_type;
                result2 += std::max_element(
                    new_probability_by_characters.begin(), new_probability_by_characters.end(),
                    [](pair_type const & a, pair_type const & b) {
                        return get_probability(a.second) < get_probability(b.second);
                    }
                )->first.get();
            }
            else {
                result2 += '_';
            }
        }

        result1 += ' ';
        result2 += ' ';

        x = cbox.x() + cbox.w();
    }

    {
        auto t2 = resolution_clock::now();
        std::cout << std::chrono::duration<double>(t2-t1).count() << "s\n";
    }

    std::cout << " ## result1: " << (result1) << "\n ## result2: " << (result2) << std::endl;
}
