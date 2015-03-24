#include "image/image.hpp"
#include "factory/data_loader.hpp"
#include "box_char/make_box_character.hpp"
#include "factory/registry.hpp"
#include "factory/definition.hpp"
#include "box_char/box.hpp"
// #include "utils/unique_sort_definition.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <cstring>
#include <cerrno>
#include <cassert>

using std::size_t;


struct DataSorted
{
    struct Buffer
    {
        using iterator = Definition const * *;

        Buffer(std::vector<Definition> const & definitions)
        : p_(std::make_unique<Definition const*[]>(definitions.size()))
        , sz_(definitions.size())
        , enable_(true)
        {
            auto p = this->p_.get();
            for (Definition const & def : definitions) {
                *p++ = &def;
            }
        };

        Buffer() = default;
        Buffer(Buffer const &) = delete;
        Buffer&operator=(Buffer const &) = delete;
        Buffer(Buffer &&) = default;
        Buffer&operator == (Buffer &&) = delete;

        iterator begin() const { return this->p_.get(); }
        iterator end() const { return this->begin() + this->sz_; }

        operator bool () const { return this->enable_; }

        void set_enable(bool x) { this->enable_ = x; }

    private:
        std::unique_ptr<Definition const *[]> const p_;
        std::size_t sz_;
        bool enable_;
    };

    struct Buffers
    {
        Buffers(std::vector<Definition> const & definitions, std::size_t data_sz)
        : buffers_(std::make_unique<Buffer[]>(data_sz))
        , sz_(data_sz)
        {
            std::uninitialized_fill(this->begin(), this->end(), definitions);
        }

        Buffer const * begin() const { return this->buffers_.get(); }
        Buffer const * end() const { return this->begin() + this->sz_; }
        Buffer * begin() { return this->buffers_.get(); }
        Buffer * end() { return this->begin() + this->sz_; }
        Buffer const & operator[](size_t i) const { return this->begin()[i]; }
        void set_enable(size_t i, bool x) { return this->buffers_.get()[i].set_enable(x); }
        void reset_enable(bool x) { for (auto & buf : *this) buf.set_enable(x); }

    private:
        std::unique_ptr<Buffer[]> buffers_;
        std::size_t sz_;
    } buffers_;

    DataLoader const & loader_;

    DataSorted(std::vector<Definition> const & definitions, DataLoader const & loader)
    : buffers_(definitions, loader.size())
    , loader_(loader)
    {
        size_t i = 0;
        for (Buffer const & buf : buffers_) {
            std::sort(buf.begin(), buf.end(), [i](auto & a, auto & b) {
                DataLoader::data_base const & data1 = a->datas[i];
                DataLoader::data_base const & data2 = b->datas[i];
                return data1.lt(data2);
            });
            ++i;
        }
    }

    struct DefLtDatas
    {
        size_t i;
        unsigned percent;

        bool operator()(Definition const * a, DataLoader::Datas const & b) const
        {
            DataLoader::data_base const & data1 = a->datas[i];
            DataLoader::data_base const & data2 = b[i];
            return data1.relationship(data2) < this->percent;
        }
    };

    std::vector<Definition const *>
    get_same_definitions(DataLoader::Datas const & datas, unsigned percent = 100)
    {
        std::vector<Definition const *> ret;
        std::vector<Definition const *> localret;
        bool is_init = false;

        size_t i = 0;
        for (Buffer const & buf : buffers_) {
            if (buf) {
                localret.clear();
                //DefLtDatas lt{i, percent};
                //auto first = std::lower_bound(buf.begin(), buf.end(), data, lt);
                //for (; first != buf.end() && lt(*first, data); ++first) {
                //    localret.push_back(*first);
                //}
                for (auto * def : buf) {
                    DataLoader::data_base const & data1 = def->datas[i];
                    DataLoader::data_base const & data2 = datas[i];
                    if (data1.relationship(data2) >= percent) {
                        localret.push_back(def);
                    }
                }

                std::sort(localret.begin(), localret.end());
                if (!is_init) {
                    ret = localret;
                    is_init = true;
                }
                else {
                    auto it1 = ret.begin();
                    auto end1 = ret.end();
                    auto it2 = localret.begin();
                    auto end2 = localret.end();
                    auto out = it1;
                    for (; it1 != end1; ++it1) {
                       for (; it2 != end2 && *it2 < *it1; ++it2) {
                       }
                       if (it2 == end2) {
                           break;
                       }
                       if (*it1 == *it2) {
                           *out++ = *it1;
                       }
                    }
                    ret.erase(out, ret.end());
                }
            }
            ++i;
        }

        return ret;
    }

