#ifndef FSDCATITERATORS_HPP_
#define FSDCATITERATORS_HPP_

#include <Iterator.hpp>
#include <Dictionary.hpp>

namespace hdt {
/**
 * @brief A class iterator of common terms found in two iterators.
 */
class CatCommon {
private:
    vector<pair<size_t, unsigned char*> > list; /// Vector of (two) pairs to be compared

    IteratorUCharString* it1;
    IteratorUCharString* it2;
    bool has_next, counted;
    size_t count1, count2, commonNum;
    pair<size_t, size_t> next_t;
    unsigned char* next_string, *prev1, *prev2;
    pair<unsigned char*, pair<size_t, size_t> > next_s;

public:
    CatCommon();
    CatCommon(IteratorUCharString* it1, IteratorUCharString* it2);

    ~CatCommon();

    bool hasNext();

    /**
     * @brief Returns the next common pair of the iterator indices.
     *
     * @return A pair of the indices of common terms;
     * one from iterator 1 and one from iterator 2.
     */
    pair<size_t, size_t> next();

    /**
     * @brief Returns the next common string.
     */
    pair<unsigned char*, pair<size_t, size_t> > nextCommonString();

    /**
     * @brief Auxiliary function that sets the next common pairs in list.
     */
    void helpNext();

    size_t getCommonNum();
};

/**
 * @brief A class iterator over two CatCommon iterators.
 */
class CatIterator {
private:
    vector<pair<size_t, pair<size_t, size_t>>> list;
    CatCommon *it1, *it2;
    bool counted;
    size_t count;

public:
    CatIterator();
    CatIterator(CatCommon* it1, CatCommon* it2);
    ~CatIterator();
    bool hasNext();
    /**
     * @return Next index of a term that appears in common in two sections.
     */
    size_t next();
    size_t getCommonNum();
};

/**
 * @brief A class iterator over the merge of two section iterators (excluding shared).
 */
class NotSharedMergeIterator : public IteratorUCharString {
private:
    IteratorUCharString *it1, *it2;
    CatIterator *it1common, *it2common;
    unsigned char *string1, *string2;
    int prevString;
    size_t num;
    vector<pair<size_t, size_t>> mapping1, mapping2;
    size_t pos;
    size_t count1, count2;
    size_t skipSection1, skipSection2;
    bool canSkip1, canSkip2;

public:
    NotSharedMergeIterator(IteratorUCharString* iterator1, IteratorUCharString* iterator2, CatIterator* it1common, CatIterator* it2common);
    NotSharedMergeIterator(CatCommon* it1, CatCommon* it2);
    ~NotSharedMergeIterator();

    bool hasNext();
    unsigned char* next();
    size_t getNumberOfElements();
    void freeStr(unsigned char*);
    /**
     * @brief Skips all invalid terms.
     */
    void skip();
    /**
     * @return a mapping between input file 1 section and output file section
     */
    vector<pair<size_t, size_t> > getMapping1();
    /**
     * @return a mapping between input file 2 section and output file section
     */
    vector<pair<size_t, size_t> > getMapping2();
};


/**
 * @brief A class iterator over the merge of two shared section iterators.
 */
class SharedMergeIterator : public IteratorUCharString {
private:
    IteratorUCharString *it1, *it2;
    CatCommon *itS1O2, *itS2O1;
    CatCommon* itCommonShared1Subjects2;
    CatCommon* itCommonShared1Objects2;
    CatCommon* itCommonShared2Subjects1;
    CatCommon* itCommonShared2Objects1;
    pair<size_t, size_t> commonShared1Subjects2;
    pair<size_t, size_t> commonShared1Objects2;
    pair<size_t, size_t> commonShared2Subjects1;
    pair<size_t, size_t> commonShared2Objects1;
    unsigned char *string1, *string2;
    int prevString;
    size_t num;
    vector<pair<size_t, size_t>> mapping1, mapping2;
    vector<pair<size_t, size_t>> mappingS1Sh, mappingS2Sh;
    vector<pair<size_t, size_t>> mappingO1Sh, mappingO2Sh;
    size_t pos;
    size_t count1, count2;
    pair<unsigned char *, pair<size_t, size_t> > add1, add2;
    bool canAdd1, canAdd2;

public:
    SharedMergeIterator(Dictionary* dict1, Dictionary* dict2);
    ~SharedMergeIterator();

    bool hasNext();
    unsigned char* next();
    size_t getNumberOfElements();
    void freeStr(unsigned char*);
    /**
     * Moves the iterator to the indices of that the two section iterators have a common term.
     * @return The next common string.
     */
    unsigned char* findNextFromCommon();
    /**
     * @return a mapping between input file 1 shared section and output file shared section
     */
    vector<pair<size_t, size_t> > getMapping1();
    /**
     * @return a mapping between input file 2 shared section and output file shared section
     */
    vector<pair<size_t, size_t> > getMapping2();
    /**
     * @return a mapping between input file 1 subject section and output file shared section
     */
    vector<pair<size_t, size_t> > getMappingS1Sh();
    /**
     * @return a mapping between input file 2 subject section and output file shared section
     */
    vector<pair<size_t, size_t> > getMappingS2Sh();
    /**
     * @return a mapping between input file 1 object section and output file shared section
     */
    vector<pair<size_t, size_t> > getMappingO1Sh();
    /**
     * @return a mapping between input file 2 object section and output file shared section
     */
    vector<pair<size_t, size_t> > getMappingO2Sh();
};
}
#endif //FSDCATITERATORS_HPP_
