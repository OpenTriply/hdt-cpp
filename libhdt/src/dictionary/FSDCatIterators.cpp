#include "FSDCatIterators.hpp"

namespace hdt {

CatCommon::CatCommon(IteratorUCharString* it1, IteratorUCharString* it2)
        : it1(it1)
        , it2(it2)
        , has_next(false)
        , counted(false)
        , count1(0)
        , count2(0)
        , commonNum(0)
{
    if (it1->hasNext()) {
        list.push_back(make_pair((size_t)1, it1->next()));
    }
    if (it2->hasNext()) {
        list.push_back(make_pair((size_t)2, it2->next()));
    }

    helpNext();
}

CatCommon::~CatCommon()
{
    delete it1;
    delete it2;
};

bool CatCommon::hasNext()
{
    return has_next;
}

pair<size_t, size_t> CatCommon::next()
{
    pair<size_t, size_t> r = make_pair(next_t.first, next_t.second);
    has_next = false;
    helpNext();
    ++commonNum;
    return r;
}

pair<unsigned char*, pair<size_t, size_t> > CatCommon::nextCommonString()
{
    pair<unsigned char*, pair<size_t, size_t> > r = make_pair(this->next_string, make_pair((size_t)next_t.first, (size_t)next_t.second));
    has_next = false;
    helpNext();
    ++commonNum;
    return r;
}

void CatCommon::helpNext()
{
    while (list.size() != 0) {
        if (list.size() == 2) {
            string s1 = string(reinterpret_cast<char*>(list[0].second));
            string s2 = string(reinterpret_cast<char*>(list[1].second));
            // Sort
            if (s1.compare(s2) > 0) {
                iter_swap(list.begin(), list.begin() + 1);
            }
            // If pair has common terms:
            if (!s1.compare(s2)) {
                has_next = true;
                next_t = make_pair(count1, count2);
                next_string = list[0].second;
                bool remove = false;
                if (it1->hasNext()) {
                    list[0] = make_pair(1, it1->next());
                    count1++;
                }
                else {
                    list.erase(list.begin());
                    remove = true;
                }

                if (it2->hasNext()) {
                    count2++;
                    remove ? (list[0] = make_pair(2, it2->next())) : (list[1] = make_pair(2, it2->next()));
                }
                else {
                    list.erase(list.begin());
                }
                break;
            }
            else {
                if (list[0].first == 1) {
                    if (it1->hasNext()) {
                        list[0] = make_pair(1, it1->next());
                        count1++;
                    }
                    else {
                        list.erase(list.begin());
                    }
                }
                else {
                    if (it2->hasNext()) {
                        count2++;
                        list[0] = make_pair(2, it2->next());
                    }
                    else {
                        list.erase(list.begin());
                    }
                }
            }
        }
        else if (list.size() == 1) {
            list.erase(list.begin());
        }
    }
}

size_t CatCommon::getCommonNum()
{
    if (!counted) {
        while (this->hasNext()) {
            this->next();
        }
        counted = true;
    }
    return commonNum;
}

CatIterator::CatIterator()
{
    it1 = nullptr;
    it2 = nullptr;
}

CatIterator::CatIterator(CatCommon* it1, CatCommon* it2)
        : it1(it1)
        , it2(it2)
        , counted(false)
        , count(0)
{
    if (it1->hasNext()) {
        list.push_back(make_pair((size_t)1, it1->next()));
    }
    if (it2->hasNext()) {
        list.push_back(make_pair((size_t)2, it2->next()));
    }
}

CatIterator::~CatIterator()
{
    if (it1)
        delete it1;
    if (it2)
        delete it2;
}

bool CatIterator::hasNext()
{
    return list.size() > 0;
}

size_t CatIterator::next()
{
    size_t r;
    //sort
    if (list.size() > 1 && list[1].second.first < list[0].second.first) {
        iter_swap(list.begin(), list.begin() + 1);
    }

    r = list[0].second.first;
    if (list[0].first == 1) {
        if (it1->hasNext()) {
            list[0] = make_pair((size_t)1, it1->next());
        }
        else {
            list.erase(list.begin());
        }
    }
    else {
        if (it2->hasNext()) {
            list[0] = make_pair((size_t)2, it2->next());
        }
        else {
            list.erase(list.begin());
        }
    }
    ++count;
    return r;
}

size_t CatIterator::getCommonNum()
{
    if (!counted) {
        while (this->hasNext()) {
            this->next();
        }
        counted = true;
    }
    return count;
}

NotSharedMergeIterator::NotSharedMergeIterator(IteratorUCharString* iterator1, IteratorUCharString* iterator2, CatIterator* it1common, CatIterator* it2common)
        : it1(iterator1)
        , it2(iterator2)
        , it1common(it1common)
        , it2common(it2common)
        , pos(0)
        , count1(0)
        , count2(0)
        , skipSection1(0)
        , skipSection2(0)
        , canSkip1(false)
        , canSkip2(false)
{
    string1 = NULL;
    string2 = NULL;
    if (it1->hasNext()) {
        string1 = it1->next();
    }
    if (it2->hasNext()) {
        string2 = it2->next();
    }
    if (it1common->hasNext()) {
        canSkip1 = true;
        skipSection1 = it1common->next();
    }
    if (it2common->hasNext()) {
        canSkip2 = true;
        skipSection2 = it2common->next();
    }

    prevString = 0;
}

NotSharedMergeIterator::~NotSharedMergeIterator()
{
    delete it1;
    delete it2;
    delete it1common;
    delete it2common;
    if(string1 != NULL) delete string1;
    if(string2 != NULL) delete string2;
}

bool NotSharedMergeIterator::hasNext()
{
    skip();
    if (prevString == 1) {
        return (string2 || it1->hasNext());
    }
    else if (prevString == 2) {
        return (string1 || it2->hasNext());
    }
    else if (prevString == -1) {
        return (it1->hasNext() || it2->hasNext());
    }
    else
        return (string1 || string2);
}

unsigned char* NotSharedMergeIterator::next()
{
    skip();
    unsigned char* retString;
    // load strings
    if (prevString == 1) {
        string1 = NULL;
        if (it1->hasNext()) {
            string1 = it1->next();
        }
    }
    else if (prevString == 2) {
        string2 = NULL;
        if (it2->hasNext()) {
            string2 = it2->next();
        }
    }
    else if (prevString == -1) {
        string1 = NULL;
        if (it1->hasNext()) {
            string1 = it1->next();
        }
        string2 = NULL;
        if (it2->hasNext()) {
            string2 = it2->next();
        }
    }

    if (string1 && string2) {
        int cmp = strcmp(reinterpret_cast<const char*>(string1), reinterpret_cast<const char*>(string2));
        if (cmp < 0) {
            retString = string1;
            prevString = 1;
            mapping1.push_back(make_pair(count1, pos+1));
            count1++;
        }
        else if (cmp > 0) {
            retString = string2;
            prevString = 2;
            mapping2.push_back(make_pair(count2, pos+1));
            count2++;
        }
        else {
            retString = string1;
            prevString = -1;
            mapping1.push_back(make_pair(count1, pos+1));
            mapping2.push_back(make_pair(count2, pos+1));
            count1++;
            count2++;
        }
    }
    else {
        if (string1) {
            prevString = 1;
            retString = string1;
            mapping1.push_back(make_pair(count1, pos+1));
            count1++;
        }
        else {
            prevString = 2;
            retString = string2;
            mapping2.push_back(make_pair(count2, pos+1));
            count2++;
        }
    }

    ++pos;
    return retString;
}

void NotSharedMergeIterator::skip()
{
    if (canSkip1) {
        while (skipSection1 == count1) {
            if (it1->hasNext()) {
                string1 = it1->next();
            }
            else {
                string1 = NULL;
            }
            count1++;
            if (it1common->hasNext()) {
                skipSection1 = it1common->next();
            }
        }
    }
    if (canSkip2) {
        while (skipSection2 == count2) {
            if (it2->hasNext()) {
                string2 = it2->next();
            }
            else {
                string2 = NULL;
            }
            count2++;
            if (it2common->hasNext()) {
                skipSection2 = it2common->next();
            }
        }
    }
}

size_t NotSharedMergeIterator::getNumberOfElements()
{
    return num;
}

void NotSharedMergeIterator::freeStr(unsigned char* ptr)
{
    delete [] ptr;
}

vector<pair<size_t, size_t> > NotSharedMergeIterator::getMapping1()
{
    return mapping1;
}

vector<pair<size_t, size_t> > NotSharedMergeIterator::getMapping2()
{
    return mapping2;
}

SharedMergeIterator::SharedMergeIterator(Dictionary* dict1, Dictionary* dict2)
        : num(0)
        , pos(0)
        , count1(0)
        , count2(0)
        , canAdd1(false)
        , canAdd2(false)
{
    it1 = dict1->getShared();
    it2 = dict2->getShared();

    itS1O2 = new CatCommon(dict1->getSubjects(), dict2->getObjects());
    itS2O1 = new CatCommon(dict2->getSubjects(), dict1->getObjects());

    itCommonShared1Subjects2 = new CatCommon(dict1->getShared(), dict2->getSubjects());
    itCommonShared1Objects2 = new CatCommon(dict1->getShared(), dict2->getObjects());
    itCommonShared2Subjects1 = new CatCommon(dict2->getShared(), dict1->getSubjects());
    itCommonShared2Objects1 = new CatCommon(dict2->getShared(), dict1->getObjects());

    commonShared1Subjects2 = make_pair(-1, -1);
    commonShared1Objects2 = make_pair(-1, -1);
    commonShared2Subjects1 = make_pair(-1, -1);
    commonShared2Objects1 = make_pair(-1, -1);

    if (itCommonShared1Subjects2->hasNext()) {
        commonShared1Subjects2 = itCommonShared1Subjects2->next();
    }

    if (itCommonShared1Objects2->hasNext()) {
        commonShared1Objects2 = itCommonShared1Objects2->next();
    }

    if (itCommonShared2Subjects1->hasNext()) {
        commonShared2Subjects1 = itCommonShared2Subjects1->next();
    }

    if (itCommonShared2Objects1->hasNext()) {
        commonShared2Objects1 = itCommonShared2Objects1->next();
    }

    string1 = NULL;
    string2 = NULL;
    if (it1->hasNext()) {
        string1 = it1->next();
    }
    if (it2->hasNext()) {
        string2 = it2->next();
    }
    if (itS1O2->hasNext()) {
        canAdd1 = true;
        add1 = itS1O2->nextCommonString();
    }
    if (itS2O1->hasNext()) {
        canAdd2 = true;
        add2 = itS2O1->nextCommonString();
    }

    prevString = 0;
}

SharedMergeIterator::~SharedMergeIterator()
{
    delete it1;
    delete it2;
    delete itS1O2;
    delete itS2O1;
    delete itCommonShared1Subjects2;
    delete itCommonShared1Objects2;
    delete itCommonShared2Subjects1;
    delete itCommonShared2Objects1;
}

bool SharedMergeIterator::hasNext()
{
    if (canAdd1 || canAdd2)
        return true;
    if (prevString == 1) {
        return (string2 || it1->hasNext());
    }
    else if (prevString == 2) {
        return (string1 || it2->hasNext());
    }
    else if (prevString == -1) {
        return (it1->hasNext() || it2->hasNext());
    }
    else
        return (string1 || string2);
}

unsigned char* SharedMergeIterator::next()
{
    // load strings
    if (prevString == 1) {
        string1 = NULL;
        if (it1->hasNext()) {
            string1 = it1->next();
        }
    }
    else if (prevString == 2) {
        string2 = NULL;
        if (it2->hasNext()) {
            string2 = it2->next();
        }
    }
    else if (prevString == -1) {
        string1 = NULL;
        if (it1->hasNext()) {
            string1 = it1->next();
        }
        string2 = NULL;
        if (it2->hasNext()) {
            string2 = it2->next();
        }
    }
    prevString = 0;
    unsigned char* retString = findNextFromCommon();
    if (retString != NULL) {
        pos++;
        return retString;
    }

    if (string1 && string2) {
        int cmp = strcmp(reinterpret_cast<const char*>(string1), reinterpret_cast<const char*>(string2));
        if (cmp < 0) {
            retString = string1;
            prevString = 1;
            mapping1.push_back(make_pair(count1, pos+1));
            if (count1 == commonShared1Subjects2.first) {
                this->mappingS2Sh.push_back(make_pair(commonShared1Subjects2.second, pos+1));
                if (itCommonShared1Subjects2->hasNext()) {
                    commonShared1Subjects2 = itCommonShared1Subjects2->next();
                }
            }
            if (count1 == commonShared1Objects2.first) {
                this->mappingO2Sh.push_back(make_pair(commonShared1Objects2.second, pos+1));
                if (itCommonShared1Objects2->hasNext()) {
                    commonShared1Objects2 = itCommonShared1Objects2->next();
                }
            }
            count1++;
        }
        else if (cmp > 0) {
            retString = string2;
            prevString = 2;
            mapping2.push_back(make_pair(count2, pos+1));
            if (count2 == commonShared2Subjects1.first) {
                this->mappingS1Sh.push_back(make_pair(commonShared2Subjects1.second, pos+1));
                if (itCommonShared2Subjects1->hasNext()) {
                    commonShared2Subjects1 = itCommonShared2Subjects1->next();
                }
            }
            if (count2 == commonShared2Objects1.first) {
                this->mappingO1Sh.push_back(make_pair(commonShared2Objects1.second, pos+1));
                if (itCommonShared2Objects1->hasNext()) {
                    commonShared2Objects1 = itCommonShared2Objects1->next();
                }
            }
            count2++;
        }
        else {
            retString = string1;
            prevString = -1;
            mapping1.push_back(make_pair(count1, pos+1));
            if (count1 == commonShared1Subjects2.first) {
                this->mappingS2Sh.push_back(make_pair(commonShared1Subjects2.second, pos+1));
                if (itCommonShared1Subjects2->hasNext()) {
                    commonShared1Subjects2 = itCommonShared1Subjects2->next();
                }
            }
            if (count1 == commonShared1Objects2.first) {
                this->mappingO2Sh.push_back(make_pair(commonShared1Objects2.second, pos+1));
                if (itCommonShared1Objects2->hasNext()) {
                    commonShared1Objects2 = itCommonShared1Objects2->next();
                }
            }
            mapping2.push_back(make_pair(count2, pos+1));
            if (count2 == commonShared2Subjects1.first) {
                this->mappingS1Sh.push_back(make_pair(commonShared2Subjects1.second, pos+1));
                if (itCommonShared2Subjects1->hasNext()) {
                    commonShared2Subjects1 = itCommonShared2Subjects1->next();
                }
            }
            if (count2 == commonShared2Objects1.first) {
                this->mappingO1Sh.push_back(make_pair(commonShared2Objects1.second, pos+1));
                if (itCommonShared2Objects1->hasNext()) {
                    commonShared2Objects1 = itCommonShared2Objects1->next();
                }
            }
            count1++;
            count2++;
        }
    }
    else {
        if (string1) {
            prevString = 1;
            retString = string1;
            mapping1.push_back(make_pair(count1, pos+1));
            if (count1 == commonShared1Subjects2.first) {
                this->mappingS2Sh.push_back(make_pair(commonShared1Subjects2.second, pos+1));
                if (itCommonShared1Subjects2->hasNext()) {
                    commonShared1Subjects2 = itCommonShared1Subjects2->next();
                }
            }
            if (count1 == commonShared1Objects2.first) {
                this->mappingO2Sh.push_back(make_pair(commonShared1Objects2.second, pos+1));
                if (itCommonShared1Objects2->hasNext()) {
                    commonShared1Objects2 = itCommonShared1Objects2->next();
                }
            }
            count1++;
        }
        else {
            prevString = 2;
            retString = string2;
            mapping2.push_back(make_pair(count2, pos+1));
            if (count2 == commonShared2Subjects1.first) {
                this->mappingS1Sh.push_back(make_pair(commonShared2Subjects1.second, pos+1));
                if (itCommonShared2Subjects1->hasNext()) {
                    commonShared2Subjects1 = itCommonShared2Subjects1->next();
                }
            }
            if (count2 == commonShared2Objects1.first) {
                this->mappingO1Sh.push_back(make_pair(commonShared2Objects1.second, pos+1));
                if (itCommonShared2Objects1->hasNext()) {
                    commonShared2Objects1 = itCommonShared2Objects1->next();
                }
            }
            count2++;
        }
    }

    ++pos;
    return retString;
}

unsigned char* SharedMergeIterator::findNextFromCommon()
{
    unsigned char* ret1 = NULL;
    unsigned char* ret2 = NULL;

    if (canAdd1) {
        unsigned char* addString1 = add1.first;
        if (string1) {
            int cmp1 = strcmp(reinterpret_cast<const char*>(addString1), reinterpret_cast<const char*>(string1));
            if (cmp1 < 0) {
                ret1 = addString1;
            }
        }
        else {
            ret1 = addString1;
        }
        if (ret1 && string2) {
            int cmp2 = strcmp(reinterpret_cast<const char*>(ret1), reinterpret_cast<const char*>(string2));
            if (cmp2 >= 0) {
                ret1 = NULL;
            }
        }
    }
    if (canAdd2) {
        unsigned char* addString2 = add2.first;
        if (string1) {
            int cmp1 = strcmp(reinterpret_cast<const char*>(addString2), reinterpret_cast<const char*>(string1));
            if (cmp1 < 0) {
                ret2 = addString2;
            }
        }
        else {
            ret2 = addString2;
        }
        if (ret2 && string2) {
            int cmp2 = strcmp(reinterpret_cast<const char*>(ret2), reinterpret_cast<const char*>(string2));
            if (cmp2 >= 0) {
                ret2 = NULL;
            }
        }
    }

    if (ret1 && ret2) {
        int cmp = strcmp(reinterpret_cast<const char*>(ret1), reinterpret_cast<const char*>(ret2));
        if (!cmp) {
            this->mappingS1Sh.push_back(make_pair(add1.second.first, pos+1));
            this->mappingO2Sh.push_back(make_pair(add1.second.second, pos+1));
            this->mappingS2Sh.push_back(make_pair(add2.second.first, pos+1));
            this->mappingO1Sh.push_back(make_pair(add2.second.second, pos+1));
            if (itS1O2->hasNext()) {
                this->add1 = itS1O2->nextCommonString();
            }
            else {
                this->canAdd1 = false;
            }

            if (itS2O1->hasNext()) {
                this->add2 = itS2O1->nextCommonString();
            }
            else {
                this->canAdd2 = false;
            }
            return ret1;
        }
        else if (cmp < 1) {
            this->mappingS1Sh.push_back(make_pair(add1.second.first, pos+1));
            this->mappingO2Sh.push_back(make_pair(add1.second.second, pos+1));
            if (itS1O2->hasNext()) {
                this->add1 = itS1O2->nextCommonString();
            }
            else {
                this->canAdd1 = false;
            }
            return ret1;
        }
        else {
            this->mappingS2Sh.push_back(make_pair(add2.second.first, pos+1));
            this->mappingO1Sh.push_back(make_pair(add2.second.second, pos+1));
            if (itS2O1->hasNext()) {
                this->add2 = itS2O1->nextCommonString();
            }
            else {
                this->canAdd2 = false;
            }
            return ret2;
        }
    }
    else if (ret1) {
        this->mappingS1Sh.push_back(make_pair(add1.second.first, pos+1));
        this->mappingO2Sh.push_back(make_pair(add1.second.second, pos+1));
        if (itS1O2->hasNext()) {
            this->add1 = itS1O2->nextCommonString();
        }
        else {
            this->canAdd1 = false;
        }
        return ret1;
    }
    else if (ret2) {
        this->mappingS2Sh.push_back(make_pair(add2.second.first, pos+1));
        this->mappingO1Sh.push_back(make_pair(add2.second.second, pos+1));
        if (itS2O1->hasNext()) {
            this->add2 = itS2O1->nextCommonString();
        }
        else {
            this->canAdd2 = false;
        }
        return ret2;
    }
    else {
        return nullptr;
    }
}

size_t SharedMergeIterator::getNumberOfElements()
{
    return num;
}

void SharedMergeIterator::freeStr(unsigned char* ptr)
{
    delete [] ptr;
}

vector<pair<size_t, size_t> > SharedMergeIterator::getMapping1()
{
    return mapping1;
}

vector<pair<size_t, size_t> > SharedMergeIterator::getMapping2()
{
    return mapping2;
}

vector<pair<size_t, size_t> > SharedMergeIterator::getMappingS1Sh()
{
    return mappingS1Sh;
}

vector<pair<size_t, size_t> > SharedMergeIterator::getMappingS2Sh()
{
    return mappingS2Sh;
}

vector<pair<size_t, size_t> > SharedMergeIterator::getMappingO1Sh()
{
    return mappingO1Sh;
}

vector<pair<size_t, size_t> > SharedMergeIterator::getMappingO2Sh()
{
    return mappingO2Sh;
}
}
