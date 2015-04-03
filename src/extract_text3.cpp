#include "image/image.hpp"
#include "factory/data_loader.hpp"
#include "box_char/make_box_character.hpp"
#include "factory/registry.hpp"
#include "factory/definition.hpp"
#include "box_char/box.hpp"
#include "filters/line.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <map>

#include <cstring>
#include <cerrno>
#include <cassert>

using std::size_t;

struct CharacterDefinitions
{
    std::string c;
    std::vector<DataLoader::Datas> datas_list;
};

struct DataSorted
{
    DataLoader const & loader_;

    using data_map_type = std::map<std::string, std::vector<DataLoader::Datas>>;
    using strings_ref_type = std::vector<std::reference_wrapper<std::string const>>;

    data_map_type const datas_map;
    strings_ref_type const strings_ref;
    std::vector<unsigned char> enable_;

    DataSorted(std::vector<Definition> definitions, DataLoader const & loader)
    : loader_(loader)
    , datas_map([&definitions]() {
        data_map_type m;
        for (Definition & def : definitions) {
            m[std::move(def.c)].push_back(std::move(def.datas));
        }
        return m;
    }())
    , strings_ref([this]() {
        strings_ref_type cont;
        for (auto & pair : datas_map) {
            cont.emplace_back(pair.first);
        }
        return cont;
    }())
    , enable_(loader.size(), true)
    {
    }

    strings_ref_type
    get_characters(DataLoader::Datas const & datas, unsigned percent = 100) const
    {
        strings_ref_type ret;
        strings_ref_type localret;
        strings_ref_type const * ref = &this->strings_ref;

        for (size_t i = 0; i < this->enable_.size(); ++i) {
            if (!this->enable_[i]) {
                continue;
            }

            for (auto & s: *ref) {
                auto it = this->datas_map.find(s);
                if (it != this->datas_map.end() && std::any_of(
                    it->second.begin(),
                    it->second.end(),
                    [i, percent, &datas](DataLoader::Datas const & datas_) {
                        return datas_[i].relationship(datas[i]) >= percent;
                    }
                )) {
                    localret.push_back(s);
                }
            }

            ret.swap(localret);
            if (ret.empty()) {
                break;
            }
            ref = &ret;
            localret.clear();
        }
        return ret;
    }

    bool is_enable(size_t i) const { return this->enable_[i]; }
    void flip_enable(size_t i) { this->set_enable(i, !this->is_enable(i)); }
    void set_enable(size_t i, bool x) { this->enable_[i] = x; }
    void reset_enable(bool x = true) { std::fill(this->enable_.begin(), this->enable_.end(), x); }
    void flip_enable() { for (auto & x : this->enable_) x = !x; }
};

