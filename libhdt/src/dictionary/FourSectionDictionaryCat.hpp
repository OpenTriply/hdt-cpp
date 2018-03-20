#ifndef FOURSECTIONDICTIONARYCAT_HPP_
#define FOURSECTIONDICTIONARYCAT_HPP_

#include <HDTSpecification.hpp>
#include <Dictionary.hpp>
#include "FourSectionDictionary.hpp"
#include "../util/catmapping.hpp"
#include "../util/catmappingBack.hpp"
#include "../util/catcommon.hpp"
#include "../util/catiterator.hpp"

namespace hdt {

class FourSectionDictionaryCat : public FourSectionDictionary {
private:
    CatMapping mappingSh1;
    CatMapping mappingSh2;
    CatMapping mappingS1;
    CatMapping mappingS2;
    CatMapping mappingO1;
    CatMapping mappingO2;
    CatMapping mappingP1;
    CatMapping mappingP2;
    CatMappingBack mappingS;

public:
    FourSectionDictionaryCat(HDTSpecification &spec);
    ~FourSectionDictionaryCat();
    void cat(Dictionary *dict1, Dictionary *dict2);
    csd::CSD *catSection(size_t numentries, DictionarySection type, csd::CSD *dict1_Section, csd::CSD *dict2_Section,
                         CatIterator *it1, CatIterator *it2, CatMapping *mappingHdt1, CatMapping *mappingHdt2);

};

}

#endif // FOURSECTIONDICTIONARYCAT_H_
