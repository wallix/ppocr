/*
* Copyright (C) 2016 Wallix
* 
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
* 
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
* 
* You should have received a copy of the GNU Lesser General Public License along
* with this library; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "ppocr/spell/dictionary.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <cstring>
#include <cerrno>

using namespace ppocr;

int main(int ac, char ** av) {
    if (ac < 2) {
        std::cerr << av[0] << " words_file...\n";
        return 1;
    }

    std::ifstream file;
    std::vector<std::string> words;

    for (int i = 1; i < ac; ++i) {
        file.open(av[i]);
        if (!file) {
            std::cerr << av[i] << ": " << strerror(errno) << std::endl;
            continue;
        }

        std::string s;
        while (std::getline(file, s)) {
            words.push_back(s);
        }
    }

    std::cout << spell::Dictionary(std::move(words));
}
