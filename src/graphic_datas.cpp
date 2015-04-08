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
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <map>

#include <cstring>
#include <cerrno>

struct EventBySignature
{
    std::vector<unsigned> events_by_sig_;
    unsigned max_events_ = 0;
    unsigned min_ = 0;
    unsigned max_ = 0;

public:
    EventBySignature() = default;

    template<class G>
    void compute(std::vector<Definition> const & definitions, G g, size_t total)
    {
        this->events_by_sig_.clear();
        this->events_by_sig_.resize(total);
        this->max_events_ = 0;
        this->min_ = total;
        this->max_ = 0;

        for (Definition const & def : definitions) {
            unsigned const n = g(def.datas);
            this->max_events_ = std::max(this->max_events_, ++this->events_by_sig_[n]);
            this->min_ = std::min(this->min_, n);
            this->max_ = std::max(this->max_, n);
        }
        ++this->max_;
    }

    unsigned min() const noexcept { return this->min_; }
    unsigned max() const noexcept { return this->max_; }
    unsigned max_events() const noexcept { return this->max_events_; }

    std::vector<unsigned>::const_iterator begin() const { return this->events_by_sig_.begin(); }
    std::vector<unsigned>::const_iterator end() const { return this->events_by_sig_.end(); }
    unsigned operator[](size_t i) const { return this->events_by_sig_[i]; }
};

class Graphic
{
    std::vector<char> graph;

public:
    Graphic() = default;

    void compute(EventBySignature const & events_by_sig, size_t total)
    {
        graph.clear();
        graph.resize((events_by_sig.max_events() + 1) * total, '-');
        unsigned x = 0;
        for (auto n : events_by_sig) {
            auto y = total * (events_by_sig.max_events() - n);
            graph[y + x] = 'x';
            while ((y += total) < graph.size()) {
                graph[y + x] = '#';
            }
            ++x;
        }
    }

    std::vector<char>::const_iterator begin() const { return this->graph.begin(); }
    std::vector<char>::const_iterator end() const { return this->graph.end(); }
};


using string_ref_t = std::reference_wrapper<std::string const>;


class ProbLetterOnValue
{
    using container_type = std::map<string_ref_t, std::map<unsigned, unsigned>, std::less<std::string>>;
    container_type m;

public:
    ProbLetterOnValue() = default;

    template<class G>
    void compute(std::vector<Definition> const & definitions, G g)
    {
        this->m.clear();
        for (Definition const & def : definitions) {
            ++this->m[def.c][g(def.datas)];
        }
    }

    container_type::const_iterator begin() const { return this->m.begin(); }
    container_type::const_iterator end() const { return this->m.end(); }
    container_type::mapped_type const & operator[](std::string const & s) const
    { return this->m.find(string_ref_t(s))->second; }
};


class ProbValueOnLetter
{
    using container_type = std::map<unsigned, std::map<string_ref_t, unsigned, std::less<std::string>>>;
    container_type m;

public:
    ProbValueOnLetter() = default;

    template<class G>
    void compute(std::vector<Definition> const & definitions, G g)
    {
        this->m.clear();
        for (Definition const & def : definitions) {
            ++this->m[g(def.datas)][def.c];
        }
    }

    container_type::const_iterator begin() const { return this->m.begin(); }
    container_type::const_iterator end() const { return this->m.end(); }
};


