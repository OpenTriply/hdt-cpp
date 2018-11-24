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
	echo "	-d, --dir DIRPATH	set a directory path relative to HOME, that contains lubm, data and lubm-batch directories"
	echo
	echo "	-g, --generate		generate lubm files"
	echo
	echo "	-h, --help		display the help and exit"
	echo
	echo "	-p,			run the parallel experiment (if not specified then runs the sequential experiment)"
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
	echo
}


function run_rdf2hdt {
	echo "> Running rdf2hdt for all files in parallel"
	datadir=$1
	shift
	./bench_rdf2hdt.sh $@ -p -d $datadir
	echo ">rdf2hdt finished"
	echo
}

function run_hdtCat {
	echo "> Running hdtCat for all files"
	datadir=$1
	parallel=$2
	shift
	shift
	if [[ parallel -eq 1 ]]; then 
		./bench_hdtCat.sh $@ -d $datadir -p
	else
		./bench_hdtCat.sh $@ -d $datadir
	fi
	echo "> hdtCat finished"
	echo
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

lubmfiles=()
hdtfiles=()

if [[ $parallel -eq 0 ]]; then
	echo "> Starting sequential benchmarking."

	mkdir -p "$lubm_data_linear"

	lubmfiles+=(
                $lubm_data_linear"/lubm.1-2.ntriples"
                $lubm_data_linear"/lubm.3-4.ntriples"
                $lubm_data_linear"/lubm.5-6.ntriples"
                $lubm_data_linear"/lubm.7-8.ntriples"
                $lubm_data_linear"/lubm.9-10.ntriples"
                $lubm_data_linear"/lubm.11-12.ntriples"
                $lubm_data_linear"/lubm.13-14.ntriples"
                $lubm_data_linear"/lubm.15-16.ntriples"
                $lubm_data_linear"/lubm.17-24.ntriples"
                $lubm_data_linear"/lubm.25-32.ntriples"
                $lubm_data_linear"/lubm.33-40.ntriples"
                $lubm_data_linear"/lubm.41-48.ntriples"
                $lubm_data_linear"/lubm.49-56.ntriples"
                $lubm_data_linear"/lubm.57-64.ntriples"
                $lubm_data_linear"/lubm.65-72.ntriples"
                $lubm_data_linear"/lubm.73-80.ntriples"
        )
	_lubmfiles+=(
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

	hdtfiles+=(
                $lubm_data_linear"/rdf2hdt_output/lubm.1-2.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.3-4.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.5-6.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.7-8.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.9-10.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.11-12.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.13-14.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.15-16.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.17-24.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.25-32.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.33-40.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.41-48.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.49-56.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.57-64.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.65-72.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.73-80.hdt"
        )

	_hdtfiles+=(
                $lubm_data_linear"/rdf2hdt_output/lubm.1-1000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.1001-2000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.2001-3000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.3001-4000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.4001-5000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.5001-6000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.6001-7000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.7001-8000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.8001-12000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.12001-16000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.16001-20000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.20001-24000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.24001-28000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.28001-32000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.32001-36000.hdt"
                $lubm_data_linear"/rdf2hdt_output/lubm.36001-40000.hdt"
        )


	if [[ $lubm_generation -eq 1 ]]; then
		echo "> Generating LUBM files."
		generate_lubm 2 8 1 $lubmdir $lubm_data_linear
		generate_lubm 8 8 17 $lubmdir $lubm_data_linear
	fi

	error=0
        # Check whether LUBM files exist
        for i in "${lubmfiles[@]}"; do
                if ! [ -f $i ]; then
                        echo "File '"$i"' has not been generated."
               		error=1
		 fi
        done

	if [[ $error -eq 1 ]]; then
                exit 0
        fi

	# Run rdf2hdt
	run_rdf2hdt $lubm_data_linear "${lubmfiles[@]}"

	# Check whether HDT files exist
	for i in "${hdtfiles[@]}"; do
                if ! [ -f $i ]; then
                        echo "File '"$i"' has not been generated."
			error=1
                fi
        done

	if [[ $error -eq 1 ]]; then
		exit 0
	fi

	# Run hdtCat linearly
	run_hdtCat $lubm_data_linear 0 "${hdtfiles[@]}"
else
	echo "> Starting parallel benchmarking."

	mkdir -p "$lubm_data_parallel"

	lubmfiles+=(
        	$lubm_data_parallel"/lubm.1-2.ntriples"
        	$lubm_data_parallel"/lubm.3-4.ntriples"
        	$lubm_data_parallel"/lubm.5-6.ntriples"
        	$lubm_data_parallel"/lubm.7-8.ntriples"
        	$lubm_data_parallel"/lubm.9-10.ntriples"
       		$lubm_data_parallel"/lubm.11-12.ntriples"
        	$lubm_data_parallel"/lubm.13-14.ntriples"
        	$lubm_data_parallel"/lubm.15-16.ntriples"
        	$lubm_data_parallel"/lubm.17-24.ntriples"
        	$lubm_data_parallel"/lubm.25-32.ntriples"
        	$lubm_data_parallel"/lubm.33-40.ntriples"
        	$lubm_data_parallel"/lubm.41-48.ntriples"
        	$lubm_data_parallel"/lubm.49-56.ntriples"
        	$lubm_data_parallel"/lubm.57-64.ntriples"
        	$lubm_data_parallel"/lubm.65-72.ntriples"
        	$lubm_data_parallel"/lubm.73-80.ntriples"
	)

	hdtfiles+=(
        	$lubm_data_parallel"/rdf2hdt_output/lubm.1-2.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.3-4.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.5-6.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.7-8.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.9-10.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.11-12.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.13-14.hdt"
       	 	$lubm_data_parallel"/rdf2hdt_output/lubm.15-16.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.17-24.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.25-32.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.33-40.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.41-48.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.49-56.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.57-64.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.65-72.hdt"
        	$lubm_data_parallel"/rdf2hdt_output/lubm.73-80.hdt"
	)


	_lubmfiles+=(
                $lubm_data_parallel"/lubm.1-2.ntriples"
                $lubm_data_parallel"/lubm.3-4.ntriples"
                $lubm_data_parallel"/lubm.5-6.ntriples"
                $lubm_data_parallel"/lubm.7-8.ntriples"
                $lubm_data_parallel"/lubm.9-10.ntriples"
                $lubm_data_parallel"/lubm.11-12.ntriples"
                $lubm_data_parallel"/lubm.13-14.ntriples"
                $lubm_data_parallel"/lubm.15-16.ntriples"
                $lubm_data_parallel"/lubm.17-18.ntriples"
                $lubm_data_parallel"/lubm.19-20.ntriples"
                $lubm_data_parallel"/lubm.21-22.ntriples"
                $lubm_data_parallel"/lubm.23-24.ntriples"
                $lubm_data_parallel"/lubm.25-26.ntriples"
        )

	_hdtfiles+=(
                $lubm_data_parallel"/rdf2hdt_output/lubm.1-2.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.3-4.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.5-6.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.7-8.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.9-10.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.11-12.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.13-14.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.15-16.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.17-18.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.19-20.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.21-22.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.23-24.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.25-26.hdt"
        )

	_lubmfiles+=(
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

	_hdtfiles+=(
                $lubm_data_parallel"/rdf2hdt_output/lubm.1-1000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.1001-2000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.2001-3000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.3001-4000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.4001-5000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.5001-6000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.6001-7000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.7001-8000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.8001-9000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.9001-10000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.10001-11000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.11001-12000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.12001-13000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.13001-14000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.14001-15000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.15001-16000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.16001-17000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.17001-18000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.18001-19000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.19001-20000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.20001-21000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.21001-22000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.22001-23000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.23001-24000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.24001-25000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.25001-26000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.26001-27000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.27001-28000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.28001-29000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.29001-30000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.30001-31000.hdt"
                $lubm_data_parallel"/rdf2hdt_output/lubm.31001-32000.hdt"
        )

	if [[ $lubm_generation -eq 1 ]]; then
                echo "> Generating LUBM files."
#		generate_lubm 2 13 1 $lubmdir $lubm_data_parallel
		generate_lubm 2 8 1 $lubmdir $lubm_data_parallel
                generate_lubm 8 8 17 $lubmdir $lubm_data_parallel
        fi

	error=0
        # Check whether LUBM files exist
        for i in "${lubmfiles[@]}"; do
        	if ! [ -f $i ]; then
                	echo "File '"$i"' has not been generated."
                       error=1
               fi
	done

	if [[ $error -eq 1 ]]; then
                exit 0
        fi

	# Run rdf2hdt
        run_rdf2hdt $lubm_data_parallel "${lubmfiles[@]}"

	# Check whether hdt files exist
        for i in "${hdtfiles[@]}"; do
               if ! [ -f $i ]; then
                       echo "File '"$i"' has not been generated."
                       error=1
               fi
        done

	if [[ $error -eq 1 ]]; then
		exit 0
	fi

	# Run hdtCat
	run_hdtCat $lubm_data_parallel 1 "${hdtfiles[@]}"
fi

# Go to run directory
popd >/dev/null