    std::vector<unsigned>
    relationships(const DataLoader::Datas& a, const DataLoader::Datas& b) const
    {
        std::vector<unsigned> ret;
        auto enable_it = this->buffers_.begin();
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

    bool print(std::ostream & os, const DataLoader::Datas& d, DataLoader const & loader) const
    {
        auto enable_it = this->buffers_.begin();
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

    bool is_enable(size_t i) const { return this->buffers_[i]; }
    void flip_enable(size_t i) { this->set_enable(i, !this->is_enable(i)); }
    void set_enable(size_t i, bool x) { this->buffers_.set_enable(i, x); }
    void reset_enable(bool x = true) { this->buffers_.reset_enable(x); }
    void flip_enable() { for (auto & buf : this->buffers_) buf.set_enable(!bool(buf)); }
};

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

    std::vector<Definition> definitions;
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

    DataSorted data_sorted(definitions, loader);


    Image img = image_from_file(av[2]);
    Bounds const bounds(img.width(), img.height());

    std::string s;
    std::istringstream iss;
    bool display_char = false;
    bool show_one_line = false;
    bool search_baseline = true;
    bool show_data_if_empty_range = false;
    unsigned conformity = 100;
    using ref_definitions_t = std::vector<Definition const *>;

    //unique_sort_definitions(definitions);

    struct CharInfo {
        ref_definitions_t ref_definitions;
        Box box;
        bool ok;
    };
    std::vector<CharInfo> char_infos;

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
        char_infos.clear();

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

            auto defs = data_sorted.get_same_definitions(data, conformity);
            bool ok = !defs.empty();
            if (ok) {
                std::string const & s = defs.front()->c;
                auto it = defs.begin();
                auto e = defs.end();
                for (; it != e; ++it) {
                    if ((*it)->c != s) {
                        ok = false;
                        break;
                    }
                }
            }

            if (num_word < 10) {
                std::cout << "0";
            }
            if (defs.empty()) {
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
                auto const sz = defs.size();
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
                for (auto * def : defs) {
                    if (conformity != 100 && !ok) {
                        std::cout << "\n " << def->c << " ";
                        for (auto percent : data_sorted.relationships(def->datas, data)) {
                            std::cout << std::setw(3) << percent << "% ";
                        }
                    }
                    else {
                        std::cout << def->c << ' ';
                    }
                }
                std::cout << "\b]";
            }
            std::cout << (show_one_line ? " " : "\n");
            ++num_word;

            char_infos.push_back({std::move(defs), cbox, ok});

            x = cbox.x() + cbox.w();
        }

        std::cout << "\nok: " << num_word_ok << " / " << num_word << "\n";

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
                    std::string const & c = info.ref_definitions.front()->c;
//                     std::cout << "  " << def.c;
                    auto p = std::find_if(info_lines.begin(), info_lines.end(), [&](Line const & l) {
                        return l.c == c;
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
                if (!info.ok && !info.ref_definitions.empty()) {
                    Box const & box = info.box;
                    std::cout << info.ref_definitions.front()->c << ':';
                    for (Definition const * def : info.ref_definitions) {
                        auto p = std::find_if(info_lines.begin(), info_lines.end(), [&](Line const & l) {
                            return l.c == def->c;
                        });
                        if (p != info_lines.end()) {
                            if (p->i == 1) {
                                if (meanline == box.top() && baseline == box.bottom()) {
                                    std::cout << ' ' << def->c;
                                }
                            }
                            else if ((p->i & 0b00110) == 0) {
                                if (meanline == box.top()
                                 && (
                                     ((p->i & 0b11000) == 0b01000 && box.bottom() < baseline)
                                  || ((p->i & 0b11000) == 0b10000 && box.bottom() > baseline)
                                )) {
                                    std::cout << ' ' << def->c;
                                }
                            }
                            else if ((p->i & 0b11000) == 0) {
                                if (baseline == box.bottom()
                                 && (
                                     ((p->i & 0b00110) == 0b00010 && box.top() < meanline)
                                  || ((p->i & 0b00110) == 0b00100 && box.top() > meanline)
                                )) {
                                    std::cout << ' ' << def->c;
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
            else if (s[0] == 'p') {
                size_t i;
                std::cin >> i;
                if (i < char_infos.size()) {
                    for (Definition const * def : char_infos[i].ref_definitions) {
                        std::cout << def->c << "  " << (def - &definitions.front()) << '\n';
                        if (display_char) {
                            std::cout << def->img;
                        }
                        data_sorted.print(std::cout, def->datas, loader);
                    }
                }
                else {
                    std::cerr << i << " unknow\n";
                }
            }
            else if (s[0] == 'i') {
                size_t i;
                std::cin >> i;
                if (i < char_infos.size()) {
                    for (Definition const * def : char_infos[i].ref_definitions) {
                        std::cout << def->c << "  " << (def - &definitions.front()) << "\n" << def->img;
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
                data_sorted.flip_enable();
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
        }
        std::cout << std::endl;

        std::cin.ignore(10000, '\n');
    } while (!std::cin.eof());
}
