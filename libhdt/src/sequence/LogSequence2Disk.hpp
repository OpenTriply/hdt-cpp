#ifndef LOGSEQUENCE2DISK_HPP_
#define LOGSEQUENCE2DISK_HPP_

#include "IntSequence.hpp"
#include "../util/SizeTArrayDisk.hpp"
#include "LogSequence2.hpp"
#include <Iterator.hpp>
#include <HDTVocabulary.hpp>

using namespace std;

namespace hdt {

class LogSequence2Disk : public IntSequence {
private:
    SizeTArrayDisk *data;
    unsigned char numbits;
    size_t numentries;
    size_t maxval;

    static const uint8_t TYPE_SEQLOG = 1;
    static const unsigned int W = sizeof(size_t)*8;

    /** size_t's required to represent n integers of e bits each */
    inline size_t numElementsFor(const size_t bitsField, const size_t numEntries) {
        return (((uint64_t)bitsField*numEntries+W-1)/W);
    }

    /** Number of bits required for last word */
    inline size_t lastWordNumBits(const size_t bitsField, const size_t numEntries) {
        size_t totalBits = bitsField*numEntries;
        if(totalBits==0) return 0;
        return (totalBits-1) % W + 1;
    }

    /** Number of bytes required to represent n integers of e bits each */
    inline size_t numBytesFor(const size_t bitsField, const size_t numEntries) {
        return ((uint64_t)bitsField*numEntries+7)/8;
    }

    /** Retrieve a given index from array A where every value uses len bits
	 * @param data
	 * @param bitsField Length in bits of each field
	 * @param index Position to store in
	 */
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

    /** Store a given value in index into array A where every value uses bitsField bits
	 * @param data
	 * @param bitsField Length in bits of each field
	 * @param index Position to store in
	 * @param value Value to be stored
	 */
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
    void push_back(size_t value);
    void reduceBits();

    void add(IteratorUInt &elements);
    size_t get(size_t position);
    size_t getNumberOfElements();
    size_t size();
    void save(std::ostream &output);
    void load(std::istream &input);
    size_t load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener=NULL);
    std::string getType();

    inline void printContents() {
//        cout << "Size of structure = " << this->data->getNumberOfElements() << endl;
//        cout << this->data->get(0) << endl;
//        for(size_t i=0; i<getNumberOfElements(); i++) {
//            cout << "At index " << i << " found this => " << this->get(i) << "." << endl;
//        }
    }

};

}

#endif //LOGSEQUENCE2DISK_HPP_
