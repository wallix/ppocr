#include "image.hpp"

#include <iostream>


bool horizontal_empty(Pixel const * d, size_t w) {
    for (auto e = d+w; d != e; ++d) {
        if (is_letter(*d)) {
            return false;
        }
    }
    return true;
}

bool vertical_empty(Pixel const * d, size_t w, size_t h) {
    for (auto e = d+w*h; d != e; d += w) {
        if (is_letter(*d)) {
            return false;
        }
    }
    return true;
}



struct box_type {
    size_t x;
    size_t y;
    size_t w;
    size_t h;

    explicit operator bool() const noexcept { return w && h; }
};

std::ostream & operator<<(std::ostream & os, box_type const & box) {
    return os << "box(" << box.x << ", " << box.y << ", " << box.w << ", " << box.h << ")";
}


box_type make_box_character(Image const & image, box_type const & cbox)
{
    box_type box = cbox;

    auto d = image.data() + box.y*cbox.w + box.x;
    for (; box.x < cbox.w; ++box.x) {
        if (!vertical_empty(d, cbox.w, box.h)) {
            break;
        }
        ++d;
    }
    box.w = box.x;
    while (box.w + 1 < cbox.w) {
        ++box.w;
        if ([](Pixel const * d, size_t w, size_t h) -> bool {
            for (auto e = d+w*h; d != e; d += w) {
                if (is_letter(*d) && (
                    (d+1 != e && is_letter(*(d+1)))
                 || (d-w+1 < e && is_letter(*(d-w+1)))
                 || (d+w+1 < e && is_letter(*(d+w+1)))
                )) {
                    return false;
                }
            }
            return true;
        }(d, cbox.w, box.h)) {
            break;
        }
        ++d;
    }
    box.w -= box.x;

    d = image.data() + box.y*cbox.w + box.x;
    for (; box.y < cbox.h; ++box.y) {
        if (!horizontal_empty(d, box.w)) {
            break;
        }
        d += cbox.w;
    }
    d = image.data() + box.h*cbox.w + box.x;
    while (--box.h > box.y) {
        d -= cbox.w;
        if (!horizontal_empty(d, box.w)) {
            break;
        }
    }
    box.h -= box.y;

    ++box.h;
    return box;
}

// vlight_type horizontal_light(vcomponent_type::const_iterator it, size_t w, size_t /*h*/) {
//     vlight_type v;
//     auto e = it + w;
//     if (it != e) {
//         v.push_back(isc(*it));
//         ++it;
//         for (; it != e; ++it) {
//             if (isc(*it) != v.back()) {
//                 v.push_back(!v.back());
//             }
//         }
//     }
//     return v;
// }

int main(int ac,  char ** av)
{
    if (ac != 2) {
        std::cerr << av[0] << " filename\n";
        return 1;
    }

    Image const img = image_from_file(av[1]);
//     std::cout << img;

    box_type box{0, 0, img.width(), img.height()};

    while (1) {
        auto const cbox = make_box_character(img, box);
        if (!cbox) {
            break;
        }
        std::cout << cbox << std::endl;

        Image img_word = img.section({cbox.x, cbox.y}, {cbox.w,  cbox.h});
        Image img_word90 = img_word.rotate90();

        std::cout << img_word;
        std::cout << img_word90;

        if (cbox.h < 2) {
            std::cout << "h1: []";
        } else {
            auto range = hrange(img_word, {0, (cbox.h-2)/3}, cbox.w);
            auto it = range.begin();
            auto last = range.end();
            std::cout << "h1: " << ((cbox.h-2)/3) << ": " << *it;
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

        box.x = cbox.x+cbox.w;
    }
}
