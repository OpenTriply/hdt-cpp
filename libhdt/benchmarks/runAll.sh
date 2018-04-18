#!/usr/bin/env bash

# Set new current working directory
pushd "$(dirname "$0")" >/dev/null

# Timestamp (appended to log files)
current_time=$(date "+%Y.%m.%d-%H.%M.%S")

# Create log dir if it does not exist
mkdir -p log

  ####################################
 # log file names and their headers #
####################################
# 1. Error log
logError=log/error.log
t_logError=$logError.$current_time

# 2. HDT/Index generation
logHDTGenTime=log/HDTgeneration_time.log
t_logHDTGenTime=$logHDTGenTime.$current_time
headers_logHDTGenTime="Filename\tTripleNum\tHDTGen_exec\tHDTGen_utime\tHDTGen_stime\tIndexGen_exec\tIndexGen_utime\tIndexGen_stime"

# Create stderr log file
touch $t_logError
touch $t_logHDTGenTime

# Set headers
echo -e $headers_logHDTGenTime >> $t_logHDTGenTime

# Directory of rdf files
data=data/

./clearAll.sh
echo 'Getting files in '$data' directory'$':'

for filename in ${data%%/}/*; do
    echo
    echo "Running tests for file: "$filename

      ###############################
     # Generation of HDT and Index #
    ###############################
    echo "1. Generating HDT and Index:"

    output="${filename%.*}"".hdt"
    echo "Output of HDT generation: "$filename >> $t_logError
    out=`src/HDTgeneration $filename $output 2>> $t_logError`
    echo >> $t_logError

    # Store HDT generation time usage to an array
    arr_usage1=($(echo "$out"|awk '/HDT generation/{nr[NR+2]}; NR in nr'))
    # Store Index generation time usage to an array
    arr_usage2=($(echo "$out"|awk '/Index generation/{nr[NR+2]}; NR in nr'))
    # Get number of lines
    info=($(echo `../tools/searchHeader $output "? <http://rdfs.org/ns/void#triples> ?"`))
    lines=${info[2]}
    lines="${lines%\"}"
    lines="${lines#\"}"
    # Create tab-separated entry
    row=$filename"\t"$lines"\t"${arr_usage1[0]}"\t"${arr_usage1[1]}"\t"${arr_usage1[2]}"\t"${arr_usage2[0]}"\t"${arr_usage2[1]}"\t"${arr_usage2[2]}
    # Append to log file
    echo -e $row >> $t_logHDTGenTime
    echo "Done"
    ./clearAll.sh
done


popd >/dev/null
