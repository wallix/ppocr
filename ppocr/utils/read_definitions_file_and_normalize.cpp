#include "read_definitions_file_and_normalize.hpp"

#include <stdexcept>
#include <algorithm>
#include <ostream>
#include <fstream>

#include <cstring>
#include <cerrno>

namespace ppocr {

std::vector<Definition> read_definitions_file_and_normalize(
    const char* filename, DataLoader& loader, std::ostream * out
) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error(strerror(errno));
    }

    std::vector<Definition> definitions = read_definitions(file, loader);

    if (!file.eof()) {
        throw std::runtime_error("read error");
    }

    if (out) {
        *out << "definitions.size = " << definitions.size() << "\n\n";
    }

    std::sort(
        definitions.begin(), definitions.end(),
        [](Definition const & lhs, Definition const & rhs) {
            return lhs.datas < rhs.datas;
        }
    );
    definitions.erase(
        std::unique(
            definitions.begin(), definitions.end(),
            [](Definition const & lhs, Definition const & rhs) {
                return lhs.datas == rhs.datas;
            }
        ),
        definitions.end()
    );

    if (out) {
        *out << "unique definitions.size = " << definitions.size() << "\n\n";
    }

    return definitions;
}

}