int main(int ac, char **av)
{
    if (ac < 3) {
        std::cerr << av[0] << " datas images [-i]\n";
        return 1;
    }

    DataLoader loader;
    all_registy(loader);

    std::ifstream file(av[1]);
    if (!file) {
        std::cerr << strerror(errno) << '\n';
        return 2;
    }

    auto definitions = read_definitions(file, loader);

    if (!file.eof()) {
        std::cerr << "read error\n";
        return 5;
    }

    DataSorted data_sorted(std::move(definitions), loader);

    Image img = image_from_file(av[2]);
    Bounds const bounds(img.width(), img.height());

    std::string s;
    std::istringstream iss;
    bool display_char = false;
    bool show_one_line = false;
    bool show_data_if_empty_range = false;
    bool show_percent = false;
    unsigned conformity = 100;

    struct Line {
        std::string c;
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
        size_t num_word_ok = 0;

        size_t const min_x = x;
        size_t min_y = img.height();
        size_t bounds_x = 0;
        size_t bounds_y = 0;

        using resolution_clock = std::chrono::high_resolution_clock;
        auto t1 = resolution_clock::now();
        while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
            min_y = std::min(cbox.y(), min_y);
            bounds_y = std::max(cbox.y() + cbox.h(), bounds_y);
            bounds_x = cbox.x() + cbox.w();
            //std::cerr << "\nbox(" << cbox << ")\n";

            Image const img_word = img.section(cbox.index(), cbox.bounds());
            //std::cerr << img_word << '\n';

            //Def const def{{}, img_word, loader.new_datas(img_word, img_word.rotate90())};
            auto data = loader.new_datas(img_word, img_word.rotate90());
            //std::cerr << data << '\n';

            if (display_char && !show_one_line) {
                std::cout << img_word << cbox << "\n";
                std::cout.write(img_word.data(), img_word.area()) << '\n';
                std::cout << loader.writer(data);
            }

            auto characters = data_sorted.get_characters(data, conformity);
            bool const ok = (characters.size() == 1);

            if (num_word < 10) {
                std::cout << "0";
            }
            if (characters.empty()) {
                std::cout << num_word << " \033[00;31m000\033[0m []";

                if (show_data_if_empty_range) {
                    std::cout << "\n" << loader.writer(data);
                }
            }
            else {
                std::cout << num_word << " ";
                if (ok) {
                    ++num_word_ok;
                    std::cout << "\033[00;32m";
                }
                auto const sz = characters.size();
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
                for (auto & c : characters) {
                    std::cout << c.get() << ' ';
                }
                std::cout << "\b]";
            }
            std::cout << (show_one_line ? " " : "\n");
            ++num_word;

            x = cbox.x() + cbox.w();
        }

        {
            auto t2 = resolution_clock::now();
            std::cout << "\nok: " << num_word_ok << " / " << num_word
              << " : " << std::chrono::duration<double>(t2-t1).count() << "s\n"
            ;

            Index const text_index(min_x, min_y);
            Bounds const text_bounds(bounds_x-min_x, bounds_y-min_y);
            std::cout << "\n box: [" << text_index << " + " << text_bounds << "]\n";
        }

        if (ac != 4) break;
        std::cout << "\n";

        auto show_name = [&]() {
            size_t num_name = 0;
            for (auto const & name : loader.names()) {
                if (num_name < 10) {
                    std::cout << "0";
                }
                std::cout << num_name << (data_sorted.is_enable(num_name) ? " x " : "   ") << name << '\n';
                ++num_name;
            }
        };
        show_name();

        if (conformity != 100) {
            std::cout << "\n conformity: " << conformity << "\n";
        }

        std::cin.clear();

        // interactive
        while ((std::cout << "\ninteractive >>>\n") && std::cin >> s && !s.empty()) {
            if (s[0] == 'r') {
                data_sorted.reset_enable();
                show_name();
            }
            else if (s[0] == 'e') {
                std::getline(std::cin, s);
                iss.clear();
                iss.str(s);
                size_t i;
                while (iss >> i) {
                    if (i < loader.size()) {
                        data_sorted.flip_enable(i);
                    }
                    else {
                        std::cerr << i << " unknow\n";
                    }
                }
                show_name();
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
                data_sorted.flip_enable();
                show_name();
            }
            else if (s[0] == 'c') {
                break;
            }
            else if (s[0] == 'f') {
                show_data_if_empty_range = !show_data_if_empty_range;
            }
            else if (s[0] == '%') {
                std::cin >> conformity;
                conformity = std::min(100u, conformity);
            }
            else if (s[0] == 'P') {
                show_percent = !show_percent;
            }
            else if (s[0] == 'h') {
                std::cout <<
                  "r: reset strategies\n"
                  "e id id2 ...: enable/disable strategy(ies)\n"
                  "x: inverse strategies\n"
                  "s: enable/disable \"one line per word\"\n"
                  "d: enable/disable \"print image with word\"\n"
                  "f: enable/disable show_data_if_empty_range\n"
                  "c: continue\n"
                  "% percent: percent min\n"
                  "P: enable/disable show_percent\n"
                  "q: quit\n"
                  "h: help\n";
            }
            else {
                std::cerr << "Unknow\n";
            }
        }
        std::cout << std::endl;

        std::cin.ignore(10000, '\n');
    } while (!std::cin.eof());
}
