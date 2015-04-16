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
#include "image/image_from_file.hpp"
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

#include "math/almost_equal.hpp"

#include "sassert.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <map>
#include <set>

#include <cstring>
#include <cerrno>

using std::size_t;

//using string_ref_t = std::reference_wrapper<std::string const>;
using definition_ref_t = std::reference_wrapper<Definition const>;

struct Probability {
    definition_ref_t refdef;
    double prob;

    Probability(Definition const & def, double prob = 1)
    : refdef(def)
    , prob(prob)
    {}

    std::string const & c() const { return refdef.get().c; }
    DataLoader::data_base const & data(size_t i) const { return refdef.get().datas[i]; }
};

using probabilities_t = std::vector<Probability>;


unsigned get_value(DataLoader::data_base const & data) {
    return *reinterpret_cast<unsigned const *>(
        reinterpret_cast<unsigned char const *>(&data) + sizeof(DataLoader::data_base)
    );
}


probabilities_t reduce_univers(
    probabilities_t const & probabilities,
    size_t sig_idx, unsigned value, unsigned interval
) {
    auto d = interval/10u;

    auto approximate_get_value = [d, sig_idx, value, interval](Definition const & def) {
        unsigned const sig_value = get_value(def.datas[sig_idx]);
        return value < sig_value
            ? (sig_value <= value + d ? (interval - (sig_value - value)) * 100u / interval : 0u)
            : (value <= sig_value + d ? (interval - (value - sig_value)) * 100u / interval : 0u);
        //return value == sig_value  ? 100 : 0;
    };

    probabilities_t new_probabilities;

    for (auto const & prob : probabilities) {
        if (unsigned x = approximate_get_value(prob.refdef)) {
            new_probabilities.emplace_back(prob.refdef, prob.prob * x / 100);
        }
    }

    return new_probabilities;
}


double accu_probability(std::vector<Probability> const & probs) {
    return std::accumulate(
        probs.begin(), probs.end(), 0.,
        [](double n, Probability const & prob) { return n+prob.prob; }
        //[](double n, Probability const & prob) { return std::max(n, prob.prob); }
    );
}


double get_probability(std::vector<Probability> const & probs) {
    return accu_probability(probs) / probs.size();
}


void sort_and_show_infos(probabilities_t & probabilities) {
    if (probabilities.empty()) {
        std::cout << "\033[00;34mbest: 0 ( )\033[0m\n\n";
        return;
    }

    std::sort(
        probabilities.begin(), probabilities.end(),
        [](Probability const & a, Probability const & b)
        { return a.c() < b.c() || (a.c() == b.c() && a.prob > b.prob); }
    );

    probabilities.erase(std::unique(
        probabilities.begin(), probabilities.end(),
        [](Probability const & a, Probability const & b)
        { return a.c() == b.c(); }
    ), probabilities.end());

    std::sort(
        probabilities.begin(), probabilities.end(),
        [](Probability const & a, Probability const & b)
        { return a.prob > b.prob; }
    );

    auto first = probabilities.begin();
    std::cout << "\033[00;34mbest: " << first->prob << " ( \033[00;31m" << first->c();
    for (auto previous = first; ++first != probabilities.end() && !(first->prob < previous->prob); ++previous) {
        std::cout << " " << first->c();
    }
    std::cout << "\033[0m )\n\n";
    for (; first != probabilities.end(); ++first) {
        std::cout << "\033[00;31m" << first->c() << "\033[0m " << first->prob << "  ";
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

    std::cout << " ## definitions.size(): " << definitions.size() << "\n\n";

    Image img = image_from_file(av[2]);
    Bounds const bounds(img.width(), img.height());
    size_t x = 0;

    probabilities_t const probabilities(definitions.begin(), definitions.end());
    probabilities_t cp_probabilities;
    probabilities_t tmp_probabilities;


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
        {01, strategies::dzdensity::traits::get_interval()},
        {01, strategies::dzdensity90::traits::get_interval()},
    };

    std::string result1;
    std::string result2;

    probabilities_t result_probabilities;

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

        cp_probabilities = probabilities;
        for (size_t i = 0; i < sizeof(intervals)/sizeof(intervals[0]); ++i) {
            if (!intervals[i].enable) {
                continue;
            }
            cp_probabilities = reduce_univers(cp_probabilities, i, get_value(datas[i]), intervals[i].interval);
        }

        tmp_probabilities = cp_probabilities;
        sort_and_show_infos(tmp_probabilities);
        if (tmp_probabilities.empty()) {
            result1 += '_';
        }
        else {
            result1 += tmp_probabilities.front().c();
        }

        if (tmp_probabilities.empty()) {
            result2 += '_';
        }
        else if (tmp_probabilities.front().prob >= 1) {
            result2 += tmp_probabilities.front().c();
        }
        else {
            for (size_t i = sizeof(intervals)/sizeof(intervals[0]), e = i + 3; i < e; ++i) {
                result_probabilities.clear();
                for (auto const & prob : cp_probabilities) {
                    unsigned const x = prob.data(i).relationship(datas[i]);
                    if (x >= 50) {
                        result_probabilities.emplace_back(prob.refdef, prob.prob * x / 100);
                    }
                }
                swap(cp_probabilities, result_probabilities);
            }

            for (size_t i = sizeof(intervals)/sizeof(intervals[0]) + 3, e = i + 2; i < e; ++i) {
                result_probabilities.clear();
                for (auto const & prob : cp_probabilities) {
                    auto first = std::lower_bound(
                        definitions.begin(), definitions.end(), prob.c(),
                        [](Definition const & a, std::string const & s) { return a.c < s; }
                    );
                    for (; first != definitions.end() && first->c == prob.c(); ++first) {
                        if (prob.data(i).relationship(first->datas[i]) >= 50) {
                            result_probabilities.push_back(prob);
                            break;
                        }
                    }
                }
                swap(cp_probabilities, result_probabilities);
            }

            sort_and_show_infos(cp_probabilities);
            if (cp_probabilities.empty()) {
                result2 += '_';
            }
            else {
                result2 += cp_probabilities.front().c();
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
