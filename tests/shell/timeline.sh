#!/bin/bash
usage() { echo "Usage: $(basename $0) [-h] | -f filepath [-i 1s]" 1>&2; exit 1; }
[ $# -eq 0 ] && echo "No arguments provided" && usage

while getopts ":hf:i:" o; do
    case "${o}" in
        f)
            FILE=${OPTARG}
            [[ ! -f $FILE ]] && echo "file [$FILE] doesn\'t exist" && usage
            ;;
        i)
            INTERVAL=${OPTARG}
            [[ $INTERVAL -le 0 ]] && echo "interval should be greater 0" && usage
            ;;
        h | *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))
INTERVAL=${INTERVAL:-1}

# sed "e sleep $INTERVAL" $FILE
while read -r line
do
    echo "$line"
    sleep $INTERVAL
done < $FILE