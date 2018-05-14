
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

/**
 * @brief HDTCat dictionary class, subclass of FourSectionDictionary.
 */
class FourSectionDictionaryCat : public FourSectionDictionary {
private:
    /**
     * @brief Helper class that keeps a mapping between a section of an input file
     * to a section of the resulting file.
     */
    class CatMapping {
    private:
        vector<pair<size_t, CatMappingType>> mapping; /// Vector of mapping pairs.

    public:
        CatMapping(size_t size)
        {
            mapping.resize(size);
        }
        ~CatMapping()
        {
        }

        /**
         * @brief Given the index of input file section term,
         * returns the index of the new file section term.
         *
         * @param index of an input section term
         * @return index of the output section term as size_t
         */
        size_t getMapping(size_t index)
        {
            return mapping[index].first;
        }

        /**
         * @brief Given the index of input file section term,
         * returns the mapping of the new file section term.
         *
         * @param index of an input section term
         * @return mapping of the output section term as CatMappingType enum
         */
        CatMappingType getType(size_t index)
        {
            return mapping[index].second;
        }

        /**
         * @brief Sets a new mapping.
         *
         * @param vec is a vector of pairs of indices mapped to each other;
         * first from input file and second from the output file.
         * @param type is the type of mapping as CatMappingType enum
         */
        void set(vector<pair<size_t, size_t>> vec, CatMappingType type)
        {
            for (size_t index = 0; index < vec.size(); index++) {
                mapping[vec[index].first] = make_pair(vec[index].second, type);
            }
        }

        size_t getSize()
        {
            return mapping.size();
        }
    };

    /**
     * @brief Helper class that keeps an inverse mapping of the subject section,
     * i.e. from the new subjects section (incl. shared) to the old sections.
     */
    class CatMappingBack {
    private:
        vector<pair<size_t, CatMappingBackType>> mapping1; /// Vector of mapping pairs (output subj/shared section -> input 1 subj/shared section)
        vector<pair<size_t, CatMappingBackType>> mapping2; /// Vector of mapping pairs (output subj/shared section -> input 2 subj/shared section)

    public:
        CatMappingBack(size_t size)
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

        ~CatMappingBack()
        {
        }

        /**
         * Get the indices of the input file section terms mapped to an output file section term index.
         *
         * @param index
         * @return A vector of 1 or 2 indices.
         */
        vector<size_t> getMapping(size_t index)
        {
            vector<size_t> r;
            if (mapping1[index].first != 0)
                r.push_back(mapping1[index].first);
            if (mapping2[index].first != 0)
                r.push_back(mapping2[index].first);
            return r;
        }

        /**
         * Get the mapping types of the input file section terms mapped to an output file section term index.
         *
         * @param index
         * @return A vector of 1 or 2 Mapping types as CatMappingBackType enum.
         */
        vector<CatMappingBackType> getType(size_t index)
        {
            vector<CatMappingBackType> r;
            if (mapping1[index].first != 0)
                r.push_back(mapping1[index].second);
            if (mapping2[index].first != 0)
                r.push_back(mapping2[index].second);
            return r;
        }

        /**
         * @brief Sets a new mapping.
         *
         * @param index of the subj/shared sections of the output file
         * @param mapping is the index of the subj/shared sections of one of the input files
         * @param type is the type of mapping as CatMappingBackType enum
         */
        void set(size_t index, size_t mapping, CatMappingBackType type)
        {
            if (mapping1[index].first == 0) {
                mapping1[index] = make_pair(mapping, type);
            }
            else {
                mapping2[index] = make_pair(mapping, type);
            }
        }

        size_t getSize()
        {
            return mapping1.size();
        }
    };

    CatMapping* mappingSh1; /// Input 1: Shared terms.
    CatMapping* mappingSh2; /// Input 2: Shared terms.
    CatMapping* mappingS1;  /// Input 1: Subject terms.
    CatMapping* mappingS2;  /// Input 2: Subject terms.
    CatMapping* mappingO1;  /// Input 1: Object terms.
    CatMapping* mappingO2;  /// Input 2: Object terms.
    CatMapping* mappingP1;  /// Input 1: Predicate terms.
    CatMapping* mappingP2;  /// Input 2: Predicate terms.
    CatMappingBack* mappingS;   /// Inverse mapping of subjects/shared.

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
};
}

#endif // FOURSECTIONDICTIONARYCAT_HPP_