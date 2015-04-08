#include "factory/data_loader.hpp"
#include "factory/registry.hpp"
#include "utils/unique_sort_definition.hpp"

#include <iostream>
#include <fstream>

#include <cstring>
#include <cerrno>

int main(int ac, char **av)
{
    if (ac < 2) {
        std::cerr << av[0] << "datas\n";
        return 1;
    }

    std::ifstream file(av[1]);
    if (!file) {
        std::cerr << strerror(errno) << '\n';
        return 2;
    }

    DataLoader loader;
    all_registy(loader);

    std::vector<Definition> definitions = read_definitions(file, loader);

    if (!file.eof()) {
        std::cerr << "read error\n";
        return 5;
    }

    unique_sort_definitions(definitions);

    for (auto & def : definitions) {
        write_definition(std::cout, def, loader);
    }
}
