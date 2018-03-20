#include "FourSectionDictionaryCat.hpp"
#include "../libdcs/CSD_PFC.h"

namespace hdt {

FourSectionDictionaryCat::FourSectionDictionaryCat(HDTSpecification & spec) { }

FourSectionDictionaryCat::~FourSectionDictionaryCat() { }

void FourSectionDictionaryCat::cat(Dictionary *dict1, Dictionary *dict2) {

    // 1. Merge predicate dictionary
    cout << "Predicates" << endl;
    this->mappingP1.init((size_t) dict1->getNpredicates());
    this->mappingP2.init((size_t) dict2->getNpredicates());
    //TODO (remove):  CatCommon c(dict1->getPredicates(), dict2->getPredicates());

    size_t numCommonPredicates = 0;

    CatCommon *commonP1P2 = new CatCommon(dict1->getPredicates(), dict2->getPredicates());
    while(commonP1P2->hasNext()) {
        commonP1P2->next();
        ++numCommonPredicates;
    }

    size_t num = dict1->getNpredicates() + dict2->getNpredicates() - numCommonPredicates;

    // TODO : write equivalent
    //csd::CSD dictionarySectionCatPredicates = catSection(num, 4, dict1->getPredicates(), dict2->getPredicates(), )
//    PFCDictionarySectionBig dictionarySectionCatPredicates = catSection(num, 4,dictionary1.getPredicates(),dictionary2.getPredicates(), Collections.<Integer>emptyList().iterator(), Collections.<Integer>emptyList().iterator(), mappingP1, mappingP2);
//    predicates = dictionarySectionCatPredicates;
}

csd::CSD *FourSectionDictionaryCat::catSection(size_t numentries, DictionarySection type, csd::CSD *dict1_Section,
                                               csd::CSD *dict2_Section, CatCommon *it1common, CatCommon *it2common,
                                               CatMapping *mappingHdt1, CatMapping *mappingHdt2) {

    csd::CSD *section = new csd::CSD_PFC();

    if (numentries > 0) {


        size_t count1 = 0;
        size_t count2 = 0;
        size_t skipSection1 = -1;
        size_t skipSection2 = -1;
        size_t buffer = 0;

        try {
            vector<pair<size_t, unsigned char *>> list;

            if(it1common->hasNext()) {
                //skipSection1 = it1common->next();
            }
            if(it2common->hasNext()) {
                //skipSection2 = it2common->next();
            }


        } catch(std::exception& e) {
            cerr << e.what() << endl;
        }


    }
    return section;
}

}
