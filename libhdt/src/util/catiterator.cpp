#include "catiterator.hpp"
#include <algorithm>

using namespace std;

namespace hdt {

CatIterator::CatIterator(CatCommon *it1, CatCommon *it2) : it1(it1), it2(it2) {
    if(it1->hasNext()) {
        list.push_back(make_pair((size_t) 1, it1->next()));
    }
    if(it1->hasNext()) {
        list.push_back(make_pair((size_t) 2, it1->next()));
    }
}

CatIterator::~CatIterator () {}

bool CatIterator::hasNext() {
    return list.size() > 0;
}


bool pairComparator(const pair<size_t, pair<size_t,size_t>>& p1, const pair<size_t, pair<size_t,size_t>>& p2) {
    return p1.second.first < p2.second.first;
}

size_t CatIterator::next() {
    size_t r;
    // TODO: check sort
    sort(list.begin(), list.end(), pairComparator);

    r = list[0].second.first;
    if(list[0].first == 1) {
        if(it1->hasNext()) {
            list[0] = make_pair((size_t) 1, it1->next());
        }
        else {
            list.erase(list.begin());
        }
    } else {
        if(it2->hasNext()) {
            list[0] = make_pair((size_t) 2, it2->next());
        }
        else {
            list.erase(list.begin());
        }
    }
    return r;
}

}
