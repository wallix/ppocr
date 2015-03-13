#include "image.hpp"
#include "data_loader.hpp"
#include "make_box_character.hpp"
#include "registry.hpp"
#include "definition.hpp"
// #include "utils/unique_sort_definition.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
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
    struct Proxy { Proxy() {} Definition const & operator()(Definition const & def) const { return def; } };
    std::vector<range_iterator<decltype(definitions.begin()), Proxy>> def_ranges;

    //unique_sort_definitions(definitions);

    do {
        def_ranges.clear();
        std::sort(definitions.begin(), definitions.end(), [&data_compare](auto & a, auto & b) {
            return data_compare.lt(a.data, b.data);
        });

        size_t x = 0;
        size_t num_word = 0;
        while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
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
            }
            else {
                if (num_word < 10 && !show_one_line) {
                    std::cout << "0";
                }
                auto first = it;
                std::string * sref = &it->c;
                bool ok = true;
                while (it != definitions.end() && data_compare.eq(it->data, data)) {
                    if (ok && it->c != *sref) {
                        ok = false;
                    }
                    sref = &it->c;
                    ++it;
                }
                def_ranges.push_back({first, it, Proxy()});
                if (first == it) {
                    std::cout << num_word << " \033[00;31m000\033[0m []";
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
                        std::cout << first->c << ' ';
                    }
                    std::cout << "\b]";
                }
                std::cout << (show_one_line ? " " : "\n");
                ++num_word;
            }

            x = cbox.x() + cbox.w();
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

        std::cout << "\n> ";

        std::cin.clear();

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
                        std::cout << def.c << "  " << (&def - &definitions.front()) << ' ' << def.img;
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
            else {
                std::cerr << "Unknow\n";
            }

            std::cout << "\n> ";
        }

        std::cin.ignore(10000, '\n');
    } while (!std::cin.eof());
}
