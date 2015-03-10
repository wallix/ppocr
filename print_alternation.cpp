#include "image.hpp"
#include "box.hpp"
#include "make_box_character.hpp"
#include "strategies/alternation.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include <cassert>


int main(int ac, char ** av)
{
    if (ac != 2) {
        std::cerr << av[0] << " filename\n";
        return 1;
    }

    Image const img = image_from_file(av[1]);
//     std::cout << img;

    struct Def {
        std::string c;
        strategies::alternations alternations;

        bool operator<(Def const & other) const {
            return alternations < other.alternations;
        }

        bool operator == (Def const & other) const {
            return alternations == other.alternations;
        }
    };

    std::vector<Def> definitions;

    size_t x = 0;
    Bounds bounds(img.width(), img.height());

    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        std::cout << "\nbox(" << cbox << ")\n";

        Image const img_word = img.section(cbox.index(), cbox.bounds());
        Image const img_word90 = img_word.rotate90();
        std::cout << img_word;
        std::cout << img_word90;

        Def def{
            static_cast<std::ostringstream&>(std::ostringstream() << img_word).str()
          , strategies::alternations(img_word, img_word90)
        };

        for (auto const & alternations : def.alternations) {
            static char const * names[]{"hl1", "hl2", "hm1", "hm2", "vl1", "vm1", "vm2"};
            std::cout << names[&alternations - &def.alternations[0]] << ':';
            for (auto const & x : alternations) {
                std::cout << ' ' << x;
            }
            std::cout << '\n';
        }

        auto p = std::lower_bound(definitions.begin(), definitions.end(), def);
        if (p != definitions.end() && *p == def) {
            std::cout << p->c << ' ';
        }
        else {
            std::cout << "[?] ";
            definitions.push_back(std::move(def));
        }

        x = cbox.x() + cbox.w();
    }
}
