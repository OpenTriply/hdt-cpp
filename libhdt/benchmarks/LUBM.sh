#!/usr/bin/env bash

#args:
#   1.

# Check if data contains proper datasets

#rdf2hdt input1.nt
#rdf2hdt input2.nt
#hdtCat input1.hdt input2.hdt input1-2.hdt

#rdf2hdt input3.nt
#hdtCat input1-2.hdt input3.hdt input1-3.hdt


pushd "$(dirname "$0")" >/dev/null

dataDir=./data

readarray inputs < $dataDir/file_order

# Create log dir if it does not exist
rm -rf log
mkdir -p log
log1=./log/rdf2hdt.csv
touch $log1

echo 'Input,Triples,User time (sec), System time (sec), Total time (sec), Peak RSS size (kbytes)' > $log1

max="${#inputs[@]}"
((max-=1))

for i in `seq 1 $max`; do

    if [ $i == 1 ]; then
        #rdf2hdt input1.nt output1.hdt
        input1="${inputs[0]}"
        output1="${input1%.*}".hdt
        \time -v ../tools/rdf2hdt $dataDir"/"$input1 $dataDir"/"$output1 -v -p
    else
        output1=$output
    fi

    #rdf2hdt input2.nt output2.hdt
    input2="${inputs["$i"]}"
    output2="${input2%.*}".hdt
    \time -v ../tools/rdf2hdt $dataDir"/"$input2 $dataDir"/"$output2 -v -p

    #hdtCat input
    output=out$i.hdt
    \time -v ../tools/hdtCat $dataDir"/"$output1 $dataDir"/"$output2 $dataDir"/"$output

done


popd >/dev/null