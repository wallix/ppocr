/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Raphael Zhou, Jonathan Poelen, Meng Tan
 */

#ifndef REDEMPTION_WAB_9_X_CAPTURE_OCR_CLASSIFICATION_HH
#define REDEMPTION_WAB_9_X_CAPTURE_OCR_CLASSIFICATION_HH

#include <mln/image/image2d.hh>

#include "fonts.hpp"
#include "labelize.hh"
#include "extract_data.hh"
#include "utf.hpp"

#include <vector>
#include <string>

namespace ocr {

namespace locale {
    struct latin_to_cyrillic_context
    {
        void latin_to_cyrillic(std::string & out)
        {
            const char * str = out.c_str();
            UTF8toUnicodeIterator utf8_it(str);
            while (*utf8_it) {
                if (*utf8_it == ' ') {
                    str = get_pos(utf8_it);
                    ++utf8_it;
                }
                else if (![&]() -> bool {
                    for (auto c : latin_cyrillic_conflicts) {
                        if (c.code_latin == *utf8_it) {
                            char const * const utf8_ptr = get_pos(utf8_it);
                            while (*++utf8_it && *utf8_it != ' ') {
                            }
                            auto stre = reinterpret_cast<char const *>(utf8_it.pos() - 1);

                            size_t latin = 0;
                            size_t cyrillic = 0;
                            size_t unknow = 0;
                            for (UTF8toUnicodeIterator w(str); w.pos() != utf8_it.pos(); ++w) {
                                if (is_latin_alpha(*w)) {
                                    if ([&]{
                                        for (auto & c : latin_cyrillic_conflicts) {
                                            if (c.code_latin == *w) {
                                                return true;
                                            }
                                        }
                                        return false;
                                    }()) {
                                        ++latin;
                                    }
                                }
                                else if (is_cyrillic_alpha(*w)) {
                                    ++cyrillic;
                                }
                                else if (*w != '?') {
                                    ++unknow;
                                }
                            }

                            if (!(unknow > (latin + cyrillic) / 2)) {
                                if (cyrillic / 2 > latin) {
                                    this->out_tmp.clear();
                                    const char * p = utf8_ptr - 1;
                                    UTF8toUnicodeIterator w(p);
                                    for (; w.pos() != utf8_it.pos(); ++w) {
                                        [&](){
                                            if (is_latin_alpha(*w)) {
                                                for (auto & c : latin_cyrillic_conflicts) {
                                                    if (c.code_latin == *w) {

                                                        out_tmp += c.str_cyrillic;
                                                        return ;
                                                    }
                                                }
                                            }
                                            out_tmp.append(p, get_pos(w));
                                        }();
                                        p = get_pos(w);
                                    }
                                    str = utf8_ptr - 1;
                                    size_t const pos = str - out.c_str();
                                    out.replace(pos, get_pos(w) - 1 - str, out_tmp);
                                    str = out.c_str() + pos;
                                    stre = str + out_tmp.size();
                                }
                                else if (*(str+1) == ' ' || !*(str+1)) {
                                    if (*str == 'c') {
                                        out.replace(str - out.c_str(), 1, "\u0441");
                                    }
                                    else if (*str == 'C') {
                                        out.replace(str - out.c_str(), 1, "\u0421");
                                    }
                                }
                            }

                            str = stre;
                            utf8_it = UTF8toUnicodeIterator(str);
                            return true;
                        }
                    }
                    return false;
                }()) {
                    ++utf8_it;
                }
            }
        }

    private:
        struct conflict_t {
            uint32_t code_latin;
            char const * str_cyrillic;
        };

        static constexpr const conflict_t latin_cyrillic_conflicts[17]{
            {'a', "\u0430"}, // "а"
            {'A', "\u0410"}, // "А"
            {'e', "\u0435"}, // "е"
            {'E', "\u0415"}, // "Е"
            {'i', "\u0456"}, // "і"
            {'I', "\u0406"}, // "І"
            {'K', "\u041A"}, // "К"
            {'o', "\u043E"}, // "о"
            {'O', "\u041E"}, // "О"
            {'M', "\u041C"}, // "М"
            {'H', "\u041D"}, // "Н"
            {'p', "\u0440"}, // "р"
            {'P', "\u0420"}, // "Р"
            {'c', "\u0441"}, // "с"
            {'C', "\u0421"}, // "С"
            {'T', "\u0422"}, // "Т"
            {'y', "\u0432"}, // "у"
        };

        static bool is_latin_alpha(uint32_t c) noexcept
        { return (97 <= c && c <= 122) || (65 <= c && c <= 90); }

