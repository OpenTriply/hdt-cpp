#include <iostream>
#include <HDTManager.hpp>
#include <sys/time.h>
#include <sys/resource.h>
#include <chrono>

using namespace std;
using namespace hdt;
using namespace std::chrono;

int main(int argc, char *argv[]) {

    struct rusage usage;
    double utime_start, utime_end, diff_utime;
    double stime_start, stime_end, diff_stime;

    string inFile = argv[1];

    // Get start execution time
    auto start1 = steady_clock::now();

    // Get start resource usage
    getrusage (RUSAGE_SELF, &usage);
    struct rusage start_usage = usage;

    // Load HDT file
    HDT *hdt = HDTManager::mapIndexedHDT(inFile.c_str());

    // Get end resource usage
    getrusage (RUSAGE_SELF, &usage);
    struct rusage end_usage = usage;

    // Get end execution time
    auto end1 = steady_clock::now();

    // Calculate intervals
    auto diff_exec1 = duration_cast<seconds>(end1-start1);

    utime_start = start_usage.ru_utime.tv_sec;
    utime_end = end_usage.ru_utime.tv_sec;
    diff_utime = utime_end - utime_start;

    stime_start = start_usage.ru_stime.tv_sec;
    stime_end = end_usage.ru_stime.tv_sec;
    diff_stime = stime_end - stime_start;

    cout << "Time(seconds): HDT loading before search" << endl;
    cout << "Exec\tUser\tSys\n" << diff_exec1.count() << "\t" << diff_utime << "\t" << diff_stime << endl;

    delete hdt; // Remember to delete instance when no longer needed!
}