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

#ifndef REDEMPTION_STRATEGIES_HPP
#define REDEMPTION_STRATEGIES_HPP

#include <vector>
#include <memory>
#include <iosfwd>


class Image;

class DataLoader
{
    struct data_base
    {
        virtual ~data_base() {}

        virtual bool less(data_base const & other) const = 0;
        virtual bool eq(data_base const & other) const = 0;
        virtual void write(std::ostream & os) = 0;
        virtual void read(std::istream & is) = 0;
    };

    template<class Strategy>
    struct data : data_base
    {
        //data() = default;
        data(data const &) = delete;
        data& operator=(data const &) = delete;

        data(Image const & img, Image const & img90)
        : strategy(img, img90)
        {}

    private:
        virtual bool eq(const data_base& other) const
        { return this->strategy == static_cast<data const&>(other).strategy; }

        virtual bool less(const data_base& other) const
        { return this->strategy < static_cast<data const&>(other).strategy; }

        virtual void write(std::ostream & os)
        { os << this->strategy; }

        virtual void read(std::istream & is)
        { is >> this->strategy; }

        Strategy strategy;
    };

    using data_ptr = std::unique_ptr<data_base>;

    struct loader_base
    {
        loader_base(std::string name) : name_(std::move(name)) {}

        std::string const & name() const noexcept { return this->name_; }


        virtual data_ptr new_strategy(Image const & img, Image const & img90) const = 0;
//         virtual data_ptr new_strategy() const = 0;

        virtual ~loader_base() {}

    private:
        std::string name_;
    };

    template<class Strategy>
    struct loader : loader_base
    {
        loader(std::string name) : loader_base{std::move(name)} {}

        virtual data_ptr new_strategy(Image const & img, Image const & img90) const
        { return data_ptr{new data<Strategy>(img, img90)}; }

//         virtual data_ptr new_strategy() const
//         { return data_ptr{new data<Strategy>}; }
    };

    struct Loader
    {
        Loader(std::unique_ptr<loader_base> p) noexcept : p(std::move(p)) {}

        std::string const & name() const noexcept
        { return p->name(); }

        data_ptr new_strategy(Image const & img, Image const & img90) const
        { return p->new_strategy(img, img90); }

//         virtual data_ptr new_strategy() const
//         { return data_ptr{new data<Strategy>}; }

        std::unique_ptr<loader_base> p;
    };

public:
    DataLoader() = default;
    DataLoader(DataLoader const &) = delete;
    DataLoader operator=(DataLoader const &) = delete;

    struct Data
    {
        bool operator<(const Data& other) const;
        bool operator==(const Data& other) const;

        friend std::istream & operator>>(std::istream & is, DataLoader::Data & data)
        { data.loader_.get().read(is, data); return is; }

        friend std::ostream & operator<<(std::ostream & os, DataLoader::Data const & data)
        { data.loader_.get().write(os, data); return os; }

    private:
        Data(std::vector<data_ptr> datas, std::reference_wrapper<const DataLoader> loader)
        : loader_(loader)
        , datas_(std::move(datas))
        {}

        std::reference_wrapper<const DataLoader> loader_;
        std::vector<data_ptr> datas_;

        friend DataLoader;
    };

    template<class Strategy>
    void registry(char const * name)
    {
        this->loaders.emplace_back(std::make_unique<loader<Strategy>>(name));
    }

//     Data new_data() const;
    Data new_data(Image const & img, Image const & img90) const;

    void read(std::istream & is, Data & data) const;
    void write(std::ostream & os, Data const & data) const;

    std::vector<Loader> loaders;
};


// struct DataReader { DataLoader const & loader; DataLoader::Data & data; };
//
// inline DataReader data_reader(DataLoader const & loader, DataLoader::Data & data)
// { return {loader, data}; }
//
// inline std::istream & operator>>(std::istream & is, DataReader const & data)
// { data.loader.read(is, data.data); return is; }
//
//
// struct DataWriter { DataLoader const & loader; DataLoader::Data const & data; };
//
// inline DataWriter data_writer(DataLoader const & loader, DataLoader::Data const & data)
// { return {loader, data}; }
//
// inline std::ostream & operator<<(std::ostream & os, DataWriter const & data)
// { data.loader.write(os, data.data); return os; }

#endif
