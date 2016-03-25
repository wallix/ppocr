#include "ppocr/ocr2/data_indexes_by_words.hpp"

#include <utility>
#include <map>

ppocr::ocr2::DataIndexesByWords::DataIndexesByWords(const ppocr::ocr2::Glyphs& glyphs)
{
    std::map<unsigned, std::vector<unsigned>> map;

    unsigned n = 0;
    for (auto & views : glyphs) {
        auto & e = map[views.front().word];
        e.emplace_back(n++);
    }

    this->indexes_by_words.resize(glyphs.size());
    for (auto & p : map) {
        if (p.second.size() > 1) {
            auto tmp = p.second.back();
            p.second.pop_back();
            indexes_by_words[tmp] = p.second;
            for (auto & i : p.second) {
                using std::swap;
                swap(i, tmp);
                indexes_by_words[tmp] = p.second;
            }
        }
    }
}
