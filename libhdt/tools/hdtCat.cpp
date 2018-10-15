#include <HDTVersion.hpp>
#include <HDT.hpp>
#include <HDTManager.hpp>
#include <string>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

void help() {
    cout << "$ hdCat [options] <HDT input file 1> <HDT input file 2> <HDT output file>" << endl;
    cout << "\t-h\t\t\tThis help" << endl;
    cout << "\t-i\t\t\tAlso generate index to solve all triple patterns." << endl;
    cout << "\t-c\t<configfile>\tHDT Config options file" << endl;
    cout << "\t-o\t<options>\tHDT Additional options (option1=value1;option2=value2;...)" << endl;
    cout << "\t-B\t\"<base URI>\"\tBase URI of the dataset." << endl;
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
    string baseUri;
    int flag;

    // For creating a temporary dir
#ifdef PATH_MAX
    char absolutePath[PATH_MAX];
#else
    char absolutePath[4096];
#endif
    const char *theDir;
    string location;
    mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

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
            case 'B':
                baseUri = optarg;
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

    if(inputFile1.empty() || inputFile2.empty()) {
        cerr << "ERROR: You must supply two HDT input files" << endl << endl;
        help();
        return 1;
    }

    if(outputFile.empty()) {
        cerr << "ERROR: You must supply an HDT output file name" << endl << endl;
        help();
        return 1;
    }

    if(baseUri.empty()) {
        baseUri="<file://"+outputFile+">";
    }

    // Process
    HDTSpecification spec(configFile);

    spec.setOptions(options);

    // Create a temporary directory
    // TODO: Support other OS's
    realpath(outputFile.c_str(), absolutePath);
    theDir = strcat(absolutePath, "_tmp");
    mkdir(theDir, mode);
    location = string(theDir) + "/";
    try {
        StopWatch globalTimer;
        ProgressListener* progress = showProgress ? new StdoutProgressListener() : nullptr;

        HDT *hdt = HDTManager::catHDT(location.c_str(), inputFile1.c_str(), inputFile2.c_str(), baseUri.c_str(), spec, progress);

        /*
         * TODO (optional) vout << Basic stats:
        */

        hdt->saveToHDT(outputFile.c_str(), progress);

        unlink((string(location) + "dictionary").c_str());
        unlink((string(location) + "triples").c_str());
        rmdir(location.c_str());


        globalTimer.stop();
        vout << "HDT Successfully generated." << endl;
        vout << "Total processing time: ";
        vout << "Clock(" << globalTimer.getRealStr();
        vout << ")  User(" << globalTimer.getUserStr();
        vout << ")  System(" << globalTimer.getSystemStr() << ")" << endl;

        if(generateIndex) {
            StopWatch indexTimer;
            hdt = HDTManager::indexedHDT(hdt, progress);

            indexTimer.stop();
            vout << "HDT Successfully indexed." << endl;
            vout << "Total processing time: ";
            vout << "Clock(" << indexTimer.getRealStr();
            vout << ")  User(" << indexTimer.getUserStr();
            vout << ")  System(" << indexTimer.getSystemStr() << ")" << endl;
        }

        delete hdt;
        delete progress;
    } catch (std::exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }

}
