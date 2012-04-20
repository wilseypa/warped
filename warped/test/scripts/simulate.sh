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
SCRIPT_DIR=`dirname $0`
MPI_HOSTS="hosts"
DATADIR="measurements"
SETUP_SCRIPT="$SCRIPT_DIR/setup.sh"
RUNS=10
PAUSE=100
CSVFILE=data.csv

# get batch script arguments
export POSIXLY_CORRECT=1
TEMP=`getopt -o d:n:p:f:s: -n "$0" \
  -l help,model-exe:,hosts-file:,data-dir:,number-runs:,pause:, \
  -l parameters-file:,delete-csvs,email-addr:,setup-script: -- "$@"`
  
eval set -- "$TEMP"
while true; do
  case "$1" in
    -d|--data-dir) DATADIR=${2%/}; shift 2;;
    -n|--number-runs) RUNS=$2; shift 2;;
    -p|--pause) PAUSE=$2; shift 2;;
    -f|--hosts-file) MPI_HOSTS=$2; shift 2;;
    --parameters-file) PARAMS_FILE=$2; shift 2;;
    --setup-script) SETUP_SCRIPT=$2; shift 2;;
    --delete-csvs) DELETE_CSVS=1; shift;;
    --email-addr) EMAIL_ADDR=$2; shift 2;;
    --help) cat << EOF
Usage: $(basename $0) [OPTION]...

Options:
  -d, --data-dir          path to the directory where simulation output 
                          will be saved [default=$DATADIR]
  -n, --number-runs       number of simulations to do [default=$RUNS]
  -p, --pause             pause interval in seconds between simulation runs
                          [default=$PAUSE]
  -f, --hosts-file        path to the MPI hosts file [default=$MPI_HOSTS]
      --parameters-file   path to file containing comma-delimited set of 
                          parameters
      --setup-script      user-supplied script to set parameters.  for each line
                          in parameters-file, this script will be called with 
                          the parameter line on STDIN [default=$SETUP_SCRIPT]
      --delete-csvs       delete intermediate lpX.csv files
                          (use for large batches)
      --email-addr        email address to send to when batch is complete
EOF
      exit;;
    --) shift; break;;
  esac
done

# get WARPED options
# unfortunately these options must be kept in sync with WARPED code!
MODEL_EXE="$1"
TEMP=`getopt -a -l simulate:,configuration:,simulateUntil: -n "$0" -- "$@"`

eval set -- "$TEMP"
while true; do
  case "$1" in
    --simulateUntil) SIMULATE_UNTIL=$2; shift 2;;
    --simulate) MODEL_CONFIG=$2; shift 2;;
    --configuration) WARPED_CONFIG=$2; shift 2;;
    --) shift; break;;
  *) echo "$1";;
  esac
done

# validate arguments
if [ ! -d "$DATADIR" ]; then 
  ERROR="${ERROR}error: invalid directory $DATADIR\n"; fi
if ! [[ "$RUNS" =~ ^[0-9]+$ ]]; then 
  ERROR="${ERROR}error: invalid # runs \"$RUNS\"\n"; fi
if ! [[ "$PAUSE" =~ ^[0-9]+$ ]]; then 
  ERROR="${ERROR}error: invalid pause interval $PAUSE\n"; fi
if [ ! -e "$MPI_HOSTS" ]; then
  ERROR="${ERROR}error: hosts file $MPI_HOSTS not found\n"; fi
if [ ! -e "$PARAMS_FILE" ] || [ ! -e "$SETUP_SCRIPT" ]; then
  if [ -n "$PARAMS_FILE" ]; then
    ERROR="${ERROR}error: parameters file given but $PARAMS_FILE and/or " \
      "$SETUP_SCRIPT do not exist"
  fi
else
  HAVE_PARAMS=1
fi
  
if [ -n "$ERROR" ]; then echo -e "${ERROR}exiting."; exit; fi

EMAIL_SUBJECT="batch simulation complete"
EMAIL_BODY="results are in $DATADIR/$CSVFILE"

PHYSICAL_LAYER=`grep -P "^\s*PhysicalLayer" "$WARPED_CONFIG" | sed "s/.*:\s*//g" | sed 's/\s*$//'`

