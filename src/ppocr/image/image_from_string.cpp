#include "ppocr/image/image_from_string.hpp"

#include <cassert>

namespace ppocr {

Image image_from_string(const Bounds& bnd, const char * pix_data)
{
    assert(pix_data[bnd.area()] == 0 && (pix_data[bnd.area()-1] == '-' || pix_data[bnd.area()-1] == 'x'));

    return Image(bnd, [pix_data, &bnd]{
        size_t const area = bnd.area();
        PtrImageData data(new Pixel[area]);
        std::copy(pix_data, pix_data+area, data.get());
        return data;
    }());
}

}
