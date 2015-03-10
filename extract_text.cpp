#include "image.hpp"
#include "data_loader.hpp"
#include "make_box_character.hpp"

#include "strategies/alternation.hpp"
#include "strategies/direction.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>

#include <cstring>
#include <cerrno>

int main(int ac, char **av)
{
    if (ac != 3) {
        std::cerr << av[0] << "datas images\n";
        return 1;
    }

    std::ifstream file(av[1]);
    if (!file) {
        std::cerr << strerror(errno) << '\n';
        return 2;
    }

    struct Def {
        std::string c;
        strategies::alternations alternations;

        bool operator<(Def const & other) const {
            return alternations < other.alternations || (alternations == other.alternations && c < other.c);
        }
    };

    std::vector<Def> definitions;

    Image img = image_from_file(av[2]);
    //std::cerr << img << '\n';

    std::string s;

    while (file >> s) {
        Def def;
        def.c = s;
        file >> def.alternations;
        definitions.push_back(std::move(def));
    }

    std::sort(definitions.begin(), definitions.end());
    definitions.erase(std::unique(definitions.begin(), definitions.end(), [](Def const & a, Def const & b) {
        return a.c == b.c && a.alternations == b.alternations;
    }), definitions.end());

    size_t x = 0;
    Bounds bounds(img.width(), img.height());

    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //std::cerr << "\nbox(" << cbox << ")\n";

        Image const img_word = img.section(cbox.index(), cbox.bounds());
        //std::cerr << img_word << '\n';

        Def const def{{}, strategies::alternations(img_word, img_word.rotate90())};

        auto it = std::lower_bound(
            definitions.begin(), definitions.end(), def.alternations
          , [](Def const & a, decltype(Def::alternations) const & b) {
              return a.alternations < b;
          }
        );
        if (it == definitions.end()) {
            std::cout << "[?] ";
        }
        else {
            std::cout << "[";
            for (; it != definitions.end() && it->alternations == def.alternations; ++it) {
                std::cout << it->c << ' ';
            }
            std::cout << "\b] ";
        }

        x = cbox.x() + cbox.w();
    }

    std::cout << "\n";
}
