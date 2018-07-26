#!/usr/bin/env bash

# Set new current working directory
pushd "$(dirname "$0")" >/dev/null


# Delete index files (if any)
`find ./ -name '*index*' | xargs rm`

# Delete output files (if any)
`find ./ -name 'cat*.hdt' | xargs rm`

# Create log dir if it does not exist
logDir=log-hdtGen
rm -rf ${logDir}
mkdir -p ${logDir}

  ####################################
 # log file names and their headers #
####################################
# 1. Error log
logError=${logDir}/error.log
logMemory=${logDir}/memory.log

# 2. HDTCat/Index generation
resultsFile=${logDir}/main.tsv
headers_resultsFile="Input_Filename_1\tInput_Filename_2\tOutput_Filename\tTripleNum\tHDTCat_exec\tHDTCat_utime\tHDTCat_stime\tIndexGen_exec\tIndexGen_utime\tIndexGen_stime\tmaxMem_kb"

# Create log files
touch $logError
touch $resultsFile

# Set headers
echo -e $headers_resultsFile >> $resultsFile

# Directory of rdf files
data=data/

#./clearAll.sh
echo 'Getting files in '$data' directory'$':'

index=0

for directory in ${data%%/}/*; do
    i=0
    files=[]
    for file in ${directory%%/}/*; do
        files[i]=$file
        i+=1
    done

    echo "running hdtCat for ${files[0]} and ${files[1]}"


    ((index+=1))
    output="${directory}""/cat"$index".hdt"

    baseFilename=`basename $output`
    memFile=${logMemory}.${baseFilename}
    top -d 1 -b  | grep HDTCat >> ${memFile} &
    topPid=`ps aux | grep "to[p] -d 1 -b" | awk '{print $2}'`

      ####################
     # HDTCat and Index #
    ####################
    echo "1. HDTCat and Index:"

    echo "Output of merging ${files[0]} and ${files[1]}" >> $logError
    out=`src/HDTCat ${files[0]} ${files[1]} $output 2>> $logError`
    #echo "$out" |& tee -a tee $logError
    echo >> "$logError"

    # Store HDTCat generation time usage to an array
    arr_usage1=($(echo "$out"|awk '/HDT Successfully generated./{nr[NR+1]}; NR in nr'))
    # Store Index generation time usage to an array
    arr_usage2=($(echo "$out"|awk '/HDT Successfully indexed./{nr[NR+1]}; NR in nr'))

    info=($(echo `../tools/searchHeader $output "? <http://rdfs.org/ns/void#triples> ?"`))
    lines=${info[2]}
    lines="${lines%\"}"
    lines="${lines#\"}"
    kill ${topPid}
    maxMemUse=`./getMaxMemFromTop.sh ${memFile}`

    a1=`echo ${arr_usage1[*]} | grep -oP '\(\K[^\)]+'`
    a2=`echo ${arr_usage2[*]} | grep -oP '\(\K[^\)]+'`

    row="`basename ${files[0]}`""\t""`basename ${files[1]}`""\t""`basename $output`""\t""$lines""\t"

    append=`echo "$a1" | tr "\n" "\t"`
    row+="$append"

    append=`echo "$a2" | tr "\n" "\t"`
    row+="$append""$maxMemUse"

    echo -e "$row" >> "$resultsFile"
    echo "Done"
done


popd >/dev/null
