#include "LogSequence2Disk.hpp"
#include "../util/crc8.h"
#include "../util/crc32.h"
#include "../libdcs/VByte.h"

namespace hdt {

LogSequence2Disk::LogSequence2Disk(const char *location, unsigned int numbits, size_t capacity, bool initialize)
        : numbits(numbits), numentries(0) {
    this->maxval = maxVal(numbits);
    size_t totalSize = numElementsFor(numbits, capacity);
    if(totalSize==0) totalSize = 1;
    this->data = new SizeTArrayDisk(location, totalSize);
    numentries = initialize ? capacity : 0;
}

LogSequence2Disk::~LogSequence2Disk() {
    delete data;
}

void LogSequence2Disk::set(size_t position, size_t value) {
    if(position>numentries) {
        throw std::runtime_error("Trying to modify a position out of the structure capacity. Use push_back() instead");
    }
    if(value>maxval) {
        throw std::runtime_error("Trying to insert a value bigger that expected. Please increase numbits when creating the data structure.");
    }
    set_field(data, numbits, position, value);
}

    size_t LogSequence2Disk::get(size_t position) {
        if(position>=numentries) {
            throw std::runtime_error("Trying to get an element bigger than the array.");
        }
        return this->get_field(data, numbits, position);
    }

void LogSequence2Disk::push_back(size_t value) {
    if(value>maxval) {
        throw std::runtime_error("Trying to insert a value bigger that expected. Please increase numbits when creating the data structure.");
    }
    size_t neededSize = numElementsFor(numbits, numentries+1);
    if(data->getNumberOfElements() < neededSize) {
        resizeData(data->getNumberOfElements()*2);
    }
    set(numentries, value);
    numentries++;
}

void LogSequence2Disk::reduceBits() {
    size_t max=0;
    for(size_t i=0; i<numentries; i++) {
        size_t value = get(i);
        max = value>max ? value : max;
    }

    unsigned int newbits = bits(max);

    if(newbits<numbits) {
        // Go through elements, read one and write one.
        // Since the number of bits is smaller they don't overlap.
        for(size_t i=0;i<numentries;i++) {
            size_t value = get_field(data, numbits, i);
            set_field(data, newbits, i, value);
        }
        numbits = newbits;;
        maxval = maxVal(numbits);

        size_t totalSize = numElementsFor(numbits, numentries);
        if(totalSize != this->data->getNumberOfElements()) {
            resizeData(totalSize);
        }
    }
}

void LogSequence2Disk::add(IteratorUInt &elements) {
    throw std::logic_error("Not implemented");
}

size_t LogSequence2Disk::getNumberOfElements() {
    return numentries;
}

void LogSequence2Disk::save(std::ostream &out) {

    CRC8 crch;
    CRC32 crcd;
    unsigned char vbyte[9];
    size_t len;

    // Write type
    uint8_t type = TYPE_SEQLOG;
    crch.writeData(out, &type, sizeof(uint8_t));

    // Write numbits
    crch.writeData(out, &numbits, sizeof(numbits));

    // Write numentries
    len=csd::VByte::encode(vbyte, numentries);
    crch.writeData(out, vbyte, len);

    // Write Header CRC
    crch.writeCRC(out);

    // Write data
    size_t numWords = numElementsFor(numbits, numentries);
    for(size_t i=0; (numWords != 0 && i < numWords-1); i++){
        size_t num = this->data->get(i);
        crcd.writeData(out, reinterpret_cast<unsigned char *>(&num), sizeof(num));
    }

    if(numWords>0) {
        long lastUsedBits = lastWordNumBits(numbits, numentries);
        size_t value = this->data->get(numWords-1);
        while(lastUsedBits>0) {
            unsigned char toWrite = value & 0xFF;
            crcd.writeData(out, &toWrite, sizeof(toWrite));
            value >>= 8;
            lastUsedBits-=8;
        }
    }

    // Write Data CRC
    crcd.writeCRC(out);

}

void LogSequence2Disk::load(std::istream &input) {
    throw std::logic_error("Not implemented");
}
size_t LogSequence2Disk::load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener) {
    throw std::logic_error("Not implemented");
}

std::string LogSequence2Disk::getType() {
    return HDTVocabulary::SEQ_TYPE_LOG2;
}

size_t LogSequence2Disk::size() {
    return numBytesFor(numbits, numentries);
}

void LogSequence2Disk::resizeData(size_t numElements) {
    data->resize(numElements);
}

void LogSequence2Disk::trimToSize() {
    size_t totalSize = numElementsFor(numbits, numentries);
    if(totalSize != this->data->getNumberOfElements()) {
        resizeData(totalSize);
    }
}

}
