#include "ppocr/image/image.hpp"
#include "ppocr/image/image_from_file.hpp"
#include "ppocr/box_char/make_box_character.hpp"

#include <iostream>

using namespace ppocr;

int main(int ac, char **av)
{
    int i_ac = 0;
    unsigned luminance = 128;
    if (ac >= 2) {
        if (av[1][0] == '-' && av[1][1] == 'l' && av[1][2] == '\0') {
            char * p;
            luminance = std::strtoul(av[2], &p, 10);
            std::cout << " ## luminance: " << (luminance) << std::endl;
            if (p == av[2] || *p != '\0' || errno) {
                return 2;
            }
            i_ac += 2;
        }
    }

    if ((ac - i_ac) < 2) {
        std::cerr << av[0] << " [-l luminance] image_file...\n";
        return 1;
    }

    for (++i_ac; i_ac < ac; ++i_ac) {
        Image img = image_from_file(av[i_ac], luminance);
        //std::cerr << img << '\n';

        size_t x = 0;
        Bounds const & bounds = img.bounds();

        std::cout << av[i_ac] << ":\n";
        while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
            std::cout << "\nbox(" << cbox << ")\n" << img.section(cbox.index(), cbox.bounds());
            x = cbox.x() + cbox.w();
        }
    }

    return 0;
}
