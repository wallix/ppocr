#include "image.hpp"
#include "make_box_character.hpp"
#include "strategies/alternation.hpp"

#include <iostream>
#include <fstream>

#include <cstring>
#include <cerrno>

int main(int ac, char **av)
{
    if (ac != 3) {
        std::cerr << av[0] << "image_file text_file\n";
        return 1;
    }

    std::ifstream file(av[2]);
    if (!file) {
        std::cerr << strerror(errno) << '\n';
        return 2;
    }

    Image img = image_from_file(av[1]);
    //std::cerr << img << '\n';

    std::string s;

    size_t x = 0;
    Bounds bounds(img.width(), img.height());

    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //std::cerr << "\nbox(" << cbox << ")\n";

        if (!(file >> s)) {
            return 10;
        }

        Image const img_word = img.section(cbox.index(), cbox.bounds());
        std::cerr << img_word << '\n';

        std::cout << s << "\n" << strategies::alternations(img_word, img_word.rotate90()) << "\n";

        x = cbox.x() + cbox.w();
    }

    if (file >> s) {
        return 20;
    }

    return 0;
}
