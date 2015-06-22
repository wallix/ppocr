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
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#ifndef REDEMPTION_CAPTURE_RDP_PPOCR_GLYPHS_HPP
#define REDEMPTION_CAPTURE_RDP_PPOCR_GLYPHS_HPP

#include <map>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <istream>
#include <algorithm>

#include "make_unique.hpp"

namespace ppocr { namespace ocr2 {

struct View {
    unsigned word;
    unsigned font;
    View() = default;
    View(View &&) = default;
    View(View const &) = default; //delete;
    View & operator=(View &&) = default;
    View & operator=(View const &) = default; //delete;
};
struct Views : std::vector<View> {
    using std::vector<View>::vector;
    Views() = default;
    Views(Views &&) = default;
    Views(Views const &) = delete;
    Views & operator=(Views &&) = default;
    Views & operator=(Views const &) = delete;
};
struct Glyphs : std::vector<Views> {
    using std::vector<Views>::vector;
    Glyphs() = default;
    Glyphs(Glyphs &&) = default;
    Glyphs(Glyphs const &) = delete;
    Glyphs & operator=(Glyphs &&) = default;
    Glyphs & operator=(Glyphs const &) = delete;

    struct string {
        unsigned sz;
        char word[1];

        char const * data() const { return this->word; }
        char const * begin() const { return this->word; }
        char const * end() const { return this->word + this->sz; }
        unsigned size() const { return this->sz; }
        char front() const { return this->word[0]; }
    };

    string const & get_word(View const & v) const
    { return this->get_word_(v.word); }

    constexpr static unsigned const no_index = ~unsigned{};

    unsigned word_index_of(std::string const & s) const {
        unsigned i = 0;
        while (i < this->buf_word_sz) {
            auto r = s.compare(0, s.size(), this->get_word_(i).data(), s.size());
            if (r <= 0) {
                if (r == 0) {
                    return i;
                }
                break;
            }
            i += (this->get_word_(i).size() + sizeof(unsigned) * 2 - 1) / sizeof(unsigned);
        }
        return no_index;
    }

private:
    std::unique_ptr<unsigned[]> buf_word;
    unsigned buf_word_sz;

    string const & get_word_(unsigned i) const
    { return *reinterpret_cast<string const *>(this->buf_word.get() + i); }

    friend std::istream & operator>>(std::istream & is, Glyphs & glyphs);
};

inline std::istream & operator>>(std::istream & is, Glyphs & glyphs) {
    std::map<std::string, unsigned> font_map;
    std::map<std::string, unsigned> word_map;

    std::string font;
    std::string word;
    while (is.ignore(1000, '\n')) {
        unsigned n;
        if (!(is >> n)) {
            break;
        }
        Views views;
        views.resize(n);

        unsigned i = 0;
        while (i < n && (is >> word >> font).ignore(100, '\n')) {
            auto it_word = word_map.find(word);
            if (it_word == word_map.end()) {
                it_word = word_map.emplace(std::move(word), word_map.size()).first;
            }
            views[i].word = it_word->second;

            auto it_font = font_map.find(font);
            if (it_font == font_map.end()) {
                it_font = font_map.emplace(std::move(font), font_map.size()).first;
            }
            views[i].font = it_font->second;

            ++i;
        }

        glyphs.push_back(std::move(views));
    }

    std::unique_ptr<unsigned[]> buf_word;
    unsigned buf_word_sz = 0;
    {
        std::vector<unsigned> reindex(word_map.size());
        for (auto & p : word_map) {
            reindex[p.second] = buf_word_sz;
            buf_word_sz += (p.first.size() + sizeof(unsigned) - 1) / sizeof(unsigned) + 1;
        }
        buf_word = std::make_unique<unsigned[]>(buf_word_sz);
        for (auto & views : glyphs) {
            for (View & v : views) {
                v.word = reindex[v.word];
            }
        }
    }
    {
        auto buf = buf_word.get();
        for (auto & p : word_map) {
            *buf = p.first.size();
            ++buf;
            std::copy(p.first.begin(), p.first.end(), reinterpret_cast<char*>(buf));
            buf += (p.first.size() + sizeof(unsigned) - 1) / sizeof(unsigned);
        }
    }

    glyphs.buf_word = std::move(buf_word);
    glyphs.buf_word_sz = buf_word_sz;

    return is;
}


struct EqViewWord {
    EqViewWord() {}
    bool operator()(View const & v1, View const & v2) {
        return v1.word == v2.word;
    }
};

struct EqViews {
    bool operator()(Views const & v1, Views const & v2) const {
        return v1.size() == v2.size() && std::equal(v1.begin(), v1.end(), v2.begin(), EqViewWord{});
    }
};

struct LtViews {
    bool operator()(Views const & v1, Views const & v2) const {
        if (v1.size() < v2.size()) {
            return true;
        }
        if (v1.size() > v2.size()) {
            return false;
        }
        auto const pair = std::mismatch(v1.begin(), v1.end(), v2.begin(), EqViewWord{});
        if (pair.first == v1.end()) {
            return false;
        }
        return pair.first->word < pair.second->word;
    }
};

} }

#endif
