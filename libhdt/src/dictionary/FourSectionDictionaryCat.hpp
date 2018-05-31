
#ifndef FOURSECTIONDICTIONARYCAT_HPP_
#define FOURSECTIONDICTIONARYCAT_HPP_

#include <HDTSpecification.hpp>
#include <HDTEnums.hpp>
#include <Dictionary.hpp>

#include <vector>
#include <algorithm>

#include "FourSectionDictionary.hpp"
#include "FSDCatIterators.hpp"

namespace hdt {

class CatMapping;

class CatMappingBack;

/**
* @brief HDTCat dictionary class, subclass of FourSectionDictionary.
*/
class FourSectionDictionaryCat : public FourSectionDictionary {
private:
    CatMapping* mappingSh1; /// Input 1: Shared terms.
    CatMapping* mappingSh2; /// Input 2: Shared terms.
    CatMapping* mappingS1; /// Input 1: Subject terms.
    CatMapping* mappingS2; /// Input 2: Subject terms.
    CatMapping* mappingO1; /// Input 1: Object terms.
    CatMapping* mappingO2; /// Input 2: Object terms.
    CatMapping* mappingP1; /// Input 1: Predicate terms.
    CatMapping* mappingP2; /// Input 2: Predicate terms.
    CatMappingBack* mappingS; /// Inverse mapping of subjects/shared.

public:
    FourSectionDictionaryCat(HDTSpecification& spec);

    ~FourSectionDictionaryCat();

    /**
     * @brief HDTCat operation upon two dictionaries.
     */
    void cat(Dictionary* dict1, Dictionary* dict2);

    /**
     * @brief Creates and returns an HDTCat Dictionary Section.
     */
    csd::CSD* catSection(size_t numentries, DictionarySection type, IteratorUCharString* it1,
                         IteratorUCharString* it2, CatIterator* it1common, CatIterator* it2common,
                         CatMapping* mappingHdt1, CatMapping* mappingHdt2);

    CatMapping* getMappingSh1();

    CatMapping* getMappingSh2();

    CatMapping* getMappingS1();

    CatMapping* getMappingS2();

    CatMapping* getMappingO1();

    CatMapping* getMappingO2();

    CatMapping* getMappingP1();

    CatMapping* getMappingP2();

    CatMappingBack* getMappingS();
};

/**
* @brief Helper class that keeps a mapping between a section of an input file
* to a section of the resulting file.
*/
class CatMapping {
private:
    vector<pair<size_t, CatMappingType> > mapping; /// Vector of mapping pairs.

public:
    CatMapping(size_t size);

    ~CatMapping() {}

    /**
     * @brief Given the index of input file section term,
     * returns the index of the new file section term.
     *
     * @param index of an input section term
     * @return index of the output section term as size_t
     */
    size_t getMapping(size_t index);

    /**
     * @brief Given the index of input file section term,
     * returns the mapping of the new file section term.
     *
     * @param index of an input section term
     * @return mapping of the output section term as CatMappingType enum
     */
    CatMappingType getType(size_t index);

    /**
     * @brief Sets a new mapping.
     *
     * @param vec is a vector of pairs of indices mapped to each other;
     * first from input file and second from the output file.
     * @param type is the type of mapping as CatMappingType enum
     */
    void set(vector<pair<size_t, size_t> > vec, CatMappingType type);

    size_t getSize();
};

/**
* @brief Helper class that keeps an inverse mapping of the subject section,
* i.e. from the new subjects section (incl. shared) to the old sections.
*/
class CatMappingBack {
private:
    vector<pair<size_t, CatMappingBackType> > mapping1; /// Vector of mapping pairs (output subj/shared section -> input 1 subj/shared section)
    vector<pair<size_t, CatMappingBackType> > mapping2; /// Vector of mapping pairs (output subj/shared section -> input 2 subj/shared section)

public:
    CatMappingBack(size_t size);

    ~CatMappingBack() {}

    /**
     * Get the indices of the input file section terms mapped to an output file section term index.
     *
     * @param index
     * @return A vector of 1 or 2 indices.
     */
    vector<size_t> getMapping(size_t index);

    /**
     * Get the mapping types of the input file section terms mapped to an output file section term index.
     *
     * @param index
     * @return A vector of 1 or 2 Mapping types as CatMappingBackType enum.
     */
    vector<CatMappingBackType> getType(size_t index);

    /**
     * @brief Sets a new mapping.
     *
     * @param index of the subj/shared sections of the output file
     * @param mapping is the index of the subj/shared sections of one of the input files
     * @param type is the type of mapping as CatMappingBackType enum
     */
    void set(size_t index, size_t mapping, CatMappingBackType type);

    size_t getSize();
};
}

#endif // FOURSECTIONDICTIONARYCAT_HPP_
