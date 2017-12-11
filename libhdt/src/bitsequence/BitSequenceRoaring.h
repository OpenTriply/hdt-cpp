#ifndef _STATIC_BITSEQUENCE_ROARING_H
#define _STATIC_BITSEQUENCE_ROARING_H

#include <stdint.h>
#include <iostream>

#include "BitSeq.h"
#include "roaring.hh"

namespace hdt
{

class BitSequenceRoaring : public BitSeq
{

private:
  const static uint8_t TYPE_BITMAP_PLAIN = 1;
  Roaring sequence;
  size_t numbits;

  inline size_t numBytes(size_t bits) const {
		return bits==0 ? 1 : ((bits-1)>>3) + 1;
	}

public:
  BitSequenceRoaring();
  ~BitSequenceRoaring();

	size_t rank0(const size_t i) const;
	size_t select0(size_t x) const;
	size_t rank1(const size_t i) const;
	size_t select1(size_t x) const;
	size_t selectNext1(const size_t start) const;
	size_t selectPrev1(const size_t start) const;
  bool access(const size_t i) const;
	size_t getNumBits() const;
  size_t countOnes() const;
	size_t countZeros() const;
	size_t getSizeBytes() const;
  void save(ostream & f) const;
  size_t load(const unsigned char *ptr, const unsigned char*maxPtr, ProgressListener *listener=NULL);
	static BitSequenceRoaring * load(istream & f);

  // Additional functions:
	void set(const size_t i, bool val);
	void append(bool bit);
  void optimize();
};

};

#endif
