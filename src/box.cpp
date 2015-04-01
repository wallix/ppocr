#include "image/image.hpp"
#include "box_char/make_box_character.hpp"

#include <iostream>

int main(int ac, char **av)
{
    if (ac < 2) {
        std::cerr << av[0] << "image_file...\n";
        return 1;
    }

    for (int i = 1; i < ac; i += 2) {
        Image img = image_from_file(av[1]);
        //std::cerr << img << '\n';

        size_t x = 0;
        Bounds const & bounds = img.bounds();

        std::cout << av[1] << ":\n";
        while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
            std::cout << "\nbox(" << cbox << ")\n" << img.section(cbox.index(), cbox.bounds());
            x = cbox.x() + cbox.w();
        }
    }

    return 0;
}
