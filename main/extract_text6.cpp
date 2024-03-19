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
#include "ppocr/image/image_from_file.hpp"
#include "ppocr/loader2/glyphs_loader.hpp"
#include "ppocr/box_char/make_box_character.hpp"
#include "ppocr/box_char/box.hpp"

#include "ppocr/defined_loader.hpp"

#include "ppocr/math/almost_equal.hpp"
#include "ppocr/utils/image_compare.hpp"

#include "ppocr/spell/dictionary.hpp"
#include "ppocr/spell/word_disambiguouser.hpp"

#include "ppocr/utils/reindex.hpp"
#include "ppocr/utils/read_file.hpp"
#include "ppocr/utils/integer_iterator.hpp"

#include <cassert>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <map>
#include <numeric>

#include <cstring>
#include <cerrno>

using namespace ppocr;

namespace {

constexpr unsigned const ocr_div = 10;

using std::size_t;

template<size_t... Ints>
struct integer_sequence
{ static constexpr std::size_t size = sizeof...(Ints); };

template<class Ints>
struct first_index;

template<size_t I, size_t... Ints>
struct first_index<integer_sequence<I, Ints...>>
{ static constexpr std::size_t value = I; };

using algo_indexes = integer_sequence<16, 12, 18, 19, 11, 15, 14, 10, 13, 17, 9, 5, 4, 1, 7, 8, 3, 0, 6, 2>;
constexpr size_t first_algo = first_index<algo_indexes>::value;

template<size_t I, class E, class... Es>
struct index_element : index_element<(I-1), Es...>
{};

template<class E, class... Es>
struct index_element<0, E, Es...>
{ using type = E; };

template<size_t I, class Pack>
struct index_pack;

template<size_t I, template<class...> class P, class... Es>
struct index_pack<I, P<Es...>> : index_element<I, Es...>
{};

template<template<class...> class Mk, class P, class Ints>
struct mk_from_pack;

template<template<class...> class Mk, template<class...> class P, class... Es, std::size_t... Ints>
struct mk_from_pack<Mk, P<Es...>, integer_sequence<Ints...>>
{ using type = Mk<typename index_element<Ints, Es...>::type...>; };


void reduce_universe(
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


struct Probability
{
    unsigned i;
    double prob;

    Probability(unsigned i, double prob = 1)
    : i(i)
    , prob(prob)
    {}
};

struct Probabilities
{
    using iterator = Probability *;

    Probabilities(size_t sz)
    : data(static_cast<Probability*>(::operator new(sz * sizeof(Probability))))
    , current(data)
    {}

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

using probabilities_t = Probabilities;


void sort_and_show_infos(probabilities_t & probabilities, loader2::Glyphs const & glyphs) {
    if (probabilities.empty()) {
        std::cout << "\033[00;34mbest: 0 ( )\033[0m\n\n";
        return;
    }

    //auto c = [&](Probability const & prob) { return glyphs[prob.i].views[0].word; };
    auto views = [&](Probability const & prob) -> loader2::Views const & { return glyphs[prob.i].views; };

    auto eq_word = [](loader2::View const & v1, loader2::View const & v2) { return v1.word == v2.word; };

    std::sort(
        probabilities.begin(), probabilities.end(),
        [&](Probability const & a, Probability const & b) -> bool {
            auto & v1 = views(a);
            auto & v2 = views(b);
            if (v1.size() < v2.size()) {
                return true;
            }
            if (v1.size() > v2.size()) {
                return false;
            }
            auto const pair = std::mismatch(v1.begin(), v1.end(), v2.begin(), eq_word);
            if (pair.first == v1.end()) {
                return a.prob > b.prob;
            }
            return pair.first->word < pair.second->word;
        }
    );

    probabilities.resize(
        std::unique(
            probabilities.begin(), probabilities.end(),
            [&](Probability const & a, Probability const & b) {
                auto & v1 = views(a);
                auto & v2 = views(b);
                return v1.size() == v2.size() && std::equal(v1.begin(), v1.end(), v2.begin(), eq_word);
            }
        ) - probabilities.begin()
    );

    std::sort(
        probabilities.begin(), probabilities.end(),
        [](Probability const & a, Probability const & b)
        { return a.prob > b.prob; }
    );

    auto print_chars = [&](Probability const & prob) {
        for (auto & view : views(prob)) {
            std::cout << view.word << " ";
        }
    };

    auto first = probabilities.begin();
    std::cout << "\033[00;34mbest: " << first->prob << " ( \033[00;31m";
    print_chars(*first);
    for (auto previous = first; ++first != probabilities.end() && !(first->prob < previous->prob); ++previous) {
        print_chars(*first);
    }
    std::cout << "\033[0m)\n\n";
    for (; first != probabilities.end(); ++first) {
        std::cout << "\033[00;31m";
        print_chars(*first);
        std::cout << "\033[0m" << first->prob << "  ";
    }

    std::cout << "\n\n";
}

template<class FirstStrategy, class... Strategies>
struct compute_image_data_type
{
    loader2::Glyphs const & glyphs;
    PpOcrDatas const & datas;

    std::vector<unsigned> const nb_u64;
    std::vector<unsigned> const shift_u64;
    std::vector<uint64_t> datas_accepted;

    template<class Strategy>
    struct MaskByValue : std::vector<std::vector<uint64_t>> {};
    struct DatasMaskByValue : MaskByValue<Strategies>... {
        template<class Strategy>
        std::vector<std::vector<uint64_t>> const & masks() const {
            return static_cast<MaskByValue<Strategy> const &>(*this);
        }
    };
    // [values_first_algo]{algo:[value][data_mask]...}
    std::vector<DatasMaskByValue> const datas_defs;

    probabilities_t probabilities;
    probabilities_t tmp_probabilities;

    std::vector<std::vector<std::string>> ambiguous;

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
    std::map<Image, std::pair<std::string, std::vector<std::string>>, def_img_compare> sorted_img;

    template<class Strategy>
    int get_interval() const {
        return static_cast<int>(this->datas.template get<Strategy>().count_posibilities() - 1) / ocr_div;
    }

    template<class Strategy>
    void initialize_datas_mask(MaskByValue<Strategy> & datas, size_t icat) {
        int interval = this->get_interval<Strategy>();
        datas.resize(interval + 1);
        size_t value = 0;
        auto const d = interval/10u / ocr_div;
        for (std::vector<uint64_t> & data_mask : datas) {
            data_mask.resize((nb_u64[icat] + 63) / 64);
            //data_mask.resize(group_glyphs.size());
            size_t idef = 0;
            auto first = this->get<Strategy>().begin() + shift_u64[icat];
            auto last = first + nb_u64[icat];
            for (; first != last; ++first) {
                auto const sig_value = *first / ocr_div;
                if (value < sig_value ? (sig_value <= value + d) : (value <= sig_value + d)) {
                    data_mask[idef / 64] |= 1ull << (idef % 64);
                }
                ++idef;
            }
            ++value;
        }
    }

    template<class Strategy>
    loader2::Data<Strategy> const & get() const
    { return this->datas.template get<Strategy>(); }

    compute_image_data_type(loader2::Glyphs const & glyphs, PpOcrDatas const & datas)
    : glyphs(glyphs)
    , datas(datas)
    , nb_u64([&]() {
        std::vector<unsigned> cat;
        int interval = this->get_interval<FirstStrategy>();
        cat.resize(interval + 1);
        for (int x : this->get<FirstStrategy>()) {
            int min = std::max<int>(x / ocr_div - interval/10, 0);
            int max = std::min<int>(x / ocr_div + interval/10, interval);
            for (; min <= max; ++min) {
                ++cat[min];
            }
        }
        return cat;
    }())
    , shift_u64([&]() {
        std::vector<unsigned> cat;
        int interval = this->get_interval<FirstStrategy>();
        cat.resize(interval + 1);
        for (auto x : this->get<FirstStrategy>()) {
            ++cat[x / ocr_div];
        }
        int d = interval/10 + 1;
        std::partial_sum(begin(cat), end(cat), begin(cat));
        std::copy_backward(begin(cat), end(cat)-d, end(cat));
        std::fill(begin(cat), begin(cat)+d, 0);
        assert(cat.back() + nb_u64.back() == this->get<FirstStrategy>().size());
        return cat;
    }())
    , datas_accepted((*std::max_element(std::begin(nb_u64), std::end(nb_u64)) + 63) / 64)
    , datas_defs([&]{
        std::vector<DatasMaskByValue> datas_defs;
        datas_defs.resize(nb_u64.size());
        for (size_t icat = 0; icat < datas_defs.size(); ++icat) {
            void(std::initializer_list<char>{((
                initialize_datas_mask<Strategies>(datas_defs[icat], icat)
            ), char())...});
        }

        return datas_defs;
    }())
    , probabilities(glyphs.size())
    , tmp_probabilities(glyphs.size())
    {
        this->result1.reserve(100);
        this->result2.reserve(100);
    }
};

template<class Data>
std::vector<unsigned> const & get_data(Data const & data) {
    return data.data();
}

template<class Store, class... Strategies>
double compute_probability(
    Store const & store, size_t idata,
    compute_image_data_type<Strategies...> & o,
    unsigned const * intervals
) {
    double prob = 1;

    using cont_t = std::array<std::reference_wrapper<std::vector<unsigned> const>, sizeof...(Strategies)>;
    cont_t datas_ref {{get_data(o.template get<Strategies>())...}};

    for (size_t i = 0; i < datas_ref.size(); ++i) {
        auto const interval = intervals[i];
        auto d = interval/10u;
        auto sig_value = datas_ref[i].get()[idata] / ocr_div;
        auto value = reinterpret_cast<unsigned const *>(&store)[i] / ocr_div;
        prob = prob * (value < sig_value
            ? (sig_value <= value + d ? (interval - (sig_value - value)) * 100u / interval : 0u)
            : (value <= sig_value + d ? (interval - (value - sig_value)) * 100u / interval : 0u)
        ) / 100;
    }

    return prob;
}

template<class Strategy>
using data_to_strategy = typename Strategy::strategy_type;

template<class LoaderStrategy>
struct data_for_strategy {
    using strategy_type = data_to_strategy<LoaderStrategy>;

    typename strategy_type::value_type x;

    typename strategy_type::value_type
    load(Image const & img, Image const & img90) {
        x = loader2::load(strategy_type(), LoaderStrategy::policy, img, img90);
        return x;
    }
};

template<class Strategy>
void reduce_complexe_universe(
    probabilities_t & in,
    probabilities_t & out,
    loader2::Data<Strategy> const & data, Image const & img, Image const & img90
) {
    data_for_strategy<Strategy> store;
    store.load(img, img90);
    auto const count = data.count_posibilities() - 1;
    auto const limit = (count+1) / 2;
    out.clear();
    for (auto & prob : in) {
        auto const x = data.relationship(data[prob.i], store.x);
        if (x >= limit) {
            out.emplace_back(prob.i, prob.prob * x / count);
        }
    }
    swap(in, out);
}

template<template<class...> class Temp, class... Strategies>
void reduce_complexe_universe(
    Temp<Strategies...>,
    probabilities_t & in,
    probabilities_t & out,
    PpOcrDatas const & datas, Image const & img, Image const & img90
) {
    void(std::initializer_list<char>{((void)(
        reduce_complexe_universe(in, out, datas.get<Strategies>(), img, img90)
    ), char())...});
}

template<class Strategy>
struct data_exclusive_universe : data_for_strategy<Strategy> {
    size_t limit;
};

template<class Store>
bool has_value(PpOcrDatas const &, unsigned, Store const &) {
    return true;
}

template<class Store, class Strategy, class... Strategies>
bool has_value(
    PpOcrDatas const & datas, unsigned i, Store const & store,
    Strategy, Strategies... other
) {
    if (datas.get<Strategy>().relationship(
        datas.get<Strategy>()[i],
        static_cast<data_for_strategy<Strategy> const&>(store).x
       ) >= static_cast<data_exclusive_universe<Strategy> const&>(store).limit
    ) {
        return has_value(datas, i, store, other...);
    }
    return false;
}

template<class Data>
size_t get_count(Data const & data) {
    return data.count_posibilities();
}

template<template<class...> class Temp, class DataByWord, class... Strategies>
void reduce_exclusive_universe(
    Temp<Strategies...>,
    probabilities_t & in,
    probabilities_t & out,
    PpOcrDatas const & datas,
    loader2::Glyphs const & /*glyphs*/,
    Image const & img, Image const & img90,
    DataByWord const & data_by_word
) {
    out.clear();

    if (in.empty()) {
        return ;
    }

    struct
    :  data_exclusive_universe<Strategies>...
    {} store;
    void(std::initializer_list<char>{((void)((
        static_cast<data_for_strategy<Strategies>&>(store).load(img, img90),
        (static_cast<data_exclusive_universe<Strategies>&>(store).limit = get_count(datas.get<Strategies>())/2)
    )), char())...});

    for (auto & prob : in) {
        if (data_by_word[prob.i].empty()) {
            out.push_back(prob);
        }
        else {
            for (auto i : data_by_word[prob.i]) {
                if (has_value(datas, i, store, Strategies()...)) {
                    out.push_back(prob);
                    break;
                }
            }
        }
    }
    swap(in, out);
}

// template<class Strategy>
// void reduce_universe(
//     probabilities_t & a,
//     unsigned value,
//     loader2::Data<Strategy> const & data
// ) {
//     auto d = (data.count_posibilities()-1)/10u;
//     a.resize(std::remove_if(a.begin(), a.end(), [&](Probability const & prop) {
// //         return value != data[prop.i];
//         return (value < data[prop.i] ? (data[prop.i] > value + d) : (value > data[prop.i] + d));
//     }) - a.begin());
// }
//
// template</*class FirstStrategy, class SecondStrategy, */class... Strategies>
// void compute_image(
//     compute_image_data_type</*FirstStrategy, SecondStrategy, */Strategies...> & o,
//     spell::Dictionary const & dict, Image & img
// ) {
//     struct
//     : /*data_for_strategy<FirstStrategy>
//     , data_for_strategy<SecondStrategy>
//     , */data_for_strategy<Strategies>...
//     {} store;
//
//     auto img90 = img.rotate90();
//
//
//
//     o.probabilities.clear();
//     for (unsigned i = 0; i < o.datas.size(); ++i) {
//         o.probabilities.emplace_back(i, 1.0);
//     }
//     void(std::initializer_list<char>{(void(reduce_universe(
//         o.probabilities,
//         static_cast<data_for_strategy<Strategies>&>(store).load(img, img90),
//         o.datas.template get<Strategies>()
//     )), char())...});
//
//     for (auto & prob : o.probabilities) {
//         void(std::initializer_list<char>{(void(update_probability(
//             prob.prob,
//             prob.i,
//             static_cast<data_for_strategy<Strategies>&>(store).load(img, img90),
//             o.datas.template get<Strategies>()
//         )), char())...});
//     }

template<class FirstStrategy, class SecondStrategy, class... Strategies, class DataByWord>
void compute_image(
    compute_image_data_type<FirstStrategy, SecondStrategy, Strategies...> & o,
    spell::Dictionary const & /*dict*/, Image & img, DataByWord const & data_by_word
) {
    struct
    : data_for_strategy<FirstStrategy>
    , data_for_strategy<SecondStrategy>
    , data_for_strategy<Strategies>...
    {} store;

    auto img90 = img.rotate90();

    static_cast<data_for_strategy<FirstStrategy>&>(store).load(img, img90);
    static_cast<data_for_strategy<SecondStrategy>&>(store).load(img, img90);
    void(std::initializer_list<char>{((void)(
        static_cast<data_for_strategy<Strategies>&>(store).load(img, img90)
    ), char())...});

    auto const icat = static_cast<data_for_strategy<FirstStrategy>&>(store).x / ocr_div;
    auto const nb_u64 = o.nb_u64[icat];
    auto & datas_defs = o.datas_defs[icat];

    o.datas_accepted = datas_defs.template masks<SecondStrategy>()[
        static_cast<data_for_strategy<SecondStrategy>&>(store).x / ocr_div
    ];
    void(std::initializer_list<char>{((void)(reduce_universe(
        o.datas_accepted,
        datas_defs.template masks<Strategies>(),
        static_cast<data_for_strategy<Strategies>&>(store).x / ocr_div
    )), char())...});

    unsigned const intervals[] = {
        unsigned(o.template get_interval<FirstStrategy>() / ocr_div),
        unsigned(o.template get_interval<SecondStrategy>() / ocr_div),
        unsigned(o.template get_interval<Strategies>() / ocr_div)...
    };

    o.probabilities.clear();
    auto const shift_idx = o.shift_u64[icat];
    for (size_t i = 0; i < nb_u64; ++i) {
        if (o.datas_accepted[i/64] & (1ull << (i %  64))) {
            o.probabilities.push_back({
                unsigned(shift_idx+i),
                compute_probability(store, shift_idx+i, o, intervals)
            });
        }
    }

    o.tmp_probabilities.resize(o.probabilities.size());
    std::copy(o.probabilities.begin(), o.probabilities.end(), o.tmp_probabilities.begin());
    sort_and_show_infos(o.tmp_probabilities, o.glyphs);

    std::pair<std::string, std::vector<std::string>> cache_element;

    auto get_views = [&](Probability const prob) -> loader2::Views const & {
        return o.glyphs[prob.i].views;
    };
    auto get_word = [&](Probability const prob) -> std::string const & {
        return get_views(prob)[0].word;
    };

    if (o.tmp_probabilities.empty()) {
        o.result1 += '_';
        cache_element.first = '_';
    }
    else {
        o.result1 += get_word(o.tmp_probabilities.front());
        cache_element.first = get_word(o.tmp_probabilities.front());
    }

    if (o.tmp_probabilities.empty()) {
        o.result2 += '_';
        o.ambiguous.push_back({});
        cache_element.second.push_back({"_"});
    }
    else if (o.tmp_probabilities[0].prob >= 1/* && tmp_probabilities.size() == 1*/) {
        o.result2 += get_word(o.tmp_probabilities[0]);
        for (auto & prob : o.tmp_probabilities) {
            if (!(prob.prob >= 1)) {
                break;
            }
            for (auto & v : get_views(prob)) {
                cache_element.second.push_back(v.word);
            }
        }
        o.ambiguous.push_back(cache_element.second);
    }
    else {
        reduce_complexe_universe(PpOcrComplexDatas(), o.probabilities, o.tmp_probabilities, o.datas, img, img90);
        reduce_exclusive_universe(PpOcrExclusiveDatas(), o.probabilities, o.tmp_probabilities, o.datas, o.glyphs, img, img90, data_by_word);

        sort_and_show_infos(o.probabilities, o.glyphs);
        if (o.probabilities.empty()) {
            o.result2 += '_';
            o.ambiguous.push_back({});
            cache_element.second.push_back({"_"});
        }
        else {
            auto prob_limit = o.probabilities.front().prob;
            for (auto & prob : o.probabilities) {
                if (prob.prob < prob_limit) {
                    break;
                }
                for (auto & v : get_views(prob)) {
                    cache_element.second.push_back(v.word);
                }
            }
            o.ambiguous.push_back(cache_element.second);
            o.result2 += get_word(o.probabilities.front());
        }
    }

    o.result1 += ' ';
    o.result2 += ' ';

    o.sorted_img.emplace(std::move(img), std::move(cache_element));
}

namespace {
struct ReindexDatas {
    std::vector<unsigned> const & indexes;
    template<class Strategy>
    void operator()(loader2::Data<Strategy> & data) const {
        auto cont = data.release();
        utils::reindex(indexes, cont);
        data = loader2::Data<Strategy>(cont);
    }
};
}

using DataByWord = std::vector<std::vector<unsigned>>;

DataByWord make_data_by_word(loader2::Glyphs const & glyphs) {
    std::map<std::string, std::vector<unsigned>> map;

    unsigned n = 0;
    for (auto & glyph : glyphs) {
        auto & e = map[glyph.views.front().word];
        e.emplace_back(n++);
    }

    DataByWord data_by_word;
    data_by_word.resize(glyphs.size());
    for (auto & p : map) {
        if (p.second.size() > 1) {
            auto tmp = p.second.back();
            p.second.pop_back();
            data_by_word[tmp] = p.second;
            for (auto & i : p.second) {
                using std::swap;
                swap(i, tmp);
                data_by_word[tmp] = p.second;
            }
        }
    }
    return data_by_word;
}

}

int main(int ac, char **av)
{
    if (ac < 4) {
        std::cerr << av[0] << "glyphs datas images [dict.trie]\n";
        return 1;
    }
    std::boolalpha(std::cout);

    spell::Dictionary dict;
    if (ac > 4) {
        utils::read_file(dict, av[4]);
        std::cout << " ## dict ? " << !dict.empty() << "\n";
    }

    auto datas = utils::load_from_file<PpOcrDatas>(av[2]);
    std::cout << " ## datas.size(): " << datas.size() << "\n";

    auto glyphs = utils::load_from_file<loader2::Glyphs>(av[1]);
    std::cout << " ## glyphs.size(): " << glyphs.size() << "\n";

    if (glyphs.size() != datas.size()) {
        throw std::bad_array_new_length();
    }

    for (auto & glyph : glyphs) {
        glyph.views.erase(std::unique(
            glyph.views.begin(), glyph.views.end(),
            [](loader2::View const & v1, loader2::View const & v2) { return v1.word == v2.word; }
        ), glyph.views.end());
    }

    std::cout << " ## sort\n";

    {
        using index_type = unsigned;
        using integer_iterator = ppocr::integer_iterator<index_type>;
        std::vector<index_type> indexes(
            integer_iterator{index_type{0}},
            integer_iterator{static_cast<index_type>(datas.size())}
        );
        assert(indexes.size() == datas.size());

        using strategy1 = typename index_pack<first_algo, PpOcrDatas>::type;
        std::sort(indexes.begin(), indexes.end(), [&](index_type i1, index_type i2) {
            return datas.get<strategy1>()[i1] < datas.get<strategy1>()[i2];
        });

        utils::reindex(indexes, glyphs);
        loader2::apply_from_datas(datas, ReindexDatas{indexes});
    }

    std::cout << " ## sort by word\n";

    auto data_by_word = make_data_by_word(glyphs);

    std::cout << " ## sort by value\n";

    typename mk_from_pack<compute_image_data_type, PpOcrSimpleDatas, algo_indexes>::type
    compute_image_data(glyphs, datas);

    std::cout << " ## go\n";

    Image img = image_from_file(av[3]);
    Bounds const bounds(img.width(), img.height());
    unsigned x = 0;

    using resolution_clock = std::chrono::high_resolution_clock;
    auto t1 = resolution_clock::now();
    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //min_y = std::min(cbox.y(), min_y);
        //bounds_y = std::max(cbox.y() + cbox.h(), bounds_y);
        //bounds_x = cbox.x() + cbox.w();
        //std::cerr << "\nbox(" << cbox << ")\n";

        Image img_word = img.section(cbox.index(), cbox.bounds());
        //std::cout << img_word;

        if (x + 4 < cbox.x()) {
            compute_image_data.ambiguous.push_back({" "});
        }

        auto it = compute_image_data.sorted_img.find(img_word);
        if (it != compute_image_data.sorted_img.end()) {
            compute_image_data.result1 += it->second.first;
            compute_image_data.result2 += it->second.second.front();
            compute_image_data.ambiguous.push_back(it->second.second);
            compute_image_data.result1 += ' ';
            compute_image_data.result2 += ' ';
        }
        else {
            std::cout << img_word;
            compute_image(compute_image_data, dict, img_word, data_by_word);
        }

        x = cbox.x() + cbox.width();
    }

    auto t2 = resolution_clock::now();
    std::cerr << std::chrono::duration<double>(t2-t1).count() << "s\n";

    std::string result3;
    auto search_fn = [](std::vector<std::string> const & s) {
        return s.empty() || (s.front().size() == 1
            && (s.front().front() == ' '
             || s.front().front() == '\''
             || s.front().front() == ','
             // punct
             || s.front().front() == '.'
             || s.front().front() == '!'
             || s.front().front() == '?'
             || s.front().front() == '"'
            )
        );
    };
    spell::WordDisambiguouser word_disambiguouser;
    auto first = compute_image_data.ambiguous.begin();
    auto last = compute_image_data.ambiguous.end();
    first = std::find_if_not(first, last, search_fn);
    while (first != last) {
        auto middle = std::find_if(first, last, search_fn);
        if (!word_disambiguouser(dict, first, middle, result3)) {
            for (; first != middle; ++first) {
                if (!first->empty()) {
                    result3 += first->front();
                }
            }
        }

        for (; middle != last; ++middle) {
            if (middle->empty()) {
                result3 += '?';
            }
            else if (search_fn(*middle)) {
                result3 += middle->front();
            }
            else {
                break;
            }
        }
        first = middle;
    }

    auto t3 = resolution_clock::now();
    std::cerr << std::chrono::duration<double>(t3-t1).count() << "s\n";

    std::cout
      << " ## result1: " << (compute_image_data.result1)
      << "\n ## result2: " << (compute_image_data.result2)
      << "\n ## result3: " << (result3)
      << std::endl
    ;
}
