
#ifndef FOURSECTIONDICTIONARYCAT_HPP_
#define FOURSECTIONDICTIONARYCAT_HPP_

#include <HDTSpecification.hpp>
#include <HDTEnums.hpp>
#include <Iterator.hpp>
#include <Dictionary.hpp>

#include <vector>
#include <algorithm>

#include "FourSectionDictionary.hpp"
#include "../util/SizeTArrayDisk.hpp"
#include "../sequence/LogSequence2Disk.hpp"

namespace hdt {

class CatMapping;
class CatMappingBack;
class CatCommon;
class CatIterator;

/*!
 *  HDTCat dictionary class, subclass of FourSectionDictionary.
 */
class FourSectionDictionaryCat : public FourSectionDictionary {
private:
    const char* location;
    CatMapping* mappingSh1; //!< Input 1 mapping: Shared terms.
    CatMapping* mappingSh2; //!< Input 2 mapping: Shared terms.
    CatMapping* mappingS1; //!< Input 1 mapping: Subject terms.
    CatMapping* mappingS2; //!< Input 2 mapping: Subject terms.
    CatMapping* mappingO1; //!< Input 1 mapping: Object terms.
    CatMapping* mappingO2; //!< Input 2 mapping: Object terms.
    CatMapping* mappingP1; //!< Input 1 mapping: Predicate terms.
    CatMapping* mappingP2; //!< Input 2 mapping: Predicate terms.
    CatMappingBack* mappingS; //!< Inverse mapping of subjects/shared.

    unsigned char *str;

    LogSequence2Disk *blocks; //!< Pointers to the first string of each bucket of a section dictionary sequence.
    string seqloc; //!< Filename for blocks

    IteratorUCharString *it1temp; //!< Auxialiary iterator for catShared.
    IteratorUCharString *it2temp; //!< Auxialiary iterator for catShared.

    // Auxiliary CatCommon iterators for catShared.
    CatCommon *itCommonSubjects1Objects2;
    CatCommon *itCommonObjects1Subjects2;
    CatCommon *itCommonShared1Subjects2;
    CatCommon *itCommonShared1Objects2;
    CatCommon *itCommonShared2Subjects1;
    CatCommon *itCommonShared2Objects1;

    //! Releases memory that str points to, and sets str to nullptr.
    //! \param s a pointer that supposedly points to the same as str.
    void freeStr(unsigned char *s);

public:
    //! Contructor of hdtCat dictionary
    //! \param location the path to a temporary directory.
    FourSectionDictionaryCat(const char *location);
    ~FourSectionDictionaryCat();

     //! hdtCat operation upon two dictionaries.
     //! \param dict1 the dictionary of first HDT file.
     //! \param dict2 the dictionary of second HDT file.
     //! \param listener the progress listener.
    void cat(Dictionary* dict1, Dictionary* dict2, ProgressListener* listener=NULL);

    //! Creates a new section (excl. shared) by merging two input sections.
    //! \param numentries the number of unique terms that are going to be stored in the section.
    //! \param type the type of section (e.g. CAT_PREDICATES etc.).
    //! \param it1 an iterator of the terms of the first input file's section.
    //! \param it2 an iterator of the terms of the second input file's section.
    //! \param it1common an iterator of common terms.
    //! \param it2common a second iterator of common terms.
    //! \param mappingHdt1 a mapping between first HDT file's section terms and output HDT section terms.
    //! \param mappingHdt2 a mapping between second HDT file's section terms and output HDT section terms.
    //! \param listener the progress listener.
    void catSection(size_t numentries, CatMappingType type, IteratorUCharString* it1,
                         IteratorUCharString* it2, CatIterator* it1common, CatIterator* it2common,
                         CatMapping *mappingHdt1, CatMapping *mappingHdt2, ProgressListener *listener=NULL);

    //! Creates the output shared section.
    //! \param numentries the number of unique terms that are going to be stored in the section.
    //! \param dict1 the dictionary of first HDT file.
    //! \param dict2 the dictionary of second HDT file.
    //! \param listener the progress listener.
    void catShared(size_t numentries, Dictionary *dict1, Dictionary *dict2, ProgressListener *listener=NULL);

