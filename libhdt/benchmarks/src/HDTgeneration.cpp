#include <HDTManager.hpp>
#include <sys/time.h>
#include <sys/resource.h>
#include <chrono>

using namespace hdt;
using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]) {

    struct rusage usage;
    double utime_start, utime_end, diff_utime;
    double stime_start, stime_end, diff_stime;

    string inFile = argv[1];
    string outFile = argv[2];

    string baseUri="<file://"+inFile+">";

    HDTSpecification spec;

    try {

        // Get start execution time
        auto start1 = steady_clock::now();

        // Get start resource usage
        getrusage (RUSAGE_SELF, &usage);
        struct rusage start_usage = usage;

        // Read RDF into HDT file.
        HDT *hdt = HDTManager::generateHDT(inFile.c_str(), baseUri.c_str(), NTRIPLES, spec);
        // Save HDT
        hdt->saveToHDT(outFile.c_str());

        // Get end resource usage
        getrusage (RUSAGE_SELF, &usage);
        struct rusage end_usage = usage;

        // Get end execution time
        auto end1 = steady_clock::now();

        // Calculate intervals
        auto diff_exec1 = duration_cast<milliseconds>(end1-start1);

        utime_start = start_usage.ru_utime.tv_sec*1000 + start_usage.ru_utime.tv_usec/1000.0;
        utime_end = end_usage.ru_utime.tv_sec*1000 + end_usage.ru_utime.tv_usec/1000.0;
        diff_utime = utime_end - utime_start;

        stime_start = start_usage.ru_stime.tv_sec*1000 + start_usage.ru_stime.tv_usec/1000.0;
        stime_end = end_usage.ru_stime.tv_sec*1000 + end_usage.ru_stime.tv_usec/1000.0;
        diff_stime = stime_end - stime_start;


        cout << "Time(milliseconds): HDT generation" << endl;
        cout << "Exec\tUser\tSys\n" << diff_exec1.count() << "\t" << diff_utime << "\t" << diff_stime << endl;

        // Get start execution time
        auto start2 = steady_clock::now();

        // Get start resource usage
        getrusage (RUSAGE_SELF, &usage);
        start_usage = usage;

        // Index generation
        hdt = HDTManager::indexedHDT(hdt);

        // Get end resource usage
        getrusage (RUSAGE_SELF, &usage);
        end_usage = usage;

        // Get end execution time
        auto end2 = steady_clock::now();

        // Calculate intervals
        auto diff_exec2 = duration_cast<milliseconds>(end2-start2);

        utime_start = start_usage.ru_utime.tv_sec*1000 + start_usage.ru_utime.tv_usec/1000.0;
        utime_end = end_usage.ru_utime.tv_sec*1000 + end_usage.ru_utime.tv_usec/1000.0;
        diff_utime = utime_end - utime_start;

        stime_start = start_usage.ru_stime.tv_sec*1000 + start_usage.ru_stime.tv_usec/1000;
        stime_end = end_usage.ru_stime.tv_sec*1000 + end_usage.ru_stime.tv_usec/1000;

        diff_stime = stime_end - stime_start;


        cout << "Time(milliseconds): Index generation" << endl;
        cout << "Exec\tUser\tSys\n" << diff_exec2.count() << "\t" << diff_utime << "\t" << diff_stime << endl;

    } catch (std::exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }

}
