#ifndef CATCOMMON_HPP_
#define CATCOMMON_HPP_

#include <Iterator.hpp>
#include <vector>

namespace hdt {

class CatCommon {

private:
    vector<pair<size_t, unsigned char *>> list;
    IteratorUCharString *it1;
    IteratorUCharString *it2;
    bool hasNext;
    size_t count1, count2;
    pair<size_t, size_t> next;

public:
    CatCommon(IteratorUCharString *it1, IteratorUCharString *it2);
    ~CatCommon();

    void helpNext();
};

}


#endif //CATCOMMON_HPP_
