#include <iostream>
#include <HDTManager.hpp>
#include "../../libhdt/src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;


int main(int argc, char *argv[]) {

    string inputFile1 = argv[1];
    string inputFile2 = argv[2];
    string outputFile = argv[3];

    string baseUri="<file://"+outputFile+">";

    HDTSpecification spec;

    try {
        if(inputFile1=="" || inputFile2=="") {
            cerr << "ERROR: You must supply two HDT input files" << endl << endl;
            return 1;
        }

        if(outputFile=="") {
            cerr << "ERROR: You must supply an HDT output file name" << endl << endl;
            return 1;
        }

        // HDTCat
        StopWatch globalTimer;
        HDT *hdt = HDTManager::catHDT(inputFile1.c_str(), inputFile2.c_str(), baseUri.c_str(), spec);
        hdt->saveToHDT(outputFile.c_str());

        globalTimer.stop();
        cout << "HDT Successfully generated." << endl;
        cout << "Total processing time: ";
        cout << "Clock(" << globalTimer.getRealStr();
        cout << ")  User(" << globalTimer.getUserStr();
        cout << ")  System(" << globalTimer.getSystemStr() << ")" << endl;

        // Index generation
        StopWatch indexTimer;
        hdt = HDTManager::indexedHDT(hdt);

        indexTimer.stop();
        cout << "HDT Successfully indexed." << endl;
        cout << "Total processing time: ";
        cout << "Clock(" << indexTimer.getRealStr();
        cout << ")  User(" << indexTimer.getUserStr();
        cout << ")  System(" << indexTimer.getSystemStr() << ")" << endl;

        delete hdt;

    } catch (std::exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }
}
