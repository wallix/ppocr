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

#ifndef REDEMPTION_SRC_CONTAINER_TRIE_HPP
#define REDEMPTION_SRC_CONTAINER_TRIE_HPP

#include <memory>
#include <vector>
#include <algorithm>
#include <type_traits>

namespace container {

template<class T>
struct trie
{
    class node_type
    {
        using value_type = T;
        using values_ = std::vector<node_type>;

        friend class trie;

        value_type value_;
        bool is_terminal_;
        trie nodes_;

    public:
        using iterator = typename values_::const_iterator;

        template<class U>
        node_type(U && x, bool is_terminal = false)
        : value_(std::forward<U>(x))
        , is_terminal_(is_terminal)
        {}

        iterator begin() const { return this->nodes_.begin(); }
        iterator end() const { return this->nodes_.end(); }

        template<class U>
        iterator find(U const & x) const { return this->nodes_.find(x); }

        trie const & nodes() const { return this->nodes_; }

        bool is_terminal() const { return this->is_terminal_; }
        std::size_t size() const { return this->nodes_.size(); }
        bool empty() const { return this->nodes_.empty(); }
        value_type const & get() const { return this->value_; }
    };

private:
    using values_ = std::vector<node_type>;

public:
    using value_type = T;
    using iterator = typename values_::const_iterator;

    trie() = default;

    /// \pre [first, last) is a sorted range
    template<class RandIt>
    trie(RandIt first, RandIt last) {
        this->insert_after(first, last);
    }

    template<class RandIt>
    void insert_after(RandIt first, RandIt last, unsigned depth = 0u) {
        if (last - first == 1) {
            for (auto back = this; depth != first->size(); ++depth) {
                back->nodes_.emplace_back((*first)[depth], depth + 1 == first->size());
                back = &back->trie_back_();
            }
        }
        else if (first != last) {
            if (first->size() <= depth + 1) {
                while (first != last && first->size() <= depth + 1) {
                    this->nodes_.emplace_back((*first)[depth], true);
                    ++first;
                }
                this->trie_back_().insert_after(first, last, depth+1);
            }
            else {
                while (first != last) {
                    auto middle = std::upper_bound(
                        first, last, (*first)[depth],
                        [depth](char c, std::string & s){ return c < s[depth]; }
                    );
                    this->nodes_.emplace_back((*first)[depth]);
                    this->trie_back_().insert_after(first, middle, depth+1);
                    first = middle;
                }
            }
        }
    }

    iterator begin() const { return this->nodes_.begin(); }
    iterator end() const { return this->nodes_.end(); }
    std::size_t size() const { return this->nodes_.size(); }
    bool empty() const { return this->nodes_.empty(); }

    template<class U>
    iterator find(U const & x) const {
        return std::lower_bound(this->begin(), this->end(), x, [](node_type const & node, U const & x) {
            return node.get() < x;
        });
    }

private:
    trie & trie_back_() { return this->nodes_.back().nodes_; }

    values_ nodes_;
};


template<class T>
struct flat_trie
{
    struct node_type;

    using iterator = node_type const *;

    struct range {
        range(node_type const * first, node_type const * last)
        : beg_(first)
        , end_(last)
        {}

        iterator begin() const { return this->beg_; }
        iterator end() const { return this->end_; }
        std::size_t size() const { return this->end() - this->begin(); }
        std::size_t empty() const { return this->beg_ == this->end_; }

        template<class U>
        iterator find(U const & x) const {
            return std::lower_bound(this->begin(), this->end(), x, [](node_type const & node, U const & x) {
                return node.get() < x;
            });
        }

    private:
        node_type const * beg_;
        node_type const * end_;
    };

    struct node_type {
        using size_type = unsigned char; // TODO depends on T
        using value_type = T;

        static_assert(~size_type{} >= ~typename std::make_unsigned<T>::type(), "unimplemented");

        node_type(node_type const &) = default;

        node_type(typename trie<T>::node_type const & node)
        : count_(node.size())
        , is_terminal_(node.is_terminal())
        , x_(node.get())
        {}

        node_type(value_type x, size_type pos, size_type sz, bool is_terminal)
        : pos_(pos)
        , count_(sz)
        , is_terminal_(is_terminal)
        , x_(x)
        {}

        range nodes() const { return {this->begin(), this->end()}; }
        iterator begin() const { return this + this->pos_; }
        iterator end() const { return this->begin() + this->size(); }

        template<class U>
        iterator find(U const & x) const { return this->nodes().find(x); }

        bool empty() const { return this->count_ == 0; }
        std::size_t size() const { return this->count_; }
        std::size_t relative_pos() const { return this->pos_; }
        bool is_terminal() const { return this->is_terminal_; }
        value_type const & get() const { return x_; }

    private:
        size_type pos_ = 0;
        size_type count_;
        bool is_terminal_;
        value_type x_;

        friend class flat_trie;
    };

    using value_type = T;

    template<class FwIt>
    flat_trie(FwIt first, FwIt last)
    : elems_(first, last)
    {}

    flat_trie(std::vector<node_type> && nodes)
    : elems_(std::move(nodes))
    {}

    flat_trie(std::vector<node_type> const & nodes)
    : elems_(nodes)
    {}

    flat_trie(trie<T> const & x)
    : elems_(x.begin(), x.end())
    {
        using iterator_base = decltype(x.begin());
        struct deref_it : iterator_base {
            deref_it(iterator_base base) : iterator_base(base) {}
            trie<T> const * operator * () const { return &iterator_base::operator*().nodes(); };
        };
        std::vector<trie<T> const *> ptries1(deref_it(x.begin()), deref_it(x.end()));
        std::vector<trie<T> const *> ptries2;
        std::size_t pos = 0;
        while (!ptries1.empty()) {
            for (auto * ptrie : ptries1) {
                std::size_t sz = elems_.size();
                elems_[pos].pos_ = sz-pos;
                if (!ptrie->empty()) {
                    elems_.insert(elems_.end(), ptrie->begin(), ptrie->end());
                    ptries2.insert(ptries2.end(), deref_it(ptrie->begin()), deref_it(ptrie->end()));
                }
                ++pos;
            }
            using std::swap;
            swap(ptries1, ptries2);
            ptries2.clear();
        }

        this->elems_.shrink_to_fit();
    }

    range nodes()
    { return {&this->elems_[0], &this->elems_[0] + (this->elems_.empty() ? 0u : this->elems_[0].size())}; }

    node_type const & operator[](std::size_t const & i) const { return this->elems_[i]; }

    std::vector<node_type> const & all() const { return this->elems_; };

private:
    std::vector<node_type> elems_;
};

}

#endif
