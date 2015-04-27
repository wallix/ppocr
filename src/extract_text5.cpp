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
#include "utils/image_compare.hpp"

#include "sassert.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <map>

#include <cstring>
#include <cerrno>

using std::size_t;

using string_ref_t = std::reference_wrapper<std::string const>;
using definition_ref_t = std::reference_wrapper<Definition const>;


unsigned get_value(DataLoader::data_base const & data) {
    return *reinterpret_cast<unsigned const *>(
        reinterpret_cast<unsigned char const *>(&data) + sizeof(DataLoader::data_base)
    );
}

constexpr size_t count_algo_base = 20;

template<class T, size_t N>
constexpr size_t  size(T(&)[N]) {
    return N;
}

struct GroupDefinition
{
    struct {
        definition_ref_t ref_def;
        std::array<unsigned, count_algo_base> values;
    } def_base;
    std::vector<string_ref_t> are_same;

    GroupDefinition(Definition const & def)
    : def_base{std::ref(def), {{
        get_value(def.datas[0]),
        get_value(def.datas[1]),
        get_value(def.datas[2]),
        get_value(def.datas[3]),
        get_value(def.datas[4]),
        get_value(def.datas[5]),
        get_value(def.datas[6]),
        get_value(def.datas[7]),
        get_value(def.datas[8]),
        get_value(def.datas[9]),
        get_value(def.datas[10]),
        get_value(def.datas[11]),
        get_value(def.datas[12]),
        get_value(def.datas[13]),
        get_value(def.datas[14]),
        get_value(def.datas[15]),
        get_value(def.datas[16]),
        get_value(def.datas[17]),
        get_value(def.datas[18]),
        get_value(def.datas[19]),
    }}}
    {}

    std::array<unsigned, 20> const & datas() const { return this->def_base.values; }
    unsigned data(size_t i) const { return this->def_base.values[i]; }
    std::string const & c() const { return this->def_base.ref_def.get().c; }
};

using group_definition_ref_t = std::reference_wrapper<GroupDefinition const>;


struct Probability
{
    unsigned i;
    double prob;
};

struct Probabilities
{
    using iterator = Probability *;

    Probability * data;
    Probability * last;

    Probabilities(size_t sz)
    : data(new Probability[sz])
    {
        for (size_t i = 0; i != sz; ++i) {
            data[i].i = unsigned(i);
            data[i].prob = 1;
        }
        last = data + sz;
    }

    ~Probabilities()
    { delete[] data; }

//     template<class It>
//     Probabilities(It first, It last)
//     : Probabilities(last-first)
//     {
//         for (size_t i = 0; i < algo_values.size(); ++i) {
//             std::transform(first, last, algo_values[i].begin(), [i](Definition const & def) {
//                 return def.datas[i];
//             });
//         }
//         auto it_ref_def = this->ref_defs.begin();
//         for (; first != last; ++first) {
//             *it_ref_def = &first->get();
//             ++it_ref_def;
//         }
//     }

    Probabilities(Probabilities &&) = delete;
    Probabilities(Probabilities const &) = delete;

    void swap(Probabilities & p) noexcept
    {
        std::swap(p.data, data);
        std::swap(p.last, last);
    }

    iterator begin() const { return data; }
    iterator end() const { return last; }
    size_t size() const { return last - data; }
    bool empty() const { return last == data; }
    void push_back(Probability const & p) { *last++ = p; }
//     template<class... Args>
//     void emplace_back(Args const & ... args) { *current++ = {args...}; }
    void clear() { last = data; }
//
    Probability & front() const { return *data; }
    Probability & back() const { return *(last-1); }
//     Probability & operator[](size_t i) const { return data[i]; }

    void resize(size_t n) {
        last = data + n;
    }
//
// // private:
//     std::array<std::vector<unsigned>, count_algo_base> algo_values;
//     std::vector<Definition const *> ref_defs;
//     std::vector<std::vector<string_ref_t>> are_sames;
//     size_t i_current = 0;
};

void swap(Probabilities & a, Probabilities & b) noexcept
{ a.swap(b); }

//using probabilities_t = std::vector<Probability>;
using probabilities_t = Probabilities;

