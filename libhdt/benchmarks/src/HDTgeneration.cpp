#include <HDTManager.hpp>

using namespace hdt;
using namespace std;

int main(int argc, char *argv[]) {

    string inFile = argv[1];
    string outFile = argv[2];
    string baseUri="<file://"+inFile+">";

    HDTSpecification spec;

    try {
        // Read RDF into HDT file.
        HDT *hdt = HDTManager::generateHDT(inFile.c_str(), baseUri.c_str(), NTRIPLES, spec);
        // Save HDT
        hdt->saveToHDT(outFile.c_str());


        // Index generation
        hdt = HDTManager::indexedHDT(hdt);

    } catch (std::exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }

}