    //! Accessors of section mappings.
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

/*!
 * Class for mapping input and output dictionary section terms.
 */
class CatMapping {
private:
    SizeTArrayDisk *mapping; //!< Object that holds an array A of the actual mapping. A: [input_id] => output_id
    SizeTArrayDisk *mappingType; //!< Object that holds an array B of the mapping types. B: [input_id] => mapping_type
    string mappingFileName;
    string mappingTypeFileName;
public:
     //! CatMapping constructor
     //! \param location is the dir path of the files that will be memory-mapped.
     //! \param section is a name for a section of an input HDT file (e.g. "P1", for the predicates of the input file 1).
     //! \param size is the number of terms contained in the section.
    CatMapping(const char *location, string section, size_t size);
    virtual ~CatMapping();

    //! Given the id of an input file section term, returns the index of the new file section term.
    //! \param index a section term id of one input HDT file
    //! \return index of the mapped section term of the output HDT file
    size_t getMapping(size_t index);

     //! Given the id of one input file section term, returns the mapping type of the mapped output term.
     //! \param index id of a section term of an input HDT file
     //! \return mapping type of the mapped section term of the output HDT file
    CatMappingType getType(size_t index);

    //! Saves a new mapping entry.
    //! \param index id of a section term of an input HDT file.
    //! \param val id of the mapped section term of the output HDT file.
    //! \param type type of mapping.
    void set(size_t index, size_t val, CatMappingType type);

    //! Get the number of all the mappings that are going to be (or are) stored.
    //! \return the number of all mappings.
    size_t getSize();
};

/*!
 * Class for keeping an inverse mapping from the output subjects and shared sections,
 * to the input subject sections.
 */
class CatMappingBack {
private:
    SizeTArrayDisk *mapping1; //!< Object that holds the first array mappings from output to input 1 subjects section.
    SizeTArrayDisk *mappingType1; //!< Object that holds the mapping types of mapping1.
    SizeTArrayDisk *mapping2; //!< Object that holds extra mapping, in case mapping1 gets filled.
    SizeTArrayDisk *mappingType2; //!< Object that holds the mapping types of mapping2.
    size_t size;    //<! Number of total possible mappings (+1)
    string mapping1FileName;
    string mappingType1FileName;
    string mapping2FileName;
    string mappingType2FileName;

public:
    //! CatMappingBack constructor
    //! \param location the dir path for the files that will be memory mapped.
    //! \param size the number of terms that will be mapped.
    CatMappingBack(const char *location, size_t size);
    virtual ~CatMappingBack();

     //! Get the id(s) of the input file section term(s) mapped to an output file section term id.
     //! \param index the id of ouput file section term.
     //! \return a vector of 1 or 2 ids.
    vector<size_t> getMapping(size_t index);

     //! Get the mapping type(s) of the input file section term(s) mapped to an output file section term id.
     //! \param index the id of ouput file section term.
     //! \return a vector of 1 or 2 types.
    vector<CatMappingBackType> getType(size_t index);

     //! Sets a new mapping.
     //! \param index the id of the subject or shared section term of the output file.
     //! \param mapping the index of the subject or shared section of one of the input files.
     //! \param type the type of mapping, indicating the origin section.
    void set(size_t index, size_t mapping, CatMappingBackType type);

    //! Returns the size of the mapping objects.
    //! \return the size of all the mapping objects.
    size_t getSize();
};

/*!
 * An iterator over common terms found in two IteratorUCharString objects.
 */
class CatCommon {
private:
    vector<pair<size_t, unsigned char*> > list; //!< Vector of (two) pairs to be compared.

    IteratorUCharString* it1; //<! First iterator.
    IteratorUCharString* it2; //<! Second iterator.

    bool has_next;
    size_t count1, count2; //<! Pointers to current positions of each input iterator.
    pair<size_t, size_t> next_t;   //<! A pair of ids that point to common terms.

public:
    CatCommon();
    CatCommon(IteratorUCharString* it1, IteratorUCharString* it2);
    ~CatCommon();

    bool hasNext();

    //! Returns the next common pair of ids.
    //! \return A pair of the indices of common terms; one from iterator 1 and one from iterator 2.
    pair<size_t, size_t> next();

     //! Auxiliary function that sets the next common pairs in list.
    void helpNext();
};

/*!
 * A class that provides an iterator ordered over two CatCommon iterators.
 */
class CatIterator {
private:
    vector<pair<size_t, pair<size_t, size_t>>> list; //!< A pair of a value and a pair taken from a CatCommon iterator.
                                                     //!< The value indicates which CatCommon iterator provides the pair.
    CatCommon *it1, *it2;

public:
    CatIterator();
    CatIterator(CatCommon* it1, CatCommon* it2);
    ~CatIterator();
    bool hasNext();

     //! \return Next id of a term that appears in two pairs of sections.
    size_t next();
};

}

#endif // FOURSECTIONDICTIONARYCAT_HPP_