struct compute_image_data_type
{
    std::vector<Definition> const & definitions;
    std::vector<GroupDefinition> const group_definitions;
    std::vector<definition_ref_t> const group_ref_def;
    std::array<std::vector<unsigned>, count_algo_base> const group_values;
    std::vector<std::vector<string_ref_t>> const group_are_same;
    probabilities_t const probabilities;
    probabilities_t cp_probabilities;
    probabilities_t tmp_probabilities;

    size_t const first_algo;

    std::string result1;
    std::string result2;

    //struct image_hash : std::hash<size_t> {
    //    image_hash() {}
    //    size_t operator()(Image const & img) const
    //    {
    //        size_t h = 0;
    //        std::accumulate(img.data(), img.data_end(), size_t{}, [](char x, size_t h) {
    //            return (h + (x*2u + 0xc70f6907UL)) ^ 658935609426;
    //        });
    //        return h;
    //    }
    //};
    //std::unordered_map<Image, std::pair<std::string, std::string>, image_hash> sorted_img;
    struct def_img_compare {
        def_img_compare() {}
        bool operator()(Image const & a, Image const & b) const
        { return image_compare(a, b) < 0; }
    };
    std::map<Image, std::pair<std::string, std::string>, def_img_compare> sorted_img;

    compute_image_data_type(std::vector<Definition> const & definitions, size_t const first_algo = 0)
    : definitions(definitions)
    , group_definitions([&]() {
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

        std::sort(
            group_definitions.begin(), group_definitions.end(),
            [first_algo](GroupDefinition const & lhs, GroupDefinition const & rhs) {
                return lhs.data(first_algo) < rhs.data(first_algo);
            }
        );
        return group_definitions;
    }())
    , group_ref_def([this]() {
        std::vector<definition_ref_t> group_ref_def;
        for (GroupDefinition const & gdef : group_definitions) {
            group_ref_def.push_back(gdef.def_base.ref_def);
        }
        return group_ref_def;
    }())
    , group_values([this]() {
        std::array<std::vector<unsigned>, count_algo_base> group_values;
        for (size_t i = 0; i < count_algo_base; ++i) {
            for (GroupDefinition const & gdef : group_definitions) {
                group_values[i].push_back(gdef.def_base.values[i]);
            }
        }
        return group_values;
    }())
    , group_are_same([this]() {
        std::vector<std::vector<string_ref_t>> group_are_same;
        for (GroupDefinition const & gdef : group_definitions) {
            group_are_same.push_back(gdef.are_same);
        }
        return group_are_same;
    }())
    , probabilities(group_definitions.size())
    , cp_probabilities(group_definitions.size())
    , tmp_probabilities(group_definitions.size())
    , first_algo(first_algo)
    {}
};



