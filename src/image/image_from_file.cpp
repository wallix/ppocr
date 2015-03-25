#include <stdexcept>
#include <cstring>
#include <cerrno>

#include "image.hpp"

#include "mln/image/image2d.hh"
#include "mln/io/ppm/load.hh"
#include "mln/io/pbm/load.hh"
#include "rgb8.hpp"

template<class T, class ToPix>
Image image2d_to_Image(mln::image2d<T> const & img, ToPix to_pix)
{
    PtrImageData vimg(new Pixel[img.ncols() * img.nrows()]);
    auto it = vimg.get();
    for (unsigned y = 0; y < img.nrows(); y++) {
        for (unsigned x = 0; x < img.ncols(); x++) {
            *it++ = to_pix(img.at(y, x));
        }
    }
    return Image(Bounds(img.ncols(), img.nrows()), std::move(vimg));
}

#include <iostream>

Image image_from_file(const char * filename)
{
    mln::image2d<ocr::rgb8> img;

    if (!mln::io::ppm::load(img, filename)) {
        mln::image2d<bool> img;
        if (!mln::io::pbm::load(img, filename)) {
            std::string desc = "image_from_file: ";
            desc += filename;
            if (errno) {
                desc += ": ";
                desc += strerror(errno);
            }
            throw std::runtime_error(std::move(desc));
        }

        return image2d_to_Image(img, [&](bool x){ return x ? 'x' : '-'; });
    }

    return image2d_to_Image(img, [&](const ocr::rgb8 & rgb){
        unsigned char c
        = ((511/*PPM_RED_WEIGHT*/   * rgb.red()   + 511)>>10)
        + ((396/*PPM_GREEN_WEIGHT*/ * rgb.green() + 511)>>10)
        + ((117/*PPM_BLUE_WEIGHT*/  * rgb.blue()  + 511)>>10);
        return (c < 128) ? '-' : 'x';
//         *it++ = !(rgb.red() == 60 && rgb.green() == 64 && rgb.blue() == 72) ? cc :ncc;
//         *it++ = (rgb.red() == 255 && rgb.green() == 255 && rgb.blue() == 255) ? ncc : cc;
    });
}

