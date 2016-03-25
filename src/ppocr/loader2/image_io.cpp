#include "ppocr/loader2/image_io.hpp"
#include "ppocr/image/image.hpp"

#include <ostream>
#include <istream>

namespace ppocr { namespace loader2 {

std::istream & read_img(std::istream & is, Image & img) {
    Bounds bnd;
    if (is >> bnd) {
        std::unique_ptr<Pixel[]> p(new Pixel[bnd.area()]);
        is.rdbuf()->snextc();
        is.read(p.get(), bnd.area());
        img = Image(bnd, std::move(p));
    }
    return is;
}

std::ostream & write_img(std::ostream & os, Image const & img) {
    os << img.bounds() << ' ';
    os.write(img.data(), img.area());
    return os;
}

} }
