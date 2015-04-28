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

    GroupDefinition(GroupDefinition const &) = delete;
    GroupDefinition(GroupDefinition &&) = default;
    GroupDefinition&operator=(GroupDefinition const &) = delete;
    GroupDefinition&operator=(GroupDefinition &&) = default;

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
    group_definition_ref_t gdef;
    double prob;

    Probability(GroupDefinition const & gdef, double prob = 1)
    : gdef(gdef)
    , prob(prob)
    {}

    std::string const & c() const { return gdef.get().c(); }
    unsigned data(size_t i) const { return gdef.get().data(i); }
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



void reduce_univers(
    std::vector<uint64_t> & datas_accepted,
    std::vector<std::vector<uint64_t>> const & datas,
    unsigned value
) {
    //size_t n = 0;
    auto accepted_it = datas_accepted.begin();
    for (auto mask : datas[value]) {
        *accepted_it &= mask;
        //n += __builtin_popcountl(*accepted_it);
        ++accepted_it;
    }
    //std::cout << " ## n: " << (n) << std::endl;
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

struct compute_image_data_type {
    std::vector<Definition> const & definitions;
    std::vector<GroupDefinition> const group_definitions;
    std::array<unsigned, 4> const nb_u64;
    std::vector<uint64_t> datas_accepted;
    // [cat][algo][value][data_mask]
    std::array<std::array<std::vector<std::vector<uint64_t>>, count_algo_base>, 4> const datas_defs;
    probabilities_t probabilities;
    probabilities_t tmp_probabilities;
    size_t const first_algo;

    std::string result1;
    std::string result2;

    struct def_img_compare {
        def_img_compare() {}
        bool operator()(Image const & a, Image const & b) const
        { return image_compare(a, b) < 0; }
    };
    std::map<Image, std::pair<std::string, std::string>, def_img_compare> sorted_img;

    static unsigned idx_cat(DataLoader::Datas const & datas) {
        auto data = reinterpret_cast<unsigned const *>(
            reinterpret_cast<unsigned char const *>(&datas[23]) + sizeof(DataLoader::data_base)
        );
        if (data[0] && data[1]) {
            return 0;
        }
        if (!data[0] && !data[1]) {
            return 1;
        }
        if (data[0] && !data[1]) {
            return 2;
        }
        return 3;
    }

    static unsigned idx_cat(GroupDefinition const & gdef) {
        return idx_cat(gdef.def_base.ref_def.get().datas);
    }

    compute_image_data_type(
        std::vector<Definition> const & definitions,
        unsigned const * intervals,
        size_t const first_algo = 0
    )
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
                    *prev = std::move(*first);
                }
            }
            group_definitions.erase(++prev, group_definitions.end());
        }

        std::sort(
            group_definitions.begin(), group_definitions.end(),
            [first_algo](GroupDefinition const & lhs, GroupDefinition const & rhs) {
                return idx_cat(lhs) < idx_cat(rhs)
                   ||  (idx_cat(lhs) == idx_cat(rhs) && lhs.data(first_algo) < rhs.data(first_algo));
            }
        );
        return group_definitions;
    }())
    , nb_u64([&]() {
        std::array<unsigned, 4> cat{{}};
        for (GroupDefinition const & gdef : group_definitions) {
            ++cat[idx_cat(gdef)];
        }
        return cat;
    }())
    , datas_accepted((*std::max_element(std::begin(nb_u64), std::end(nb_u64)) + 63) / 64)
    , datas_defs([&]{
        std::array<std::array<std::vector<std::vector<uint64_t>>, count_algo_base>, 4> datas_defs;
        unsigned accu_shift = 0;
        for (size_t icat = 0; icat < datas_defs.size(); ++icat) {
            size_t i = 0;
            for (auto & datas : datas_defs[icat]) {
                datas.resize(intervals[i] + 1);
                size_t value = 0;
                auto const d = intervals[i]/10u;
                for (std::vector<uint64_t> & data_mask : datas) {
                    data_mask.resize((nb_u64[icat] + 63) / 64);
                    //data_mask.resize(group_definitions.size());
                    size_t idef = 0;
                    auto first = this->group_definitions.begin() + accu_shift;
                    auto last = first + nb_u64[icat];
                    for (; first != last; ++first) {
                        auto const sig_value = first->data(i);
                        if (value < sig_value ? (sig_value <= value + d) : (value <= sig_value + d)) {
                            data_mask[idef / 64] |= 1ull << (idef % 64);
                        }
                        ++idef;
                    }
                    ++value;
                }
                ++i;
            }
            accu_shift += nb_u64[icat];
        }
        assert(accu_shift == this->group_definitions.size());

        return datas_defs;
    }())
    , probabilities(group_definitions.size())
    , tmp_probabilities(group_definitions.size())
    , first_algo(first_algo)
    {
        this->result1.reserve(100);
        this->result2.reserve(100);
    }
};

