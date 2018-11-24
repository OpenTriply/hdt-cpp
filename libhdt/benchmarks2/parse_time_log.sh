#!/usr/bin/env bash

# Go to script directory
pushd "$(dirname "$0")" >/dev/null


# Globals
outputdir=./data
name=$outputdir"/log.csv"
files=()

function showhelp {
        echo
        echo "Usage: $0 [OPTIONS] FILE..."
        echo
        echo "	-h, --help		display the help and exit"
        echo
	echo "	-n, --name NAME		name for the csv file (default: log.csv)"
	echo "	-o, --output DIR	path to the directory where the csv file will be stored"
	echo
}

# Parse command line
while [[ $# -gt 0 ]]; do

key="$1"

case $key in
        -h|--help)      showhelp
                        exit 0
                        ;;
	-n|--name)	tempname=$2
			shift
			shift
			;;
	-o|--output)	output=$2
			shift
			shift
			;;
	*)		file=$1
			if ! [[ -f $file ]]; then
				echo "ERROR: File '"$file"' does not exist."
				exit 0
			fi
			files+=($file)
			shift
			;;
esac
done

if ! [ -z tempname ]; then
	name=$outputdir"/"$tempname
fi

echo
echo "Creating performance csv files"

headers = (
	"Command",
	"User Time (seconds)",
	"System Time (seconds)",
	"Wall Time (h:mm:ss or m:ss)",
	"Max RSS (kbytes)",
	"Major Page Faults",
	"Minor Page Faults",
	"Voluntary Context Switches",
	"Involuntary Context Switches",
	"File System Inputs",
	"File System Outputs"
)

# Create csv file
touch $name
for iter in $(seq 0 $((${#headers[@]}-1))); do
	echo -n $header >> $name
	if [[ iter -ne $((${#headers[@]}-1)); then
		echo -n "," >> $name
	done
done

for file in ${files[@]}; do
	echo -n "- Parsing file: "$file"..."
	
	echo "Done"
done

echo
echo "--CSV files generated successfully--"
echo

# Go to run directory
popd >/dev/null

