#include "dcompass.hpp"
#include "image.hpp"

#include <ostream>
#include <istream>

// #include <iostream>

namespace strategies
{

static unsigned count_dcompass(Bounds const & bnd, Pixel const * p, Pixel const * ep, bool is_top)
{
    // TODO /!\ uint(0) - 1u is undefinid
    unsigned d = 0;
    size_t ih = 0;
    size_t const wdiv2 = bnd.w()/2;
//     std::cout << "bnd: " << bnd.w() <<  ' ' << bnd.h() << std::endl;
    for (; p != ep; p += bnd.w(), ++ih) {
// //         if (!is_top) {
// //             std::cout << wdiv2 << " * (" << bnd.h() << " - " << ih << ") / " << bnd.h() << '\n';
// //         }
// //         else {
// //             std::cout << wdiv2 << " * (1 + " << ih << ") / " << bnd.h() << '\n';
// //         }
        size_t x = wdiv2 - bnd.w() / (!is_top ? bnd.h() - ih : 1 + ih) / 2;
//         if (is_top) {
//             std::cout << "x = wdiv2 - x = " << wdiv2 <<'-' << x << '\n';
//             x = wdiv2 - x;
//         }
        auto leftp = p + x;
        auto rightp = p + bnd.w() - x;
//         std::cout << x << ' ' << (bnd.w() - x) << std::endl;
        for (; leftp != rightp; ++leftp) {
            if (is_pix_letter(*leftp)) {
                ++d;
            }
        }
    }
    return d;
}

static int horizontal_dcompass(const Image& img)
{
    Bounds const bnd(img.width(), img.height() / 2);
    auto p = img.data();
    auto ep = img.data({0, bnd.h()});
    long const top = count_dcompass(bnd, p, ep, true);
    p = ep;
    if (img.height() & 1) {
        p += img.width();
    }
    long const bottom = count_dcompass(bnd, p, img.data_end(), false);

//     std::cout << img<< top << ' ' << bottom << "\n----" << std::endl;

    return top < bottom ? 1 : top > bottom ? 2 : 0;
}

dcompass::dcompass(const Image& img, const Image& img90)
: d(static_cast<cardinal_direction>(horizontal_dcompass(img) | horizontal_dcompass(img90) * 4))
{}


std::istream& operator>>(std::istream& is, dcompass& d)
{ return is >> reinterpret_cast<std::underlying_type_t<dcompass::cardinal_direction>&>(d.d); }

std::ostream& operator<<(std::ostream& os, const dcompass& d)
{ return os << d.direction(); }

}
