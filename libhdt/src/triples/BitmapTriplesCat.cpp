#include "BitmapTriplesCat.hpp"

namespace hdt {

BitmapTriplesCat::BitmapTriplesCat()
        : BitmapTriples()
{
}

BitmapTriplesCat::~BitmapTriplesCat()
{
}

void BitmapTriplesCat::cat(Triples* hdt1, Triples* hdt2, FourSectionDictionaryCat* dict, ProgressListener* listener)
{
    BitmapTriplesIteratorCat* joinIterator = new BitmapTriplesIteratorCat(hdt1, hdt2, dict);
    //this->load()
    //this->load(joinIterator, listener);
}

BitmapTriplesIteratorCat::BitmapTriplesIteratorCat(Triples* hdt1, Triples* hdt2, FourSectionDictionaryCat* dictCat)
        : count(1)
{
    this->dictionaryCat = dictCat;
    this->hdt1 = hdt1;
    this->hdt2 = hdt2;
    //arrayOfTriples = getTripleID(1);
    count++;
}

BitmapTriplesIteratorCat::~BitmapTriplesIteratorCat() {}

//bool BitmapTriplesIteratorCat::hasNext() {
//return false;
////    if(count < dictionaryCat->getMappingS()->getSize()) {
////        return true;
////    }
////    else {
////        if(list->hasNext()) {
////            return true;
////        }
////        else {
////            return false;
////        }
////    }
//}
//
//TripleID *BitmapTriplesIteratorCat::next() {
//
//}
//
//bool BitmapTriplesIteratorCat::hasPrevious() {
//    return false;
//}
//
//TripleID *BitmapTriplesIteratorCat::previous(){
//    return nullptr;
//}
//
//void BitmapTriplesIteratorCat::goToStart() {
//}
//
//size_t BitmapTriplesIteratorCat::estimatedNumResults() {
//
//}
//
//ResultEstimationType BitmapTriplesIteratorCat::numResultEstimation() {
//    return UNKNOWN;
//}
//
//TripleComponentOrder BitmapTriplesIteratorCat::getOrder() {
//    return Unknown;
//}
//
//bool BitmapTriplesIteratorCat::canGoTo() {
//    return false;
//}
//
//void BitmapTriplesIteratorCat::goTo(unsigned int pos) {
//}
//
//void BitmapTriplesIteratorCat::skip(unsigned int pos) {
//}
//
//bool BitmapTriplesIteratorCat::findNextOccurrence(unsigned int value, unsigned char component) {
//    return false;
//}
//
//bool BitmapTriplesIteratorCat::isSorted(TripleComponentRole role) {
//    return false;
//}
//
//vector<TripleID> BitmapTriplesIteratorCat::getTripleID(size_t count) {
//    set<TripleID> tripleSet;
//    vector<size_t> mapping = dictionaryCat->getMappingS()->getMapping(count);
//    vector<CatMappingBackType> mappingType = dictionaryCat->getMappingS()->getType(count);
//
//    IteratorTripleID *it = nullptr;
//    for(size_t i=0; i<mapping.size(); i++){
//        if(mappingType[i] == 1) {
//            //it = hdt1->search(new TripleID(mapping[i], 0, 0));
//            while(it->hasNext()) {
//                //set.add();
//                //TODO: add to set
//            }
//        }
//        if(mapping[i] == 2) {
//            //it = hdt2->search(new TripleID(mapping[i], 0, 0));
//            while(it->hasNext()) {
//                //TODO: add to set
//            }
//        }
//        // Free memory if needed.
//        if(it != nullptr) {
//            delete it;
//            it = nullptr;
//        }
//    }
//    //TODO : vector<Triple ID> triples =
//    //TODO: sort vector
//    //TODO: return triples
//    //return NULL;
//
//
//}

TripleID BitmapTriplesIteratorCat::mapTriple(TripleID* tripleID, size_t num)
{
    if (num == 1) {
        size_t new_subject1 = mapIdSection(tripleID->getSubject(), dictionaryCat->getMappingSh1(), dictionaryCat->getMappingS1());
        size_t new_predicate1 = mapIdPredicate(tripleID->getPredicate(), dictionaryCat->getMappingP1());
        size_t new_object1 = mapIdSection(tripleID->getObject(), dictionaryCat->getMappingSh1(), dictionaryCat->getMappingO1());
        return new TripleID(new_subject1, new_predicate1, new_object1);
    }
    else {
        size_t new_subject2 = mapIdSection(tripleID->getSubject(), dictionaryCat->getMappingSh2(), dictionaryCat->getMappingS2());
        size_t new_predicate2 = mapIdPredicate(tripleID->getPredicate(), dictionaryCat->getMappingP2());
        size_t new_object2 = mapIdSection(tripleID->getObject(), dictionaryCat->getMappingSh2(), dictionaryCat->getMappingO2());
        return new TripleID(new_subject2, new_predicate2, new_object2);
    }
}

size_t BitmapTriplesIteratorCat::mapIdSection(size_t id, CatMapping* catMappingShared, CatMapping* catMapping)
{
    if (id <= catMappingShared->getSize()) {
        return catMappingShared->getMapping(id - 1);
    }
    else {
        if (catMapping->getType(id - catMappingShared->getSize() - 1) == 1) {
            return catMapping->getMapping(id - catMappingShared->getSize() - 1);
        }
        else {
            return catMapping->getMapping(id - catMappingShared->getSize() - 1) + dictionaryCat->getShared()->getNumberOfElements();
        }
    }
}

size_t BitmapTriplesIteratorCat::mapIdPredicate(size_t id, CatMapping* catMappingShared, CatMapping* catMapping)
{
    return catMapping->getMapping(id - 1);
}
}
