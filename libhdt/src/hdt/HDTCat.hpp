#ifndef HDTCAT_HPP_
#define HDTCAT_HPP_

#include "BasicHDT.hpp"
#include "../dictionary/FourSectionDictionaryCat.hpp"

namespace hdt {

/**
 * @brief The HDTCat class represents the output HDT file of HDTCat.
 */
class HDTCat : public BasicHDT {
private:
    FourSectionDictionaryCat *dictionaryCat;
public:
    HDTCat(HDTSpecification &spec, HDT *hdt1, HDT *hdt2, ProgressListener *listener);
};

}

#endif //HDTCAT_HPP_