template<class G>
void run(
    EventBySignature & events_by_sig,
    Graphic & graph,
    ProbLetterOnValue & prob_letter_on_value,
    ProbValueOnLetter & prob_value_on_letter,
    std::vector<Definition> const & definitions,
    G g, size_t total_sig
) {
    events_by_sig.compute(definitions, g, total_sig);

    graph.compute(events_by_sig, total_sig);
    {
        auto ievent = events_by_sig.max_events();
        for (auto first = graph.begin(); first != graph.end(); first += total_sig) {
            std::cout << std::setw(4) << ievent-- << ' ';
            std::cout.write(&*first, total_sig) << "\n";
        }
        std::cout << "\n";
    }

    std::cout
      << " [" << events_by_sig.min() << " - "
      << events_by_sig.max() << "] ; max_events: "
      << events_by_sig.max_events() << "\n\n"
    ;

    auto const total_events = double(definitions.size());

    if (1) {
        std::fixed(std::cout);
        std::cout.precision(6);

        prob_letter_on_value.compute(definitions, g);
        {
            for (auto & p : prob_letter_on_value) {
                std::cout << "\033[00;35m" << p.first.get() << "\033[0m  ";
                for (auto & p2 : p.second) {
                    std::cout
                      << "P(" << double(p2.second) / total_events
                      << " /" << std::setw(3) << p2.first << ")  "
                    ;
                }
                std::cout << "\n";
            }
        }

        std::cout << "\n\n";

        prob_value_on_letter.compute(definitions, g);
        {
            for (auto & p : prob_value_on_letter) {
                std::cout
                  << "\033[00;35m" << std::setw(3) << p.first
                  << "\033[0m  P(" << double(events_by_sig[p.first]) / total_events << ")  "
                ;
                for (auto & p2 : p.second) {
                    auto const & values = prob_letter_on_value[p2.first];
                    auto const sum = double(std::accumulate(
                        values.begin(), values.end(), 0u,
                        [](unsigned a, std::pair<unsigned const, unsigned> const & pair){ return a + pair.second; }
                    ));
                    std::cout << "P("
                      << double(p2.second) / total_events << " /"
                      << p2.first.get() << " ["
                      << std::setprecision(3)
                      << double(p2.second) / sum << "])  "
                      << std::setprecision(6)
                    ;
                }
                std::cout << "\n";
            }
        }

        std::cout.unsetf(std::ios::floatfield);
    }
}



