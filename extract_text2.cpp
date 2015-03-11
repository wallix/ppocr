#include "image.hpp"
#include "data_loader.hpp"
#include "make_box_character.hpp"
#include "registry.hpp"
#include "definition.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <cstring>
#include <cerrno>

int main(int ac, char **av)
{
    if (ac != 3) {
        std::cerr << av[0] << " datas images\n";
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

    Image img = image_from_file(av[2]);
    //std::cerr << img << '\n';

    std::string s;

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

    std::sort(definitions.begin(), definitions.end(), [](auto & a, auto & b) {
        return a.data < b.data;
    });

    size_t x = 0;
    Bounds bounds(img.width(), img.height());

    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //std::cerr << "\nbox(" << cbox << ")\n";

        Image const img_word = img.section(cbox.index(), cbox.bounds());
        //std::cerr << img_word << '\n';

        //Def const def{{}, img_word, loader.new_data(img_word, img_word.rotate90())};
        auto data = loader.new_data(img_word, img_word.rotate90());
        //std::cerr << data << '\n';

        auto it = std::lower_bound(
            definitions.begin(), definitions.end(), data
          , [](Definition const & a, DataLoader::Data const & b) {
              return a.data < b;
          }
        );
        if (it == definitions.end()) {
            std::cout << "? ";
        }
        else {
            std::cout << "[";
            bool const is_empty = !(it != definitions.end() && it->data == data);
            for (; it != definitions.end() && it->data == data; ++it) {
                std::cout << it->c << ' ';
            }
            std::cout << (is_empty ? "] " : "\b] ");
        }

        x = cbox.x() + cbox.w();
    }

    std::cout << "\n";
}
