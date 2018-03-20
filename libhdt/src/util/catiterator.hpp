#ifndef CATITERATOR_HPP_
#define CATITERATOR_HPP_

#include <Iterator.hpp>
#include <vector>
#include "catcommon.hpp"

namespace hdt {

class CatIterator {
private:
    vector <pair<size_t, pair<size_t,size_t>>> list;
    CatCommon *it1, *it2;
public:
    CatIterator(CatCommon *it1, CatCommon *it2);

    ~CatIterator();

    bool hasNext();

    size_t next();

};

}

#endif //CATITERATOR_HP__
