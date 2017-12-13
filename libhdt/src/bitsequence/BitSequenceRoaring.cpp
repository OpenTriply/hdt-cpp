
#include <cassert>
#include <stdexcept>
#include <cmath>
#include <string.h>

#include "BitSequenceRoaring.h"
#include "roaring.hh"

#include "../util/bitutil.h"
#include "../libdcs/VByte.h"

#include "../util/crc8.h"
#include "../util/crc32.h"

#define CHECKPTR(base, max, size) if(((base)+(size))>(max)) throw std::runtime_error("Could not read completely the HDT from the file.");

namespace hdt
{
    // Auxialiary function.
    bool roaring_to_bit(uint32_t value, void *array) {
        /*
         * Stores the value-th bit to the proper place in the sequence.
         */
        size_t byte = value >> 3;
        size_t bit = 1 << (value % (8*sizeof(unsigned char)));
        *(((unsigned char *) array)+byte) |= bit;
        return true;
    }

    // Class functions
    BitSequenceRoaring::BitSequenceRoaring() : numbits(0)
    {
    }
    BitSequenceRoaring::BitSequenceRoaring(int capacity) : numbits(0)
    {
    }
    BitSequenceRoaring::~BitSequenceRoaring()
    = default;

    size_t BitSequenceRoaring::rank0(const size_t i) const
    {
        return i-rank1(i);
    }

    size_t BitSequenceRoaring::select0(size_t x) const
    {
        throw std::runtime_error("BitSequenceRoaring select0: Not implemented");
    }

    size_t BitSequenceRoaring::rank1(const size_t i) const
    {
        return sequence.rank(i);
    }

    size_t BitSequenceRoaring::select1(size_t x) const
    {
        size_t numones = rank1(numbits);
        if(x > numones) {
            return numbits;
        }
        else if(x == 0) {
            return -1;
        }
        uint32_t element;
        sequence.select((uint32_t) x-1, &element);
        return element;
    }

    size_t BitSequenceRoaring::selectNext1(const size_t start) const
    {
        return select1((start==0 ? 0 : rank1(start-1))+1);
    }

    size_t BitSequenceRoaring::selectPrev1(const size_t start) const
    {
        throw std::runtime_error("BitSequenceRoaring selectPrev1: Not implemented");
        return 0;
    }

    bool BitSequenceRoaring::access(const size_t i) const
    {
        return sequence.contains(i);
    }

    size_t BitSequenceRoaring::getNumBits() const
    {
        return numbits;
    }

    size_t BitSequenceRoaring::countOnes() const
    {
        return rank1(numbits);
    }

    size_t BitSequenceRoaring::countZeros() const
    {
        return numbits-countOnes();
    }

    size_t BitSequenceRoaring::getSizeBytes() const
    {
        return sequence.getSizeInBytes();
    }

    void BitSequenceRoaring::save(ostream & out) const
    {
        CRC8 crch;
        CRC32 crcd;
        unsigned char arr[9];
        unsigned char *array;

        // Write type
        unsigned char type=TYPE_BITMAP_PLAIN;
        crch.writeData(out, &type, sizeof(type));

        // Write NumBits
        size_t len = csd::VByte::encode(arr, numbits);
        crch.writeData(out, arr, len);

        // Write header CRC
        crch.writeCRC(out);

        // Write data
        len = numBytes(numbits);
        array = new unsigned char[len]();
        sequence.iterate(hdt::roaring_to_bit, array);
        crcd.writeData(out, (unsigned char*)&array[0], len);

        crcd.writeCRC(out);
        delete[] array;
    }


