#include "image.hpp"
#include "box.hpp"
#include "make_box_character.hpp"

#include <iostream>


int main(int ac,  char ** av)
{
    if (ac != 2) {
        std::cerr << av[0] << " filename\n";
        return 1;
    }

    Image const img = image_from_file(av[1]);
//     std::cout << img;

    Index index;
    Bounds bounds(img.width(), img.height());

    while (auto const cbox = make_box_character(img, index, bounds)) {
        std::cout << cbox << std::endl;

        Image img_word = img.section(cbox.index(), cbox.bounds());
        Image img_word90 = img_word.rotate90();

        std::cout << img_word;
        std::cout << img_word90;

        if (cbox.h() < 2) {
            std::cout << "h1: []";
        } else {
            auto range = hrange(img_word, {0, (cbox.h()-2)/3}, cbox.w());
            auto it = range.begin();
            auto last = range.end();
            std::cout << "h1: " << ((cbox.h()-2)/3) << ": " << *it;
            while (rng::next_alternation(it, last)) {
                std::cout << ',' << *it;
            }
            endl(std::cout);
        }

//         auto vlight1 = (cbox.h < 2)
//           ? vlight_type()
//           : horizontal_light(vbox.cbegin() + (cbox.h-2)/3*cbox.w,               cbox.w, cbox.h);
//         auto vlight2 = horizontal_light(vbox.cbegin() + (cbox.h*2-1)/3*cbox.w,  cbox.w, cbox.h);
//         auto vlight3 = vertical_light(vbox.cbegin() + (cbox.w/2),               cbox.w, cbox.h);
//
//         auto vmask1 = horizontal_mask(vbox.cbegin(),                        cbox.h/3, cbox.w, cbox.h);
//         auto vmask2 = horizontal_mask(vbox.cend() - (cbox.h/3) * cbox.w,    cbox.h/3, cbox.w, cbox.h);
//         auto vmask3 = vertical_mask(vbox.cbegin(),                          cbox.w/3, cbox.w, cbox.h);
//         auto vmask4 = vertical_mask(vbox.cbegin() + (cbox.w/3*2),           cbox.w/3, cbox.w, cbox.h);
//
//         Def def = {std::string(), {{
//             std::move(vlight1)
//           , std::move(vlight2)
//           , std::move(vlight3)
//           , std::move(vmask1)
//           , std::move(vmask2)
//           , std::move(vmask3)
//           , std::move(vmask4)
//         }}};
//
//         auto p = std::lower_bound(all.begin(), all.end(), def);
//         if (p != all.end() && *p == def) {
//             std::cout << p->w << ' ';
//         }
//         else {
//             std::cout << "[?] ";
//         }

        index = Index(cbox.x()+cbox.w(), index.y());
    }
}
