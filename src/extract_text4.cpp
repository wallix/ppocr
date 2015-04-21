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

struct GroupDefinition
{
    definition_ref_t def_base;
    std::vector<string_ref_t> are_same;

    GroupDefinition(Definition const & def)
    : def_base(def)
    {}

    DataLoader::Datas const & datas() const { return this->def_base.get().datas; }
    DataLoader::data_base const & data(size_t i) const { return this->def_base.get().datas[i]; }
    std::string const & c() const { return this->def_base.get().c; }
};

using group_definition_ref_t = std::reference_wrapper<GroupDefinition const>;


struct Probability
{
    group_definition_ref_t gdef;
    double prob;

    Probability(GroupDefinition const & gdef, double prob = 1)
    : gdef(gdef)
    , prob(prob)
    {}

    std::string const & c() const { return gdef.get().c(); }
    DataLoader::data_base const & data(size_t i) const { return gdef.get().datas()[i]; }
    std::vector<string_ref_t> const & are_same() const { return gdef.get().are_same; }
};

struct Probabilities
{
    using iterator = Probability *;

    Probabilities(size_t sz)
    : data(static_cast<Probability*>(::operator new(sz * sizeof(Probability))))
    , current(data)
    {}

    template<class It>
    Probabilities(It first, It last)
    : data(static_cast<Probability*>(::operator new((last-first) * sizeof(Probability))))
    , current(data + (last-first))
    { std::copy(first, last, data); }

    Probabilities(Probabilities &&) = delete;
    Probabilities(Probabilities const &) = delete;

    void swap(Probabilities & p) noexcept
    {
        std::swap(p.data, data);
        std::swap(p.current, current);
    }

    ~Probabilities() {
      ::operator delete(this->data);
    }

    iterator begin() const { return data; }
    iterator end() const { return current; }
    size_t size() const { return current - data; }
    bool empty() const { return current == data; }
    void push_back(Probability const & p) { *current++ = p; }
    template<class... Args>
    void emplace_back(Args const & ... args) { *current++ = {args...}; }
    void clear() { current = data; }

    Probability & front() const { return *data; }
    Probability & back() const { return *(current-1); }
    Probability & operator[](size_t i) const { return data[i]; }

    void resize(size_t n) {
        current = data + n;
    }

private:
    Probability * data;
    Probability * current;
};

void swap(Probabilities & a, Probabilities & b) noexcept
{ a.swap(b); }

//using probabilities_t = std::vector<Probability>;
using probabilities_t = Probabilities;



unsigned get_value(DataLoader::data_base const & data) {
    return *reinterpret_cast<unsigned const *>(
        reinterpret_cast<unsigned char const *>(&data) + sizeof(DataLoader::data_base)
    );
}


