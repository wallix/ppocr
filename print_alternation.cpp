#include "image.hpp"
#include "box.hpp"
#include "data_loader.hpp"
#include "registry.hpp"
#include "definition.hpp"
#include "make_box_character.hpp"

#include <algorithm>
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

    Definition definition;

    size_t x = 0;
    Bounds bounds(img.width(), img.height());

    DataLoader loader;
    all_registy(loader);

    std::vector<Definition> definitions;

    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        std::cout << "\nbox(" << cbox << ")\n";

        Definition def(std::string{}, img.section(cbox.index(), cbox.bounds()), loader);

        auto p = std::lower_bound(definitions.begin(), definitions.end(), def, [](auto & a, auto & b) {
            return a.data < b.data;
        });
        if (p != definitions.end() && p->data == def.data) {
            std::cout << p->c << ' ';
        }
        else {
            write_definition(std::cout << def.img << "[?]", def, loader);
            definitions.insert(p, std::move(def));
        }

        x = cbox.x() + cbox.w();
    }
}