void compute_image(
    compute_image_data_type & o,
    DataLoader::Datas const & datas,
    Image & img,
    unsigned const * intervals
) {
    auto const icat = compute_image_data_type::idx_cat(datas);
    auto const nb_u64 = o.nb_u64[icat];
    auto const shift_idx = std::accumulate(o.nb_u64.begin(), o.nb_u64.begin()+icat, 0);
    auto & datas_defs = o.datas_defs[icat];

    o.datas_accepted.clear();
    o.datas_accepted.resize((nb_u64 + 63) / 64, ~uint64_t{});

    unsigned constexpr l[] {16, 12, 18, 19, 11, 15, 14, 10, 13, 17, 9, 5, 4, 1, 7, 8, 3, 0, 6, 2};
    for (size_t i : l) {
        reduce_univers(
            o.datas_accepted,
            datas_defs[i],
            get_value(datas[i])
        );
    }

    o.probabilities.clear();
    for (size_t i = 0; i < nb_u64; ++i) {
        if (o.datas_accepted[i/64] & (1ull << (i %  64))) {
            double prob = 1;
            for (size_t ialgo = 0; ialgo < count_algo_base; ++ialgo) {
                auto const interval = intervals[ialgo];
                auto const d = interval/10u;
                unsigned const sig_value = o.group_definitions[shift_idx+i].data(ialgo);
                unsigned const value = get_value(datas[ialgo]);
                prob = prob * (value < sig_value
                    ? (sig_value <= value + d ? (interval - (sig_value - value)) * 100u / interval : 0u)
                    : (value <= sig_value + d ? (interval - (value - sig_value)) * 100u / interval : 0u)
                ) / 100;
            }
            o.probabilities.push_back({o.group_definitions[shift_idx+i], prob});
        }
    }
    o.tmp_probabilities.resize(o.probabilities.size());
    std::copy(o.probabilities.begin(), o.probabilities.end(), o.tmp_probabilities.begin());
    sort_and_show_infos(o.tmp_probabilities);

    auto & cache_element = o.sorted_img.emplace(std::move(img), std::pair<std::string, std::string>()).first->second;

    if (o.tmp_probabilities.empty()) {
        o.result1 += '_';
        cache_element.first = '_';
    }
    else {
        o.result1 += o.tmp_probabilities.front().c();
        cache_element.first = o.tmp_probabilities.front().c();
    }

    if (o.tmp_probabilities.empty()) {
        o.result2 += '_';
        cache_element.second = '_';
    }
    else if (o.tmp_probabilities[0].prob >= 1/* && tmp_probabilities.size() == 1*/) {
        o.result2 += o.tmp_probabilities[0].c();
        cache_element.second = o.tmp_probabilities[0].c();
    }
    else {
        for (size_t i = count_algo_base, e = i + 3; i < e; ++i) {
            o.tmp_probabilities.clear();
            for (auto & prob : o.probabilities) {
                unsigned const x = prob.gdef.get().def_base.ref_def.get().datas[i].relationship(datas[i]);
                if (x >= 50) {
                    o.tmp_probabilities.emplace_back(std::move(prob.gdef), prob.prob * x / 100);
                }
            }
            swap(o.probabilities, o.tmp_probabilities);
        }

        for (size_t i = count_algo_base + 3, e = i + 2; i < e; ++i) {
            o.tmp_probabilities.clear();
            for (auto & prob : o.probabilities) {
                auto first = std::lower_bound(
                    o.definitions.begin(), o.definitions.end(), prob.c(),
                    [](Definition const & a, std::string const & s) { return a.c < s; }
                );
                for (; first != o.definitions.end() && first->c == prob.c(); ++first) {
                    if (prob.gdef.get().def_base.ref_def.get().datas[i].relationship(first->datas[i]) >= 50) {
                        o.tmp_probabilities.push_back(std::move(prob));
                        break;
                    }
                }
            }
            swap(o.probabilities, o.tmp_probabilities);
        }

        sort_and_show_infos(o.probabilities);
        if (o.probabilities.empty()) {
            o.result2 += '_';
            cache_element.second = '_';
        }
        else {
            o.result2 += o.probabilities.front().c();
            cache_element.second = o.probabilities.front().c();
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

    unsigned intervals[] = {
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
    static_assert(size(intervals) == count_algo_base, "intervals.size error");

    size_t const first_algo = 16;
    compute_image_data_type compute_image_data(definitions, intervals, first_algo);

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
            compute_image(compute_image_data, datas, img_word, intervals);
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
