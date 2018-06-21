#ifndef HDTCAT_HPP_
#define HDTCAT_HPP_

#include "BasicHDT.hpp"
#include "../dictionary/FourSectionDictionaryCat.hpp"
#include "../triples/BitmapTriplesCat.hpp"

namespace hdt {

/**
 * @brief The HDTCat class represents the output HDT file of HDTCat.
 */
class HDTCat : public BasicHDT {
private:
    FourSectionDictionaryCat *dictionaryCat;
    BitmapTriplesCat *triplesCat;
public:
    HDTCat(HDTSpecification &spec, string baseUri, HDT *hdt1, HDT *hdt2, ProgressListener *listener);
    ~HDTCat();
    void fillHeaderCat(const string& baseUri);
};

}

#endif //HDTCAT_HPP_
