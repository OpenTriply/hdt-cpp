#include "catmapping.hpp"
#include <iostream>

namespace hdt {

CatMapping::CatMapping() {}

CatMapping::~CatMapping() {}

void CatMapping::init(size_t size) {
    this->mapping.resize(size);
}

size_t CatMapping::getMapping(size_t index) {
    return mapping[index].first;
}

Mapping CatMapping::getType(size_t index) {
    return mapping[index].second;
}

void CatMapping::set(size_t index, size_t mapping, Mapping type) {
    this->mapping[index] = make_pair(mapping, type);
}

size_t CatMapping::getSize() {
    return mapping.size();
}

}

