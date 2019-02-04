#!/usr/bin/env bash

# Defaults and globals
count=0
datadir="`realpath $(dirname "$0")`"/data
hdt_files=()
logfiles=()
parallel=0
output=/dev/stdout
step=1
csvfilename=hdtCat.$(date +"%d.%m.%Y-%H.%M.%S").csv

hdtCatRecursive()
{
    # Termination condition
    if [[ $# -eq 1 ]]; then
        step=1
        echo "--All parallel hdtCat runs successfully finished--" > ${output}
        echo > ${output}
        return
    fi

    echo "- Recursion level: "$step > ${output}

    # Total hdtCat inputs in this step
    max=$#

    # Next call argument list
    next=()

    # If even arguments, keep the first in the next call argument list
    if [[ $((max%2)) -eq 1 ]]; then
        next+=($1)
        ((max-=1))
        shift
    fi

    pid=()
    time_pid=()
    el=0

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
        echo  -n "- Running hdtCat "$left_file" "$right_file" "$output_file > ${output}
        (\time -v "$(dirname "$0")"/../tools/hdtCat $left_file $right_file $output_file >/dev/null) 2> "${log}" &

        # Save pid
        time_pid[$el]=$!
        str="$(pstree -p ${time_pid[$el]}  | grep '([0-9]\+)' | grep -o '[0-9]\+')"
        ptree=( $str )

        if [[ ${#ptree[@]} -eq 0 || ${#ptree[@]} -eq 1 ]]; then
            echo "...Done" > ${output}
            unset 'time_pid[${#time_pid[@]}-1]'
            echo "- This hdtCat process finished too fast to wait" > ${output}
        else
            for p in ${ptree[@]}; do
                if [[ ${time_pid[$el]} -ne $p ]]; then
                    pid[$el]=$p
                fi
            done
            echo "with PID: "${pid[$el]} > ${output}

            let el=$el+1
        fi

        # Move to next input pair of input files
        shift
        shift
    done

    # Wait for all hdtCat calls in this level to be finished
    echo "- Waiting for all hdtCat processes to finish. (PID list: "${pid[@]}")" > ${output}
    while true; do

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

	echo > ${output}
	echo "--hdtCat runs finished for step: "$step"--" > ${output}
	echo > ${output}

	# Recursion step
	((step+=1))
	hdtCatRecursive ${next[@]}

}

showhelp()
{
    echo
    echo "Usage: "$(basename $0)" [OPTIONS] FILE..."
    echo "Run hdtCat with given FILEs iteratively and save performance statistics."
    echo
    echo "  -d, --datadir DIR       DIR directory where the output logs will be saved"
    echo
    echo "  -h, --help              display the help and exit"
    echo
    echo "  -p,                     run hdtCat processes in parallel and not sequantially"
    echo
    echo "  -q, --quiet             quiet/suppress stdout"
    echo
}

# Parse command line
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -d|--datadir)   datadir=$(realpath $2)
                        if ! [ -d $datadir ]; then
                        echo "ERROR: output directory ('`echo $(realpath "$datadir")`') does not exist"
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

        -q|--quiet)     output=/dev/null
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
    echo "hdtCat will run sequentially" > ${output}
    echo > ${output}

    max=$((${#hdt_files[@]}-1))
    left_file=${hdt_files[0]}

    for iter in $(seq 1 $max); do
        # Create log files
        log=$datadir/hdtCat_logs/out.$iter.log
        logfiles+=($log)

        # Define hdtCat arguments
        right_file=${hdt_files[$iter]}
        output_file=$datadir/hdtCat_output/out.$iter.hdt

        echo "- Running hdtCat "$left_file" "$right_file" "$output_file > ${output}
        (\time -v "$(dirname "$0")"/../tools/hdtCat $left_file $right_file $output_file >/dev/null) 2> "${log}"

        # Set next left input file
        left_file=$output_file
    done
    echo > ${output}
    echo "--All sequential hdtCat runs finished successfully--" > ${output}
    echo > ${output}
# Apply hdtCat in parallel
else
    total_steps=0
    for (( y=${#hdt_files[@]}-1 ; $y > 0; y >>= 1 )); do
        let total_steps=$total_steps+1
    done

    echo "hdtCat will run in parallel in "$total_steps" levels." > ${output}
    echo > ${output}
    hdtCatRecursive ${hdt_files[@]}
fi

# Parse log files
logdir=$datadir/hdtCat_logs

if [[ "$output" = "/dev/null" ]]; then
    "`realpath $(dirname "$0")`"/parse_time_log.sh ${logfiles[@]} -o $logdir -n $csvfilename -q
else
    "`realpath $(dirname "$0")`"/parse_time_log.sh ${logfiles[@]} -o $logdir -n $csvfilename
fi