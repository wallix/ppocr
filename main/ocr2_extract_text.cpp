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

#include "ppocr/box_char/make_box_character.hpp"
#include "ppocr/image/image_from_file.hpp"
#include "ppocr/spell/dictionary.hpp"
#include "ppocr/utils/read_file.hpp"
#include "ppocr/defined_loader.hpp"

#include "ppocr/ocr2/disambiguous_with_dict.hpp"
#include "ppocr/ocr2/views_index_ordered.hpp"
#include "ppocr/ocr2/filter_by_lines.hpp"
#include "ppocr/ocr2/filter_by_font.hpp"
#include "ppocr/ocr2/probabilities.hpp"
#include "ppocr/ocr2/image_context.hpp"
#include "ppocr/ocr2/compute_image.hpp"
#include "ppocr/ocr2/words_infos.hpp"
#include "ppocr/ocr2/ambiguous.hpp"
#include "ppocr/ocr2/glyphs.hpp"
#include "ppocr/ocr2/cache.hpp"

#include <iostream>
#include <chrono>

using namespace ppocr;

namespace detail_ {
    template<class>
    struct first_type;

    template<template<class...> class Template, class T, class... Ts>
    struct first_type<Template<T, Ts...>>
    { using type = T; };
}

int main(int ac, char **av)
{
    if (ac < 4) {
        std::cerr << av[0] << " glyphs datas images [words.info] [dict.trie]\n";
        return 1;
    }
#ifdef NDEBUG
    std::ios::sync_with_stdio(false);
#endif
    std::boolalpha(std::cout);

    auto const dict = [&]{
        spell::Dictionary dict;
        if (ac > 5) {
            utils::read_file(dict, av[5]);
            std::cout << " ## dict ? " << !dict.empty() << "\n";
        }
        return dict;
    }();

    auto const datas = utils::load_from_file<PpOcrDatas>(av[2]);
    std::cout << " ## datas.size(): " << datas.size() << "\n";

    auto const glyphs = utils::load_from_file<ocr2::Glyphs>(av[1]);
    std::cout << " ## glyphs.size(): " << glyphs.size() << "\n";

    if (glyphs.size() != datas.size()) {
        throw std::bad_array_new_length();
    }

    ocr2::WordsInfos const words_infos(
        glyphs,
        ac > 4 ? utils::load_from_file<ocr2::WWordsLines>(av[4]) : ocr2::WWordsLines()
    );

    ocr2::DataIndexesByWords const data_indexes_by_words(glyphs);

    auto const id_views = ocr2::get_views_indexes_ordered(glyphs);

    using FirstLoaderStrategy = typename ::detail_::first_type<PpOcrDatas>::type;
    ocr2::DataIndexesOrdered<FirstLoaderStrategy> const first_strategy_ordered(datas);

    std::vector<unsigned> spaces;
    std::vector<Box> boxes;

    ocr2::Probabilities probabilities(glyphs.size());
    ocr2::Probabilities tmp_probabilities(glyphs.size());

    ocr2::ImageContext img_ctx;

    ocr2::image_cache_type_t images_cache;

    ocr2::ambiguous_t ambiguous;

    std::cout << " ## go" << std::endl;

    Image const img = image_from_file(av[3]);
    Bounds const bounds = img.bounds();
    unsigned x = 0;

    using resolution_clock = std::chrono::high_resolution_clock;
    auto t1 = resolution_clock::now();
    unsigned i_space = 0;
    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //min_y = std::min(cbox.y(), min_y);
        //bounds_y = std::max(cbox.y() + cbox.h(), bounds_y);
        //bounds_x = cbox.x() + cbox.w();
        //std::cerr << "\nbox(" << cbox << ")\n";

        Image const & img_word = img_ctx.img(cbox.bounds(), [&](Pixel * pixels) {
            section(img, pixels, cbox.index(), cbox.bounds());
        });
        //std::cout << img_word;

        if (x + 4 < cbox.x()) {
            spaces.push_back(i_space);
        }
        ++i_space;

        auto it = images_cache.find(img_word);
        if (it != images_cache.end()) {
            ambiguous.emplace_back(it->second);
        }
        else {
            std::cout << img_word;
            auto cache_element = ocr2::compute_image(
                PpOcrSimpleDatas{},
                PpOcrComplexDatas{},
                PpOcrExclusiveDatas{},
                probabilities,
                tmp_probabilities,
                datas,
                first_strategy_ordered,
                data_indexes_by_words,
                glyphs,
                id_views,
                img_word,
                img_ctx.img90(),
                0.5
            );
            auto it = images_cache.emplace(
                img_word.clone(),
                std::move(cache_element)
            ).first;
            ambiguous.emplace_back(it->second);
            for (ocr2::View const & view : it->second) {
                std::cout << glyphs.get_word(view) << " ";
            }
            std::cout << "\n";
        }

        boxes.push_back(cbox);

        x = cbox.x() + cbox.w();
    }


    auto t2 = resolution_clock::now();
    std::cerr << std::chrono::duration<double>(t2-t1).count() << "s\n";

    ocr2::filter_by_lines(ambiguous, words_infos, boxes);

    ocr2::filter_by_font(ambiguous);

    std::string result;
    spaces.push_back(ambiguous.size());
    unsigned unrecognized_count = ocr2::disambiguous_with_dict(
        ambiguous,
        glyphs,
        dict,
        spaces.cbegin(),
        result
    );

    auto t3 = resolution_clock::now();
    std::cerr << std::chrono::duration<double>(t3-t1).count() << "s\n";

    std::cout
      << " ## result: " << result
      << "\nunrecognized_count: " << unrecognized_count
      << "\ncount: " << ambiguous.size()
      << "\nunrecognized_rate: " << double(unrecognized_count * 100) / (ambiguous.empty()? ambiguous.size() :1)
      << std::endl
    ;
}
