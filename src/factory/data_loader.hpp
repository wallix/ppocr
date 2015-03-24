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

template<class IteratorBase, class Proxy>
struct proxy_iterator : IteratorBase, private Proxy
{
    proxy_iterator(IteratorBase base, Proxy proxy)
    : IteratorBase(base)
    , Proxy(proxy)
    {}

    proxy_iterator(IteratorBase base)
    : IteratorBase(base)
    {}

    decltype(auto) operator*() { return this->proxy()(*this->base()); }
    decltype(auto) operator->() { return &this->proxy()(*this->base()); }
    decltype(auto) operator[](std::size_t i) { return this->proxy()(*(this->base()+i)); }

private:
    IteratorBase & base() { return static_cast<IteratorBase&>(*this); }
    Proxy & proxy() { return static_cast<Proxy&>(*this); }
};

namespace detail {
    struct DefaultProxy {
        DefaultProxy() {}
        template<class T> T const & operator()(T const & x) { return x; }
        template<class T> T& operator()(T& x) { return x; }
        template<class T> T operator()(T&& x) { return std::forward<T>(x); }
    };
}

template<class Iterator, class Proxy = detail::DefaultProxy>
struct range_iterator
{
    range_iterator(Iterator first, Iterator last)
    : first_(first)
    , last_(last)
    {}

    range_iterator(Iterator first, Iterator last, Proxy proxy)
    : first_(first)
    , last_(last)
    , proxy_(proxy)
    {}

    proxy_iterator<Iterator, Proxy> begin() const { return {this->first_, this->proxy_}; }
    proxy_iterator<Iterator, Proxy> end() const { return {this->last_, this->proxy_}; }
    decltype(auto) operator[](std::size_t i) const { return this->proxy_(*(this->first_+i)); }
    decltype(auto) front() const { return this->proxy_(*this->first_); }
    decltype(auto) back() const { return this->proxy_(*(this->last_-1)); }

    auto size() const { return this->last_ - this->first_; }
    bool empty() const { return this->first_ == this->last_; }

private:
    Iterator first_;
    Iterator last_;
    Proxy proxy_;
};


class Image;

struct DataLoader
{
    struct data_base
    {
        virtual ~data_base() {}

        virtual bool lt(data_base const & other) const = 0;
        virtual bool eq(data_base const & other) const = 0;
        virtual void write(std::ostream & os) const = 0;
        virtual void read(std::istream & is) = 0;
        virtual unsigned relationship(const data_base& other) const = 0;
    };

    template<class Strategy>
    struct data : data_base
    {
        data() = default;
        data(data const &) = delete;
        data& operator=(data const &) = delete;

        data(Image const & img, Image const & img90)
        : strategy(img, img90)
        {}

    private:
        virtual bool eq(const data_base& other) const override
        { return this->strategy == static_cast<data const&>(other).strategy; }

        virtual bool lt(const data_base& other) const override
        { return this->strategy < static_cast<data const&>(other).strategy; }

        virtual unsigned relationship(const data_base& other) const
        { return this->strategy.relationship(static_cast<data const&>(other).strategy); }

        virtual void write(std::ostream & os) const override
        { os << this->strategy; }

        virtual void read(std::istream & is) override
        { is >> this->strategy; }

        Strategy strategy;
    };

    using data_ptr = std::unique_ptr<data_base>;

    struct loader_base
    {
        virtual data_ptr new_strategy() const = 0;
        virtual data_ptr new_strategy(Image const & img, Image const & img90) const = 0;

        virtual ~loader_base() {}
    };

    template<class Strategy>
    struct loader : loader_base
    {
        virtual data_ptr new_strategy() const override
        { return data_ptr{new data<Strategy>()}; }

        virtual data_ptr new_strategy(Image const & img, Image const & img90) const override
        { return data_ptr{new data<Strategy>(img, img90)}; }
    };

    struct Loader
    {
        Loader(std::string name, std::unique_ptr<loader_base> p) noexcept
        : p_(std::move(p))
        , name_(std::move(name))
        {}

        std::string const & name() const noexcept { return this->name_; }

        data_ptr new_strategy(Image const & img, Image const & img90) const
        { return p_->new_strategy(img, img90); }

        data_ptr new_strategy() const
        { return p_->new_strategy(); }

        std::unique_ptr<loader_base> p_;
        std::string name_;
    };

    using data_ptr_sequence = std::vector<data_ptr>;

public:
    DataLoader() = default;
    DataLoader(DataLoader const &) = delete;
    DataLoader operator=(DataLoader const &) = delete;

    struct Datas
    {
        Datas() = default;

        bool operator<(const Datas & other) const;
        bool operator==(const Datas & other) const;

        std::vector<unsigned> relationships(const Datas& other) const;

        data_base const & operator[](std::size_t i) const { return *this->datas_[i].get(); }

        struct proxy
        {
            proxy() {}

            data_base const & operator()(data_ptr const & p) const
            { return *p.get(); }
        };

        range_iterator<data_ptr_sequence::const_iterator, proxy> datas() const
        { return {this->datas_.begin(), this->datas_.end(), proxy()}; };

        std::size_t size() const noexcept
        { return this->datas_.size(); }

    private:
        Datas(data_ptr_sequence datas)
        : datas_(std::move(datas))
        {}

        data_ptr_sequence datas_;

        friend DataLoader;
    };

    template<class Strategy>
    void registry(char const * name)
    {
        this->loaders.emplace_back(name, std::make_unique<loader<Strategy>>());
    }

    Datas new_data() const;
    Datas new_data(std::istream & is) const;
    Datas new_data(Image const & img, Image const & img90) const;

    void read(std::istream & is, Datas & data) const;
    void write(std::ostream & os, Datas const & data) const;

    struct DatasReader { DataLoader const & loader; Datas & data; };
    DatasReader reader(Datas & data) const { return {*this, data}; }

    struct DatasWriter { DataLoader const & loader; Datas const & data; };
    DatasWriter writer(Datas const & data) const { return {*this, data}; }

    struct proxy
    {
        proxy() {}

        std::string const & operator()(Loader const & p) const
        { return p.name(); }
    };

    using name_list = range_iterator<std::vector<Loader>::const_iterator, proxy>;

    name_list names() const
    { return {this->loaders.begin(), this->loaders.end(), proxy()}; };

    std::size_t size() const
    { return this->loaders.size(); }

private:
    std::vector<Loader> loaders;
};


inline std::istream & operator>>(std::istream & is, DataLoader::data_base & data)
{ data.read(is); return is; }

inline std::ostream & operator<<(std::ostream & os, DataLoader::data_base const & data)
{ data.write(os); return os; }


inline std::istream & operator>>(std::istream & is, DataLoader::DatasReader r)
{ r.loader.read(is, r.data); return is; }

inline std::ostream & operator<<(std::ostream & os, DataLoader::DatasWriter w)
{ w.loader.write(os, w.data); return os; }

#endif