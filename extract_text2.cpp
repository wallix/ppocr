#include "image.hpp"
#include "data_loader.hpp"
#include "make_box_character.hpp"
#include "registry.hpp"
#include "definition.hpp"
#include "box.hpp"
// #include "utils/unique_sort_definition.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <cstring>
#include <cerrno>

int main(int ac, char **av)
{
    if (ac < 3) {
        std::cerr << av[0] << " datas images [-i]\n";
        return 1;
    }

    std::ifstream file(av[1]);
    if (!file) {
        std::cerr << strerror(errno) << '\n';
        return 2;
    }

    DataLoader loader;
    all_registy(loader);

    struct DataCompareParametrable
    {
        std::vector<bool> enable_seq;

        bool lt(const DataLoader::Data& a, const DataLoader::Data& b) const
        {
            auto enable_it = this->enable_seq.begin();
            auto it2 = b.datas().begin();
            for (DataLoader::data_base const & data : a.datas()) {
                if (*enable_it) {
                    if (data.less(*it2)) {
                        return true;
                    }
                    if (!data.eq(*it2)) {
                        return false;
                    }
                }
                ++it2;
                ++enable_it;
            }
            return false;
        }

        bool eq(const DataLoader::Data& a, const DataLoader::Data& b) const
        {
            auto enable_it = this->enable_seq.begin();
            auto it2 = b.datas().begin();
            for (DataLoader::data_base const & data : a.datas()) {
                if (*enable_it) {
                    if (!data.eq(*it2)) {
                        return false;
                    }
                }
                ++it2;
                ++enable_it;
            }
            return true;
        }

        bool print(std::ostream & os, const DataLoader::Data& d, DataLoader const & loader) const
        {
            auto enable_it = this->enable_seq.begin();
            auto name_it = loader.names().begin();
            for (DataLoader::data_base const & data : d.datas()) {
//                 if (*enable_it) {
                    os << (*enable_it ? " + " : " - ") << *name_it << ": ";
                    data.write(os);
                    os << '\n';
//                 }
                ++name_it;
                ++enable_it;
            }
            return true;
        }

        std::vector<unsigned>
        relationships(const DataLoader::Data& a, const DataLoader::Data& b) const
        {
            std::vector<unsigned> ret;
            auto enable_it = this->enable_seq.begin();
            auto it2 = b.datas().begin();
            for (DataLoader::data_base const & data : a.datas()) {
                if (*enable_it) {
                    ret.push_back(data.relationship(*it2));
                }
                ++it2;
                ++enable_it;
            }
            return ret;
        }
    };

    DataCompareParametrable data_compare;
    data_compare.enable_seq.resize(loader.size(), true);

    std::vector<Definition> definitions;

    Image img = image_from_file(av[2]);
    //std::cerr << img << '\n';

    {
        Definition def;
        while (file) {
            read_definition(file, def, loader);
            if (def) {
                //std::cerr << def.c << ' ' << bnd <<  '\n' /*<< def.img*/ << def.data << std::endl;
                definitions.push_back(std::move(def));
            }
        }
    }

    if (!file.eof()) {
        std::cerr << "read error\n";
        return 5;
    }

    Bounds const bounds(img.width(), img.height());

    std::string s;
    std::istringstream iss;
    bool display_char = false;
    bool show_one_line = false;
    bool search_baseline = true;
    bool show_data_if_empty_range = false;
    unsigned conformity = 100;
    struct Proxy { Proxy() {} Definition const & operator()(Definition const & def) const { return def; } };
    using range_definition = range_iterator<decltype(definitions.begin()), Proxy>;
    std::vector<range_definition> def_ranges;

    //unique_sort_definitions(definitions);

    struct CharInfo {
        Box box;
        range_definition def_range;
        bool ok;
    };
    std::vector<CharInfo> char_infos;

    struct Line {
        std::string s;
        unsigned i;
    };
    std::vector<Line> info_lines;
    {
        std::ifstream file("/tmp/l");
        std::string s; unsigned i;
        while (file >> s >> i) {
            info_lines.push_back({s, i});
        }
    }

    do {
        def_ranges.clear();
        char_infos.clear();
        std::sort(definitions.begin(), definitions.end(), [&data_compare](auto & a, auto & b) {
            return data_compare.lt(a.data, b.data);
        });

        size_t x = [&] {
            size_t x = 0;
            auto d = img.data();
            for (; x < img.width(); ++x, ++d) {
                if (!utils::vertical_empty(d, img.bounds())) {
                    break;
                }
            }
            return x;
        }();
        size_t num_word = 0;

        size_t const min_x = x;
        size_t min_y = img.height();
        size_t bounds_x = 0;
        size_t bounds_y = 0;

        while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
            min_y = std::min(cbox.y(), min_y);
            bounds_y = std::max(cbox.y() + cbox.h(), bounds_y);
            bounds_x = cbox.x() + cbox.w();
            //std::cerr << "\nbox(" << cbox << ")\n";

            Image const img_word = img.section(cbox.index(), cbox.bounds());
            //std::cerr << img_word << '\n';

            //Def const def{{}, img_word, loader.new_data(img_word, img_word.rotate90())};
            auto data = loader.new_data(img_word, img_word.rotate90());
            //std::cerr << data << '\n';

            if (display_char && !show_one_line) {
                std::cout << img_word;
                std::cout.write(img_word.data(), img_word.area()) << '\n';
                std::cout << loader.writer(data);
            }

            auto it = std::lower_bound(
                definitions.begin(), definitions.end(), data
              , [&data_compare](Definition const & a, DataLoader::Data const & b) {
                return data_compare.lt(a.data, b);
              }
            );
            if (it == definitions.end()) {
                std::cout << "?\n";
                char_infos.push_back({cbox, range_definition(), false});
            }
            else {
                if (num_word < 10 && !show_one_line) {
                    std::cout << "0";
                }
                auto first = it;
                std::string * sref = &it->c;
                bool ok = true;
                {
                    auto it_to_end = [&](auto f) {
                        while (it != definitions.end() && f(it->data, data)) {
                            if (ok && it->c != *sref) {
                                ok = false;
                            }
                            sref = &it->c;
                            ++it;
                        }
                    };

                    if (conformity == 100) {
                        it_to_end([&](auto & a, auto & b) { return data_compare.eq(a, b); });
                    }
                    else {
                        it_to_end([&](auto & a, auto & b) {
                            for (auto percent : data_compare.relationships(a, b)) {
                                if (percent < conformity) {
                                    return false;
                                }
                            }
                            return true;
                        });
                    }
                }
                def_ranges.push_back({first, it, Proxy()});
                char_infos.push_back({cbox, def_ranges.back(), ok});
                if (first == it) {
                    std::cout << num_word << " \033[00;31m000\033[0m []";

                    if (show_data_if_empty_range) {
                        std::cout << "\n" << loader.writer(data);
                    }
                }
                else {
                    std::cout << num_word << " ";
                    if (ok) {
                        std::cout << "\033[00;32m";
                    }
                    auto const sz = it - first;
                    if (!show_one_line) {
                        if (sz < 100) {
                            std::cout << "0";
                        }
                        if (sz < 10) {
                            std::cout << "0";
                        }
                    }
                    std::cout << sz;
                    if (ok) {
                        std::cout << "\033[0m";
                    }
                    std::cout << " [";
                    for (; first !=  it; ++first) {
                        if (conformity != 100 && !ok) {
                            std::cout << "\n " << first->c << " ";
                            for (auto percent : data_compare.relationships(first->data, data)) {
                                std::cout << std::setw(3) << percent << "% ";
                            }
                        }
                        else {
                            std::cout << first->c << ' ';
                        }
                    }
                    std::cout << "\b]";
                }
                std::cout << (show_one_line ? " " : "\n");
                ++num_word;
            }

            x = cbox.x() + cbox.w();
        }

        if (search_baseline)
        {
            Index text_index(min_x, min_y);
            Bounds text_bounds(bounds_x-min_x, bounds_y-min_y);
            std::cout << "\n box: [" << text_index << " + " << text_bounds << "]\n";

            struct Stat { size_t n; size_t val; };
            struct StatEqVal {
                size_t val;
                bool operator()(const Stat & s) const { return s.val == val; }
            };
            std::vector<Stat> baseline_stats;
            std::vector<Stat> topline_stats;
            std::vector<Stat> bottomline_stats;

            //size_t middle = img.height()/2;
            size_t meanline = ~size_t(0);
            size_t baseline = ~size_t(0);
            for (CharInfo const & info : char_infos) {
//                 std::cout << info.ok;
                if (info.ok) {
                    Box const & box = info.box;
                    Definition const & def = info.def_range.front();
//                     std::cout << "  " << def.c;
                    auto p = std::find_if(info_lines.begin(), info_lines.end(), [&](Line const & l) {
                        return l.s == def.c;
                    });
                    if (p != info_lines.end()) {
                        if (p->i == 1) {
                            meanline = box.top();
                            baseline = box.bottom();
                            break;
                        }
                        else if ((p->i & 0b00110) == 0) {
                            meanline = box.top();
                        }
                        else if ((p->i & 0b11000) == 0) {
                            baseline = box.bottom();
                        }

                        if (meanline != ~size_t(0) && baseline != ~size_t(0)) {
                            break;
                        }
                    }
                }
//                 std::cout << "\n";
            }

            std::cout
              << "\n meanline: " << meanline
              << "\n baseline: " << baseline
              << "\n\n"
            ;

            for (CharInfo const & info : char_infos) {
                if (!info.ok && !info.def_range.empty()) {
                    Box const & box = info.box;
                    std::cout << info.def_range.front().c << ':';
                    for (Definition const & def : info.def_range) {
                        auto p = std::find_if(info_lines.begin(), info_lines.end(), [&](Line const & l) {
                            return l.s == def.c;
                        });
                        if (p != info_lines.end()) {
                            if (p->i == 1) {
                                if (meanline == box.top() && baseline == box.bottom()) {
                                    std::cout << ' ' << def.c;
                                }
                            }
                            else if ((p->i & 0b00110) == 0) {
                                if (meanline == box.top()
                                 && (
                                     ((p->i & 0b11000) == 0b01000 && box.bottom() < baseline)
                                  || ((p->i & 0b11000) == 0b10000 && box.bottom() > baseline)
                                )) {
                                    std::cout << ' ' << def.c;
                                }
                            }
                            else if ((p->i & 0b11000) == 0) {
                                if (baseline == box.bottom()
                                 && (
                                     ((p->i & 0b00110) == 0b00010 && box.top() < meanline)
                                  || ((p->i & 0b00110) == 0b00100 && box.top() > meanline)
                                )) {
                                    std::cout << ' ' << def.c;
                                }
                            }
                        }
                    }
                    std::cout << "\n";
                }
            }
        }

        if (ac != 4) break;
        std::cout << "\n";

        auto show_name = [&]() {
            size_t num_name = 0;
            for (auto const & name : loader.names()) {
                if (num_name < 10) {
                    std::cout << "0";
                }
                std::cout << num_name << (data_compare.enable_seq[num_name] ? " x " : "   ") << name << '\n';
                ++num_name;
            }
        };
        show_name();

        if (conformity != 100) {
            std::cout << "\n conformity: " << conformity << "\n";
        }

        std::cout << "\n> ";

        std::cin.clear();

        // interactive
        while (std::cin >> s && !s.empty()) {
            if (s[0] == 'r') {
                data_compare.enable_seq.clear();
                data_compare.enable_seq.resize(loader.size(), true);
                show_name();
            }
            else if (s[0] == 'e') {
                std::getline(std::cin, s);
                iss.clear();
                iss.str(s);
                size_t i;
                while (iss >> i) {
                    if (i < loader.size()) {
                        data_compare.enable_seq[i] = !data_compare.enable_seq[i];
                    }
                    else {
                        std::cerr << i << " unknow\n";
                    }
                }
                show_name();
            }
            else if (s[0] == 'p') {
                size_t i;
                std::cin >> i;
                if (i < def_ranges.size()) {
                    for (Definition const & def : def_ranges[i]) {
                        std::cout << def.c << "  " << (&def - &definitions.front()) << '\n';
                        if (display_char) {
                            std::cout << def.img;
                        }
                        data_compare.print(std::cout, def.data, loader);
                    }
                }
                else {
                    std::cerr << i << " unknow\n";
                }
            }
            else if (s[0] == 'i') {
                size_t i;
                std::cin >> i;
                if (i < def_ranges.size()) {
                    for (Definition const & def : def_ranges[i]) {
                        std::cout << def.c << "  " << (&def - &definitions.front()) << "\n" << def.img;
                    }
                }
                else {
                    std::cerr << i << " unknow\n";
                }
            }
            else if (s[0] == 's') {
                show_one_line = !show_one_line;
            }
            else if (s[0] == 'd') {
                display_char = !display_char;
            }
            else if (s[0] == 'q') {
                std::cin.clear(std::ios::eofbit);
                break;
            }
            else if (s[0] == 'x') {
                for (auto && x : data_compare.enable_seq) {
                    x = !x;
                }
                show_name();
            }
            else if (s[0] == 'c') {
                break;
            }
            else if (s[0] == 'l') {
                search_baseline = !search_baseline;
            }
            else if (s[0] == 'f') {
                show_data_if_empty_range = !show_data_if_empty_range;
            }
            else if (s[0] == '%') {
                std::cin >> conformity;
                conformity = std::min(100u, conformity);
            }
            else if (s[0] == 'h') {
                std::cout <<
                  "r: reset strategies\n"
                  "e id id2 ...: enable/disable strategy(ies)\n"
                  "x: inverse strategies\n"
                  "p id: print definition\n"
                  "i id: print definition.img\n"
                  "s: enable/disable \"one line per word\"\n"
                  "d: enable/disable \"print image with word\"\n"
                  "l: enable/disable search_baseline\n"
                  "f: enable/disable show_data_if_empty_range\n"
                  "c: continue\n"
                  "% conformity: percent min\n"
                  "q: quit\n"
                  "h: help\n";
            }
            else {
                std::cerr << "Unknow\n";
            }

            std::cout << "\n> ";
        }

        std::cin.ignore(10000, '\n');
    } while (!std::cin.eof());
}
