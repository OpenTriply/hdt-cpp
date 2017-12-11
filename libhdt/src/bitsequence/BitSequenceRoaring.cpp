
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
BitSequenceRoaring::BitSequenceRoaring() : numbits(0)
{
}

BitSequenceRoaring::~BitSequenceRoaring()
{
}

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

void BitSequenceRoaring::save(ostream & f) const
{

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

	return count;
}

BitSequenceRoaring * BitSequenceRoaring::load(istream & f)
{
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