void reduce_univers(
    compute_image_data_type & o,
    probabilities_t const & probabilities,
    probabilities_t & probabilities_cp,
    size_t sig_idx, unsigned value, unsigned interval, bool first
) {
    auto d = interval/10u;

    auto approximate_get_value = [d, sig_idx, value, interval](unsigned sig_value) {
        return value < sig_value
            ? (sig_value <= value + d ? (interval - (sig_value - value)) * 100u / interval : 0u)
            : (value <= sig_value + d ? (interval - (value - sig_value)) * 100u / interval : 0u);
        //return value == sig_value  ? 100 : 0;
    };

    auto cp = probabilities_cp.begin();
    auto & values = o.group_values[sig_idx];

    if (first) {
        auto first = std::lower_bound(
            probabilities.begin(), probabilities.end(), value < d ? 0u : value - d,
            [&](Probability & prob, unsigned x) {
                return values[prob.i] < x;
            }
        );
        auto last = std::upper_bound(
            first, probabilities.end(), std::min(value + d, interval),
            [&](unsigned x, Probability & prob) {
                return x < values[prob.i];
            }
        );
        for (; first != last; ++first) {
            *cp = {first->i, first->prob * approximate_get_value(values[first->i]) / 100};
            ++cp;
        }
    }
    else {
        auto first = probabilities.begin();
        auto last = probabilities.end();
        for (; first != last; ++first) {
            if (unsigned x = approximate_get_value(values[first->i])) {
                *cp = {first->i, first->prob * x / 100};
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


void sort_and_show_infos(compute_image_data_type const & o, probabilities_t & probabilities) {
    if (probabilities.empty()) {
        std::cout << "\033[00;34mbest: 0 ( )\033[0m\n\n";
        return;
    }

    std::sort(
        probabilities.begin(), probabilities.end(),
        [&o](Probability const & a, Probability const & b) -> bool {
            if (o.group_ref_def[a.i].get().c < o.group_ref_def[b.i].get().c) {
                return true;
            }
            if (o.group_ref_def[a.i].get().c > o.group_ref_def[b.i].get().c) {
                return false;
            }
            if (o.group_are_same[a.i].size() < o.group_are_same[b.i].size()) {
                return true;
            }
            if (o.group_are_same[a.i].size() > o.group_are_same[b.i].size()) {
                return false;
            }
            auto const pair = std::mismatch(
                o.group_are_same[a.i].begin(), o.group_are_same[a.i].end(), o.group_are_same[b.i].begin(),
                std::equal_to<std::string>()
            );
            if (pair.first == o.group_are_same[a.i].end()) {
                return a.prob > b.prob;
            }
            return pair.first->get() < pair.second->get();
        }
    );

    probabilities.resize(
        std::unique(
            probabilities.begin(), probabilities.end(),
            [&o](Probability const & a, Probability const & b) {
                if (o.group_ref_def[a.i].get().c == o.group_ref_def[b.i].get().c
                 && o.group_are_same[a.i].size() == o.group_are_same[b.i].size()) {
                    return std::equal(
                        o.group_are_same[a.i].begin(),
                        o.group_are_same[a.i].end(),
                        o.group_are_same[b.i].begin(),
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

    auto print_chars = [&o](unsigned i) {
        std::cout << o.group_ref_def[i].get().c;
        if (!o.group_are_same[i].empty()) {
            for (std::string const & c : o.group_are_same[i]) {
                std::cout << " " << c;
            }
        }
    };

    auto first = probabilities.begin();
    std::cout << "\033[00;34mbest: " << first->prob << " ( \033[00;31m";
    print_chars(first->i);
    for (auto previous = first; ++first != probabilities.end() && !(first->prob < previous->prob); ++previous) {
        std::cout << " ";
        print_chars(first->i);
    }
    std::cout << "\033[0m )\n\n";
    for (; first != probabilities.end(); ++first) {
        std::cout << "\033[00;31m";
        print_chars(first->i);
        std::cout << "\033[0m " << first->prob << "  ";
    }

    std::cout << "\n\n";
}

void compute_image(compute_image_data_type & o, DataLoader::Datas const & datas, Image & img)
{
    static struct { bool enable; unsigned interval; } const intervals[] = {
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
    static_assert(size(intervals) == count_algo_base, "intervals.size error");

//     {
//         std::vector<size_t> reduce_values;
//         for (size_t i = 0; i < sizeof(intervals)/sizeof(intervals[0]); ++i) {
//             if (!intervals[i].enable) {
//                 continue;
//             }
//             cp_probabilities = probabilities;
//             reduce_univers(cp_probabilities, i, get_value(datas[i]), intervals[i].interval);
//             reduce_values.emplace_back(cp_probabilities.size());
//         }
//
//         auto i = 0u;
//         for (auto reduce : reduce_values) {
//             if (!intervals[i++].enable) {
//                 continue;
//             }
//             std::cout << (i-1) << ") \033[00;36m" << std::setw(4) << reduce << "\033[0m  ";
//         }
//         std::cout << "\n";
//     }

    o.cp_probabilities.resize(o.probabilities.size());
    reduce_univers(o,
        o.probabilities, o.cp_probabilities, o.first_algo,
        get_value(datas[o.first_algo]),
        intervals[o.first_algo].interval,
        true
    );
    unsigned constexpr l[] {16, 12, 18, 19, 11, 15, 14, 10, 13, 17, 9, 5, 4, 1, 7, 8, 3, 0, 6, 2};
    static_assert(size(l) == count_algo_base, "l.size error");
    for (size_t i : l) {
        if (!intervals[i].enable || i == o.first_algo) {
            continue;
        }
        reduce_univers(o,
            o.cp_probabilities, o.cp_probabilities, i,
            get_value(datas[i]),
            intervals[i].interval,
            false
        );
    }

    auto & cache_element = o.sorted_img.emplace(std::move(img), std::pair<std::string, std::string>()).first->second;

    o.tmp_probabilities.resize(o.cp_probabilities.size());
    std::copy(o.cp_probabilities.begin(), o.cp_probabilities.end(), o.tmp_probabilities.begin());
    sort_and_show_infos(o, o.tmp_probabilities);
    if (o.tmp_probabilities.empty()) {
        o.result1 += '_';
        cache_element.first = '_';
    }
    else {
        auto & s = o.group_ref_def[o.tmp_probabilities.front().i].get().c;
        o.result1 += s;
        cache_element.first = s;
    }

    if (o.tmp_probabilities.empty()) {
        o.result2 += '_';
        cache_element.second = '_';
    }
    else if (o.tmp_probabilities.front().prob >= 1/* && tmp_probabilities.size() == 1*/) {
        auto & s = o.group_ref_def[o.tmp_probabilities.front().i].get().c;
        o.result2 += s;
        cache_element.second = s;
    }
    else {
        for (size_t i = count_algo_base, e = i + 3; i < e; ++i) {
            o.tmp_probabilities.clear();
            for (auto & prob : o.cp_probabilities) {
                unsigned const x = o.group_ref_def[prob.i].get().datas[i].relationship(datas[i]);
                if (x >= 50) {
                    o.tmp_probabilities.push_back({prob.i, prob.prob * x / 100});
                }
            }
            swap(o.cp_probabilities, o.tmp_probabilities);
        }

        for (size_t i = count_algo_base + 3, e = i + 2; i < e; ++i) {
            o.tmp_probabilities.clear();
            for (auto & prob : o.cp_probabilities) {
                auto first = std::lower_bound(
                    o.definitions.begin(), o.definitions.end(), o.group_ref_def[prob.i].get().c,
                    [](Definition const & a, std::string const & s) { return a.c < s; }
                );
                for (; first != o.definitions.end() && first->c == o.group_ref_def[prob.i].get().c; ++first) {
                    if (o.group_ref_def[prob.i].get().datas[i].relationship(first->datas[i]) >= 50) {
                        o.tmp_probabilities.push_back(std::move(prob));
                        break;
                    }
                }
            }
            swap(o.cp_probabilities, o.tmp_probabilities);
        }

        sort_and_show_infos(o, o.cp_probabilities);
        if (o.cp_probabilities.empty()) {
            o.result2 += '_';
            cache_element.second = '_';
        }
        else {
            auto & s = o.group_ref_def[o.cp_probabilities.front().i].get().c;
            o.result2 += s;
            cache_element.second = s;
        }
    }

    o.result1 += ' ';
    o.result2 += ' ';
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

    size_t const first_algo = 16;
    compute_image_data_type compute_image_data(definitions, first_algo);

    std::cout << " ## group_definitions.size(): " << compute_image_data.group_definitions.size() << "\n\n";

    Image img = image_from_file(av[2]);
    Bounds const bounds(img.width(), img.height());
    size_t x = 0;

    using resolution_clock = std::chrono::high_resolution_clock;
    auto t1 = resolution_clock::now();
    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //min_y = std::min(cbox.y(), min_y);
        //bounds_y = std::max(cbox.y() + cbox.h(), bounds_y);
        //bounds_x = cbox.x() + cbox.w();
        //std::cerr << "\nbox(" << cbox << ")\n";

        Image img_word = img.section(cbox.index(), cbox.bounds());
        std::cout << img_word;

        auto it = compute_image_data.sorted_img.find(img_word);
        if (it != compute_image_data.sorted_img.end()) {
            compute_image_data.result1 += it->second.first;
            compute_image_data.result2 += it->second.second;
            compute_image_data.result1 += ' ';
            compute_image_data.result2 += ' ';
        }
        else {
            auto datas = loader.new_datas(img_word, img_word.rotate90());
            compute_image(compute_image_data, datas, img_word);
        }

        x = cbox.x() + cbox.w();
    }

    {
        auto t2 = resolution_clock::now();
        std::cerr << std::chrono::duration<double>(t2-t1).count() << "s\n";
    }

    std::cout
      << " ## result1: " << (compute_image_data.result1)
      << "\n ## result2: " << (compute_image_data.result2)
      << std::endl
    ;
}
