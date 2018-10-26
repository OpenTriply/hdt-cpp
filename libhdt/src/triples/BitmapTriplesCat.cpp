#include <unistd.h>
#include "BitmapTriplesCat.hpp"

namespace hdt {

BitmapTriplesCat::BitmapTriplesCat(const char *location) : location(location)
{
    vectorY = nullptr;
    vectorZ = nullptr;
    bitY = nullptr;
    bitZ = nullptr;
}

BitmapTriplesCat::~BitmapTriplesCat()
{
    delete vectorY;
    vectorY = nullptr;

    delete vectorZ;
    vectorZ = nullptr;

    delete bitY;
    bitY = nullptr;

    delete bitZ;
    bitZ = nullptr;
}

void BitmapTriplesCat::cat(IteratorTripleID *it, ProgressListener* listener)
{
    string triplesFileName = string(location) + "triples";
    string fileNameY = string(location) + "vectorY";
    string fileNameZ = string(location) + "vectorZ";

    ofstream outFinal;
    ControlInformation *ci = nullptr;
    std::exception *err = nullptr;

    try {
        size_t number = it->estimatedNumResults();

        vectorY = new LogSequence2Disk(fileNameY.c_str(), bits(number), number);

        vectorZ = new LogSequence2Disk(fileNameZ.c_str(), bits(number), number);

        bitY = new BitSequence375();
        bitZ = new BitSequence375();

        size_t lastX = 0, lastY = 0, lastZ = 0;
        size_t x, y, z;

        size_t numTriples = 0;


        while (it->hasNext()) {
            TripleID *tripleID = it->next();

            swapComponentOrder(tripleID, SPO, SPO);

            x = tripleID->getSubject();
            y = tripleID->getPredicate();
            z = tripleID->getObject();

            if (x == 0 || y == 0 || z == 0) {
                cerr << "ERROR: Triple with at least one component zero." << endl;
                continue;
            }

            if (numTriples == 0) {
                // First triple
                vectorY->push_back(y);
                vectorZ->push_back(z);
            } else if (x != lastX) {
                if (x != lastX + 1) {
                    throw std::runtime_error("Error: The subjects must be correlative.");
                }
                bitY->append(true);
                vectorY->push_back(y);

                bitZ->append(true);
                vectorZ->push_back(z);
            } else if (y != lastY) {
                if (y < lastY) {
                    throw std::runtime_error("Error: The predicates must be in increasing order.");
                }

                // Y changed
                bitY->append(false);
                vectorY->push_back(y);

                bitZ->append(true);
                vectorZ->push_back(z);
            } else {
                if (z <= lastZ) {
                    throw std::runtime_error("Error, The objects must be in increasing order.");
                }

                // Z changed
                bitZ->append(false);
                vectorZ->push_back(z);
            }

            lastX = x;
            lastY = y;
            lastZ = z;

            NOTIFYCOND(listener, "Converting to BitmapTriples", numTriples, number);
            numTriples++;
        }

        if(numTriples > 0) {
            bitY->append(true);
            bitZ->append(true);
        }

        vectorY->reduceBits();
        vectorZ->trimToSize();

        outFinal.open(triplesFileName, ios::binary | ios::out | ios::trunc);
        if(!outFinal.good()) {
            throw std::runtime_error("Error opening file to save the triples.");
        }

        // Write Control Information
        ci = new ControlInformation();
        ci->setFormat(getType());
        ci->setUint("order", SPO);
        ci->save(outFinal);
        //TODO: check if more C.I. needs to be written

        // Write bitmap triples arrays.
        IntermediateListener iListener(listener);
        iListener.setRange(0,5);
        iListener.notifyProgress(0, "BitmapTriplesCat saving Bitmap Y");
        bitY->save(outFinal);

        iListener.setRange(5,15);
        iListener.notifyProgress(0, "BitmapTriplesCat saving Bitmap Z");
        bitZ->save(outFinal);

        iListener.setRange(15,30);
        iListener.notifyProgress(0, "BitmapTriplesCat saving Stream Y");
        vectorY->save(outFinal);

        iListener.setRange(30,100);
        iListener.notifyProgress(0, "BitmapTriplesCat saving Stream Z");
        vectorZ->save(outFinal);

    } catch(std::exception& e) {
        err = &e;
        cout << "ERROR: " << e.what() << endl;
    }

    // Clean-up
    if(outFinal.is_open()) {
        outFinal.close();
    }
    delete ci;
    unlink(fileNameY.c_str());
    unlink(fileNameZ.c_str());
    if (err) throw *err;
}

BitmapTriplesIteratorCat::BitmapTriplesIteratorCat(Triples* triplesHDT1, Triples* triplesHDT2, FourSectionDictionaryCat* dictCat)
    : count(1)
{
    this->triplesHDT1 = triplesHDT1;
    this->triplesHDT2 = triplesHDT2;
    this->dictionaryCat = dictCat;

    // Get triples with subject ID = 1
    arrayOfTriples = getTripleID(static_cast<size_t >(1));
    // Sort
    sort(SPO);
    // Remove duplicates
    removeDuplicates();
    // Set iterator
    triplesIterator = arrayOfTriples.begin();
    // Increase subject ID counter
    count++;
}

BitmapTriplesIteratorCat::~BitmapTriplesIteratorCat() = default;

bool BitmapTriplesIteratorCat::hasNext()
{
    return count < dictionaryCat->getMappingS()->getSize() || triplesIterator != arrayOfTriples.end();
}

TripleID* BitmapTriplesIteratorCat::next()
{
    TripleID* ret;
    if (triplesIterator == arrayOfTriples.end()) {
        arrayOfTriples.clear();
        // Get next set of of triples with subject ID assigned to count.
        arrayOfTriples = getTripleID(count);
        // Sort
        sort(SPO);
        // Remove Duplicates
        removeDuplicates();
        // Set iterator
        triplesIterator = arrayOfTriples.begin();
        // Increase subject ID counter.
        count++;
    }
    // Dereference and reference to cast iterator type into TripleID pointer.
    ret = &*triplesIterator;
    // Move to next position.
    triplesIterator++;

    return ret;
}

vector<TripleID> BitmapTriplesIteratorCat::getTripleID(size_t count)
{
    vector<TripleID> triples;
    vector<size_t> mapping = dictionaryCat->getMappingS()->getMapping(count);
    vector<CatMappingBackType> mappingType = dictionaryCat->getMappingS()->getType(count);

    IteratorTripleID* it = nullptr;
    for (size_t i = 0; i < mapping.size(); i++) {
        if (mappingType[i]==1||mappingType[i]==2){
            bool isFirst = mappingType[i] == 1;
            TripleID pattern(mapping[i], static_cast<size_t>(0), static_cast<size_t>(0));

            it = (isFirst?triplesHDT1:triplesHDT2)->search(pattern);
            while (it->hasNext()) {
                TripleID* tid = mapTriple(it->next(), static_cast<size_t>(isFirst?1:2));
                triples.push_back(*tid);
                delete tid;
            }
            delete it;
        }
    }
    return triples;
}

TripleID* BitmapTriplesIteratorCat::mapTriple(TripleID* tripleID, size_t num)
{
    if (num == 1) {
        size_t new_subject1 = mapIdSection(tripleID->getSubject(), dictionaryCat->getMappingSh1(), dictionaryCat->getMappingS1());
        size_t new_predicate1 = mapIdPredicate(tripleID->getPredicate(), dictionaryCat->getMappingP1());
        size_t new_object1 = mapIdSection(tripleID->getObject(), dictionaryCat->getMappingSh1(), dictionaryCat->getMappingO1());
        return new TripleID(new_subject1, new_predicate1, new_object1);
    }
    else {
        size_t new_subject2 = mapIdSection(tripleID->getSubject(), dictionaryCat->getMappingSh2(), dictionaryCat->getMappingS2());
        size_t new_predicate2 = mapIdPredicate(tripleID->getPredicate(), dictionaryCat->getMappingP2());
        size_t new_object2 = mapIdSection(tripleID->getObject(), dictionaryCat->getMappingSh2(), dictionaryCat->getMappingO2());
        return new TripleID(new_subject2, new_predicate2, new_object2);
    }
}

size_t BitmapTriplesIteratorCat::mapIdSection(size_t id, CatMapping* catMappingShared, CatMapping* catMapping)
{
    if (id <= catMappingShared->getSize()) {
        return catMappingShared->getMapping(id - 1);
    }
    else if (catMapping->getType(id - catMappingShared->getSize() - 1) == 1) {
        return catMapping->getMapping(id - catMappingShared->getSize() - 1);
    }
    else {
        return catMapping->getMapping(id - catMappingShared->getSize() - 1) + dictionaryCat->getNumSared();
    }
}

size_t BitmapTriplesIteratorCat::mapIdPredicate(size_t id, CatMapping* catMapping)
{
    return catMapping->getMapping(id - 1);
}

}
