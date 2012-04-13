#!/bin/bash
#
# Author: Ryan Child
#
# Runs batch WARPED simulations, launches simulations according to the 
# PhysicalLayer option in WARPED configuration.  For example, MPICH2
# recommends use of a process manager to launch MPI programs.
#
# Run with --help to see list of options

# defaults
MPI_HOSTS=hosts
MODEL_EXE=./pholdSim
MODEL_CONFIG=phold/LargePHOLD
WARPED_CONFIG=parallel.config
DATADIR=measurements
RUNS=10
PAUSE=100

# get user-supplied arguments
TEMP=`getopt -o d:n:p:e:f:m:w:s: -n "$0" \
  -l help,model-exe:,hosts-file:model-config:,warped-config:,simulate-until: \
  -l data-dir:,number-runs:,pause: \
  -- "$@"`
  
eval set -- "$TEMP"
while true; do
  case "$1" in
    -d|--data-dir) DATADIR=${2%/}; shift 2;;
    -n|--number-runs) RUNS=$2; shift 2;;
    -p|--pause) PAUSE=$2; shift 2;;
    -e|--model-exe) MODEL_EXE=$2; shift 2;;
    -f|--hosts-file) MPI_HOSTS=$2; shift 2;;
    -m|--model-config) MODEL_CONFIG=$2; shift 2;;
    -w|--warped-config) WARPED_CONFIG=$2; shift 2;;
    -s|--simulate-until) SIMULATE_UNTIL=$2; shift 2;;
    --help) cat << EOF
Usage: $(basename $0) [OPTION]...

Options:
  -d, --data-dir          path to the directory where simulation output 
                          will be saved [default=$DATADIR]
  -n, --number-runs       number of simulations to do [default=$RUNS]
  -p, --pause             pause interval in seconds between simulation runs
                          [default=$PAUSE]
  -e, --model-exe         path to the model executable [defaut=$MODEL_EXE]
  -f, --hosts-file        path to the MPI hosts file [default=$MPI_HOSTS]
  -m, --model-config      path to model configuration file
                          [default=$MODEL_CONFIG]
  -w, --warped-config     path to the WARPED configuration file
                          [default=$WARPED_CONFIG]
  -s, --simulate-until    GVT to simulate until (same as WARPED -simulateUntil)
EOF
      exit;;
    --) shift; break;;
  esac
done

# validate arguments
if [ ! -d "$DATADIR" ]; then 
  ERROR="${ERROR}error: invalid directory $DATADIR\n"; fi
if ! [[ "$RUNS" =~ ^[0-9]+$ ]]; then 
  ERROR="${ERROR}error: invalid # runs \"$RUNS\"\n"; fi
if ! [[ "$PAUSE" =~ ^[0-9]+$ ]]; then 
  ERROR="${ERROR}error: invalid pause interval $PAUSE\n"; fi
if [ ! -x "$MODEL_EXE" ]; then 
  ERROR="${ERROR}error: model executable $MODEL_EXE not found\n"; fi
if [ ! -e "$MODEL_CONFIG" ]; then 
  ERROR="${ERROR}error: model configuration file $MODEL_CONFIG not found\n"; fi
if [ ! -e "$WARPED_CONFIG" ]; then
  ERROR="${ERROR}error: WARPED configuration file $WARPED_CONFIG not found\n"; fi
if [ ! -e "$MPI_HOSTS" ]; then
  ERROR="${ERROR}error: hosts file $MPI_HOSTS not found\n"; fi
if [ -n "$SIMULATE_UNTIL" ] && (! [[ "$SIMULATE_UNTIL" =~ ^[0-9]+$ ]]); then 
  ERROR="${ERROR}error: invalid simulate-until \"$SIMULATE_UNTIL\"\n"; fi
  
if [ -n "$ERROR" ]; then echo -e "${ERROR}exiting."; exit; fi

PHYSICAL_LAYER=`grep -P "^\s*PhysicalLayer" "$WARPED_CONFIG" | sed "s/.*:\s*//g" | sed 's/\s*$//'`

# need full paths if using TCPSelect
if [ "$PHYSICAL_LAYER" = TCPSelect ]; then
  MODEL_EXE=`readlink -f "$MODEL_EXE"`
  MODEL_CONFIG=`readlink -f "$MODEL_CONFIG"`
  WARPED_CONFIG=`readlink -f "$SIMULATION_CONFIG"`
fi

CMD_PARAMS="-simulate $MODEL_CONFIG \
-configuration $WARPED_CONFIG"
if [ -n "$SIMULATE_UNTIL" ]
then
  CMD_PARAMS="$CMD_PARAMS -simulateUntil $SIMULATE_UNTIL"
fi

# build simulation command
case $PHYSICAL_LAYER in
  MPI)
    CMD="mpiexec.hydra -f $MPI_HOSTS $MODEL_EXE $CMD_PARAMS"
    ;;
  TCPSelect)
    # if using TCPSelect, the simulation must be run from the home directory
    CMD="cd && $MODEL_EXE $CMD_PARAMS"
    ;;
esac

for i in `seq 1 "$RUNS"`; do
  RUN=`printf "%02d" $i`

  # do simulation run
  echo "running simulation #$RUN"
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

  # build directory structure
  CSVDIR="$DATADIR/$RUN"
  if [ ! -d "$CSVDIR" ]
  then
    mkdir "$CSVDIR"
  fi

  # copy the CSVs to a dir for their run
  mv *.csv "$CSVDIR"

  # let the CPU cool down for $PAUSE seconds
  if [ "$i" -ne "$RUNS" ]; then
    echo "waiting $PAUSE seconds before next simulation run..."
    sleep $PAUSE
  fi

done
