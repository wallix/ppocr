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

struct Probabilities : std::vector<Probability>
{
    using std::vector<Probability>::vector;
    double universe;
};

using probability_by_characters_t = std::map<string_ref_t, Probabilities, std::less<std::string>>;


unsigned get_value(DataLoader::data_base const & data) {
    return *reinterpret_cast<unsigned const *>(
        reinterpret_cast<unsigned char const *>(&data) + sizeof(DataLoader::data_base)
    );
}


probability_by_characters_t reduce_univers(
    probability_by_characters_t const & probability_by_characters,
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

    probability_by_characters_t new_probability_by_characters;

    for (auto const & sref_prob : probability_by_characters) {
        size_t weight = 0; // P(x and Sn=value)
        size_t total = 100 * sref_prob.second.size();
        for (Probability const & prob : sref_prob.second) {
            weight += approximate_get_value(prob.refdef);
        }
        for (Probability const & prob : sref_prob.second) {
            if (unsigned x = approximate_get_value(prob.refdef)) {
                auto & probs = new_probability_by_characters[sref_prob.first];
                probs.emplace_back(prob.refdef, prob.prob * x / 100/* * weight / total*/);
            }
        }
        auto it = new_probability_by_characters.find(sref_prob.first);
        if (it != new_probability_by_characters.end()) {
            assert(total >= weight);
            // P($n(-1) and Sn)
            it->second.universe = sref_prob.second.universe * weight / total;
        }
    }

//     size_t total = 0;
//     size_t weight = 0; // P(x and Sn=value)
//
//     for (auto const & sref_prob : probability_by_characters) {
//         total += /*d*/100 * sref_prob.second.size();
//         for (Probability const & prob : sref_prob.second) {
//             weight += approximate_get_value(prob.refdef);
//         }
//     }
//
//     for (auto const & sref_prob : probability_by_characters) {
//         for (Probability const & prob : sref_prob.second) {
//             if (unsigned x = approximate_get_value(prob.refdef)) {
//                 auto & probs = new_probability_by_characters[sref_prob.first];
//                 probs.emplace_back(prob.refdef, prob.prob * x / 100 * weight / total);
//             }
//         }
//         auto it = new_probability_by_characters.find(sref_prob.first);
//         if (it != new_probability_by_characters.end()) {
//             assert(total >= weight);
//             // P($n(-1) and Sn)
//             it->second.universe = sref_prob.second.universe * weight / total;
//         }
//     }

    return new_probability_by_characters;
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


struct Info { std::string const * s_p; double accu_prob; double universe; double prob; size_t sz; double best;};

void init_and_show_infos(
    std::vector<Info> & infos,
    probability_by_characters_t const & probability_by_characters,
    probability_by_characters_t const & original_probability_by_characters
) {
    infos.resize(probability_by_characters.size());
    double best = 0;
    std::set<string_ref_t, std::less<std::string>> best_pstrs;
    {
        size_t orig_sz = 0;
        for (auto & p : original_probability_by_characters) {
            orig_sz += p.second.size();
        }

        auto it = infos.begin();
        for (auto & p : probability_by_characters) {
            it->s_p = &p.first.get();
            it->accu_prob = accu_probability(p.second);
            it->sz = p.second.size();
            it->universe = p.second.universe;
            it->prob = it->accu_prob / (it->universe/* * orig_sz*/);
            it->best = std::max_element(
                p.second.begin(), p.second.end(),
                [](Probability const & prob1, Probability const & prob2) { return prob1.prob < prob2.prob; }
            )->prob;
            if (best < it->best) {
                best = it->best;
                best_pstrs.clear();
                best_pstrs.insert(*it->s_p);
            }
            else if (almost_equal(best, it->best, 1)) {
                best_pstrs.insert(*it->s_p);
            }
            ++it;
        }
    }
    std::sort(
        infos.begin(), infos.end(),
        [](Info const & a, Info const & b) { return a.prob > b.prob; }
    );

    std::cout << "\033[00;34mbest: " << best << " ( ";
    for (auto & sref : best_pstrs) {
        std::cout << sref.get() << " ";
    }
    std::cout << ")\033[0m  ";
    for (auto & info : infos) {
        std::cout
            << "\033[00;31m" << *info.s_p << "\033[0m [\033[00;32m"
            << info.accu_prob << "/" << info.universe
            << "\033[0m" << "|n(" << info.sz << ") = " << info.prob << "] "
            << "\033[00;34mbest(" << info.best << ")\033[0m  ";
        ;
    }

//     struct Best { std::reference_wrapper<Probability const> prob; string_ref_t s; };
//     std::vector<Best> best;
//     for (auto & p : probability_by_characters) {
//         for (Probability const & prob : p.second) {
//             best.push_back({{prob}, p.first});
//         }
//     }
//     std::sort(
//         best.begin(), best.end(),
//         [](Best const & a, Best const & b) { return a.prob.get().prob > b.prob.get().prob; }
//     );
//     best.erase(best.begin(), std::unique(
//         best.begin(), best.end(),
//         [](Best const & a, Best const & b) { return almost_equal(a.prob.get().prob, b.prob.get().prob, 1); }
//     ));
//     if (!best.empty()) {
//         std::cout << "\n\nbest: ";
//         auto x = best.front().prob.get().prob;
//         auto last = std::upper_bound(best.begin(), best.end(), x, [](double x, Best const & best) { return best.prob.get().prob < x; });
//         for (auto first = best.begin(); first != last; ++first) {
//             std::cout << first->s.get() << "(" << first->prob.get().prob << ")  ";
//         }
//     }

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

    probability_by_characters_t const probability_by_characters = [&definitions]() {
        probability_by_characters_t ret;
        for (auto const & def : definitions) {
            ret[def.c].emplace_back(def, 1.);
        }
        for (auto & pair : ret) {
            pair.second.universe = definitions.size();
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
        init_and_show_infos(infos, new_probability_by_characters, probability_by_characters);
        if (new_probability_by_characters.empty()) {
            result1 += '_';
        }
        else {
            result1 += *infos.front().s_p;
        }

        if (new_probability_by_characters.empty()) {
            result2 += '_';
        }
        else if (infos.front().prob >= 95) {
            result2 += *infos.front().s_p;
        }
        else {
            {
                probability_by_characters_t result_probability_by_characters;
                for (size_t i = sizeof(intervals)/sizeof(intervals[0]), e = i + 3/*5*/; i < e; ++i) {
                    size_t total = 0;
                    size_t weight = 0;

                    for (auto const & sref_prob : new_probability_by_characters) {
                        total += 100 * sref_prob.second.size();
                        for (Probability const & prob : sref_prob.second) {
                            weight += prob.refdef.get().datas[i].relationship(datas[i]);
                        }
                    }

                    result_probability_by_characters.clear();
                    for (auto const & sref_prob : new_probability_by_characters) {
                        // P(x and Sn=value)
                        for (Probability const & prob : sref_prob.second) {
                            unsigned const x = prob.refdef.get().datas[i].relationship(datas[i]);
                            if (x >= 50) {
                                auto & probs = result_probability_by_characters[sref_prob.first];
                                probs.emplace_back(prob.refdef, prob.prob * x / 100 * weight / total);
                            }
                        }
                        auto it = result_probability_by_characters.find(sref_prob.first);
                        if (it != result_probability_by_characters.end()) {
                            assert(total >= weight);
                            it->second.universe = sref_prob.second.universe * weight / total;
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

            init_and_show_infos(infos, new_probability_by_characters, probability_by_characters);

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
