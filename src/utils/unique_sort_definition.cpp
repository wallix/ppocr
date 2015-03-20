#include "unique_sort_definition.hpp"
#include "image_compare.hpp"

#include <algorithm>
#include <tuple>

void unique_sort_definitions(std::vector< Definition >& defs)
{
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
            int const cmp = image_compare(a.img, b.img);
            if (cmp < 0) {
                return true;
            }
            if (cmp > 0) {
                return false;
            }
        }

        return a.datas < b.datas;
    });
    defs.erase(std::unique(defs.begin(), defs.end(), [](Definition const & a, Definition const & b) {
        return a.c == b.c && a.img == b.img && a.datas == b.datas;
    }), defs.end());
}
