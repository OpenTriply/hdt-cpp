#ifndef CATMAPPINGBACK_HPP_
#define CATMAPPINGBACK_HPP_

#include <HDTEnums.hpp>
#include <vector>

using namespace std;

namespace hdt {

class CatMappingBack {
private:
    vector<pair<size_t, Mapping>> mapping1;
    vector<pair<size_t, Mapping>> mapping2;

public:
    CatMappingBack();
    ~CatMappingBack();

    void init(size_t size);

    vector<size_t> getMapping(size_t index);

    vector<Mapping> getType(size_t index);

    void set(size_t i, size_t mapping, Mapping type);

    size_t getSize();
};

}

#endif //CATMAPPINGBACK_HPP_