# need full paths if using TCPSelect
if [ "$PHYSICAL_LAYER" = TCPSelect ]; then
  MODEL_EXE=`readlink -f "$MODEL_EXE"`
  MODEL_CONFIG=`readlink -f "$MODEL_CONFIG"`
  WARPED_CONFIG=`readlink -f "$WARPED_CONFIG"`
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
    # and we have to make sure the SSH_COMMAND environment variable is set
    cd
    export SSH_COMMAND=/usr/bin/ssh

    # unfortunately, this file name must be kept in sync with WARPED code
    # DO NOT CHANGE
    TCPSELECT_HOSTS="procgroup"

    if ! [ -e "$TCPSELECT_HOSTS" ]; then
      echo "error: you must have a $TCPSELECT_HOSTS file in your home \
directory to use TCPSelect"
      echo "exiting"
      exit
    fi
    CMD="$MODEL_EXE $CMD_PARAMS"
    ;;
esac

# print header
if [ -n "$HAVE_PARAMS" ]; then
  head -n 1 "$PARAMS_FILE" | tr -d "\n" >> "$DATADIR/$CSVFILE"
  echo "runtime,rollbacks,efficiency,eventrate" >> "$DATADIR/$CSVFILE"
fi

for p in `tail -n+2 "$PARAMS_FILE" 2>/dev/null || echo 0`; do
  INTERMEDIATEDIR="${DATADIR}"
  if [ -n "$HAVE_PARAMS" ]; then
    PARAMS=(${p//,/ })

    # make changes to input files
    echo "$p" | "$SETUP_SCRIPT"

    # build directory structure
    if [ -z "$DELETE_CSVS" ]; then
      INTERMEDIATEDIR="${INTERMEDIATEDIR}/"
      for param in "${PARAMS[@]}"; do
        INTERMEDIATEDIR="${INTERMEDIATEDIR}${param}_"
      done
      INTERMEDIATEDIR="${INTERMEDIATEDIR%?}"
      if [ ! -d "$INTERMEDIATEDIR" ]; then
        mkdir "$INTERMEDIATEDIR"
      fi
    fi
  fi

  for i in `seq 1 "$RUNS"`; do
    RUN=`printf "%02d" $i`

    # do simulation run
    echo "running simulation #$RUN"
    $CMD

    # extract runtime and rollback info from the CSVs
    let "ROLLBACKS=0"
    let "COMMITTED=0"
    let "EXECUTED=0"
    for f in lp*.csv; do
      LINE=`tail -n 1 $f`
      ARR=(${LINE//,/ })
      RUNTIME="${ARR[0]}"
      let "ROLLBACKS += ${ARR[1]}"
      let "COMMITTED += ${ARR[2]}"
      let "EXECUTED += ${ARR[3]}"
    done
    EFFICIENCY=`echo – | awk "{print $COMMITTED / $EXECUTED}"`
    EVENTRATE=`echo - | awk "{print $COMMITTED / $RUNTIME}"`

    if [ -n "$HAVE_PARAMS" ]; then
      for param in "${PARAMS[@]}"; do
        echo -n "$param," >> "$DATADIR/$CSVFILE"
      done
    fi
    echo "$RUNTIME,$ROLLBACKS,$EFFICIENCY,$EVENTRATE" >> "$DATADIR/$CSVFILE"

    # build directory structure
    if [ -z "$DELETE_CSVS" ]; then
      CSVDIR="$INTERMEDIATEDIR/$RUN"
      if [ ! -d "$CSVDIR" ]; then
        mkdir "$CSVDIR"
      fi

      # copy the CSVs to a dir for their run
      mv lp*.csv "$CSVDIR"
    else
      rm -f lp*.csv
    fi

    if grep -iq "temperature above threshold" /var/log/kern.log; then
      EMAIL_SUBJECT="simulation batch terminated"
      EMAIL_BODY="detected kernel throttling CPU freq due to temperatures above threshold"
      break 2
    fi

    # let the CPU cool down for $PAUSE seconds
    echo "waiting $PAUSE seconds before next simulation run..."
    sleep $PAUSE

  done
done

if [ -n "$EMAIL_ADDR" ]; then
  echo "$EMAIL_BODY" | mailx -s \ "$EMAIL_SUBJECT" -- "$EMAIL_ADDR"
fi
