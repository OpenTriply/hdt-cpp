#!/usr/bin/env bash

set -e

# Defaults and globals
datadir="`realpath $(dirname "$0")`"/data
logfiles=()
outputstream=/dev/stdout
parallel=0
remove=0
rdf_files=()

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
        -d|--datadir)   datadir=$(realpath $2)
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
        -q|--quiet)     outputstream=/dev/null
                        shift
                        ;;
        -r|--remove)	remove=1
                        shift
                        ;;
        *)	        	if ! [ -f $key ]; then
                            echo "ERROR: File '"$key"' does not exist."
                            exit 0
                        fi
                        rdf_files+=($key)
                        shift
                        ;;
    esac
done

if [ ${#rdf_files[@]} -eq 0 ]; then
        echo "ERROR: No RDF files given as argument. Cannot run benchmark."
        exit 1;
fi

# Create result directories
mkdir -p $datadir/rdf2hdt_logs
mkdir -p $datadir/rdf2hdt_output


pid=()
time_pid=()
i=0

for f in ${rdf_files[@]}; do
	output=$datadir/rdf2hdt_output/$(basename "${f%.*}".hdt)
	echo -n "- Running: rdf2hdt "$f" "$output > ${outputstream}
	log=$datadir/rdf2hdt_logs/$(basename "${f%.*}".$i.log)
	logfiles+=($log)
	if [[ $parallel -eq 0 ]]; then
		(\time -v "$(dirname "$0")"/../tools/rdf2hdt $f $output) 2> "${log}"
		echo > ${outputstream}
	else
		(\time -v "$(dirname "$0")"/../tools/rdf2hdt $f $output) 2> "${log}" &
	    time_pid[$i]=$!
	    str="$(pstree -p ${time_pid[$i]}  | grep '([0-9]\+)' | grep -o '[0-9]\+')"
        ptree=( $str )

        if [[ ${#ptree[@]} -eq 0 || ${#ptree[@]} -eq 1 ]]; then
            echo "...Done" > ${outputstream}
            unset 'time_pid[${#time_pid[@]}-1]'
            echo "- This rdf2hdt process finished too fast to wait" > ${outputstream}
        else
            for p in ${ptree[@]}; do
                if [[ ${time_pid[$i]} -ne $p ]]; then
                    pid[$i]=$p
                fi
            done
            echo "with PID: "${pid[$i]} > ${outputstream}

            let i=$i+1
        fi

        echo " with pid: "${pid[$i]} > ${outputstream}
	fi
done

# Wait for all hdtCat calls in this level to be finished
echo "- Waiting for all rdf2hdt processes to finish. (PID list: "${pid[@]}")" > ${outputstream}
while [ $parallel -eq 1 ]; do
    i=0
    wait=0

    while [ $i -le $((${#pid[@]}-1)) ]; do
        if kill -0 ${pid[$i]} > /dev/null 2>&1; then
            wait=1
            break
        fi
        let i=$i+1
    done

    if [[ wait -eq 1 ]]; then
        sleep 1
    else
        break
    fi
done


#Parse log files
logdir=$datadir/rdf2hdt_logs
csvfilename=rdf2hdt.$(date +"%d.%m.%Y-%H.%M.%S").csv

if [[ "$output" = "/dev/null" ]]; then
    "`realpath $(dirname "$0")`"/parse_time_log.sh ${logfiles[@]} -o $logdir -n $csvfilename -q
else
    "`realpath $(dirname "$0")`"/parse_time_log.sh ${logfiles[@]} -o $logdir -n $csvfilename
fi

if [[ $remove -eq 1 ]]; then
	rm -rf $datadir/rdf2hdt_output >/dev/null 2>&1
fi

# Go to run directory
popd >/dev/null 2>&1

echo > ${outputstream}
echo "-- rdf2hdt benchmarking successfully finished --" > ${outputstream}
echo > ${outputstream}