int main(int ac, char ** av) {
    if (ac < 1) {
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

    std::cout << "definitions.size = " << definitions.size() << "\n\n";


    auto get_value = [](DataLoader::data_base const & data) {
        return *reinterpret_cast<unsigned const *>(
            reinterpret_cast<unsigned char const *>(&data) + sizeof( DataLoader::data_base)
        );
    };

    EventBySignature events_by_sig;
    Graphic graph;
    ProbLetterOnValue prob_letter_on_value;
    ProbValueOnLetter prob_value_on_letter;

    unsigned intervals[] = {
        strategies::hdirection::traits::get_interval(),
        strategies::hdirection90::traits::get_interval(),
//         strategies::hdirection2::traits::get_interval(),
//         strategies::hdirection290::traits::get_interval(),
//         strategies::hgravity::traits::get_interval(),
//         strategies::hgravity90::traits::get_interval(),
//         strategies::hgravity2::traits::get_interval(),
//         strategies::hgravity290::traits::get_interval(),
//         strategies::proportionality::traits::get_interval(),
//         strategies::dvdirection::traits::get_interval(),
//         strategies::dvdirection90::traits::get_interval(),
//         strategies::dvdirection2::traits::get_interval(),
//         strategies::dvdirection290::traits::get_interval(),
//         strategies::dvgravity::traits::get_interval(),
//         strategies::dvgravity90::traits::get_interval(),
//         strategies::dvgravity2::traits::get_interval(),
//         strategies::dvgravity290::traits::get_interval(),
//         strategies::density::traits::get_interval(),
    };
    for (size_t i = 0; i < sizeof(intervals)/sizeof(intervals[0]); ++i) {
        std::cout << "name: " << loader.names()[i] << "\n";
        run(
            events_by_sig,
            graph,
            prob_letter_on_value,
            prob_value_on_letter,
            definitions,
            [&get_value, i](DataLoader::Datas const & datas) { return get_value(datas[i]); },
            intervals[i] + 1
        );
    }


    auto const total_sig1 = (intervals[0] + 1) / 10;
    auto const total_sig2 = (intervals[1] + 1) / 10;
    auto const total_sig = total_sig1 * total_sig2;
//     run(
//         events_by_sig,
//         graph,
//         prob_letter_on_value,
//         prob_value_on_letter,
//         definitions,
//         [total_sig1, &get_value](DataLoader::Datas const & datas) {
//             return (get_value(datas[0]) * total_sig1 + get_value(datas[1])) / 10;
//         },
//         total_sig
//     );
    run(
        events_by_sig,
        graph,
        prob_letter_on_value,
        prob_value_on_letter,
        definitions,
        [total_sig2, &get_value](DataLoader::Datas const & datas) {
            return (get_value(datas[0]) + get_value(datas[1]) * total_sig2) / 10;
        },
        total_sig
    );

    auto first = definitions.begin();
    auto last = definitions.end();
    auto pos = first;
    std::reference_wrapper<std::string const> sref(first->c);
    std::vector<Definition> new_defs;
    while (1) {
        bool const is_finish = ++first == last;
        if (is_finish || first->c != sref.get()) {
            std::cout << "\n# " << sref.get() << "\n";
            new_defs.assign(std::make_move_iterator(pos), std::make_move_iterator(first));
            run(
                events_by_sig,
                graph,
                prob_letter_on_value,
                prob_value_on_letter,
                new_defs,
                [total_sig2, get_value](DataLoader::Datas const & datas) {
                    return (get_value(datas[0]) + get_value(datas[1]) * total_sig2) / 10;
                },
                total_sig
            );
            std::move(new_defs.begin(), new_defs.end(), pos);
            if (!is_finish) {
                pos = first;
                sref = first->c;
            }
        }

        if (is_finish) {
            break;
        }
    }


//     run(
//         events_by_sig,
//         graph,
//         prob_letter_on_value,
//         prob_value_on_letter,
//         definitions,
//         [intervals, &get_value](DataLoader::Datas const & datas) {
//             auto const n1 = get_value(datas[0]);
//             if (!(n1 <= ((intervals[0] + 1)*55/100) && n1 >= ((intervals[0] + 1)*45/100))) {
//                 return false;
//             }
//             auto const n2 = get_value(datas[1]);
//             return (n2 <= ((intervals[1] + 1)*55/100) && n2 >= ((intervals[1] + 1)*45/100));
//         },
//         2
//     );





//     auto const total_sig = strategies::hdirection::traits::get_interval() + 1;
//     auto const total_events = definitions.size();
//
//     auto get_value = [](DataLoader::data_base const & data) {
//         return *reinterpret_cast<unsigned const *>(
//             reinterpret_cast<unsigned char const *>(&data) + sizeof( DataLoader::data_base)
//         );
//     };
//
//     std::vector<unsigned> events_by_sig(total_sig);
//     unsigned max_events = 0;
//     for (Definition & def : definitions) {
//         max_events = std::max(max_events, ++events_by_sig[get_value(def.datas[0])]);
//     }
//
//     {
//         std::vector<char> graph((max_events + 1) * total_sig, '-');
//         unsigned x = 0;
//         for (auto n : events_by_sig) {
//             auto y = total_sig * (max_events - n);
//             graph[y + x] = 'x';
//             while ((y += total_sig) < graph.size()) {
//                 graph[y + x] = '#';
//             }
//             ++x;
//         }
//
//         auto ievent = max_events;
//         for (auto first = graph.begin(); first != graph.end(); first += total_sig) {
//             std::cout << std::setw(4) << ievent-- << ' ';
//             std::cout.write(&*first, total_sig) << "\n";
//         }
//         std::cout << "\n";
//     }
//
//     unsigned const sig_min = std::find_if(
//         events_by_sig.begin(), events_by_sig.end(), [](unsigned i) { return i; }
//     ) - events_by_sig.begin();
//     unsigned const sig_max = std::find_if(
//         events_by_sig.rbegin(), events_by_sig.rend() + sig_min, [](unsigned i) { return i; }
//     ).base() - events_by_sig.begin();
//
//     std::cout << " [" << sig_min << " - " << sig_max << "] ; max_events: " << (max_events) << "\n";
//
//
//     using string_ref_t = std::reference_wrapper<std::string const>;
//
//     {
//         std::map<string_ref_t, std::map<unsigned, unsigned>, std::less<std::string>> m;
//         for (Definition const & def : definitions) {
//             ++m[def.c][get_value(def.datas[0])];
//         }
//
//         for (auto & p : m) {
//             std::cout << "\033[00;35m" << p.first.get() << "\033[0m  ";
//             for (auto & p2 : p.second) {
//                 std::cout << "P(" << double(events_by_sig[p2.first]) / double(total_events) << " /" << p2.first << ")  ";
//             }
//             std::cout << "\n";
//         }
//     }
//
//     std::cout << "\n\n";
//
//     {
//         std::map<unsigned, std::map<string_ref_t, unsigned, std::less<std::string>>> m;
//         for (Definition const & def : definitions) {
//             ++m[get_value(def.datas[0])][def.c];
//         }
//
//         for (auto & p : m) {
//             std::cout << "\033[00;35m" << std::setw(3) << p.first << "\033[0m  ";
//             for (auto & p2 : p.second) {
//                 std::cout << "P(" << double(p2.second) / double(total_events) << " /" << p2.first.get() << ")  ";
//             }
//             std::cout << "\n";
//         }
//     }



//     std::vector<std::set<std::reference_wrapper<std::string const>, std::less<std::string>>> map(w*h);
//
//     for (Definition const & def : definitions) {
//         if (def.c.size() != 1) {
//             continue;
//         }
//         DataLoader::Datas const & datas = def.datas;
//         auto get_value = [](DataLoader::data_base const & data) {
//             return *reinterpret_cast<unsigned const *>(
//                 reinterpret_cast<unsigned char const *>(&data) + sizeof( DataLoader::data_base)
//             );
//         };
//         unsigned const x = get_value(datas[0]);
//         unsigned const y = get_value(datas[1]);
//         map[y*w+x].insert(def.c);
//     }
//
//     auto it = map.begin();
//     for (size_t x = 0; x < w; ++x) {
//         for (size_t y = 0; y < h; ++y, ++it) {
//             if (it->empty()) {
//                 std::cout << "-";
//             }
//             else {
//                 for (auto & s : *it) {
//                     std::cout << s.get();
//                 }
//             }
//         }
//         std::cout << "\n";
//     }




//     struct Column { std::string c; unsigned value; };
//     std::vector<Column> columns;
//
//     struct LessColumnValue {
//         LessColumnValue() {}
//         bool operator()(Column const & c1, Column const & c2) const noexcept
//         { return c1.value < c2.value; }
//     };
//
//     std::vector<std::vector<std::reference_wrapper<std::string const>>> characters_by_columns;
//
//     unsigned coef[] = {
//         strategies::hdirection::traits::get_interval(),
//         strategies::hdirection90::traits::get_interval(),
//         strategies::hdirection2::traits::get_interval(),
//         strategies::hdirection290::traits::get_interval(),
//         strategies::hgravity::traits::get_interval(),
//         strategies::hgravity90::traits::get_interval(),
//         strategies::hgravity2::traits::get_interval(),
//         strategies::hgravity290::traits::get_interval(),
//         strategies::proportionality::traits::get_interval(),
//         strategies::dvdirection::traits::get_interval(),
//         strategies::dvdirection90::traits::get_interval(),
//         strategies::dvdirection2::traits::get_interval(),
//         strategies::dvdirection290::traits::get_interval(),
//         strategies::dvgravity::traits::get_interval(),
//         strategies::dvgravity90::traits::get_interval(),
//         strategies::dvgravity2::traits::get_interval(),
//         strategies::dvgravity290::traits::get_interval(),
//         strategies::density::traits::get_interval(),
//         0,
//         0
//     };
//
//     for (size_t i = 0; i < 20; ++i) {
//         for (Definition const & def : definitions) {
//             DataLoader::Datas const & datas = def.datas;
//             using Data = DataLoader::data_base;
//             columns.push_back(Column{def.c, *reinterpret_cast<unsigned const *>(
//                 reinterpret_cast<unsigned char const *>(&datas[i]) + sizeof(Data)
//             )});
//         }
//
//         auto const minmax = std::minmax_element(columns.cbegin(), columns.cend(), LessColumnValue{});
//         unsigned const min = minmax.first->value;
//         unsigned const max = minmax.second->value;
//
//         characters_by_columns.clear();
//         characters_by_columns.resize(max - min + 1);
//
//         std::cout << "\n ## " << loader.names()[i] << "\n   [ " << min << "-" << max << " ]\n";
//
//         std::cout.fill('-');
//         right(std::cout);
//         char const * colors[]{
// //             "\033[07;31mx\033[0m",
// //             "\033[07;32mx\033[0m",
//             "\033[07;33mx\033[0m",
//             "\033[07;34mx\033[0m",
// //             "\033[07;35mx\033[0m",
//         };
//         unsigned icolor = 0;
//         unsigned dmax = 0;
//         auto first = columns.begin();
//         while (first != columns.end()) {
//             auto last = std::find_if(
//                 first+1, columns.end(),
//                 [first](Column const & c) { return c.c != first->c; }
//             );
//             std::sort(first, last, LessColumnValue{});
//             auto const dmax_tmp = (last-1)->value - first->value;
//             auto const dmin_tmp = std::inner_product(
//                 first+1, last, first, 0u,
//                 [](unsigned a, unsigned b) { return std::min(a, b); },
//                 [](Column const & c1, Column const & c2) { return c2.value - c1.value; }
//             );
//
//             std::cout << "  # dmin: " << dmin_tmp << "  dmax: " << dmax_tmp << "\n";
//             for (; first != last; ++first) {
//                 std::cout
//                   << std::setw(first->value) << ""
//                   << colors[icolor]
//                   << std::setw(max - first->value) << ""
//                   << " " << first->c << "\n"
//                 ;
//                 characters_by_columns[first->value - min].push_back(first->c);
//             }
//             icolor = (icolor + 1) % (sizeof(colors)/sizeof(colors[0]));
//             dmax = std::max(dmax_tmp, dmax);
//         }
//
//         std::cout << "\n";
//
//         std::cout.fill(' ');
//         left(std::cout);
//         unsigned n = min;
//         for (auto & characters : characters_by_columns) {
//             std::cout << std::setw(4) << n++ << " ";
//             for (std::string const & c : characters) {
//                 std::cout << c << " ";
//             }
//             std::cout << "\n";
//         }
//
//         std::cout << " # dmax: " << dmax << "   pmax: ";
//         if (coef[i]) {
//             std::cout << "+-" << (dmax * 50 / coef[i]) << "\n";
//         }
//         else {
//             std::cout << "???\n";
//         }
//
//         columns.clear();
//     }





//     std::string s;
//     while (std::cout << "> ") {
//         std::cin >> s;
//
//         if (std::cin.eof()) {
//             break;
//         }
//
//         if (!std::cin) {
//             std::cin.clear();
//             continue;
//         }
//
//         auto pos = std::lower_bound(
//             definitions.begin(), definitions.end(), s,
//             [](Definition const & def, std::string const & s) {
//                 return def.c < s;
//             }
//         );
//
//         columns.clear();
//         for (; pos != definitions.end() && pos->c == s; ++pos) {
//             std::cout << pos->img;
//             DataLoader::Datas const & datas = pos->datas;
//             for (size_t i = 0; i < 1/*20*/; ++i) {
//                 using Data = DataLoader::data_base;
//                 columns.push_back(Column{pos->c, *reinterpret_cast<unsigned const *>(
//                     reinterpret_cast<unsigned char const *>(&datas[i]) + sizeof(Data)
//                 )});
//             }
//         }
//
//         std::sort(
//             columns.begin(), columns.end(),
//             [](Column const & c1, Column const & c2) { return c1.c < c2.c; }
//         );
//
//         unsigned max = std::max_element(
//             columns.begin(), columns.end(),
//             [](Column const & c1, Column const & c2) { return c1.value < c2.value; }
//         )->value;
//         std::cout << "max: " << max << "\n";
//
//         std::cout.fill('-');
//         for (Column & c : columns) {
//             std::cout << std::setw(c.value) << "" << "x" << std::setw(max-c.value) << "-" << c.c <<  "\n";
//         }
//
//         std::cout << "\n";
//     }
}
