
#include "FourSectionDictionaryCat.hpp"
#include "../libdcs/CSD_PFC.h"

using namespace std;

namespace hdt {

FourSectionDictionaryCat::FourSectionDictionaryCat(HDTSpecification& spec)
{
    delete subjects;
    subjects = nullptr;
    delete predicates;
    predicates = nullptr;
    delete objects;
    objects = nullptr;
    delete shared;
    shared = nullptr;

    /// Set blocksize if specified in spec file.
    blocksize = 16;
    string blockSizeStr = "";
    try {
        blockSizeStr = spec.get("dict.block.size");
    }
    catch (exception& e) {
    }

    if (blockSizeStr != "") {
        blocksize = atoi((const char*)blockSizeStr.c_str());
    }
}

FourSectionDictionaryCat::~FourSectionDictionaryCat() {
    delete mappingS1;
    delete mappingS2;
    delete mappingP1;
    delete mappingP2;
    delete mappingO1;
    delete mappingO2;
    delete mappingSh1;
    delete mappingSh2;
    delete mappingS;
}

void FourSectionDictionaryCat::cat(Dictionary* dict1, Dictionary* dict2)
{

    /// Step 1: Merge predicate sections
    //    CatCommon* commonP1P2 = new CatCommon(dict1->getPredicates(), dict2->getPredicates());
    //    size_t numCommonPredicates = commonP1P2->getCommonNum();
    //    delete commonP1P2;
    //    size_t numPredicates = dict1->getNpredicates() + dict2->getNpredicates() - numCommonPredicates;
    //    cout << "Final number of predicates: " << numPredicates << endl;
    NotSharedMergeIterator* nsmit = new NotSharedMergeIterator(dict1->getPredicates(), dict2->getPredicates(),
                                                               new CatIterator(), new CatIterator());
    predicates = new csd::CSD_PFC(nsmit, blocksize);
    /// Store mappings.
    mappingP1 = new CatMapping(dict1->getNpredicates());
    mappingP2 = new CatMapping(dict2->getNpredicates());
    mappingP1->set(nsmit->getMapping1(), CAT_PREDICATES);
    mappingP2->set(nsmit->getMapping2(), CAT_PREDICATES);
    delete nsmit;

    /// Step 2: Merge subject sections
    size_t sizeS1 = dict1->getSubjects()->getNumberOfElements();
    size_t sizeS2 = dict2->getSubjects()->getNumberOfElements();
    //    // Case 1: Subjects of dict1 ~ Objects and Shared of dict2
    //    CatIterator* commonSubject1 = new CatIterator(new CatCommon(dict1->getSubjects(), dict2->getShared()),
    //                                                  new CatCommon(dict1->getSubjects(), dict2->getObjects()));
    //    size_t numCommonSubject1 = commonSubject1->getCommonNum();
    //    // Case 2: Subjects of dict2 ~ Objects and Shared of dict1
    //    CatIterator* commonSubject2 = new CatIterator(new CatCommon(dict2->getSubjects(), dict1->getShared()),
    //                                                  new CatCommon(dict2->getSubjects(), dict1->getObjects()));
    //    size_t numCommonSubject2 = commonSubject2->getCommonNum();
    //    // Case 3: Subjects of dict1 ~ Subjects of dict2
    //    CatCommon* commonS1S2 = new CatCommon(dict1->getSubjects(), dict2->getSubjects());
    //    size_t numCommonSubjects = commonS1S2->getCommonNum();
    //
    //    size_t numSubjects = sizeS1 + sizeS2 - numCommonSubjects - numCommonSubject1 - numCommonSubject2;
    //    delete commonSubject1;
    //    delete commonSubject2;
    //    delete commonS1S2;
    //    cout << "Final number of subjects: " << numSubjects << endl;

    /// Get common terms: [1] Dict1 subjects and [2] Dict2 shared and objects.
    CatIterator* commonSubject1 = new CatIterator(new CatCommon(dict1->getSubjects(), dict2->getShared()),
                                                  new CatCommon(dict1->getSubjects(), dict2->getObjects()));
    /// Get common terms: [1] Dict2 subjects and [2] Dict1 shared and objects.
    CatIterator* commonSubject2 = new CatIterator(new CatCommon(dict2->getSubjects(), dict1->getShared()),
                                                  new CatCommon(dict2->getSubjects(), dict1->getObjects()));
    nsmit = new NotSharedMergeIterator(dict1->getSubjects(), dict2->getSubjects(), commonSubject1, commonSubject2);
    subjects = new csd::CSD_PFC(nsmit, blocksize);
    /// Store mappings.
    mappingS1 = new CatMapping(sizeS1);
    mappingS2 = new CatMapping(sizeS2);
    mappingS1->set(nsmit->getMapping1(), CAT_SUBJECTS);
    mappingS2->set(nsmit->getMapping2(), CAT_SUBJECTS);
    delete nsmit;

    //// Step 3: Merge object section ////
    size_t sizeO1 = dict1->getObjects()->getNumberOfElements();
    size_t sizeO2 = dict2->getObjects()->getNumberOfElements();

    //    // Case 1: Objects of dict1 ~ Subjects and Shared of dict2
    //    CatIterator* commonObject1 = new CatIterator(new CatCommon(dict1->getObjects(), dict2->getShared()),
    //                                                 new CatCommon(dict1->getObjects(), dict2->getSubjects()));
    //    size_t numCommonObject1 = commonObject1->getCommonNum();
    //    // Case 2: Objects of dict2 ~ Subjects and Shared of dict1
    //    CatIterator* commonObject2 = new CatIterator(new CatCommon(dict2->getObjects(), dict1->getShared()),
    //                                                 new CatCommon(dict2->getObjects(), dict1->getSubjects()));
    //    size_t numCommonObject2 = commonObject2->getCommonNum();
    //    // Case 3: Objects of dict1 ~ Objects of dict2
    //    CatCommon* commonO1O2 = new CatCommon(dict1->getObjects(), dict2->getObjects());
    //    size_t numCommonObjects = commonO1O2->getCommonNum();
    //    size_t numObjects = sizeO1 + sizeO2 - numCommonObjects - numCommonObject1 - numCommonObject2;
    //    delete commonObject1;
    //    delete commonObject2;
    //    delete commonO1O2;
    //    cout << "Final number of objects: " << numObjects << endl;

    CatIterator* commonObject1 = new CatIterator(new CatCommon(dict1->getObjects(), dict2->getShared()),
                                                 new CatCommon(dict1->getObjects(), dict2->getSubjects()));
    CatIterator* commonObject2 = new CatIterator(new CatCommon(dict2->getObjects(), dict1->getShared()),
                                                 new CatCommon(dict2->getObjects(), dict1->getSubjects()));
    nsmit = new NotSharedMergeIterator(dict1->getObjects(), dict2->getObjects(), commonObject1, commonObject2);
    objects = new csd::CSD_PFC(nsmit, blocksize);

    /// Store mappings.
    mappingO1 = new CatMapping(sizeO1);
    mappingO2 = new CatMapping(sizeO2);
    mappingO1->set(nsmit->getMapping1(), CAT_OBJECTS);
    mappingO2->set(nsmit->getMapping2(), CAT_OBJECTS);
    delete nsmit;

    //// Step 4: Merge shared section ////
    //    CatCommon* common = new CatCommon(dict1->getSubjects(), dict2->getObjects());
    //    size_t numCommonS1O2 = common->getCommonNum();
    //    delete common;
    //
    //    common = new CatCommon(dict1->getObjects(), dict2->getSubjects());
    //    size_t numCommonO1S2 = common->getCommonNum();
    //    delete common;
    //
    //    common = new CatCommon(dict1->getShared(), dict2->getShared());
    //    size_t numCommonSh1Sh2 = common->getCommonNum();
    //    delete common;
    //
    //    size_t numShared = dict1->getShared()->getNumberOfElements() + dict2->getShared()->getNumberOfElements() - numCommonSh1Sh2 + numCommonS1O2 + numCommonO1S2;
    //    cout << "Final number of shared: " << numShared << endl;

    SharedMergeIterator* smit = new SharedMergeIterator(dict1, dict2);
    shared = new csd::CSD_PFC(smit, blocksize);

    /// Store mappings.
    mappingSh1 = new CatMapping(dict1->getShared()->getNumberOfElements());
    mappingSh2 = new CatMapping(dict2->getShared()->getNumberOfElements());
    mappingSh1->set(smit->getMapping1(), CAT_SHARED);
    mappingSh2->set(smit->getMapping2(), CAT_SHARED);
    mappingS1->set(smit->getMappingS1Sh(), CAT_SHARED);
    mappingS2->set(smit->getMappingS2Sh(), CAT_SHARED);
    mappingO1->set(smit->getMappingO1Sh(), CAT_SHARED);
    mappingO2->set(smit->getMappingO2Sh(), CAT_SHARED);
    delete smit;

    /// Store inverse mappings.
    mappingS = new CatMappingBack(subjects->getLength() + shared->getLength());
    for (size_t i = 0; i < mappingSh1->getSize(); i++) {
        mappingS->set(mappingSh1->getMapping(i), i + 1, CAT_SUBJ1);
    }

    for (size_t i = 0; i < mappingSh2->getSize(); i++) {
        mappingS->set(mappingSh2->getMapping(i), i + 1, CAT_SUBJ2);
    }

    for (size_t i = 0; i < mappingS1->getSize(); i++) {
        if (mappingS1->getType(i) == 1) {
            mappingS->set(mappingS1->getMapping(i), (i + 1 + (size_t)dict1->getNshared()), CAT_SUBJ1);
        }
        else {
            mappingS->set(mappingS1->getMapping(i) + (size_t)shared->getLength(), (i + 1 + (size_t)dict1->getNshared()), CAT_SUBJ1);
        }
    }

    for (size_t i = 0; i < mappingS2->getSize(); i++) {
        if (mappingS2->getType(i) == 1) {
            mappingS->set(mappingS2->getMapping(i), (i + 1 + (size_t)dict2->getNshared()), CAT_SUBJ2);
        }
        else {
            mappingS->set(mappingS2->getMapping(i) + (size_t)shared->getLength(), (i + 1 + (size_t)dict2->getNshared()), CAT_SUBJ2);
        }
    }

    //    for (size_t i = 0; i < mappingS->getSize(); i++) {
    //        if (mappingS->getMapping(i).size() == 2) {
    //            cout << mappingS->getMapping(i)[0] << " " << mappingS->getMapping(i)[1] << endl;
    //        }
    //        else if (mappingS->getMapping(i).size() == 1) {
    //            cout << mappingS->getMapping(i)[0] << endl;
    //        }
    //        else {
    //            cout << "Nothing returned!" << endl;
    //        }
    //    }
}

CatMapping* FourSectionDictionaryCat::getMappingSh1()
{
    return mappingSh1;
}
CatMapping* FourSectionDictionaryCat::getMappingSh2()
{
    return mappingSh2;
}
CatMapping* FourSectionDictionaryCat::getMappingS1()
{
    return mappingS1;
}
CatMapping* FourSectionDictionaryCat::getMappingS2()
{
    return mappingS2;
}
CatMapping* FourSectionDictionaryCat::getMappingO1()
{
    return mappingO1;
}
CatMapping* FourSectionDictionaryCat::getMappingO2()
{
    return mappingO2;
}
CatMapping* FourSectionDictionaryCat::getMappingP1()
{
    return mappingP1;
}
CatMapping* FourSectionDictionaryCat::getMappingP2()
{
    return mappingP2;
}
CatMappingBack* FourSectionDictionaryCat::getMappingS()
{
    return mappingS;
}

// CatMapping Methods:
CatMapping::CatMapping(size_t size)
{
    mapping.resize(size);
}

size_t CatMapping::getMapping(size_t index)
{
    return mapping[index].first;
}

CatMappingType CatMapping::getType(size_t index)
{
    return mapping[index].second;
}

void CatMapping::set(vector<pair<size_t, size_t> > vec, CatMappingType type)
{
    for (size_t index = 0; index < vec.size(); index++) {
        mapping[vec[index].first] = make_pair(vec[index].second, type);
    }
}

size_t CatMapping::getSize()
{
    return mapping.size();
}

// CatMappingBack Methods:
CatMappingBack::CatMappingBack(size_t size)
{
    mapping1.resize(size + 1);
    for (size_t i = 0; i < mapping1.size(); i++) {
        mapping1[i] = make_pair(0, CAT_NONE);
    }
    this->mapping2.resize(size + 1);
    for (size_t i = 0; i < mapping2.size(); i++) {
        mapping2[i] = make_pair(0, CAT_NONE);
    }
}

vector<size_t> CatMappingBack::getMapping(size_t index)
{
    vector<size_t> r;
    if (mapping1[index].first != 0)
        r.push_back(mapping1[index].first);
    if (mapping2[index].first != 0)
        r.push_back(mapping2[index].first);
    return r;
}

vector<CatMappingBackType> CatMappingBack::getType(size_t index)
{
    vector<CatMappingBackType> r;
    if (mapping1[index].first != 0)
        r.push_back(mapping1[index].second);
    if (mapping2[index].first != 0)
        r.push_back(mapping2[index].second);
    return r;
}

void CatMappingBack::set(size_t index, size_t mapping, CatMappingBackType type)
{
    if (mapping1[index].first == 0) {
        mapping1[index] = make_pair(mapping, type);
    }
    else {
        mapping2[index] = make_pair(mapping, type);
    }
}

size_t CatMappingBack::getSize()
{
    return mapping1.size();
}
}
