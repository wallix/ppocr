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
*   Copyright (C) Wallix 2015
*   Author(s): Jonathan Poelen
*/

#ifndef PPOCR_STRATEGIES_ALTERNATION_HPP
#define PPOCR_STRATEGIES_ALTERNATION_HPP

#include <array>
#include <iosfwd>

namespace ppocr {

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
        struct alternations_type {
            bool start_contains_letter;
            std::size_t count;

            bool operator==(alternations_type const & other) const noexcept
            { return start_contains_letter == other.start_contains_letter && count == other.count; }

            bool operator<(alternations_type const & other) const noexcept
            {
                return count < other.count
                    || (count == other.count && start_contains_letter < other.start_contains_letter);
            }
        };
        using sequence_type = std::array<alternations_type, 7>;

        alternations() = default;

        alternations(const Image & img, const Image & img90);

        alternations_type const & operator[](size_t i) const /*noexcept*/
        { return seq_alternations[i]; }

        std::size_t size() const noexcept { return seq_alternations.size(); }

        sequence_type::const_iterator begin() const noexcept
        { return seq_alternations.begin(); }
        sequence_type::const_iterator end() const noexcept
        { return seq_alternations.end(); }

        bool operator<(alternations const & other) const
        { return seq_alternations < other.seq_alternations; }

        bool operator==(alternations const & other) const
        { return seq_alternations == other.seq_alternations; }

        unsigned relationship(const alternations& other) const;
        unsigned best_difference() const;

        friend std::istream & operator>>(std::istream &, alternations &);

    private:
        sequence_type seq_alternations;
    };

    std::ostream & operator<<(std::ostream &, alternations const &);


    /// TODO alternations -> alternation< Hl1>,  alternation< Hl2>,  alternation< Hm1>, etc
}

}

#endif
