#ifndef CATMAPPING_HPP_
#define CATMAPPING_HPP_

#include <HDTEnums.hpp>
#include <vector>

using namespace std;

namespace hdt {

class CatMapping {
private:
    vector<pair<size_t, Mapping>> mapping;

public:

    CatMapping();

    ~CatMapping();

    void init(size_t size);

    size_t getMapping(size_t index);

    Mapping getType(size_t index);

    void set(size_t i, size_t mapping, Mapping type);

    size_t getSize();
};

}

#endif //CATMAPPING_HPP_
