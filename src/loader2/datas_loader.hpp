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
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#ifndef PPOCR_SRC_LOADER2_DATAS_LOADER_HPP
#define PPOCR_SRC_LOADER2_DATAS_LOADER_HPP

#include <cstddef>
#include <vector>
#include <type_traits>

#include "image/image.hpp"

namespace ppocr {

class Image;

namespace loader2 {

using std::size_t;

enum class PolicyLoader { img, img90 };

template<class Strategy_, PolicyLoader Policy>
struct Strategy
{
    using strategy_type = Strategy_;
    using policy = std::integral_constant<PolicyLoader, Policy>;
};

template<class Strategy>
struct Data
{
    using strategy_type = typename Strategy::strategy_type;
    using value_type = typename strategy_type::value_type;
    using relationship_type = typename strategy_type::relationship_type;

    value_type const & operator[](size_t i) const { return data.values[i]; }

    unsigned relationship(value_type const & a, value_type const & b) const
    { return static_cast<relationship_type const &>(data)(a, b); }

    void load(Image const & img, Image const & img90) {
        this->data.load_impl(img, img90, typename Strategy::policy());
    }

private:
    struct impl : strategy_type, relationship_type /*empty class optimization*/ {
        std::vector<value_type> values;

        impl()
        : relationship_type(static_cast<strategy_type const &>(*this).relationship())
        {}

        template<PolicyLoader Policy>
        using policy = std::integral_constant<PolicyLoader, Policy>;

        void load_impl(Image const & img, Image const & img90, policy<PolicyLoader::img>) {
            this->values.push_back(this->load(img, img90));
        }

        void load_impl(Image const & img, Image const & img90, policy<PolicyLoader::img90>) {
            this->values.push_back(this->load(img90, img));
        }
    } data;
};

template<class... Strategies>
struct DatasLoader
{
    DatasLoader() = default;
    DatasLoader(DatasLoader const &) = delete;

    void load(Image const & img) {
        this->load(img, img.rotate90());
    }

    void load(Image const & img, Image const & img90) {
        (void)std::initializer_list<char>{
            (static_cast<Data<Strategies>&>(this->datas).load(img, img90), '\0')...
        };
    }

    template<class Strategy>
    Data<Strategy> const & get() const
    { return static_cast<Data<Strategy> const &>(this->datas); }

private:
    struct DatasStrategies : Data<Strategies>... {
    } datas;
};

} }

#endif
