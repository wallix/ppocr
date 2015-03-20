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

#ifndef REDEMPTION_STRATEGIES_ALTERNATION_HPP
#define REDEMPTION_STRATEGIES_ALTERNATION_HPP

#include <vector>
#include <array>
#include <iosfwd>


class Image;

namespace strategies
{
    /**
     *                             ::::::::::::
     *                             :xx        :
     *                             :xx        :
     *                             :xx        :
     *                             :xx        :
     *    Hl1, Hl2,                :xx        :
     *    Hm1, Hm2,                :xxxxxxx   :
     *    Vl1,                     :xxxxxxxx  :
     *    Vm1, Vm2                 :xx     xxx:
     *                             :xx      xx:
     *                             :xx     xxx:
     *                             :xxxxxxxxx :
     *                             :xxxxxxx   :
     *                             ::::::::::::
     *
     *   ::::::::::::
     *   :xx        :                                           ----------
     *   :xx        :                                          :xx        :
     *   :xx        :                                          :xx        : Hm1 = 1, 0
     *    ----------                                           :xx        : h = (img.h/3)
     *   |xx        | Hl1 = 1, 0 ; y = ((img.h-2)/3)           :xx        :
     *    ----------                                            ----------
     *   :xx        :                                          :xx        :
     *   :xxxxxxx   :                                          :xxxxxxx   :
     *   :xxxxxxxx  :                                          :xxxxxxxx  :
     *    ----------                                           :xx     xxx:
     *   |xx     xxx| Hl2 = 1, 0, 1 ; y = ((img.h*2-1)/3)       ----------
     *    ----------                                           :xx      xx:
     *   :xx      xx:                                          :xx     xxx: Hm2 = 1
     *   :xx     xxx:                                          :xxxxxxxxx : h = (img.h/3)
     *   :xxxxxxxxx :                                          :xxxxxxx   :
     *   :xxxxxxx   :                                           ----------
     *   ::::::::::::
     *
     *
     *  Vl1 = 0, 1, 0, 1 ; x = (img.w-1)/2      Vm1 = 1 ; w = img.w/3 ; Vm2 = 0, 1, 0
     *           ::::::_:::::::                        :___::::::___:
     *           :xx  | |     :                        |xx |    |   |
     *           :xx  | |     :                        |xx |    |   |
     *           :xx  | |     :                        |xx |    |   |
     *           :xx  | |     :                        |xx |    |   |
     *           :xx  | |     :                        |xx |    |   |
     *           :xxxx|x|xx   :                        |xxx|xxxx|   |
     *           :xxxx|x|xxx  :                        |xxx|xxxx|x  |
     *           :xx  | |  xxx:                        |xx |    |xxx|
     *           :xx  | |   xx:                        |xx |    | xx|
     *           :xx  | |  xxx:                        |xx |    |xxx|
     *           :xxxx|x|xxxx :                        |xxx|xxxx|xx |
     *           :xxxx|x|xx   :                        |xxx|xxxx|   |
     *           ::::::-:::::::                        :---::::::---:
     */
    struct alternations
    {
        using sequence_type = std::vector<bool>;

        alternations() = default;

        alternations(const Image & img, const Image & img90);

        sequence_type const & operator[](size_t i) const /*noexcept*/
        { return seq_alternations[i]; }

        size_t size() const noexcept { return seq_alternations.size(); }
        auto begin() const noexcept { return seq_alternations.begin(); }
        auto end() const noexcept { return seq_alternations.end(); }

        bool operator<(alternations const & other) const
        { return seq_alternations < other.seq_alternations; }

        bool operator==(alternations const & other) const
        { return seq_alternations == other.seq_alternations; }

        unsigned relationship(const alternations& other) const;

        friend std::istream & operator>>(std::istream &, alternations &);

    private:
        std::array<sequence_type, 7> seq_alternations;
    };

    std::ostream & operator<<(std::ostream &, alternations const &);


    /// TODO alternations -> alternation< Hl1>,  alternation< Hl2>,  alternation< Hm1>, etc

//     class alternations
//     {
//         using internal_value_ = uint_fast16_t;
//
//     public:
//         using value_type = bool;
//
//         struct const_iterator
//         {
//             using iterator_category = std::forward_iterator_tag;
//
//             const_iterator(internal_value_ const & v, unsigned offset = 0)
//             : refv(v)
//             , offset(offset)
//             {}
//
//             const_iterator & operator++() {
//                 ++offset;
//                 return *this;
//             }
//
//             bool operator*() const noexcept {
//                 return refv & (1 << offset);
//             }
//
//             bool operator<(const_iterator const & other) const noexcept {
//                 return offset < other.offset;
//             }
//
//             bool operator == (const_iterator const & other) const noexcept {
//                 return offset == other.offset;
//             }
//
//             bool operator != (const_iterator const & other) const noexcept {
//                 return offset != other.offset;
//             }
//
//         private:
//             std::reference_wrapper<const internal_value_> refv;
//             unsigned offset;
//         };
//
//         vlight_type() = default;
//
//         vlight_type(size_t v, size_t offset) noexcept
//         : v(v)
//         , sz(offset)
//         {
//             assert(sz < 16);
//         }
//
//
//         void push_back(bool x) {
//             assert(sz < 16);
//             if (x) {
//                 v |= 1 << sz;
//             }
//             ++sz;
//         }
//
//         size_t size() const {
//             return sz;
//         }
//
//         bool front() const {
//             return v & 1;
//         }
//
//         bool back() const {
//             return v & (1 << (sz-1));
//         }
//
//         const_iterator begin() const {
//             return const_iterator(v, 0);
//         }
//
//         const_iterator end() const {
//             return const_iterator(v, sz);
//         }
//
//         bool operator < (vlight_type const & other) const noexcept {
//             return sz < other.sz && v < other.v;
//         }
//
//         bool operator == (vlight_type const & other) const noexcept {
//             return v == other.v && sz == other.sz;
//         }
//
//         internal_value_ data() const noexcept {
//             return v;
//         }
//
//     private:
//         internal_value_ v = 0;
//         unsigned sz = 0;
//     };
}

#endif