    size_t BitSequenceRoaring::load(const unsigned char *ptr, const unsigned char*maxPtr, ProgressListener *listener)
    {
        size_t count=0;

        // Check type
        CHECKPTR(&ptr[count], maxPtr, 1);
        if(ptr[count++]!=TYPE_BITMAP_PLAIN) {
            throw std::runtime_error("Trying to read a BitSequenceRoaring but the type does not match");
        }
        // Read numbits
        uint64_t totalBits;
        count += csd::VByte::decode(&ptr[count], maxPtr, &totalBits);

        if(sizeof(size_t)==4 && totalBits>0xFFFFFFFF) {
            throw std::runtime_error("This File is too big to be processed using 32Bit version. Please compile with 64bit support");
        }
        this->numbits = (size_t) totalBits;

        // CRC
        CRC8 crch;
        crch.update(&ptr[0], count);
        CHECKPTR(&ptr[count], maxPtr, 1);
        if(ptr[count++]!=crch.getValue()) {
            throw std::runtime_error("Wrong checksum in BitSequenceRoaring Header.");
        }

        // Read buffer
        size_t sizeBytes = numBytes(numbits);
        if(&ptr[count+sizeBytes]>=maxPtr) {
            throw std::runtime_error("BitSequenceRoaring tries to read beyond the end of the file");
        }

        // Store Bitmap
        size_t upper = ceil(totalBits/sizeof(char));
        char cur_byte;
        uint64_t cur_bit = 0;
        for(int i=0; i<upper; i++) {
            cur_byte = ptr[i];
            for(int j=0; j< sizeof(char); j++) {
                cur_bit = ((cur_byte >> j)  & 0x01);
                if(cur_bit) {
                    sequence.add(cur_byte*sizeof(char) + j);
                }
            }
        }
        count += sizeBytes;

        CHECKPTR(&ptr[count], maxPtr, 4);
        count += 4; // CRC of data
        cout << endl << "IN LOAD WITH POINTERS!" << endl;
        return count;
    }

    BitSequenceRoaring * BitSequenceRoaring::load(istream & in)
    {
        CRC8 crch;
        CRC32 crcd;
        unsigned char arr[9];

        // Read Type
        unsigned char type;
        in.read((char*)&type, sizeof(type));
        if(type!=TYPE_BITMAP_PLAIN) {    // throw exception
            throw std::runtime_error("Trying to read a BitmapPlain but the type does not match");
        }
        crch.update(&type, sizeof(type));

        BitSequenceRoaring * ret = new BitSequenceRoaring();

        // Load number of total bits
        uint64_t totalBits = csd::VByte::decode(in);
        if(sizeof(size_t)==4 && totalBits>0xFFFFFFFF) {
            throw std::runtime_error("This File is too big to be processed using 32Bit version. Please compile with 64bit support");
        }
        ret->numbits = (size_t) totalBits;


        size_t len = csd::VByte::encode(arr, ret->numbits);
        crch.update(arr,len);
        crc8_t filecrch = crc8_read(in);
        if(filecrch!=crch.getValue()) {
            throw std::runtime_error("Wrong checksum in BitSequenceRoaring Header.");
        }

        // Calculate numWords and create array
        size_t numwords = ret->numWords(ret->numbits);
        size_t *array = new size_t[numwords];
        //ret->data.resize(ret->numwords);
        //ret->array = &ret->data[0];

        // Read array from file, byte-aligned.
        size_t bytes = ret->numBytes(ret->numbits);
        in.read((char*)&array, bytes);
        if(in.gcount()!=bytes) {
            throw std::runtime_error("BitSequenceRoaring error reading array of bits.");
        }

        crcd.update((unsigned char*)&array[0], bytes);
        crc32_t filecrcd = crc32_read(in);
        if(filecrcd!=crcd.getValue()) {
            throw std::runtime_error("Wrong checksum in BitSequenceRoaring Data.");
        }
        delete[] array;
        cout << endl << "IN LOAD WITH STREAM!" << endl;
        return ret;
    }

    void BitSequenceRoaring::set(const size_t i, bool val)
    {
        if(val) {
            sequence.add(i);
        }
        else {
            sequence.remove(i);
        }
        numbits = i>=numbits ? i+1 : numbits;
    }

    void BitSequenceRoaring::append(bool bit)
    {
        this->set(numbits, bit);
    }

    void BitSequenceRoaring::optimize()
    {
        sequence.runOptimize();
        sequence.shrinkToFit();
    }

}
