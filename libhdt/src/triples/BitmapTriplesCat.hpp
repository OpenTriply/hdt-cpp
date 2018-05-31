#ifndef BITMAPTRIPLESCAT_HPP_
#define BITMAPTRIPLESCAT_HPP_

#include <Triples.hpp>
#include <HDTSpecification.hpp>
#include <Iterator.hpp>
#include <set>

#include "BitmapTriples.hpp"
#include "../dictionary/FourSectionDictionaryCat.hpp"

using namespace std;

namespace hdt {

class BitmapTriplesCat : public BitmapTriples {
private:
public:
    BitmapTriplesCat();
    ~BitmapTriplesCat();

    void cat(Triples* hdt1, Triples* hdt2, FourSectionDictionaryCat* dict, ProgressListener* listener);
};

class BitmapTriplesIteratorCat : public IteratorTripleID {
private:
    Triples *hdt1, *hdt2;
    FourSectionDictionaryCat* dictionaryCat;
    vector<TripleID> arrayOfTriples;
    size_t count;

public:
    BitmapTriplesIteratorCat(Triples* hdt1, Triples* hdt2, FourSectionDictionaryCat* dictCat);
    ~BitmapTriplesIteratorCat();

    //    bool hasNext();
    //    TripleID *next();
    //    bool hasPrevious();
    //    TripleID *previous();
    //    void goToStart();
    //    size_t estimatedNumResults();
    //    ResultEstimationType numResultEstimation();
    //    TripleComponentOrder getOrder();
    //    bool canGoTo();
    //    void goTo(unsigned int pos);
    //    void skip(unsigned int pos);
    //    bool findNextOccurrence(unsigned int value, unsigned char component);
    //    bool isSorted(TripleComponentRole role);
    //
    //    vector<TripleID> getTripleID(size_t count);
    TripleID mapTriple(TripleID* tripleID, size_t num);
    size_t mapIdSection(size_t id, CatMapping* catMappingShared, CatMapping* catMapping);
    size_t mapIdPredicate(size_t id, CatMapping* catMappingShared, CatMapping* catMapping);
};
}

#endif //BITMAPTRIPLESCAT_HPP_
