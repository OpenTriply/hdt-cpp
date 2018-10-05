#ifndef SIZETARRAYDISK_HPP_
#define SIZETARRAYDISK_HPP_

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#include <windows.h>
#include <winbase.h>
#endif

#include <string>
#include <stdlib.h>

using namespace std;

namespace hdt {

class SizeTArrayDisk {
private:
//TODO: Make cross-platform code.
// File Handler
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    HANDLE fd,h;
#else
    int fd;    //<! The file descriptor of the file.
#endif

    size_t *array;  //<! The start address of the memory mapped file.
    const char * location;  //<! The absolute path of the memory mapped file.
    size_t numElements; //<! The maximum number of elements of the array.
    size_t mappedSize;  //<! The mapped size in bytes.

    //! Calls mmap
    void mapFile();
    //! Calls munmap
    void unmapFile();

public:
    //! Creates a read/write memory mapped file that holds an array of size_t's.
    //! \param location the absolute path of the file.
    //! \param numEntries the size of the array.
    SizeTArrayDisk(const char *location, size_t numEntries);
    ~SizeTArrayDisk();

    //! Returns the value stored in the given array index.
    //! \param index an existing index of the array.
    //! \return the value stored in the given index.
    size_t get(size_t index);

    //! Sets a value in the given array index.
    //! \param index an existing index of the array.
    //! \param val a value to be stored in the array.
    void set(size_t index, size_t val);

    //! Returns the maximum number of the elements that can be stored in the array.
    //! \return attribute numElements.
    size_t getNumberOfElements();

    //! Returns the mapped size in bytes.
    //! \return mapped size in bytes.
    size_t getMappedSize();

    //! Resizes the memory mapping according to a new array size.
    //! \param newNumElements the new size of the array.
    void resize(size_t newNumElements);
};

}

#endif //SIZETARRAYDISK_HPP_
