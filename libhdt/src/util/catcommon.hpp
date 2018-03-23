#ifndef CATCOMMON_HPP_
#define CATCOMMON_HPP_

#include <Iterator.hpp>
#include <vector>

namespace hdt {

class CatCommon {

private:
    // Vector of (two) pairs to be compared
    vector<pair<size_t, unsigned char *>> list;

    IteratorUCharString *it1;
    IteratorUCharString *it2;
    bool has_next, counted;
    size_t count1, count2, commonNum;
    pair<size_t, size_t> next_t;

public:
    CatCommon(IteratorUCharString *it1, IteratorUCharString *it2);
    ~CatCommon();

    /**
     * @return True if iterators have another pair of common terms,
     * otherwise False.
     */
    bool hasNext();

    /**
     * Returns the next common pair of the iterators.
     * @return A pair of indexes of common terms;
     * one from iterator 1 and one from iterator 2.
     */
    pair<size_t,size_t> next();

    /**
     * Auxiliary function that sets the next common pairs in list.
     */
    void helpNext();

    /**
     * Get the total of common terms.
     * @return Number of common terms.
     */
    size_t getCommonNum();
};

}

#endif //CATCOMMON_HPP_
