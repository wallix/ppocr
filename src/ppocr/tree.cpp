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

#include "ppocr/factory/data_loader.hpp"
#include "ppocr/factory/definition.hpp"
#include "ppocr/factory/registry.hpp"

#include "ppocr/image/image.hpp"

#include "ppocr/strategies/utils/basic_proportionality.hpp"
#include "ppocr/strategies/hdirection.hpp"
#include "ppocr/strategies/hdirection2.hpp"
#include "ppocr/strategies/hgravity.hpp"
#include "ppocr/strategies/hgravity2.hpp"
#include "ppocr/strategies/proportionality.hpp"
#include "ppocr/strategies/dvdirection.hpp"
#include "ppocr/strategies/dvdirection2.hpp"
#include "ppocr/strategies/dvgravity.hpp"
#include "ppocr/strategies/dvgravity2.hpp"
#include "ppocr/strategies/density.hpp"
#include "ppocr/strategies/dzdensity.hpp"

#include "ppocr/utils/read_definitions_file_and_normalize.hpp"

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


struct Node {
    std::vector<std::reference_wrapper<Definition const>> ref_defs;
    std::vector<Node> table;
};


template<class Definitions>
void init_node(Definitions const & definitions, Node & node, unsigned i, unsigned interval) {
    if (definitions.empty()) {
        return;
    }
    auto & table = node.table;
    auto const d = interval/10u;
    table.resize(d);
    for (Definition const & def : definitions) {
        auto const value = get_value(def.datas[i]) / 10;
        table[std::min(value, d-1u)].ref_defs.emplace_back(def);
        if (value > 0) {
            table[value-1].ref_defs.emplace_back(def);
        }
        if (value + 1 < d) {
            table[value+1].ref_defs.emplace_back(def);
        }
    }
    for (Node & node : table) {
        std::sort(
            node.ref_defs.begin(), node.ref_defs.end(),
            [](Definition const & a, Definition const & b) {
                return a.datas < b.datas;
            }
        );
        node.ref_defs.erase(
            std::unique(
                node.ref_defs.begin(), node.ref_defs.end(),
                [](Definition const & a, Definition const & b) {
                    return a.datas == b.datas;
                }
            ), node
            .ref_defs.end()
        );
    }
}

template<class It>
void rec_init_node(
    It first_idx, It last_idx,
    Node & root_node,
    unsigned const * intervals,
    std::vector<unsigned> & values_trace
) {
    size_t i = 0;
    values_trace.push_back(i);
    size_t const idx_value = values_trace.size() - 1;
    for (Node & node : root_node.table) {
        values_trace[idx_value] = i;
        init_node(node.ref_defs, node, *first_idx, intervals[*first_idx]+1);
        node.ref_defs.clear();
        node.ref_defs.shrink_to_fit();
        if (first_idx+1 != last_idx) {
            auto next_idx = first_idx + 1;
            rec_init_node(next_idx, last_idx, node, intervals, values_trace);
        }
        else {
            size_t i_table = 0;
            for (Node const & n : node.table) {
                if (!n.ref_defs.empty()) {
//                     for (auto i : values_trace) {
//                         std::cout << std::setw(4) << i;
//                     }
//                     std::cout << std::setw(4) << i_table << "  ";
                    std::cout << n.ref_defs.size() << "\n";
//                     for (Definition const & def : n.ref_defs) {
//                         std::cout << " " << def.c;
//                     }
//                     std::cout << "\n";
                }
                ++i_table;
            }
        }
        node.table.clear();
        node.table.shrink_to_fit();
        ++i;
    }
    values_trace.pop_back();
}

void rec_node_print(std::vector<Node> const & nodes, unsigned depth = 0) {
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (!nodes[i].ref_defs.empty()) {
            std::cout << std::setw(depth + 2) << "  " << std::left << std::setw(3) << i << " " << nodes[i].ref_defs.size() << "\n";
            rec_node_print(nodes[i].table, depth + 6);
        }
    }
}


int main(int ac, char **av)
{
    if (ac < 2) {
        std::cerr << av[0] << " datas images\n";
        return 1;
    }

    DataLoader loader;
    all_registy(loader);

    std::vector<Definition> definitions = read_definitions_file_and_normalize(av[1], loader, &std::cout);
    definitions.shrink_to_fit();

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
    //constexpr size_t count_interval = sizeof(intervals) / sizeof(intervals[0]);

    //constexpr unsigned indexes[] {16, 12, 18, 19, 11, 15, 14, 10, 13, 17, 9, 5, 4, 1, 7, 8, 3, 0, 6, 2};
    constexpr unsigned indexes[] {19, 16, 15};

    Node root_node;
    init_node(definitions, root_node, indexes[0], intervals[indexes[0]]+1);
    std::vector<unsigned> values_trace;
    rec_init_node(&indexes[1], &indexes[3], root_node, intervals, values_trace);

    //rec_node_print(root_node.table);
}
