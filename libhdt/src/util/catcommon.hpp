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
    bool has_next;
    size_t count1, count2;
    pair<size_t, size_t> next_t;

public:
    CatCommon(IteratorUCharString *it1, IteratorUCharString *it2);
    ~CatCommon();

    bool hasNext();
    pair<size_t,size_t> next();
    void helpNext();
};

}


#endif //CATCOMMON_HPP_
