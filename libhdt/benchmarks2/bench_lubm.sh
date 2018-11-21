#!/usr/bin/env bash

# Go to script directory
pushd "$(dirname "$0")" >/dev/null

# Initialize default variables
# 1. Flags
parallel=0
lubm_generation=0
# 2. Directories
maindir=${HOME}"/ssd/hdt-cpp/libhdt/benchmarks2"
lubmdir=$maindir"/lubm"
lubm_data_linear=$maindir"/data/lubm_linear"
lubm_data_parallel=$maindir"/data/lubm_parallel"

# Forced termination cleanup
trap "trap - SIGTERM && rm -rf $maindir/lubm-batch/tmp* && kill -- -$$" SIGINT SIGTERM EXIT

function showhelp {
	echo
	echo  "Usage: $0 [OPTIONS]"
	echo
	echo "	-d, --dir	set a directory path relative to HOME, that contains lubm, data and lubm-batch directories"
	echo
	echo "	-g, --generate	generate lubm files"
	echo
	echo "	-h, --help	display the help and exit"
	echo
	echo "	-p,		run the parallel experiment (if not specified then runs the linear experiment)"
	echo
}

function generate_lubm {
	univ=$1
	files=$2
	index=$3
	l=$4
	dir=$5

	$maindir/lubm-batch/lubm.sh -u $univ -n $files -i $index -t $maindir/lubm-batch --lubm $l -O $dir -p &
	wait
	echo "> LUBM files succesfully generated."
}


function run_rdf2hdt {
	echo "> Running rdf2hdt for all files in parallel"
	./bench_rdf2hdt.sh $@ -p
	echo ">rdf2hdt finished"
}

while [[ $# -gt 0 ]]; do

key="$1"

case $key in
	-d|--main-dir)	maindir=$HOME"/""$2"
			if ! [ -d $maindir ]; then
				echo "> ERROR: given directory path ($maindir) does not exist"
				exit 0
			fi
			lubmdir=$maindir"/lubm"
                        if ! [ -d $lubmdir ]; then
                                echo "> ERROR: path to lubm ($lubmdir) does not exist"
                                exit 0
                        fi
			lubm_data_linear=$maindir"/data/lubm_linear"
			lubm_data_parallel=$maindir"/data/lubm_parallel"
			shift
			shift
			;;
	-g|--generate)	lubm_generation=1
			shift
			;;
	-h|--help)	showhelp
			exit 0
			;;
	-p|--parallel)	parallel=1
			shift
			;;
esac


done

mkdir -p $maindir"/data"
mkdir -p "$lubm_data_linear"
mkdir -p "$lubm_data_parallel"

lubmfiles=()

if [[ $parallel -eq 0 ]]; then
	echo "> Starting linear benchmarking."

	lubmfiles+=(
		$lubm_data_linear"/lubm.1-1000.ntriples"
		$lubm_data_linear"/lubm.1001-2000.ntriples"
                $lubm_data_linear"/lubm.2001-3000.ntriples"
                $lubm_data_linear"/lubm.3001-4000.ntriples"
                $lubm_data_linear"/lubm.4001-5000.ntriples"
                $lubm_data_linear"/lubm.5001-6000.ntriples"
                $lubm_data_linear"/lubm.6001-7000.ntriples"
                $lubm_data_linear"/lubm.7001-8000.ntriples"
                $lubm_data_linear"/lubm.8001-12000.ntriples"
                $lubm_data_linear"/lubm.12001-16000.ntriples"
                $lubm_data_linear"/lubm.16001-20000.ntriples"
                $lubm_data_linear"/lubm.20001-24000.ntriples"
                $lubm_data_linear"/lubm.24001-28000.ntriples"
                $lubm_data_linear"/lubm.28001-32000.ntriples"
                $lubm_data_linear"/lubm.32001-36000.ntriples"
                $lubm_data_linear"/lubm.36001-40000.ntriples"
	)

	if [[ $lubm_generation -eq 1 ]]; then
		echo "> Generating LUBM files."
		generate_lubm 1000 8 1 $lubmdir $lubm_data_linear
		generate_lubm 4000 8 8100 $lubmdir $lubm_data_linear
	fi
        # Check whether LUBM files exist
        for i in "${lubmfiles[@]}"; do
                if ! [ -f $i ]; then
                        echo "File '"$i"' has not been generated."
                        exit 0
                fi
        done

	# Run rdf2hdt
	run_rdf2hdt "${lubmfiles[@]}"

else
	echo "> Starting parallel benchmarking."
	lubmfiles+=(
                $lubm_data_parallel"/lubm.1-1000.ntriples"
                $lubm_data_parallel"/lubm.1001-2000.ntriples"
                $lubm_data_parallel"/lubm.2001-3000.ntriples"
                $lubm_data_parallel"/lubm.3001-4000.ntriples"
                $lubm_data_parallel"/lubm.4001-5000.ntriples"
                $lubm_data_parallel"/lubm.5001-6000.ntriples"
                $lubm_data_parallel"/lubm.6001-7000.ntriples"
                $lubm_data_parallel"/lubm.7001-8000.ntriples"
                $lubm_data_parallel"/lubm.8001-9000.ntriples"
                $lubm_data_parallel"/lubm.9001-10000.ntriples"
                $lubm_data_parallel"/lubm.10001-11000.ntriples"
                $lubm_data_parallel"/lubm.11001-12000.ntriples"
                $lubm_data_parallel"/lubm.12001-13000.ntriples"
                $lubm_data_parallel"/lubm.13001-14000.ntriples"
                $lubm_data_parallel"/lubm.14001-15000.ntriples"
                $lubm_data_parallel"/lubm.15001-16000.ntriples"
		$lubm_data_parallel"/lubm.16001-17000.ntriples"
                $lubm_data_parallel"/lubm.17001-18000.ntriples"
                $lubm_data_parallel"/lubm.18001-19000.ntriples"
                $lubm_data_parallel"/lubm.19001-20000.ntriples"
                $lubm_data_parallel"/lubm.20001-21000.ntriples"
                $lubm_data_parallel"/lubm.21001-22000.ntriples"
                $lubm_data_parallel"/lubm.22001-23000.ntriples"
                $lubm_data_parallel"/lubm.23001-24000.ntriples"
                $lubm_data_parallel"/lubm.24001-25000.ntriples"
                $lubm_data_parallel"/lubm.25001-26000.ntriples"
                $lubm_data_parallel"/lubm.26001-27000.ntriples"
                $lubm_data_parallel"/lubm.27001-28000.ntriples"
                $lubm_data_parallel"/lubm.28001-29000.ntriples"
                $lubm_data_parallel"/lubm.29001-30000.ntriples"
                $lubm_data_parallel"/lubm.30001-31000.ntriples"
                $lubm_data_parallel"/lubm.31001-32000.ntriples"
        )

	if [[ $lubm_generation -eq 1 ]]; then
                echo "> Generating LUBM files."
		generate_lubm 2 5 1 $lubmdir $lubm_data_parallel
        fi
        # Check whether LUBM files exist
        for i in "${lubmfiles[@]}"; do
        	if ! [ -f $lubm_data_parallel/$i ]; then
                	echo "File '"$lubm_data_parallel/$i"' has not been generated."
                        exit 0
               	fi
	done

	# Run rdf2hdt
        run_rdf2hdt "${lubmfiles[@]}"
	#run_rdf2hdt $lubm_data_parallel/*
fi

# Go to run directory
popd >/dev/null
