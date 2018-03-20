#include "catmappingBack.hpp"
#include <iostream>

namespace hdt {

    CatMappingBack::CatMappingBack() {}

    CatMappingBack::~CatMappingBack() {}

    void CatMappingBack::init(size_t size) {
        this->mapping1.resize(size+1);
        this->mapping2.resize(size+1);
    }

    vector<size_t> CatMappingBack::getMapping(size_t index) {
        vector<size_t> r;
        if(mapping1[index].first != 0)
            r.push_back(mapping1[index].first);
        if(mapping2[index].first != 0)
            r.push_back(mapping2[index].first);
        return r;
    }

    vector<Mapping> CatMappingBack::getType(size_t index) {
        vector<Mapping> r;
        if(mapping1[index].first != 0)
            r.push_back(mapping1[index].second);
        if(mapping2[index].first != 0)
            r.push_back(mapping2[index].second);
        return r;
    }

    void CatMappingBack::set(size_t index, size_t mapping, Mapping type) {
        if(mapping1[index].first == 0) {
            this->mapping1[index] = make_pair(mapping, type);
        }
        else {
            this->mapping2[index] = make_pair(mapping, type);
        }
    }

    size_t CatMappingBack::getSize() {
        return mapping1.size();
    }

}



