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
//// File Handler
//#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
//    HANDLE fd,h;
//#else
//    int fd;
//#endif

    size_t *array;
    const char * location;
    size_t numElements;
    size_t mappedSize;
    int fd;

    void mapFile();
    void unmapFile();

public:
    ///
    /// @param location The absolute path of temporary directory
    /// @param size
    SizeTArrayDisk(const char *location, size_t size);
    ~SizeTArrayDisk();

    ///
    /// @param index
    /// @return
    size_t get(size_t index);

    ///
    /// @param index
    /// @param val
    void set(size_t index, size_t val);

    ///
    /// \return
    size_t getNumberOfElements();

    /// Returns mapped size in bytes.
    /// @return
    size_t getMappedSize();
};

}

#endif //SIZETARRAYDISK_HPP_
