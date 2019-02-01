#!/usr/bin/env bash

# Initialize globals
rdf_files=()
parallel=0
remove=0
logfiles=()
datadir="`realpath $(dirname "$0")`"/data

#function cleanup {
#	trap - SIGTERM
#	if [[ $remove -eq 1 ]]; then
#        	rm -rf $datadir/rdf2hdt_output
#	fi

#	kill -- -$$ >/dev/null 2>&1
#	popd >/dev/null 2>&1
#}

# Forced termination cleanup
#trap cleanup SIGINT

function showhelp {
    echo
    echo "Usage: "$(basename $0)" [OPTIONS] FILE..."
    echo "Run rdf2hdt with given FILEs and save performance statistics."
    echo
    echo "  -d, --datadir DIR   DIR directory  where the output will be saved"
    echo
    echo "  -h, --help          display the help and exit"
    echo
    echo "  -p,                 run for all FILEs in parallel"
    echo
    echo "  -q, --quiet         quiet/suppress stdout"
    echo
    echo "  -r, --remove        remove output hdt files after each rdf2hdt run"
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
	-r|--remove)	remove=1
			shift
			;;
	*)		if ! [ -f $key ]; then
				echo "ERROR: File '"$key"' does not exist."
				exit 0
			fi
			rdf_files+=($key)
			shift
			;;
esac

done
if [ ${#rdf_files[@]} -eq 0 ]; then
        echo "No RDF files given as argument. Cannot run benchmark"
        exit 1;
fi

# Create result directories
mkdir -p $datadir/rdf2hdt_logs
mkdir -p $datadir/rdf2hdt_output


i=-1
for f in ${rdf_files[@]}; do
	i=$((i+1))
	output=$datadir/rdf2hdt_output/$(basename "${f%.*}".hdt)
	echo -n "- Running: rdf2hdt "$f" "$output
	log=$datadir/rdf2hdt_logs/$(basename "${f%.*}".log)
	logfiles+=($log)
	if [[ $parallel -eq 0 ]]; then
		(\time -v ../tools/rdf2hdt $f $output) 2> "${log}" 
		echo
	else
		(\time -v ../tools/rdf2hdt $f $output) 2> "${log}" &
	        pid[$i]=$!
        	echo " with pid: "${pid[$i]}
	fi
done

# If parallel, wait for background processes
if [[ $parallel -eq 1 ]]; then
	max=$(( ${#rdf_files[@]} - 1 ))
	for i in $(seq 0 $max);
        do
                echo "Waiting for rdf2hdt upon file ${rdf_files[$i]} (PID ${pid[$i]}) to finish."
                wait ${pid[i]}

		if [[ remove -eq 1 ]]; then
			f=${rdf_files[$i]}
			output=$datadir/rdf2hdt_output/$(basename "${f%.*}".hdt)
			rm $output
		fi
	done
fi

#Parse log files
logdir=$datadir/rdf2hdt_logs
csvfilename="rdf2hdt.log"
./parse_time_log.sh ${logfiles[@]} -o $logdir -n $csvfilename

if [[ $remove -eq 1 ]]; then
	rm -rf $datadir/rdf2hdt_output >/dev/null 2>&1
fi

# Go to run directory
popd >/dev/null 2>&1

echo
echo "-- rdf2hdt benchmarking successfully finished --"
echo
