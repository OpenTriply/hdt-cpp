#!/usr/bin/env bash

# Go to script directory
pushd "$(dirname "$0")" >/dev/null


# Globals
average=0
outputdir=.

function showhelp {
        echo
        echo "Usage: $0 [OPTIONS] -f FILE"
        echo
        echo "	-h, --help	display the help and exit"
        echo
        echo "	-a, --average	run for all FILEs in parallel"
        echo
	echo "	-f, --file	input log file"
	echo
	echo "	-o, --output	path to the directory where the output files will be stored"
	echo
}

# Parse command line
while [[ $# -gt 0 ]]; do

key="$1"

case $key in
        -h|--help)      showhelp
                        exit 0
                        ;;
        -a|--average)   average=1
                        shift
        		;;
	-f|--file)	file=$2
			shift
			shift
			;;
	-o|--output)	outputdir=$2
			shift
			shift
			;;
esac

done

# Go to run directory
popd >/dev/null

