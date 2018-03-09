#include "catcommon.hpp"
#include <algorithm>

using namespace std;

namespace hdt {

CatCommon::CatCommon(IteratorUCharString *it1, IteratorUCharString *it2) : it1(it1), it2(it2) {
    if(it1->hasNext()) {
        list.push_back(make_pair((size_t) 1, it1->next()));
    }
    if(it2->hasNext()) {
        list.push_back(make_pair((size_t) 2, it2->next()));
    }

    count1 = count2 = 0;
    hasNext = false;

    helpNext();
}

CatCommon::~CatCommon() {}

bool pairComparator(const pair<size_t, unsigned char*>& p1, const pair<size_t, unsigned char*>& p2) {

    string s1 = string(reinterpret_cast<char*>(p1.second));
    string s2 = string(reinterpret_cast<char*>(p2.second));
    return s1.compare(s2) < 0;
}

void CatCommon::helpNext() {

    while(list.size() != 0) {

        sort(list.begin(), list.end(), pairComparator);

        if(list.size() == 2) {
            string s1 = string(reinterpret_cast<char*>(list[0].second));
            string s2 = string(reinterpret_cast<char*>(list[1].second));

            if(s1.compare(s2) != 0) {
                hasNext = true;
                next = make_pair(count1, count2);
                bool remove = false;

                if(it1->hasNext()) {
                    list[0] = make_pair(1, it1->next());
                    count1++;
                }
                else {
                    list.erase(list.begin());
                    remove = true;
                }

                if(it2->hasNext()) {
                    count2++;
                    remove ? (list[0] = make_pair(2, it2->next())) : (list[1] = make_pair(2, it2->next()));
                }
                else {
                    list.erase(list.begin());
                }

                break;
            }
            else {
                if(list[0].first == 1) {
                    if(it1->hasNext()) {
                        count2++;
                        list[0] = make_pair(2, it2->next());
                    }
                    else {
                        list.erase(list.begin());
                    }
                }
            }
        }
        else if(list.size() == 1) {
            list.erase(list.begin());
        }
    }
}

}
