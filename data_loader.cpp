#include "data_loader.hpp"

#include <cassert>
#include <cstring>
#include <ostream>


bool DataLoader::Data::operator<(const DataLoader::Data& other) const
{
    auto it2 = other.datas_.begin();
    for (data_ptr const & data : this->datas_) {
        // TODO compare (this is a default implementation)
        if (data->less(**it2)) {
            return true;
        }
        if (!data->eq(**it2)) {
            return false;
        }
        ++it2;
    }
    return false;
}

bool DataLoader::Data::operator==(const DataLoader::Data& other) const
{
    auto it2 = other.datas_.begin();
    for (data_ptr const & data : this->datas_) {
        if (!data->eq(**it2)) {
            return false;
        }
        ++it2;
    }
    return true;
}

DataLoader::Data DataLoader::new_data() const
{
    std::vector<data_ptr> datas;

    for (auto & factory : loaders) {
        datas.push_back(factory.new_strategy());
    }

    return {std::move(datas)};
}

DataLoader::Data DataLoader::new_data(std::istream& is) const
{
    auto data = this->new_data();
    read(is, data);
    return data;
}

DataLoader::Data DataLoader::new_data(Image const & img, Image const & img90) const
{
    std::vector<data_ptr> datas;

    for (auto & factory : loaders) {
        datas.push_back(factory.new_strategy(img, img90));
    }

    return {std::move(datas)};
}

void DataLoader::read(std::istream& is, DataLoader::Data& data) const
{
    std::string s;
    // TODO dichotomic
    for (auto & p : data.datas_) {
        is >> s;
        p->read(is);
    }
}

void DataLoader::write(std::ostream& os, DataLoader::Data const & data) const
{
    auto it = this->loaders.begin();
    for (auto & p : data.datas_) {
        os << it->name() << ' ';
        p->write(os);
        os << '\n';
        ++it;
    }
}
