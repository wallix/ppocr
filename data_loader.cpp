#include "data_loader.hpp"

#include <cassert>
#include <cstring>
#include <ostream>


bool DataLoader::Data::operator<(const DataLoader::Data& other) const
{
    auto it = this->datas_.begin();
    auto it2 = other.datas_.begin();
    for (auto e = this->datas_.end(); it != e; ++it, ++it2) {
        if ((*it)->less(**it2)) {
            return true;
        }
        if (!(*it)->eq(**it2)) {
            return false;
        }
    }
    return false;
}

bool DataLoader::Data::operator==(const DataLoader::Data& other) const
{
    auto it = this->datas_.begin();
    auto it2 = other.datas_.begin();
    for (auto e = this->datas_.end(); it != e; ++it, ++it2) {
        if (!(*it)->eq(**it2)) {
            return false;
        }
    }
    return true;
}

// DataLoader::Data DataLoader::new_data() const
// {
//     std::vector<data_ptr> datas;
//
//     for (auto & factory : loaders) {
//         datas.push_back(factory->construct());
//     }
//
//     return {std::move(datas)};
// }

DataLoader::Data DataLoader::new_data(Image const & img, Image const & img90) const
{
    std::vector<data_ptr> datas;

    for (auto & factory : loaders) {
        datas.push_back(factory.new_strategy(img, img90));
    }

    return {std::move(datas), *this};
}


void DataLoader::read(std::istream& is, DataLoader::Data& data) const
{
    assert(data.datas_.size() == this->loaders.size() && &data.loader_.get() == this);

    std::string s;
    // TODO dichotomic
    for (auto & p : data.datas_) {
        is >> s;
        p->read(is);
    }
}

void DataLoader::write(std::ostream& os, DataLoader::Data const & data) const
{
    assert(data.datas_.size() == this->loaders.size() && &data.loader_.get() == this);

    auto it = this->loaders.begin();
    for (auto & p : data.datas_) {
        os << it->name() << ' ';
        p->write(os);
        os << '\n';
        ++it;
    }
}

