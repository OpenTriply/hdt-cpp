#!/usr/bin/env bash

# Go to script directory
pushd "$(dirname "$0")" >/dev/null

# Initialize globals
hdt_files=()
parallel=0
logfiles=()
datadir="./data"
count=0
step=1

hdtCatRecursive()
{

	# Termination condition
	if [[ $# -eq 1 ]]; then
		step=1
		echo "--All parallel hdtCat runs successfully finished--"
		echo
		return
	fi

	echo "Recursion level: "$step

	max=$#
	log_files=()

	# Next call argument list
	next=()

	# If even arguments, keep the first in the next call argument list
	if [[ $((max%2)) -eq 1 ]]; then
		next+=($1)
		((max-=1))
		shift
	fi

	# Run hdtCat on every two files
        for iter in $(seq 1 $((max/2))); do

		# Create log files
		log=$datadir/hdtCat_logs/out.$step.$iter.log
                logfiles+=($log)

		# Define hdtCat arguments
		left_file=$1
                right_file=$2
		output_file=$datadir/hdtCat_output/out.$step.$iter.hdt

		# Add output file to the next call argument list
		next+=("$output_file")

		# Execute hdtCat in the background
		echo -n "- Running hdtCat "$left_file" "$right_file" "$output_file
                (\time -v ../tools/hdtCat $left_file $right_file $output_file >/dev/null) 2> "${log}" &

		# Save pid
		pid[$iter]=$!
                echo " with pid: "${pid[$iter]}

		# Move to next input pair of input files
		shift
		shift
	done

	# Wait for all hdtCat calls in this level to be finished
	for i in $(seq 1 $((max/2)));
        do
                echo "Waiting for hdtCat (PID ${pid[$i]}) to finish."
                wait ${pid[i]}

		#TODO: parse logs
                #./parse_time_log.sh -f $datadir/hdtCat_logs/${logfiles[$i]} -o $datadir/hdtCat_logs
        done

	echo
	echo "--hdtCat runs finished for step: "$step"--"
	echo

	# Recursion step
	((step+=1))
	hdtCatRecursive ${next[@]}

}

showhelp()
{
        echo
        echo "Usage: $0 [OPTIONS] FILE..."
        echo "Run hdtCat with given FILEs iteratively and save performance statistics."
        echo
        echo "  -d, --datadir DIR       specify a directory DIR where the output will be saved"
        echo
        echo "  -h, --help              display the help and exit"
        echo
        echo "  -p,                     run hdtCat in parallel and not iteratively"
        echo
}

# Parse command line
while [[ $# -gt 0 ]]; do

key="$1"

case $key in
        -d|--datadir)   datadir=$2
                        if ! [ -d $datadir ]; then
                                echo "ERROR: given data directory ('"$datadir"') does not exist"
                                exit 0
                        fi
                        shift
                        shift
                        ;;
        -h|--help)      showhelp
                        exit 0
                        ;;
        -p|--parallel)  parallel=1
                        shift
                        ;;
        *)              if ! [ -f $key ]; then
                                echo "ERROR: File '"$key"' does not exist."
                                exit 0
                        fi
                        hdt_files+=($key)
			((count+=1))
                        shift
                        ;;
esac

done

if [[ $count -lt 2 ]]; then
	echo "ERROR: At least 2 files should be given as input."
	exit 0
fi

mkdir -p $datadir/hdtCat_output
mkdir -p $datadir/hdtCat_logs

# Apply hdtCat sequentially
if  [[ parallel -eq 0 ]]; then
	echo "hdtCat will run sequentially"
	max=$((${#hdt_files[@]}-1))
	left_file=${hdt_files[0]}

        for iter in $(seq 1 $max); do
                # Create log files
                log=$datadir/hdtCat_logs/out.s.$iter.log

		# Define hdtCat arguments
		right_file=${hdt_files[$iter]}
                output_file=$datadir/hdtCat_output/out$iter.hdt

		echo "- Running hdtCat "$left_file" "$right_file" "$output_file
                (\time -v ../tools/hdtCat $left_file $right_file $output_file >/dev/null) 2> "${log}"

		# Set next left input file
		left_file=$output_file

		# Parse log
		#./parse_time_logs -f $datadir/hdtCat_logs/${log[$i]} -o $datadir/hdtCat_logs
        done
	echo
	echo "--All sequential hdtCat runs finished successfully--"
	echo
# Apply hdtCat in parallel
else
	total_steps=0
	temp=${#hdt_files[@]}
	while [[ temp -gt 0 ]]; do
		((total_steps+=1))
		((temp/=2))
	done

	echo "hdtCat will run in parallel in "$total_steps" levels."
	hdtCatRecursive ${hdt_files[@]}
fi


# Go to run directory
popd >/dev/null
