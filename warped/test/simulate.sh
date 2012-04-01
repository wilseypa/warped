#!/bin/bash

if [ "$#" -ne 2 ]
then
  echo "usage: $0 <directory> <number of runs>"
  exit
fi

if [ -z $1 ]
then
  echo "usage: $0 <directory>"
  exit
fi

if [ ! -d $1 ]
then
  echo "$1 does not exist"
  exit
fi

DOWNTIME=800
DATADIR=${1%/}

for i in `seq 1 "$2"`
do
  RUN=`printf "%02d" $i`
  CSVDIR="$DATADIR/$RUN"

  echo "$RUN"; exit

  echo "waiting $DOWNTIME seconds before next simulation run..."
  sleep $DOWNTIME

  echo "beginning simulation run \#$RUN"
  mpiexec.hydra -n 8 -binding cpu:cores ./pholdSim -simulate phold/LargePHOLD -configuration parallel.config -simulateUntil 5000

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
