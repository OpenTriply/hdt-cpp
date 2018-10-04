#include "SizeTArrayDisk.hpp"

#include <inttypes.h>
#include <fcntl.h>	// open

#ifndef WIN32
#include <unistd.h>
#endif

#include <sys/stat.h>	// stat
#include <sys/types.h>

#include <iostream>
#include <string>
#include <sstream>

#ifndef WIN32
#include <sys/mman.h> // For mmap
#endif

using namespace std;

namespace hdt {

SizeTArrayDisk::SizeTArrayDisk(const char *location, size_t numElements) :
    array(NULL),
    location(location),
    numElements(numElements),
    mappedSize(numElements* sizeof(size_t))
{

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#else // Linux and OSX

    if(mappedSize == 0) return;

    // Open file
    fd = open(location, O_RDWR | O_CREAT | O_TRUNC, (mode_t) 0600);
    if(fd <= 0) {
        throw std::runtime_error("Error creating memory mapped file");
    }

    // Stretch file
    if(lseek(fd, mappedSize-1, SEEK_SET) == -1){
        throw std::runtime_error("Error in lseek");
    }

    // Update file size by writing something at the end (i.e. a '\0' char)
    if(write(fd, "", 1) == -1) {
        throw std::runtime_error("Error writing last byte of the file");
    }

    // map
    this->mapFile();

#endif
}

SizeTArrayDisk::~SizeTArrayDisk() {
    if(mappedSize == 0 ) return;
    this->unmapFile();
    close(fd);
}

void SizeTArrayDisk::mapFile() {
    cout << "Mapping file: " << location << endl;
    if(array != NULL) {
        return;
    }
    array = static_cast<size_t*>( mmap(NULL, mappedSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0) );
    if(array == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }
}

void SizeTArrayDisk::unmapFile() {
    cout << "Unmapping file: " << location << endl;
    if(array != NULL && array != MAP_FAILED) {
        if(munmap(array, mappedSize) == -1) {
            throw std::runtime_error("munmap failed");
        }
    }
    array = NULL;
    mappedSize = 0;
}

size_t SizeTArrayDisk::get(size_t index) {
    if(mappedSize == 0 ) return 0;
    return array[index];
}

void SizeTArrayDisk::set(size_t index, size_t val) {
    if(mappedSize == 0 ) return;
    array[index] = val;
}

size_t SizeTArrayDisk::getNumberOfElements() {
    return numElements;
}

size_t SizeTArrayDisk::getMappedSize() {
    return mappedSize;
}

}