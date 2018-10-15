#ifndef BITMAPTRIPLESCAT_HPP_
#define BITMAPTRIPLESCAT_HPP_

#include <Triples.hpp>
#include <HDTSpecification.hpp>
#include <Iterator.hpp>
#include <unordered_set>

#include "BitmapTriples.hpp"
#include "TriplesList.hpp"
#include "../dictionary/FourSectionDictionaryCat.hpp"
#include "TriplesComparator.hpp"

using namespace std;

namespace hdt {

//! Bitmap triples of the output file of hdtCat.
class BitmapTriplesCat : public BitmapTriples {
private:
    const char *location;   //<! Absolute path to a temporary directory.
    LogSequence2Disk *vectorY;
    LogSequence2Disk *vectorZ;
    BitSequence375 *bitY;
    BitSequence375 *bitZ;

public:
    explicit BitmapTriplesCat(const char *location);
    ~BitmapTriplesCat() override;

    //! Creates the bitmap triples for hdtCat.
    //! \param it an iterator of bitmap triples.
    //! \param listener the listener.
    void cat(IteratorTripleID *it, ProgressListener* listener = nullptr);
};

//! Iterator of bitmap triples, special for hdtCat.
class BitmapTriplesIteratorCat : public IteratorTripleID {
private:
    Triples *triplesHDT1, *triplesHDT2; //<! Triples of input HDTs.
    FourSectionDictionaryCat* dictionaryCat;    //!< The merged dictionary of hdtCat.
    vector<TripleID> arrayOfTriples;    //!< A vector of triples having the same subject.
    vector<TripleID>::iterator triplesIterator; //!< Iterator over arrayOfTriples.
    size_t count;   //!< Counter that keeps track of the current subject.

    //! Sorts arrayOfTriples vector.
    //! \param order the triple component order.
    inline void sort(TripleComponentOrder order) {
        std::sort(arrayOfTriples.begin(), arrayOfTriples.end(), TriplesComparator(order));
    }

    //! Removed all duplicates from arrayOfTriples vector.
    inline void removeDuplicates() {
        if(arrayOfTriples.size()<=1)
            return;

        size_t j = 0;
        for(size_t i=1; i<arrayOfTriples.size(); i++) {
            if(!arrayOfTriples[i].isValid()) {
                cerr << "WARNING: Triple with null component: " << arrayOfTriples[i] << endl;
            }
            if(arrayOfTriples[i] != arrayOfTriples[j] && arrayOfTriples[i].isValid()) {
                j++;
                arrayOfTriples[j] = arrayOfTriples[i];
            }
        }
        arrayOfTriples.resize(j+1);
    }

public:
    //! Constructor
    //! \param TriplesHDT1  the triples of first input HDT.
    //! \param TriplesHDT2  the triples of second input HDT.
    //! \param dictCat      the dictionary of the output HDT, previously created.
    BitmapTriplesIteratorCat(Triples* TriplesHDT1, Triples* TriplesHDT2, FourSectionDictionaryCat* dictCat);
    ~BitmapTriplesIteratorCat() override;

    bool hasNext() override;
    TripleID* next() override;

    //! Returns the number of triples as an estimation. Apparently, it does not remove duplicates from the estimation.
    //! \return estimated number of triples.
    size_t estimatedNumResults() override;


    //! Given a subject ID, returns all the triples with this subject ID from both of the input HDTs.
    //! \param count a subject ID.
    //! \return a vector of TripleIDs.
    vector<TripleID> getTripleID(size_t count);

    //! Given a tripleID of an input HDT file and a number indicating the input file,
    //! return the corresponding output TripleID.
    //! \param tripleID a triple ID of an input HDT file.
    //! \param num a number indicating the input file used (1 or 2).
    //! \return the corresponding triple ID of the output HDT file.
    TripleID* mapTriple(TripleID* tripleID, size_t num);

    //! Given an input section ID, return the corresponding output section ID. (only for subjects, objects)
    //! \param id a section ID of an input HDT.
    //! \param catMappingShared the shared mappings of a section.
    //! \param catMapping   the section mappings.
    //! \return an output section ID.
    size_t mapIdSection(size_t id, CatMapping* catMappingShared, CatMapping* catMapping);

    //! Given an input predicate ID, return the corresponding output predicate ID.
    //! \param id an input predicate ID.
    //! \param catMapping the predicate mappings.
    //! \return an output predicate ID.
    size_t mapIdPredicate(size_t id, CatMapping* catMapping);
};
}

#endif //BITMAPTRIPLESCAT_HPP_
