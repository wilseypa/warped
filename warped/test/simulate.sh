#!/bin/bash

# default batch arguments
DATADIR=measurements
RUNS=1
PAUSE=200

# default simulation arguments
NODES=`cat /proc/cpuinfo | grep processor | wc -l`
MODEL=pholdSim
MODEL_CONFIGURATION=phold/LargePHOLD
SIMULATION_CONFIGURATION=parallel.config
# comment out to omit this argument
SIMULATE_UNTIL=50000

# get batch arguments
TEMP=`getopt -o d:n:p: -l help -n "$0" -- "$@"`
eval set -- "$TEMP"
while true; do
  case "$1" in
    -d) DATADIR=${2%/}; shift 2;;
    -n) RUNS=$2; shift 2;;
    -p) PAUSE=$2; shift 2;;
    --help) echo "Usage: $0 -n runs -d directory -p pause_interval"; exit;;
    --) shift; break;;
  esac
done

# sanity check on batch arguments
if [ ! -d "$DATADIR" ]; then echo "error: invalid directory '$DATADIR'"; ERROR=1; fi
if ! [[ "$RUNS" =~ ^[0-9]+$ ]]; then echo "error: invalid # runs '$RUNS'"; ERROR=1; fi
if ! [[ "$PAUSE" =~ ^[0-9]+$ ]]; then echo "error: invalid pause interval '$PAUSE'"; ERROR=1; fi
if [ -n "$ERROR" ]; then exit; fi

# make sure we have the model executable and configuration files
if [ ! -x "$MODEL" ] || 
   [ ! -e "$MODEL_CONFIGURATION" ] || 
   [ ! -e "$SIMULATION_CONFIGURATION" ]
then
  echo "$0 must be run from the warped test (model) directory"
  exit
fi

# build simulation command
CMD="mpiexec.hydra -n "$NODES" -binding cpu:cores ./"$MODEL" \
  -simulate $MODEL_CONFIGURATION -configuration $SIMULATION_CONFIGURATION"
if [ -n "$SIMULATE_UNTIL" ]
then
  CMD="$CMD -simulateUntil $SIMULATE_UNTIL"
fi

for i in `seq 1 "$RUNS"`
do
  RUN=`printf "%02d" $i`
  CSVDIR="$DATADIR/$RUN"

  # build directory structure
  if [ ! -d "$CSVDIR" ]
  then
    mkdir "$CSVDIR"
  fi

  # do simulation run
  echo "beginning simulation run \#$RUN"
  $CMD

  # extract runtime and rollback info from the CSVs
  RUNTIME=0
  let "ROLLBACKS=0"
  for f in *.csv
  do
    LINE=`tail -n 1 $f`
    RUNTIME=${LINE%\,*}
    let "ROLLBACKS += ${LINE#*\,}"
  done
  echo "$RUNTIME,$ROLLBACKS" >> "$DATADIR/data.csv"

  # copy the CSVs to a dir for their run
  mv *.csv "$CSVDIR"

  # let the CPU cool down for $PAUSE seconds
  if [ "$i" -ne "$RUNS" ]; then
    echo "waiting $PAUSE seconds before next simulation run..."
    sleep $PAUSE
  fi

done
