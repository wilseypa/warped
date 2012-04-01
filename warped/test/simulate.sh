#!/bin/bash

TEMP=`getopt -o d:n: -l help -n "$0" -- "$@"`
eval set -- "$TEMP"

while true; do
  case "$1" in
    -d) DATADIR=${2%/}; shift 2;;
    -n) NODES=$2; shift 2;;
    --help) echo "Usage: $0 -n nodes -d directory"; exit;;
    --) shift; break;;
  esac
done

if [ ! -d "$DATADIR" ]; then echo "error: invalid directory '$DATADIR'"; ERROR=1; fi
if [ -n "$NODES" ] && ! [[ "$NODES" =~ ^[0-9]+$ ]]; then echo "error: invalid nodes '$NODES'"; ERROR=1; fi
if [ -n "$ERROR" ]; then exit; fi

if [ -z "$NODES" ]; then NODES=`cat /proc/cpuinfo | grep processor | wc -l`; fi

DOWNTIME=800
MODEL=pholdSim
MODEL_CONFIGURATION=phold/LargePHOLD
SIMULATION_CONFIGURATION=parallel.config
# comment out to omit this argument
SIMULATE_UNTIL=5000

if [ ! -x "$MODEL" ] || 
   [ ! -e "$MODEL_CONFIGURATION" ] || 
   [ ! -e "$SIMULATION_CONFIGURATION" ]
then
  echo "$0 must be run from the warped test (model) directory"
  exit
fi

ARGS="-simulate $MODEL_CONFIGURATION -configuration $SIMULATION_CONFIGURATION"
if [ -n "$SIMULATE_UNTIL" ]
then
  ARGS="$ARGS -simulateUntil $SIMULATE_UNTIL"
fi

for i in `seq 1 "$2"`
do
  RUN=`printf "%02d" $i`
  CSVDIR="$DATADIR/$RUN"

  echo "$RUN"; exit

  echo "waiting $DOWNTIME seconds before next simulation run..."
  sleep $DOWNTIME

  echo "beginning simulation run \#$RUN"
  mpiexec.hydra -n "$NODES" -binding cpu:cores ./"$MODEL" "$ARGS"

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
  if [ ! -d "$CSVDIR" ]
  then
    mkdir "$CSVDIR"
  fi

  mv *.csv "$CSVDIR"
  exit

done
