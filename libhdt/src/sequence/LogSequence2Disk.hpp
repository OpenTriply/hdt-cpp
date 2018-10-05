#ifndef LOGSEQUENCE2DISK_HPP_
#define LOGSEQUENCE2DISK_HPP_

#include "IntSequence.hpp"
#include "../util/SizeTArrayDisk.hpp"
#include "LogSequence2.hpp"
#include <Iterator.hpp>
#include <HDTVocabulary.hpp>

using namespace std;

namespace hdt {

//! LogSequence2 implementation with memory mapping.
class LogSequence2Disk : public IntSequence {
private:
    SizeTArrayDisk *data;   //<! memory mapped files.
    unsigned char numbits;  //<! number of bits stored in data.
    size_t numentries;      //<! number of entries in data.
    size_t maxval;          //<! maximum permitable value of an entry.

    static const uint8_t TYPE_SEQLOG = 1;
    static const unsigned int W = sizeof(size_t)*8;

    //! \param bitsField number of bits.
    //! \param numEntries number of entries.
    //! \return size_t's required to represent n integers of e bits each
    inline size_t numElementsFor(const size_t bitsField, const size_t numEntries) {
        return (((uint64_t)bitsField*numEntries+W-1)/W);
    }

    //! \param bitsField number of bits.
    //! \param numEntries number of entries.
    //! \return number of bits required for last word
    inline size_t lastWordNumBits(const size_t bitsField, const size_t numEntries) {
        size_t totalBits = bitsField*numEntries;
        if(totalBits==0) return 0;
        return (totalBits-1) % W + 1;
    }

    //! \param bitsField number of bits.
    //! \param numEntries number of entries.
    //! \return number of bytes required to represent n integers of e bits each.
    inline size_t numBytesFor(const size_t bitsField, const size_t numEntries) {
        return ((uint64_t)bitsField*numEntries+7)/8;
    }

	 //! \param data
	 //! \param bitsField length in bits of each field.
	 //! \param index position to store in.
	 //! \return
    inline size_t get_field(SizeTArrayDisk *data, const size_t bitsField, const size_t index) {
        if(bitsField==0) return 0;

        size_t bitPos = index*bitsField;
        size_t i=bitPos/W;
        size_t j=bitPos%W;
        size_t result;
        if (j+bitsField <= W) {
            result = (data->get(i) << (W-j-bitsField)) >> (W-bitsField);
        } else {
            result = data->get(i) >> j;
            result = result | (data->get(i+1) << ( (W<<1) -j-bitsField)) >> (W-bitsField);
        }
        return result;
    }

	 //! Store a given value in index into array A where every value uses bitsField bits.
	 //! \param data
	 //! \param bitsField length in bits of each field.
	 //! \param index position to store in.
	 //! \param value value to be stored.
    inline void set_field(SizeTArrayDisk *data, const size_t bitsField, const size_t index, const size_t value) {
        if(bitsField == 0) return;

        size_t bitPos = index*bitsField;
        size_t i=bitPos/W;
        size_t j=bitPos%W;

        size_t mask = ~(~((size_t)0) << bitsField) << j;
        data->set(i, (data->get(i) & ~mask) | (value << j));

        if (j+bitsField>W) {
            mask = (~((size_t)0) << (bitsField+j-W));
            data->set(i+1, (data->get(i+1) & mask)| value >> (W-j));
        }
    }

public:
    LogSequence2Disk(const char *location);
    LogSequence2Disk(const char *location, unsigned int numbits);
    LogSequence2Disk(const char *location, unsigned int numbits, size_t capacity);
    LogSequence2Disk(const char *location, unsigned int numbits, size_t capacity, bool initialize);

    virtual ~LogSequence2Disk();

    void set(size_t position, size_t value);

    //! Pushes back in data, the given value.
    //! \param value a size_t value to be appended in data.
    void push_back(size_t value);
    //! Trims unnecessary bits at the end of data.
    void reduceBits();

    void add(IteratorUInt &elements);
    size_t get(size_t position);
    size_t getNumberOfElements();
    size_t size();
    void save(std::ostream &output);
    void load(std::istream &input);
    size_t load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener=NULL);
    std::string getType();
    //! Resizes data.
    //! \param numElements the new size of data.
    void resizeData(size_t numElements);
};

}

#endif //LOGSEQUENCE2DISK_HPP_
