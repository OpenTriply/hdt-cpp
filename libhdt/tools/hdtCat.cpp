#include <HDTVersion.hpp>
#include <HDT.hpp>
#include <HDTManager.hpp>
#include <string>
#include <getopt.h>
#include <iostream>
#include <fstream>

#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

void help() {
    cout << "$ hdCat [options] <HDT input file 1> <HDT input file 2> <HDT output file>" << endl;
    cout << "\t-h\t\t\tThis help" << endl;
    cout << "\t-i\t\t\tAlso generate index to solve all triple patterns." << endl;
    cout << "\t-c\t<configfile>\tHDT Config options file" << endl;
    cout << "\t-o\t<options>\tHDT Additional options (option1=value1;option2=value2;...)" << endl;
    cout << "\t-V\tPrints the HDT version number." << endl;
    cout << "\t-p\tPrints a progress indicator." << endl;
    cout << "\t-v\tVerbose output" << endl;
}

int main(int argc, char **argv) {
    string inputFile1;
    string inputFile2;
    string outputFile;
    bool verbose=false;
    bool showProgress=false;
    bool generateIndex=false;
    string configFile;
    string options;
    int flag;

    while ((flag = getopt (argc, argv, "c:o:vpiVh")) != -1)
    {
        switch(flag)
        {
            case 'c':
                configFile = optarg;
                break;
            case 'o':
                options = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            case 'p':
                showProgress = true;
                break;
            case 'i':
                generateIndex=true;
                break;
            case 'V':
                cout << HDTVersion::get_version_string(".") << endl;
                return 0;
            case 'h':
                help();
                return 0;
            default:
                cerr << "ERROR: Unknown option" << endl;
                help();
                return 1;
        }
    }

#define vout if (!verbose) {} else std::cerr /* Verbose output */

    if (!configFile.empty()) {
        vout << "Configfile: " << configFile << endl;
    }
    if (!options.empty()) {
        vout << "Options: " << options << endl;
    }
    if(argc-optind<3) {
        cerr << "ERROR: You must supply two input files and one output file" << endl << endl;
        help();
        return 1;
    }

    inputFile1 = argv[optind];
    inputFile2 = argv[optind+1];
    outputFile = argv[optind+2];

    if(inputFile1=="" || inputFile2=="") {
        cerr << "ERROR: You must supply two HDT input files" << endl << endl;
        help();
        return 1;
    }

    if(outputFile=="") {
        cerr << "ERROR: You must supply an HDT output file name" << endl << endl;
        help();
        return 1;
    }

    // Process
    HDTSpecification spec(configFile);

    spec.setOptions(options);

    try {
        StopWatch globalTimer;
        ProgressListener* progress = showProgress ? new StdoutProgressListener() : NULL;
        HDT *hdt = HDTManager::catHDT(inputFile1.c_str(), inputFile2.c_str(), spec, progress);

//        if(outputFile!="-") {
//            RDFSerializer *serializer = RDFSerializer::getSerializer(outputFile.c_str(), notation);
//            hdt->saveToRDF(*serializer);
//            delete serializer;
//        } else {
//            RDFSerializer *serializer = RDFSerializer::getSerializer(cout, notation);
//            hdt->saveToRDF(*serializer);
//            delete serializer;
//        }
//        delete hdt;

        globalTimer.stop();
        vout << "HDT Successfully generated." << endl;
        vout << "Total processing time: ";
        vout << "Clock(" << globalTimer.getRealStr();
        vout << ")  User(" << globalTimer.getUserStr();
        vout << ")  System(" << globalTimer.getSystemStr() << ")" << endl;

        if(generateIndex) {
            hdt = HDTManager::indexedHDT(hdt, progress);
        }

        delete progress;
    } catch (std::exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }

}