        static bool is_cyrillic_alpha(uint32_t c) noexcept
        { return ((1040 <= c && c <= 1071) || (1072 <= c && c <= 1103)); }

        static const char * get_pos(UTF8toUnicodeIterator const & it) noexcept
        { return reinterpret_cast<const char*>(it.pos()); }

        std::string out_tmp;
    };
    constexpr decltype(latin_to_cyrillic_context::latin_cyrillic_conflicts)
        latin_to_cyrillic_context::latin_cyrillic_conflicts;
}

struct classifier_type
: private locale::latin_to_cyrillic_context
{
    std::string out;

private:
    std::string out_tmp;

public:
    unsigned character_count;
    unsigned unrecognized_count;
    unsigned first_unrecognized_index;
    unsigned font_id;

    classifier_type(unsigned font = -1u)
    : character_count(0)
    , unrecognized_count(0)
    , first_unrecognized_index(0)
    , font_id(font)
    {}

    unsigned unrecognized_rate() const
    {
        if (this->character_count) {
            return this->unrecognized_count * 100 / this->character_count;
        }
        return 100;
    }

    void clear()
    {
        this->out.clear();
        this->character_count = 0;
        this->unrecognized_count = 0;
        this->first_unrecognized_index = 0;
        this->font_id = 0;
    }

    void classify(
        std::vector<ocr::label_attr_t> attrs, const ::mln::image2d<bool> & input
      , ocr::fonts::LocaleId local_id, unsigned font_id
    ) {
        this->clear();
        this->font_id = font_id;
        this->out.reserve(attrs.size());
        bool beginning = true;
        typedef std::vector<label_attr_t>::iterator iterator;
        iterator first = attrs.begin();
        iterator last = attrs.end();

        if (first == last) {
            return ;
        }

        const fonts::Font & font = fonts::fonts[local_id][font_id];

        for (; first < last; ++first) {
            const ::mln::box2d & bbox = first->bbox;
            const char* c = font.classify(bbox.ncols(), bbox.nrows(), first->area, fonts::Pixel(input, bbox, bbox.ncols()));

            if (!beginning) {
                if (first->bbox.pmin()[1] - (first-1)->bbox.pmax()[1] >= int(font.whitespace_width)) {
                    this->out += ' ';
                }
                this->out += c;
                ++this->character_count;

                if (c == unknown) {
                    ++this->unrecognized_count;
                }
            }
            else if (c == unknown) {
                ++this->first_unrecognized_index;
            }
            else {
                beginning = false;
                this->out += c;
                ++this->character_count;
            }
        }

        if (font.replacements) {
            for (auto it = font.replacements; it->pattern; ++it) {
                std::string::size_type pos = 0;
                while (pos != std::string::npos) {
                    const auto fpos = this->out.find(it->pattern, pos);
                    if (fpos != std::string::npos) {
                        this->out.replace(fpos, strlen(it->pattern), it->replace);
                    }
                    pos = fpos;
                }
            }
        }

        if (local_id == ocr::fonts::LocaleId::cyrillic) {
            this->latin_to_cyrillic(this->out);
        }
    }

    inline bool is_recognize() const
    {
        return 0 == this->unrecognized_count && this->first_unrecognized_index < 7 && this->character_count > 5;
    }
};


struct Classification
{
    typedef classifier_type result_type;

    Classification()
    {}

    const classifier_type & classify(
        const ::mln::image2d<bool> & input
      , ocr::fonts::LocaleId local_id = ocr::fonts::LocaleId::latin
      , unsigned font_id = -1u
    ) {
        this->attrs.clear();
        labelize(this->attrs, input);

        if (font_id == -1u) {
            this->classifier.clear();
            classifier_type classify_info;
            unsigned unrecognized_rate = 100;
            const unsigned nfonts = fonts::nfonts[local_id];
            for (unsigned id = 0; id < nfonts; ++id) {
                classify_info.classify(this->attrs, input, local_id, id);
                const unsigned unrecognized_rate2 = classify_info.unrecognized_rate();
                if (unrecognized_rate2 < unrecognized_rate) {
                    this->classifier = classify_info;
                    unrecognized_rate = unrecognized_rate2;
                    if (this->classifier.is_recognize()) {
                        break;
                    }
                }
            }
        }
        else {
            this->classifier.classify(this->attrs, input, local_id, font_id);
        }
        return this->classifier;
    }

private:
    Classification(const Classification &);
    Classification & operator=(const Classification &);

    std::vector<label_attr_t> attrs;
    classifier_type classifier;
};

}

#endif
