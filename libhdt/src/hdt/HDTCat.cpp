#include "HDTCat.hpp"

namespace hdt {

HDTCat::HDTCat(HDTSpecification &spec, HDT *hdt1, HDT *hdt2, ProgressListener *listener) : BasicHDT(spec)
{
    dictionaryCat = new FourSectionDictionaryCat(spec);
    dictionaryCat->cat(hdt1->getDictionary(), hdt2->getDictionary());
    this->dictionary = dictionaryCat;
    triplesCat = new BitmapTriplesCat();
    triplesCat->cat(hdt1->getTriples(), hdt2->getTriples(), dictionaryCat, listener);
    //this->triples = triplesCat;
}

}
