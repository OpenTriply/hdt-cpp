#include <unistd.h>
#include "FourSectionDictionaryCat.hpp"
#include <HDTVocabulary.hpp>
#include "../libdcs/VByte.h"
#include "../util/crc8.h"
#include "../util/crc32.h"

using namespace std;

namespace hdt {

bool sortBySec(const pair<size_t, unsigned char *> &a, const pair<size_t, unsigned char *> &b) {
    string s1 = string(reinterpret_cast<char *>(a.second));
    string s2 = string(reinterpret_cast<char *>(b.second));
    return (s1.compare(s2) < 0);
}

void FourSectionDictionaryCat::freeStr(unsigned char *s) {
    delete[] s;
    this->str = nullptr;
}

FourSectionDictionaryCat::FourSectionDictionaryCat(const char *location) : location(location)
{
    // Delete unnecessary allocated memory.
    delete subjects;
    subjects = nullptr;
    delete predicates;
    predicates = nullptr;
    delete objects;
    objects = nullptr;
    delete shared;
    shared = nullptr;

    /// Set blocksize if specified in spec file.
    blocksize = 16;
    string blockSizeStr;
    try {
        blockSizeStr = spec.get("dict.block.size");
    }
    catch (exception& e) {
    }

    if (!blockSizeStr.empty()) {
        blocksize = static_cast<uint32_t>(atoi(blockSizeStr.c_str()));
    }

    // Attributes that will be created dynamically:
    mappingS1 = nullptr;
    mappingS2 = nullptr;
    mappingP1 = nullptr;
    mappingP2 = nullptr;
    mappingO1 = nullptr;
    mappingO2 = nullptr;
    mappingSh1 = nullptr;
    mappingSh2 = nullptr;
    mappingS = nullptr;

    str = nullptr;

    blocks = nullptr;

    it1temp = nullptr;
    it2temp = nullptr;

    itCommonSubjects1Objects2 = nullptr;
    itCommonObjects1Subjects2 = nullptr;
    itCommonShared1Subjects2 = nullptr;
    itCommonShared1Objects2 = nullptr;
    itCommonShared2Subjects1 = nullptr;
    itCommonShared2Objects1 = nullptr;
}

FourSectionDictionaryCat::~FourSectionDictionaryCat() {

    delete mappingS1;
    delete mappingS2;
    delete mappingP1;
    delete mappingP2;
    delete mappingO1;
    delete mappingO2;
    delete mappingSh1;
    delete mappingSh2;
    delete mappingS;

    delete str;

    delete blocks;

    delete it1temp;
    delete it2temp;

    delete itCommonSubjects1Objects2;
    delete itCommonObjects1Subjects2;
    delete itCommonShared1Subjects2;
    delete itCommonShared1Objects2;
    delete itCommonShared2Subjects1;
    delete itCommonShared2Objects1;
}

void FourSectionDictionaryCat::cat(Dictionary* dict1, Dictionary* dict2, ProgressListener* listener)
{
    cout << "PREDICATES-------------------" << endl;
    // Construct predicate mappings.
    mappingP1 = new CatMapping(location, "P1", dict1->getNpredicates());
    mappingP2 = new CatMapping(location, "P2", dict2->getNpredicates());

    // Calculate the total number of output predicates.
    CatCommon* commonP1P2 = new CatCommon(dict1->getPredicates(), dict2->getPredicates());
    size_t numCommonPredicates = 0;
    while(commonP1P2->hasNext()) {
        commonP1P2->next();
        numCommonPredicates++;
    }
    delete commonP1P2;

    size_t numPredicates = dict1->getNpredicates() + dict2->getNpredicates() - numCommonPredicates;

    // Create predicate section.
    catSection(numPredicates, CAT_PREDICATES, dict1->getPredicates(), dict2->getPredicates(),
               new CatIterator(), new CatIterator(), mappingP1, mappingP2, listener);


    cout << "SUBJECTS---------------------" << endl;
    // Get subject number, excluding shared
    size_t sizeS1 = dict1->getNsubjects() - dict1->getNshared();
    size_t sizeS2 = dict2->getNsubjects() - dict2->getNshared();

    // Construct subject mappings.
    mappingS1 = new CatMapping(location, "S1", sizeS1);
    mappingS2 = new CatMapping(location, "S2", sizeS2);

    // CASE 1: Find common terms between subjects of dict1 and objects/shared of dict2.
    CatIterator *commonSubject1 = new CatIterator(new CatCommon(dict1->getSubjects(), dict2->getShared()),
                                                  new CatCommon(dict1->getSubjects(), dict2->getObjects()));

    size_t numCommonSubject1Hdt2 = 0;
    while(commonSubject1->hasNext()){
        commonSubject1->next();
        numCommonSubject1Hdt2++;
    }
    delete commonSubject1;

    // CASE 2: Find commond terms between subjects of dict2 and objects/shared of dict1.
    CatIterator* commonSubject2 = new CatIterator(new CatCommon(dict2->getSubjects(), dict1->getShared()),
                                                  new CatCommon(dict2->getSubjects(), dict1->getObjects()));

    size_t numCommonSubject2Hdt1 = 0;
    while(commonSubject2->hasNext()){
        commonSubject2->next();
        numCommonSubject2Hdt1++;
    }
    delete commonSubject2;

    // CASE 3: Find common terms between subjects of dict1 and subjects of dict2
    CatCommon* commonS1S2 = new CatCommon(dict1->getSubjects(), dict2->getSubjects());
    size_t numCommonSubjects = 0;
    while(commonS1S2->hasNext()) {
        commonS1S2->next();
        numCommonSubjects++;
    }
    delete commonS1S2;

    // Calculate the total number of output subjects.
    size_t numSubjects = sizeS1 + sizeS2 - numCommonSubjects - numCommonSubject1Hdt2 - numCommonSubject2Hdt1;

    // Create subject section.
    catSection(numSubjects, CAT_SUBJECTS, dict1->getSubjects(), dict2->getSubjects(),
               new CatIterator(new CatCommon(dict1->getSubjects(), dict2->getShared()),
                               new CatCommon(dict1->getSubjects(), dict2->getObjects())),
               new CatIterator(new CatCommon(dict2->getSubjects(), dict1->getShared()),
                               new CatCommon(dict2->getSubjects(), dict1->getObjects())),
               mappingS1, mappingS2, listener);

    cout << "OBJECTS----------------------" << endl;
    // Get object number, excluding shared.
    size_t sizeO1 = dict1->getNobjects() - dict1->getNshared();
    size_t sizeO2 = dict2->getNobjects() - dict2->getNshared();

    // Construct object mappings.
    mappingO1 = new CatMapping(location, "O1", sizeO1);
    mappingO2 = new CatMapping(location, "O2", sizeO2);

    // CASE 1: Find common terms between objects of dict1 and subjects/shared of dict2
    CatIterator* commonObject1 = new CatIterator(new CatCommon(dict1->getObjects(), dict2->getShared()),
                                                 new CatCommon(dict1->getObjects(), dict2->getSubjects()));
    size_t numCommonObject1Hdt2 = 0;
    while(commonObject1->hasNext()) {
        commonObject1->next();
        numCommonObject1Hdt2++;
    }
    delete commonObject1;

    // CASE 2: Find common terms between objects of dict2 and subjects/shared of dict1.
    CatIterator* commonObject2 = new CatIterator(new CatCommon(dict2->getObjects(), dict1->getShared()),
                                                 new CatCommon(dict2->getObjects(), dict1->getSubjects()));

    size_t numCommonObject2Hdt1 = 0;
    while(commonObject2->hasNext()) {
        commonObject2->next();
        numCommonObject2Hdt1++;
    }
    delete commonObject2;

    // CASE 3: Find common terms between objects of dict1 and objects of dict2
    CatCommon* commonO1O2 = new CatCommon(dict1->getObjects(), dict2->getObjects());
    size_t numCommonObjects = 0;
    while(commonO1O2->hasNext()) {
        commonO1O2->next();
        numCommonObjects++;
    }
    delete commonO1O2;

    // Calculate the total number of output objects.
    size_t numObjects = sizeO1 + sizeO2 - numCommonObjects - numCommonObject1Hdt2 - numCommonObject2Hdt1;

    // Create object section.
    catSection(numObjects, CAT_OBJECTS, dict1->getObjects(), dict2->getObjects(),
               new CatIterator(new CatCommon(dict1->getObjects(), dict2->getShared()),
                               new CatCommon(dict1->getObjects(), dict2->getSubjects())),
               new CatIterator(new CatCommon(dict2->getObjects(), dict1->getShared()),
                               new CatCommon(dict2->getObjects(), dict1->getSubjects())),
               mappingO1, mappingO2, listener);

    cout << "SHARED-----------------------" << endl;

    // Find common between subjects of dict1 and objects of dict2.
    CatCommon* common = new CatCommon(dict1->getSubjects(), dict2->getObjects());
    size_t numCommonS1O2 = 0;
    while(common->hasNext()) {
        common->next();
        numCommonS1O2++;
    }
    delete common;

    // Find common between objects of dict1 and subjects of dict2.
    common = new CatCommon(dict1->getObjects(), dict2->getSubjects());
    size_t numCommonO1S2 = 0;
    while(common->hasNext()) {
        common->next();
        numCommonO1S2++;
    }
    delete common;

    // Find common terms between shared of dict1 and shared of dict2.
    common = new CatCommon(dict1->getShared(), dict2->getShared());
    size_t numCommonSh1Sh2 = 0;
    while(common->hasNext()) {
        common->next();
        numCommonSh1Sh2++;
    }
    delete common;

    // Calculate the total number of output shared terms.
    numShared = dict1->getNshared() + dict2->getNshared() - numCommonSh1Sh2 + numCommonS1O2 + numCommonO1S2;

    // Create the shared section.
    catShared(numShared, dict1, dict2, listener);

    // Putting the sections together
    string dictFileName = string(location) + "dictionary";
    ofstream outFinal;
    string inputFileName;
    ifstream in;
    ControlInformation *ci = nullptr;
    std::exception* err = nullptr;
    try {

        outFinal.open(dictFileName, ios::binary | ios::out | ios::trunc);
        if(!outFinal.good()) {
            throw std::runtime_error("Error opening file to save dictionary.");
        }

        ci = new ControlInformation();

        //TODO: add more control information (?)
        ci->setFormat(HDTVocabulary::DICTIONARY_TYPE_FOUR);
        ci->setUint("mapping", MAPPING2);
//        ci->setUint("sizeStrings", ???);
        ci->save(outFinal);

        const size_t buf_size = 100000;
        vector<char> buffer(buf_size+1, 0);

        for (auto &j : {1,2,4,3}) {

            inputFileName = string(location) + "section" + to_string(j);
            in.open(inputFileName, ios::binary | ios::in);
            if(!in.good()) {
                throw std::runtime_error("Error opening file to read section.");
            }
            while(!in.eof()) {
                in.read(buffer.data(), buf_size);
                streamsize s = ((in) ? buf_size : in.gcount());

                buffer[s] = 0;
                outFinal.write(buffer.data(), s);
            }
            in.close();
            unlink(inputFileName.c_str());
        }

    } catch (std::exception& e) {
        err=&e;
    }

    // Clean-up
    delete ci;
    if(in.is_open()) {
        in.close();
        unlink(inputFileName.c_str());
    }

    if(outFinal.is_open())
        outFinal.close();

    if (err) throw *err;

    /// Store inverse mappings.
    mappingS = new CatMappingBack(location, numSubjects+numShared);
    for (size_t i = 0; i < mappingSh1->getSize(); i++) {
        mappingS->set(mappingSh1->getMapping(i), i + 1, CAT_SUBJ1);
    }

    for (size_t i = 0; i < mappingSh2->getSize(); i++) {
        mappingS->set(mappingSh2->getMapping(i), i + 1, CAT_SUBJ2);
    }

    for (size_t i = 0; i < mappingS1->getSize(); i++) {
        if (mappingS1->getType(i) == CAT_SHARED) {
            mappingS->set(mappingS1->getMapping(i), (i + 1 + (size_t)dict1->getNshared()), CAT_SUBJ1);
        }
        else {
            mappingS->set(mappingS1->getMapping(i) + numShared, (i + 1 + (size_t)dict1->getNshared()), CAT_SUBJ1);
        }
    }

    for (size_t i = 0; i < mappingS2->getSize(); i++) {
        if (mappingS2->getType(i) == CAT_SHARED) {
            mappingS->set(mappingS2->getMapping(i), (i + 1 + (size_t)dict2->getNshared()), CAT_SUBJ2);
        }
        else {
            mappingS->set(mappingS2->getMapping(i) + numShared, (i + 1 + (size_t)dict2->getNshared()), CAT_SUBJ2);
        }
    }
}

void FourSectionDictionaryCat::catSection(size_t numentries, CatMappingType type, IteratorUCharString *it1,
                                          IteratorUCharString *it2, hdt::CatIterator *it1common,
                                          hdt::CatIterator *it2common, hdt::CatMapping *mappingHdt1,
                                          hdt::CatMapping *mappingHdt2, ProgressListener *listener)
{
    size_t count1 = 0, count2 = 0;
    ssize_t skipSection1 = -1, skipSection2 = -1;   //!< terms to be skipped.
    string s1, s2;  //!< used for comparison.

    size_t numBlocks = 0; //!< number of pfc buckets
    size_t numElements = 0; //!< number of strings in the buckets
    uint64_t bytes = 0; //!< total bytes of the buckets sequence
    size_t numbits = sizeof(size_t)==8 ? 37 : 32; //!< numbits of pfc bucket

    if(str != nullptr) freeStr(str);
    size_t currentLength = 0;
    string previousStr;

    // Temporary file keeping the section,
    string fileName = string(location) + "section" + to_string(type);
    ofstream out;

    // Temporary file keeping the section buffer.
    string fileName_2 = string(location) + "section_buffer_" + to_string(type);
    ofstream out_2;

    ifstream in; //!< Input stream for writing from buffer file to section.

    uint64_t reservedSize = 1024 * blocksize * sizeof(char); //!< Section buffer reserved size.
    vector<unsigned char> section_buffer(reservedSize, 0);
    size_t written_bytes=0;

    // Vector of two pairs (id and string of term) to be compared.
    vector <pair<size_t, unsigned char *>> list;

    std::exception* err = nullptr;

    try {
        seqloc = string(location) + "LogSequence2Disk" + to_string(type); //<! Filename for blocks.
        blocks = new LogSequence2Disk(seqloc.c_str(), static_cast<unsigned int>(numbits), numentries / blocksize);

        // Open buffer file for writing.
        out_2.open(fileName_2, ios::binary | ios::out | ios::trunc);
        if (!out_2.good()) {
            throw std::runtime_error("Failed to open section buffer.");
        }

        if (numentries > 0) {
            if (it1->hasNext()) {
                list.emplace_back((size_t) 1, it1->next());
            }
            if (it2->hasNext()) {
                list.emplace_back((size_t) 2, it2->next());
            }
            if (it1common->hasNext()) {
                skipSection1 = it1common->next();
            }
            if (it2common->hasNext()) {
                skipSection2 = it2common->next();
            }

            while (!list.empty()) {
                // TODO: Listener notify here
                // Sort the two strings.
                sort(list.begin(), list.end(), sortBySec);


                bool isFirst = list[0].first == 1;
                ssize_t *skipSection = (isFirst) ? &skipSection1 : &skipSection2;
                size_t *count = (isFirst) ? &count1 : &count2;
                IteratorUCharString * it = (isFirst) ? it1 : it2;
                CatIterator *itCommon = (isFirst) ? it1common : it2common;

                if (*count == *skipSection) {

                    if (it->hasNext()) {
                        delete[] list[0].second;
                        list[0].second = it->next();
                        (*count)++;
                    } else {
                        delete[] list[0].second;
                        list.erase(list.begin());
                    }
                    if (itCommon->hasNext()) {
                        (*skipSection) = itCommon->next();
                    }
                    continue;
                }

                delete[] str;

                str = list[0].second;
                currentLength = strlen((char *) str);

                // Flush buffer to file if expected new buffer length exceeds the reserved size.
                if((bytes-written_bytes+currentLength+11) > reservedSize) {
                    out_2.write(reinterpret_cast<char *>(section_buffer.data()), bytes-written_bytes);
                    fill(section_buffer.begin(), section_buffer.end(), 0);
                    written_bytes+=(bytes-written_bytes);

                    // Check if string can fit the whole buffer
                    if (currentLength + 11 > reservedSize){
                        section_buffer.resize(currentLength+11);
                    } else if (section_buffer.size() > reservedSize){
                        section_buffer.resize(reservedSize);
                    }
                }

                if((numElements % blocksize) == 0) {
                    blocks->push_back(bytes);
                    numBlocks++;
                    // The string is explicitly copied to the encoded sequence.
                    strncpy((char*)(section_buffer.data()+bytes-written_bytes), (char*)str, currentLength);
                    bytes+=currentLength;
                }
                else {
                    // Regular string

                    // Calculate the length of the common prefix
                    size_t delta = 0;
                    auto *prev = (unsigned char *)previousStr.c_str();
                    size_t lstr1 = previousStr.length();
                    size_t lstr2 = currentLength;
                    size_t length = lstr1 < lstr2 ? lstr1 : lstr2;

                    while( (delta<length) && (str[delta] == prev[delta])) {
                        delta++;
                    }

                    // The prefix is differentially encoded
                    bytes += csd::VByte::encode(section_buffer.data()+bytes-written_bytes, delta);

                    // The suffix is copied to the sequence
                    strncpy((char*)(section_buffer.data()+bytes-written_bytes), (char*)str+delta, currentLength-delta);
                    bytes+=currentLength-delta;
                }

                // Add terminator of string.
                section_buffer.push_back('\0');
                bytes++;

                // Save previous
                previousStr.assign((char *)str);

                if(list.size() >= 2) {
                    s1 = string(reinterpret_cast<char *>(list[0].second));
                    s2 = string(reinterpret_cast<char *>(list[1].second));
                }
                if (list.size() >= 2 && s1 == s2) {
                    mappingHdt1->set(count1++, numElements + 1, type);
                    mappingHdt2->set(count2++, numElements + 1, type);

                    freeStr((list[0].second));
                    if (it1->hasNext()) {
                        list[0] = make_pair(1, it1->next());
                        delete[] list[1].second;
                        if (it2->hasNext()) {
                            list[1] = make_pair(2, it2->next());
                        } else {
                            list.erase(list.begin() + 1);
                        }
                    } else {
                        list.erase(list.begin());
                        freeStr((list[0].second));
                        if (it2->hasNext()) {
                            list[0] = make_pair(2, it2->next());
                        } else {
                            list.erase(list.begin());
                        }
                    }
                } else {
                    CatMapping *mappingHdt = isFirst ? mappingHdt1 : mappingHdt2;
                    mappingHdt->set((*count)++, numElements + 1, type);

                    freeStr((list[0].second));
                    if (it->hasNext()) {
                        list[0] = make_pair(isFirst?1:2, it->next());
                    } else {
                        list.erase(list.begin());
                    }
                }

                // New string processed
                numElements++;
            }

        }

        blocks->push_back(bytes);
        out_2.write(reinterpret_cast<char *>(section_buffer.data()), bytes-written_bytes);
        out_2.close();
        blocks->reduceBits();

        out.open(fileName, ios::binary | ios::out | ios::trunc);

        if(!out.good()){
            throw std::runtime_error("Error opening file to save dictionary section.");
        }

        CRC8 crch;
        CRC32 crcd;
        unsigned char buf[27]; //!< 9 bytes per VByte (max) * 3 values.

        // Save type
        unsigned char dicttype = csd::PFC;
        crch.writeData(out, &dicttype, sizeof(dicttype));

        // Save sizes
        uint8_t pos = 0;
        pos += csd::VByte::encode(&buf[pos], numentries);
        pos += csd::VByte::encode(&buf[pos], bytes);
        pos += csd::VByte::encode(&buf[pos], blocksize);

        crch.writeData(out, buf, pos);
        crch.writeCRC(out);

        blocks->save(out);
        delete blocks;
        blocks = nullptr;

        // Write contents of buffer file in section.
        size_t bufsize = 100000;
        vector<char> buffer(bufsize+1, 0);
        in.open(fileName_2, ios::binary | ios::in);
        if (!in.good()) {
            throw std::runtime_error("Error opening file to read section.");
        }
        while (!in.eof()) {
            in.read(buffer.data(), bufsize);
            streamsize s = ((in) ? bufsize : in.gcount());
            buffer[s] = 0;
            crcd.writeData(out, reinterpret_cast<unsigned char *>(buffer.data()), static_cast<size_t>(s));
        }
        buffer.clear();

        crcd.writeCRC(out);

    } catch (std::exception& e) {
        err = &e;
    }

    // Clean up
    delete it1;
    delete it2;
    delete it1common;
    delete it2common;
    if(out.is_open()) {
        out.close();
        unlink(seqloc.c_str());
    }
    if (out_2.is_open()) {
        out_2.close();
        unlink(fileName_2.c_str());
    }
    else if (in.is_open()){
        in.close();
        unlink(fileName_2.c_str());
    }
    for (auto &i : list) {
        delete[] i.second;
    }
    if (err) throw *err;
}

void FourSectionDictionaryCat::catShared(size_t numentries, Dictionary *dict1, Dictionary *dict2,
                                         ProgressListener *listener) {
    // Construct shared mappings.
    mappingSh1 = new CatMapping(location, "SH1", dict1->getNshared());
    mappingSh2 = new CatMapping(location, "SH2", dict2->getNshared());

    size_t count1 = 0, count2 = 0;
    string s1, s2;  //!< used for comparison

    size_t numBlocks = 0; //!< number of pfc buckets
    size_t numElements = 0; //!< number of strings in the buckets
    uint64_t bytes = 0; //!< total bytes of the buckets sequence
    size_t numbits = sizeof(size_t)==8 ? 37 : 32; //!< numbits of pfc bucket

    if(str != nullptr) freeStr(this->str);
    size_t currentLength = 0;
    string previousStr;

    // Temporary file keeping the shared section,
    string fileName = string(location) + "section" + to_string(CAT_SHARED);
    ofstream out;

    // Temporary file keeping the shared section buffer.
    string fileName_2 = string(location) + "section_buffer_1";
    ofstream out_2;

    ifstream in; //!< Input stream for writing from buffer to shared section.

    uint64_t reservedSize = 1024 * blocksize * sizeof(char);
    vector<unsigned char> section_buffer(reservedSize, 0);
    size_t written_bytes=0;

    // Vector of pairs (id and string of term) to be compared / max capacity=4.
    vector <pair<size_t, unsigned char *>> list;

    std::exception* err = nullptr;

    try {

        seqloc = string(location) + "LogSequence2Disk" + to_string(CAT_SHARED); //!< Filename for blocks
        blocks = new LogSequence2Disk(seqloc.c_str(), static_cast<unsigned int>(numbits), numentries / blocksize);

        out_2.open(fileName_2, ios::binary | ios::out | ios::trunc);
        if (!out_2.good()) {
            throw std::runtime_error("Failed to open section buffer.");
        }

        if(numentries > 0) {
            // Itarators of shared terms.
            it1temp = dict1->getShared();
            it2temp = dict2->getShared();

            // Common terms iterators.
            itCommonSubjects1Objects2 = new CatCommon(dict1->getSubjects(), dict2->getObjects());
            itCommonObjects1Subjects2 = new CatCommon(dict1->getObjects(), dict2->getSubjects());
            itCommonShared1Subjects2 = new CatCommon(dict1->getShared(), dict2->getSubjects());
            itCommonShared1Objects2 = new CatCommon(dict1->getShared(), dict2->getObjects());
            itCommonShared2Subjects1 = new CatCommon(dict2->getShared(), dict1->getSubjects());
            itCommonShared2Objects1 = new CatCommon(dict2->getShared(), dict1->getObjects());

            // Used to store next common terms between different sections.
            pair<size_t, size_t> commonShared1Subjects2 = make_pair((size_t) -1, (size_t) -1);
            pair<size_t, size_t> commonShared1Objects2 = make_pair((size_t) -1, (size_t) -1);
            pair<size_t, size_t> commonShared2Subjects1 = make_pair((size_t) -1, (size_t) -1);
            pair<size_t, size_t> commonShared2Objects1 = make_pair((size_t) -1, (size_t) -1);
            pair<size_t, size_t> idS1O2;
            pair<size_t, size_t> idO1S2;

            if(itCommonShared1Subjects2->hasNext()) {
                commonShared1Subjects2 = itCommonShared1Subjects2->next();
            }
            if(itCommonShared1Objects2->hasNext()) {
                commonShared1Objects2 = itCommonShared1Objects2->next();
            }
            if(itCommonShared2Subjects1->hasNext()) {
                commonShared2Subjects1 = itCommonShared2Subjects1->next();
            }
            if(itCommonShared2Objects1->hasNext()) {
                commonShared2Objects1 = itCommonShared2Objects1->next();
            }

            if (it1temp->hasNext()) {
                list.emplace_back((size_t) 1, it1temp->next());
            }
            if (it2temp->hasNext()) {
                list.emplace_back((size_t) 2, it2temp->next());
            }
            if(itCommonSubjects1Objects2->hasNext()) {
                idS1O2 = itCommonSubjects1Objects2->next();
                list.emplace_back((size_t)3, (unsigned char *) dict1->idToString(idS1O2.first, SUBJECT).c_str());
            }
            if(itCommonObjects1Subjects2->hasNext()) {
                idO1S2 = itCommonObjects1Subjects2->next();
                list.emplace_back((size_t)4, (unsigned char *) dict1->idToString(idO1S2.second, OBJECT).c_str());
            }

            while(!list.empty()) {
                // Sort list of id-string pairs.
                sort(list.begin(), list.end(), sortBySec);

                delete[] str;
                str = list[0].second;
                currentLength = strlen((char *) str);

                // Flush buffer to file if expected new buffer length exceeds the reserved size.
                if((bytes-written_bytes+currentLength+11) > reservedSize) {
                    out_2.write(reinterpret_cast<char *>(section_buffer.data()), bytes-written_bytes);
                    fill(section_buffer.begin(), section_buffer.end(), 0);
                    written_bytes+=(bytes-written_bytes);

                    // Check if string is not bigger than buffer size.
                    if (currentLength + 11 > reservedSize){
                        section_buffer.resize(currentLength+11);
                    } else if (section_buffer.size() > reservedSize){
                        section_buffer.resize(reservedSize);
                    }
                }

                if((numElements % blocksize) == 0) {
                    blocks->push_back(bytes);
                    numBlocks++;
                    // The string is explicitly copied to the encoded sequence.
                    strncpy((char*)(section_buffer.data()+bytes-written_bytes), (char*)str, currentLength);
                    bytes+=currentLength;
                }
                else {
                    // Regular string

                    // Calculate the length of the common prefix
                    size_t delta = 0;
                    auto *prev = (unsigned char *)previousStr.c_str();
                    size_t lstr1 = previousStr.length();
                    size_t lstr2 = currentLength;
                    size_t length = lstr1 < lstr2 ? lstr1 : lstr2;

                    while( (delta<length) && (str[delta] == prev[delta])) {
                        delta++;
                    }

                    // The prefix is differentially encoded
                    bytes += csd::VByte::encode(section_buffer.data()+bytes-written_bytes, delta);

                    // The suffix is copied to the sequence
                    strncpy((char*)(section_buffer.data()+bytes-written_bytes), (char*)str+delta, currentLength-delta);
                    bytes+=currentLength-delta;
                }

                // Add terminator of string.
                section_buffer.push_back('\0');
                bytes++;

                // Save previous
                previousStr.assign((char *)str);
                if(list.size() >= 2) {
                    s1 = string(reinterpret_cast<char *>(list[0].second));
                    s2 = string(reinterpret_cast<char *>(list[1].second));
                }
                if(list.size() >= 2 && s1 == s2) {
                    // This case can only happen if the iterators are from the shared section
                    mappingSh1->set(count1, numElements+1, CAT_SHARED);
                    mappingSh2->set(count2, numElements+1, CAT_SHARED);

                    if(count1 == commonShared1Subjects2.first) {
                        mappingS2->set(commonShared1Subjects2.second, numElements+1, CAT_SHARED);
                    }
                    if(count1 == commonShared1Objects2.first) {
                        mappingO2->set(commonShared1Objects2.second, numElements+1, CAT_SHARED);
                    }
                    if(count2 == commonShared2Subjects1.first) {
                        mappingS1->set(commonShared2Subjects1.second, numElements+1, CAT_SHARED);
                    }
                    if(count2 == commonShared2Objects1.first) {
                        mappingO1->set(commonShared2Objects1.second, numElements+1, CAT_SHARED);
                    }

                    bool removed = false;
                    if(it1temp->hasNext()) {
                        count1++;
                        freeStr(list[0].second);
                        list[0] = make_pair((size_t) 1, it1temp->next());

                        if(count1>commonShared1Subjects2.first && itCommonShared1Subjects2->hasNext()) {
                            commonShared1Subjects2 = itCommonShared1Subjects2->next();
                        }
                        if(count1>commonShared1Objects2.first && itCommonShared1Objects2->hasNext()) {
                            commonShared1Subjects2 = itCommonShared1Objects2->next();
                        }
                    }
                    else {
                        removed = true;
                    }
                    if (it2temp->hasNext()) {
                        delete[] list[1].second;
                        list[1] = make_pair((size_t) 2, it2temp->next());
                        count2++;
                        if (count2>commonShared2Subjects1.first && itCommonShared2Subjects1->hasNext()){
                            commonShared2Subjects1 = itCommonShared2Subjects1->next();
                        }

                        if (count2>commonShared2Objects1.first && itCommonShared2Objects1->hasNext()){
                            commonShared2Objects1 = itCommonShared2Objects1->next();
                        }
                    } else {
                        delete[] list[1].second;
                        list.erase(list.begin() + 1);
                    }
                    if (removed){
                        freeStr(list[0].second);
                        list.erase(list.begin());
                    }
                } else if(list[0].first==1) {
                    mappingSh1->set(count1, numElements+1, CAT_SHARED);

                    if (count1 == commonShared1Subjects2.first){
                        mappingS2->set(commonShared1Subjects2.second, numElements+1, CAT_SHARED);
                    }
                    if (count1 == commonShared1Objects2.first){
                        mappingO2->set(commonShared1Objects2.second, numElements+1, CAT_SHARED);
                    }
                    freeStr(list[0].second);
                    if (it1temp->hasNext()) {
                        count1++;
                        list[0] = make_pair((size_t) 1, it1temp->next());

                        if (count1>commonShared1Subjects2.first && itCommonShared1Subjects2->hasNext()){
                            commonShared1Subjects2 = itCommonShared1Subjects2->next();
                        }

                        if (count1>commonShared1Objects2.first && itCommonShared1Objects2->hasNext()){
                            commonShared1Objects2 = itCommonShared1Objects2->next();
                        }
                    } else {
                        list.erase(list.begin());
                    }
                } else if (list[0].first == 2) {
                    mappingSh2->set(count2, numElements+1, CAT_SHARED);

                    //Check if this is in common with Subjects2 or Objects2 for the mapping
                    if (count2 == commonShared2Subjects1.first){
                        mappingS1->set(commonShared2Subjects1.second, numElements+1, CAT_SHARED);
                    }
                    if (count2 == commonShared2Objects1.first){
                        mappingO1->set(commonShared2Objects1.second, numElements+1, CAT_SHARED);
                    }
                    freeStr(list[0].second);
                    if (it2temp->hasNext()) {
                        list[0] = make_pair((size_t) 2, it2temp->next());
                        count2++;

                        if (count2>commonShared2Subjects1.first && itCommonShared2Subjects1->hasNext()){
                            commonShared2Subjects1 = itCommonShared2Subjects1->next();
                        }

                        if (count2>commonShared2Objects1.first && itCommonShared2Objects1->hasNext()){
                            commonShared2Objects1 = itCommonShared2Objects1->next();
                        }
                    } else {
                        list.erase(list.begin());
                    }
                } else if (list[0].first == 3){
                    mappingS1->set(idS1O2.first, numElements+1, CAT_SHARED);
                    mappingO2->set(idS1O2.second, numElements+1, CAT_SHARED);

                    if (itCommonSubjects1Objects2->hasNext()) {
                        idS1O2 = itCommonSubjects1Objects2->next();
                        freeStr(list[0].second);
                        list[0] = make_pair((size_t) 3, (unsigned char *) dict1->idToString(idS1O2.first+1, SUBJECT).c_str());
                    } else {
                        freeStr(list[0].second);
                        list.erase(list.begin());
                    }
                } else {
                    mappingO1->set(idO1S2.first, numElements+1, CAT_SHARED);
                    mappingS2->set(idO1S2.second, numElements+1, CAT_SHARED);

                    if (itCommonObjects1Subjects2->hasNext()) {
                        idO1S2 = itCommonObjects1Subjects2->next();
                        list[0] = make_pair((size_t) 4, (unsigned char *) dict2->idToString(idO1S2.second+1, SUBJECT).c_str());
                    } else {
                        freeStr(list[0].second);
                        list.erase(list.begin());
                    }
                }
                numElements++;
            }
            // Storing the final byte position in the vector of positions
            blocks->push_back(bytes);
            out_2.write(reinterpret_cast<char *>(section_buffer.data()), bytes-written_bytes);
            out_2.close();
            blocks->reduceBits();
        }

        out.open(fileName, ios::binary | ios::out | ios::trunc);
        if(!out.good()) {
            throw std::runtime_error("Could not open file to save dictionary shared section.");
        }

        CRC8 crch;
        CRC32 crcd;
        unsigned char buf[27]; //!< 9 bytes per VByte (max) * 3 values.

        // Save type
        unsigned char dicttype = csd::PFC;
        crch.writeData(out, &dicttype, sizeof(dicttype));

        // Save sizes
        uint8_t pos = 0;
        pos += csd::VByte::encode(&buf[pos], numentries);
        pos += csd::VByte::encode(&buf[pos], bytes);
        pos += csd::VByte::encode(&buf[pos], blocksize);

        crch.writeData(out, buf, pos);
        crch.writeCRC(out);

        blocks->save(out);
        delete blocks;
        blocks = nullptr;

        // Write contents of buffer file in section.
        size_t bufsize = 100000;
        vector<char> buffer(bufsize+1, 0);
        in.open(fileName_2, ios::binary | ios::in);
        if (!in.good()) {
            throw std::runtime_error("Error opening file to read section.");
        }
        while (!in.eof()) {
            in.read(buffer.data(), bufsize);
            streamsize s = ((in) ? bufsize : in.gcount());
            buffer[s] = 0;
            crcd.writeData(out, reinterpret_cast<unsigned char *>(buffer.data()), static_cast<size_t>(s));
        }
        buffer.clear();

        crcd.writeCRC(out);

    } catch (std::exception& e) {
        err = &e;
    }

    if(out.is_open()) {
        out.close();
        unlink(seqloc.c_str());
    }
    if (out_2.is_open()) {
        out_2.close();
        unlink(fileName_2.c_str());
    }
    else if (in.is_open()){
        in.close();
        unlink(fileName_2.c_str());
    }
    for (auto &i : list) {
        delete[] i.second;
    }

    if (err){
        throw *err;
    }
}

size_t FourSectionDictionaryCat::getNumSared() {
    return this->numShared;
}

CatMapping* FourSectionDictionaryCat::getMappingSh1()
{
    return mappingSh1;
}
CatMapping* FourSectionDictionaryCat::getMappingSh2()
{
    return mappingSh2;
}
CatMapping* FourSectionDictionaryCat::getMappingS1()
{
    return mappingS1;
}
CatMapping* FourSectionDictionaryCat::getMappingS2()
{
    return mappingS2;
}
CatMapping* FourSectionDictionaryCat::getMappingO1()
{
    return mappingO1;
}
CatMapping* FourSectionDictionaryCat::getMappingO2()
{
    return mappingO2;
}
CatMapping* FourSectionDictionaryCat::getMappingP1()
{
    return mappingP1;
}
CatMapping* FourSectionDictionaryCat::getMappingP2()
{
    return mappingP2;
}
CatMappingBack* FourSectionDictionaryCat::getMappingS()
{
    return mappingS;
}

// CatMapping Methods:
CatMapping::CatMapping(const char *location, string section, size_t size)
{
    this->mappingFileName = string(location) + section;
    mapping = new SizeTArrayDisk(mappingFileName.c_str(), size);

    this->mappingTypeFileName = mappingFileName + "Types";
    mappingType = new SizeTArrayDisk(mappingTypeFileName.c_str(), size);
}

CatMapping::~CatMapping() {
    delete mapping;
    delete mappingType;
}

size_t CatMapping::getMapping(size_t index)
{
    return mapping->get(index);
}

CatMappingType CatMapping::getType(size_t index)
{
    return (CatMappingType) mappingType->get(index);
}

void CatMapping::set(size_t index, size_t val, CatMappingType type)
{
    this->mapping->set(index, val);
    this->mappingType->set(index, (size_t) type);
}

size_t CatMapping::getSize()
{
    return mapping->getNumberOfElements();
}

// CatMappingBack Methods:
CatMappingBack::CatMappingBack(const char *location, size_t size)
{
    this->size = size+1;

    this->mapping1FileName = string(location) + "mapping_back_1";
    mapping1 = new SizeTArrayDisk(mapping1FileName.c_str(), this->size);

    this->mappingType1FileName = string(location) + "mapping_back_type_1";
    mappingType1 = new SizeTArrayDisk(mappingType1FileName.c_str(), this->size);

    this->mapping2FileName = string(location) + "mapping_back_2";
    mapping2 = new SizeTArrayDisk(mapping2FileName.c_str(), this->size);

    this->mappingType2FileName = string(location) + "mapping_back_type_2";
    mappingType2 = new SizeTArrayDisk(mappingType2FileName.c_str(), this->size);
}

CatMappingBack::~CatMappingBack() {
    delete mapping1;
    delete mappingType1;
    delete mapping2;
    delete mappingType2;
}

vector<size_t> CatMappingBack::getMapping(size_t index)
{
    vector<size_t> r;
    if (mapping1->get(index) != 0)
        r.push_back(mapping1->get(index));
    if (mapping2->get(index) != 0)
        r.push_back(mapping2->get(index));
    return r;
}

vector<CatMappingBackType> CatMappingBack::getType(size_t index)
{
    vector<CatMappingBackType> r;
    if (mapping1->get(index) != 0)
        r.push_back((CatMappingBackType) mappingType1->get(index));
    if (mapping2->get(index) != 0)
        r.push_back((CatMappingBackType) mappingType2->get(index));
    return r;
}

void CatMappingBack::set(size_t index, size_t mapping, CatMappingBackType type)
{
    if (mapping1->get(index) == 0) {
        mapping1->set(index, mapping);
        mappingType1->set(index, type);
    }
    else {
        mapping2->set(index, mapping);
        mappingType2->set(index, type);
    }
}

size_t CatMappingBack::getSize()
{
    return this->size;
}

// CatCommon Methods
CatCommon::CatCommon() {
    it1 = nullptr;
    it2 = nullptr;
}

CatCommon::CatCommon(IteratorUCharString* it1, IteratorUCharString* it2) : it1(it1), it2(it2), has_next(false), count1(0), count2(0)
{
    if (it1->hasNext()) {
        list.emplace_back((size_t)1, it1->next());
    }
    if (it2->hasNext()) {
        list.emplace_back(make_pair((size_t)2, it2->next()));
    }

    helpNext();
}

CatCommon::~CatCommon()
{
    delete it1;
    delete it2;

    if(!list.empty()) {
        if(list[0].second != nullptr) {
            delete[] list[0].second;
            list[0].second = nullptr;
        }
        if(list.size() == 2 && list[1].second != nullptr) {
            delete[] list[1].second;
            list[1].second = nullptr;
        }
    }
};

bool CatCommon::hasNext()
{
    return has_next;
}

pair<size_t, size_t> CatCommon::next()
{
    pair<size_t, size_t> r = make_pair(next_t.first, next_t.second);
    has_next = false;
    helpNext();
    return r;
}

void CatCommon::helpNext()
{
    while (!list.empty()) {
        if (list.size() == 2) {
            string s1 = string(reinterpret_cast<char*>(list[0].second));
            string s2 = string(reinterpret_cast<char*>(list[1].second));
            // Sort
            if (s1.compare(s2) > 0) {
                iter_swap(list.begin(), list.begin() + 1);
            }

            // Delete first string, since it is stored in s1.
            if(list[0].second != nullptr) {
                delete[] list[0].second;
                list[0].second = nullptr;
            }

            // If pairs have common terms:
            if (s1 == s2) {
                has_next = true;
                next_t = make_pair(count1, count2);
		        // Delete second string
                if (list[1].second != nullptr) {
                    delete[] list[1].second;
                    list[1].second = nullptr;
                }

                bool remove = false;
                if (it1->hasNext()) {
                    list[0] = make_pair(1, it1->next());
                    count1++;
                }
                else {
                    list.erase(list.begin());
                    remove = true;
                }

                if (it2->hasNext()) {
                    remove ? (list[0] = make_pair(2, it2->next())) : (list[1] = make_pair(2, it2->next()));
                    count2++;
                }
                else {
                    if(list[0].second != nullptr) {
                        delete[] list[0].second;
                        list[0].second = nullptr;
                    }
                    list.erase(list.begin());
                }
                return;
            }
            else {
                bool isFirst = list[0].first == 1;
                IteratorUCharString *it = isFirst ? it1 : it2;
                size_t *count = isFirst ? &count1 : &count2;

                if (it->hasNext()) {
                    list[0] = make_pair(isFirst?1:2, it->next());
                    (*count)++;
                }
                else {
                    list.erase(list.begin());
                }
            }
        }
        else if (list.size() == 1) {
            delete[] list[0].second;
            list[0].second = nullptr;
            list.erase(list.begin());
        }
    }
}

// CatIterator Methods:
CatIterator::CatIterator()
{
    it1 = nullptr;
    it2 = nullptr;
}

CatIterator::CatIterator(CatCommon* it1, CatCommon* it2) : it1(it1), it2(it2)
{
    if (it1->hasNext()) {
        list.emplace_back(make_pair((size_t)1, it1->next()));
    }
    if (it2->hasNext()) {
        list.emplace_back(make_pair((size_t)2, it2->next()));
    }
}

CatIterator::~CatIterator()
{
    delete it1;
    delete it2;
}

bool CatIterator::hasNext()
{
    return !list.empty();
}

size_t CatIterator::next()
{
    size_t r;
    //sort
    if (list.size() > 1 && list[1].second.first < list[0].second.first) {
        iter_swap(list.begin(), list.begin()+1);
    }

    bool isFirst = list[0].first == 1;
    CatCommon *it = isFirst ? it1 : it2;

    r = list[0].second.first;

    if (it->hasNext()) {
        list[0] = make_pair(isFirst?(size_t)1:(size_t)2, it->next());
    }
    else {
        list.erase(list.begin());
    }
    return r;
}

}
