#include "unique_sort_definition.hpp"
#include "image_compare.hpp"

#include <algorithm>
#include <tuple>

namespace ppocr {

void unique_sort_definitions(std::vector< Definition >& defs)
{
    if (defs.empty()) {
        return;
    }

    std::sort(defs.begin(), defs.end(), [](Definition const & a, Definition const & b) {
        {
            int const cmp = a.c.compare(b.c);
            if (cmp < 0) {
                return true;
            }
            if (cmp > 0) {
                return false;
            }
        }

        {
            int const cmp = a.font_name.compare(b.font_name);
            if (cmp < 0) {
                return true;
            }
            if (cmp > 0) {
                return false;
            }
        }

        return image_compare(a.img, b.img) < 0;
    });

    defs.erase(std::unique(defs.begin(), defs.end(), [](Definition const & a, Definition const & b) {
        return a.c == b.c && a.font_name == b.font_name && a.img == b.img;
    }), defs.end());
}

}