void reduce_univers(
    probabilities_t const & probabilities,
    probabilities_t & probabilities_cp,
    size_t sig_idx, unsigned value, unsigned interval, bool first
) {
    auto d = interval/10u;

    auto approximate_get_value = [d, sig_idx, value, interval](GroupDefinition const & gdef) {
        unsigned const sig_value = get_value(gdef.data(sig_idx));
        return value < sig_value
            ? (sig_value <= value + d ? (interval - (sig_value - value)) * 100u / interval : 0u)
            : (value <= sig_value + d ? (interval - (value - sig_value)) * 100u / interval : 0u);
        //return value == sig_value  ? 100 : 0;
    };

    auto cp = probabilities_cp.begin();
    if (first) {
        auto first = std::lower_bound(
            probabilities.begin(), probabilities.end(), value < d ? 0u : value - d,
            [&](Probability const & prob, unsigned x) {
                return get_value(prob.data(sig_idx)) < x;
            }
        );
        auto last = std::upper_bound(
            first, probabilities.end(), std::min(value + d, interval),
            [&](unsigned x, Probability const & prob) {
                return x < get_value(prob.data(sig_idx));
            }
        );
        for (; first != last; ++first) {
            *cp = {first->gdef, first->prob * approximate_get_value(first->gdef) / 100};
            ++cp;
        }
    }
    else {
        auto first = probabilities.begin();
        auto last = probabilities.end();
        for (; first != last; ++first) {
            if (unsigned x = approximate_get_value(first->gdef)) {
                *cp = {first->gdef, first->prob * x / 100};
                ++cp;
            }
        }
    }

    probabilities_cp.resize(cp - probabilities_cp.begin());
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
        [](Probability const & a, Probability const & b) -> bool {
            if (a.c() < b.c()) {
                return true;
            }
            if (a.c() > b.c()) {
                return false;
            }
            if (a.are_same().size() < b.are_same().size()) {
                return true;
            }
            if (a.are_same().size() > b.are_same().size()) {
                return false;
            }
            auto const pair = std::mismatch(
                a.are_same().begin(), a.are_same().end(), b.are_same().begin(),
                std::equal_to<std::string>()
            );
            if (pair.first == a.are_same().end()) {
                return a.prob > b.prob;
            }
            return pair.first->get() < pair.second->get();
        }
    );

    probabilities.resize(
        std::unique(
            probabilities.begin(), probabilities.end(),
            [](Probability const & a, Probability const & b) {
                if (a.c() == b.c() && a.are_same().size() == b.are_same().size()) {
                    return std::equal(
                        a.are_same().begin(), a.are_same().end(), b.are_same().begin(),
                        std::equal_to<std::string>()
                    );
                }
                return false;
            }
        ) - probabilities.begin()
    );

    std::sort(
        probabilities.begin(), probabilities.end(),
        [](Probability const & a, Probability const & b)
        { return a.prob > b.prob; }
    );

    auto print_chars = [](GroupDefinition const & gdef) {
        std::cout << gdef.c();
        if (!gdef.are_same.empty()) {
            for (std::string const & c : gdef.are_same) {
                std::cout << " " << c;
            }
        }
    };

    auto first = probabilities.begin();
    std::cout << "\033[00;34mbest: " << first->prob << " ( \033[00;31m";
    print_chars(first->gdef);
    for (auto previous = first; ++first != probabilities.end() && !(first->prob < previous->prob); ++previous) {
        std::cout << " ";
        print_chars(first->gdef);
    }
    std::cout << "\033[0m )\n\n";
    for (; first != probabilities.end(); ++first) {
        std::cout << "\033[00;31m";
        print_chars(first->gdef);
        std::cout << "\033[0m " << first->prob << "  ";
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

    std::vector<Definition> const definitions = read_definitions(file, loader);

    if (!file.eof()) {
        std::cerr << "read error\n";
        return 5;
    }

    std::cout << " ## definitions.size(): " << definitions.size() << "\n\n";

    std::vector<GroupDefinition> group_definitions(definitions.begin(), definitions.end());

    std::sort(
        group_definitions.begin(), group_definitions.end(),
        [](GroupDefinition const & lhs, GroupDefinition const & rhs) {
            bool const is_less = lhs.datas() < rhs.datas();
            return is_less || (lhs.datas() == rhs.datas() && lhs.c() < rhs.c());
        }
    );
    if (!group_definitions.empty())
    {
        auto first = group_definitions.begin();
        auto prev = group_definitions.begin();
        auto last = group_definitions.end();
        while (++first != last) {
            if (first->datas() == prev->datas()) {
                if (prev->are_same.empty() ? first->c() != prev->c() : prev->are_same.empty() && prev->are_same.back().get() != first->c()) {
                    prev->are_same.push_back(first->c());
                }
            }
            else {
                ++prev;
               *prev = *std::move(first);
            }
        }
        group_definitions.erase(++prev, group_definitions.end());
    }

    std::cout << " ## group_definitions.size(): " << group_definitions.size() << "\n\n";

    size_t const first_algo = 16;

    std::sort(
        group_definitions.begin(), group_definitions.end(),
        [](GroupDefinition const & lhs, GroupDefinition const & rhs) {
            return lhs.data(first_algo) < rhs.data(first_algo);
        }
    );

    Image img = image_from_file(av[2]);
    Bounds const bounds(img.width(), img.height());
    size_t x = 0;

    probabilities_t const probabilities(group_definitions.begin(), group_definitions.end());
    probabilities_t cp_probabilities(group_definitions.size());
    probabilities_t tmp_probabilities(group_definitions.size());


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

    probabilities_t result_probabilities(group_definitions.size());

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

//         {
//             std::vector<size_t> reduce_values;
//             for (size_t i = 0; i < sizeof(intervals)/sizeof(intervals[0]); ++i) {
//                 if (!intervals[i].enable) {
//                     continue;
//                 }
//                 cp_probabilities = probabilities;
//                 reduce_univers(cp_probabilities, i, get_value(datas[i]), intervals[i].interval);
//                 reduce_values.emplace_back(cp_probabilities.size());
//             }
//
//             auto i = 0u;
//             for (auto reduce : reduce_values) {
//                 if (!intervals[i++].enable) {
//                     continue;
//                 }
//                 std::cout << (i-1) << ") \033[00;36m" << std::setw(4) << reduce << "\033[0m  ";
//             }
//             std::cout << "\n";
//         }

        cp_probabilities.resize(probabilities.size());
        reduce_univers(probabilities, cp_probabilities, first_algo, get_value(datas[first_algo]), intervals[first_algo].interval, true);
        for (size_t i = 0; i < sizeof(intervals)/sizeof(intervals[0]); ++i) {
            if (!intervals[i].enable || i == first_algo) {
                continue;
            }
            reduce_univers(cp_probabilities, cp_probabilities, i, get_value(datas[i]), intervals[i].interval, false);
        }

        tmp_probabilities.resize(cp_probabilities.size());
        std::copy(cp_probabilities.begin(), cp_probabilities.end(), tmp_probabilities.begin());
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
        else if (tmp_probabilities[0].prob >= 1/* && tmp_probabilities.size() == 1*/) {
            result2 += tmp_probabilities[0].c();
        }
        else {
            for (size_t i = sizeof(intervals)/sizeof(intervals[0]), e = i + 3; i < e; ++i) {
                result_probabilities.clear();
                for (auto & prob : cp_probabilities) {
                    unsigned const x = prob.data(i).relationship(datas[i]);
                    if (x >= 50) {
                        result_probabilities.emplace_back(std::move(prob.gdef), prob.prob * x / 100);
                    }
                }
                swap(cp_probabilities, result_probabilities);
            }

            for (size_t i = sizeof(intervals)/sizeof(intervals[0]) + 3, e = i + 2; i < e; ++i) {
                result_probabilities.clear();
                for (auto & prob : cp_probabilities) {
                    auto first = std::lower_bound(
                        definitions.begin(), definitions.end(), prob.c(),
                        [](Definition const & a, std::string const & s) { return a.c < s; }
                    );
                    for (; first != definitions.end() && first->c == prob.c(); ++first) {
                        if (prob.data(i).relationship(first->datas[i]) >= 50) {
                            result_probabilities.push_back(std::move(prob));
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
