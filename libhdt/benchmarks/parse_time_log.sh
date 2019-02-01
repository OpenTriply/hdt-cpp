#!/usr/bin/env bash

# Globals
outputdir="`realpath $(dirname "$0")`"/data
name=$outputdir/log.$(date +"%d.%m.%Y-%H.%M.%S").csv
files=()
output=/dev/stdout

function showhelp {
    echo
    echo "Usage: "$(basename $0)" [OPTIONS] FILE..."
    echo
    echo "  -h, --help          display the help and exit"
    echo
	echo "  -n, --name NAME     name for the csv file (default: log.csv)"
    echo
	echo "  -o, --output DIR    path to the directory where the csv file will be stored"
    echo
    echo "  -q, --quiet         quiet/suppress stdout"
	echo
}

# Parse command line
while [[ $# -gt 0 ]]; do

key="$1"
case $key in

    -h|--help)      showhelp
                    exit 0
                    ;;
    -n|--name)      tempname=$2
                    shift
                    shift
                    ;;
    -o|--output)	outputdir=$(realpath $2)

                    if ! [ -d $outputdir ]; then
                        echo "ERROR: output directory ('`echo $(realpath "$outputdir")`') does not exist"
                        exit 0
                    fi
                    shift
                    shift
                    ;;
    -q|--quiet)     output=/dev/null
                    shift
                    ;;
	*)              file=$1
                    if ! [[ -f $file ]]; then
                        echo "ERROR: File '"$file"' does not exist."
                        exit 0
                    fi
                    files+=($file)
                    shift
                    ;;
esac
done

if [ ${#files[@]} -eq 0 ]; then
	echo "ERROR: No files given."
	exit 0;
fi

if [ -n "${tempname}" ]; then
	name=$outputdir"/"$tempname
fi

echo > ${output}
echo "- Creating performance csv files" > ${output}

headers=(
        "Command"
        "User Time (seconds)"
        "System Time (seconds)"
        "Wall Time (h:mm:ss or m:ss)"
        "Max RSS (kbytes)"
        "Major Page Faults"
        "Minor Page Faults"
        "Voluntary Context Switches"
        "Involuntary Context Switches"
        "Swaps"
        "File System Inputs"
        "File System Outputs"
)

# Create csv file
> $name
for iter in $(seq 0 $((${#headers[@]}-1))); do
        echo -n ${headers[$iter]} >> $name;
        if [[ iter -ne $((${#headers[@]}-1)) ]]; then
                echo -n "," >> $name
        fi
done
echo >> $name
for file in ${files[@]}; do
	echo -n "- Parsing file: "$file"..." > ${output}
	command=$(cat $file | sed -n -e 's/^.*Command being timed: //p')
	utime=$(cat $file | sed -n -e 's/^.*User time (seconds): //p')
	stime=$(cat $file | sed -n -e 's/^.*System time (seconds): //p')
	wtime=$(cat $file | sed -n -e 's/^.*Elapsed (wall clock) time (h:mm:ss or m:ss): //p')
	max_rss=$(cat $file | sed -n -e 's/^.*Maximum resident set size (kbytes): //p')
	major_pf=$(cat $file | sed -n -e 's/^.*Major (requiring I\/O) page faults: //p')
	minor_pf=$(cat $file | sed -n -e 's/^.*Minor (reclaiming a frame) page faults: //p')
	vol_cs=$(cat $file | sed -n -e 's/^.*Voluntary context switches: //p')
	invol_cs=$(cat $file | sed -n -e 's/^.*Involuntary context switches: //p')
	swaps=$(cat $file | sed -n -e 's/^.*Swaps: //p')
	inputs=$(cat $file | sed -n -e 's/^.*File system inputs: //p')
	outputs=$(cat $file | sed -n -e 's/^.*File system outputs: //p')
	echo $command","$utime","$stime","$wtime","$max_rss","$major_pf","$minor_pf","$vol_cs","$invol_cs","$swaps","$inputs","$outputs >> $name
	echo "Done" > ${output}
done

echo > ${output}
echo "--CSV file generated successfully at: ${name}--" > ${output}
echo > ${output}
