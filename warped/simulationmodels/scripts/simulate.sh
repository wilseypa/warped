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
SETUP_SCRIPT="$SCRIPT_DIR/setup.sh"
RUNS=1
PAUSE=0
CSVFILE=data.csv
TIME_EXE="/usr/bin/time"
MPI_EXE="mpiexec.hydra"
KERN_LOG="/var/log/kern.log"

# get batch script arguments
export POSIXLY_CORRECT=1
TEMP=`getopt -o d:n:p:f:s:o: -n "$0" \
  -l help,model-exe:,hosts-file:,data-dir:,number-runs:,pause:, \
  -l parameters-file:,email-addr:,setup-script:,output:,time -- "$@"`
  
eval set -- "$TEMP"
while true; do
  case "$1" in
    -d|--data-dir) DATADIR=${2%/}; shift 2;;
    -n|--number-runs) RUNS=$2; shift 2;;
    -p|--pause) PAUSE=$2; shift 2;;
    -f|--hosts-file) MPI_HOSTS=$2; shift 2;;
    -o|--output) CSVFILE=$2; shift 2;;
    --parameters-file) PARAMS_FILE=$2; shift 2;;
    --setup-script) SETUP_SCRIPT=$2; shift 2;;
    --email-addr) EMAIL_ADDR=$2; shift 2;;
    --time) USE_GNU_TIME=1; shift;;
    --help) cat << EOF
Usage: $(basename $0) [OPTION]... [--] WARPED executable and options

Options:
  -d, --data-dir          path to the directory where intermediate lpX.csv
                          files will be saved
  -n, --number-runs       number of simulations to do [default=$RUNS]
  -p, --pause             pause interval in seconds between simulation runs
                          [default=$PAUSE]
  -f, --hosts-file        path to the MPI hosts file [default=$MPI_HOSTS]
  -o, --output            output file [default=$CSVFILE]
      --parameters-file   path to file containing comma-delimited set of 
                          parameters
      --setup-script      user-supplied script to set parameters.  for each line
                          in parameters-file, this script will be called with 
                          the parameter line on STDIN [default=$SETUP_SCRIPT]
      --email-addr        email address to send to when batch is complete
      --time              use GNU time to profile simulations and save 
                          results to lpX.csv. used with -d
EOF
      exit;;
    --) shift; break;;
  esac
done

# get WARPED options
# unfortunately these options must be kept in sync with WARPED code!
MODEL_EXE="$1"
TEMP=`getopt -a -l debug,simulate:,configuration:,simulateUntil: \
  -n "$0" -- "$@"`

eval set -- "$TEMP"
while true; do
  case "$1" in
    --simulateUntil) SIMULATE_UNTIL=$2; shift 2;;
    --simulate) MODEL_CONFIG=$2; shift 2;;
    --configuration) WARPED_CONFIG=$2; shift 2;;
    --debug) WARPED_DEBUG=$2; shift;;
    --) shift; break;;
  *) echo "$1";;
  esac
done

PHYSICAL_LAYER=`grep -P "^\s*PhysicalLayer" "$WARPED_CONFIG" | sed "s/.*:\s*//g" | sed 's/\s*$//'`

# need full paths if using TCPSelect
if [ "$PHYSICAL_LAYER" = TCPSelect ]; then
  MODEL_EXE=`readlink -f "$MODEL_EXE"`
  MODEL_CONFIG=`readlink -f "$MODEL_CONFIG"`
  WARPED_CONFIG=`readlink -f "$WARPED_CONFIG"`
fi

# validate arguments
if [ ! -x `which "$MPI_EXE"` ] && [ "$PHYSICAL_LAYER" = "MPI" ]; then
  ERROR="${ERROR}error: unable to find mpi launcher $MPI_EXE\n"; fi
if [ ! -x "$MODEL_EXE" ]; then
  ERROR="${ERROR}error: unable to find model executable $MODEL_EXE\n"; fi
if [ ! -e "$MODEL_CONFIG" ]; then
  ERROR="${ERROR}error: unable to find model config file $MODEL_CONFIG\n"; fi
if [ ! -e "$WARPED_CONFIG" ]; then
  ERROR="${ERROR}error: unable to find warped config file $WARPED_CONFIG\n"; fi
if ! [[ "$RUNS" =~ ^[0-9]+$ ]]; then 
  ERROR="${ERROR}error: invalid # runs \"$RUNS\"\n"; fi
if ! [[ "$PAUSE" =~ ^[0-9]+$ ]]; then 
  ERROR="${ERROR}error: invalid pause interval $PAUSE\n"; fi
if [ ! -e "$MPI_HOSTS" ]; then
  ERROR="${ERROR}error: hosts file $MPI_HOSTS not found\n"; fi
if [ ! -e "$PARAMS_FILE" ] || [ ! -e "$SETUP_SCRIPT" ]; then
  if [ -n "$PARAMS_FILE" ]; then
ERROR="${ERROR}error: parameters file given but $PARAMS_FILE and/or " \
"$SETUP_SCRIPT do not exist\n"
  fi
else
  HAVE_PARAMS=1
fi
  
if [ -n "$ERROR" ]; then echo -e "${ERROR}exiting."; exit; fi

if [ -n "$USE_GNU_TIME" ] && [ ! -x "$TIME_EXE" ]; then
WARNING="${WARNING}warning: cannot execute $TIME_EXE, cannot use GNU time " \
"profiling\n"
fi
if [ -n "$USE_GNU_TIME" ] && [ -z "$DATADIR" ]; then
WARNING="${WARNING}warning: no data dir specified, cannot GNU time profiling\n"
fi


if [ -n "$WARNING" ]; then echo -e "$WARNING"; USE_GNU_TIME=""; fi

touch "$CSVFILE"
CSVFILE=`readlink -f "$CSVFILE"`
EMAIL_SUBJECT="batch simulation complete"
EMAIL_BODY="results are in $CSVFILE"

CMD_PARAMS="-simulate $MODEL_CONFIG \
-configuration $WARPED_CONFIG"
if [ -n "$SIMULATE_UNTIL" ]; then
  CMD_PARAMS="$CMD_PARAMS -simulateUntil $SIMULATE_UNTIL"
fi
if [ -n "$WARPED_DEBUG" ]; then
  CMD_PARAMS="$CMD_PARAMS -debug"
fi

MPI_ARGS="-f $MPI_HOSTS"
if [ -n "$USE_GNU_TIME" ]; then
  TIME_CMD="$TIME_EXE -f %U,%S,%P,%w,%c"
  MPI_ARGS="$MPI_ARGS -prepend-rank"
  TMPFILE="simoutput.tmp"
fi

# build simulation command
case $PHYSICAL_LAYER in
  MPI)
    CMD="$MPI_EXE $MPI_ARGS $TIME_CMD $MODEL_EXE $CMD_PARAMS"
    ;;
  TCPSelect)
    # if using TCPSelect, the simulation must be run from the home directory
    cd

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

    if [ -n "$USE_GNU_TIME" ]; then
      echo "sorry, unable to use gnu time profiling with TCPSelect"
    fi
    ;;
esac

# print header
if [ -n "$HAVE_PARAMS" ]; then
  head -n 1 "$PARAMS_FILE" | tr -d "\n" >> "$CSVFILE"
  echo ",runtime,rollbacks,efficiency,eventrate" >> "$CSVFILE"
fi

for p in `tail -n+2 "$PARAMS_FILE" 2>/dev/null || echo 0`; do
  INTERMEDIATEDIR="${DATADIR}"
  if [ -n "$HAVE_PARAMS" ]; then
    PARAMS=(${p//,/ })

    # make changes to input files
    echo "$p" | "$SETUP_SCRIPT"

    # build directory structure
    if [ -n "$DATADIR" ]; then
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
    if [ -n "$TMPFILE" ]; then
      $CMD 2>&1 | tee "$TMPFILE"
    else
      $CMD
    fi

    # extract runtime, rollback, efficiency and event rate info from the CSVs
    let "ROLLBACKS=0"
    let "COMMITTED=0"
    let "EXECUTED=0"
    NUMLPS=`ls lp*.csv | wc -l`
    let "MAXLP = $NUMLPS - 1"
    for n in `seq 0 "$MAXLP"`; do
      f="lp$n.csv"
      LINE=`tail -n 1 $f`
      ARR=(${LINE//,/ })
      if [ -z "$RUNTIME" ]; then
        RUNTIME="${ARR[0]}"
      else
        RUNTIME=`echo - | awk "{if(${ARR[0]}>$RUNTIME){print ${ARR[0]}}else{print $RUNTIME}}"`
      fi
      let "ROLLBACKS += ${ARR[1]}"
      let "COMMITTED += ${ARR[2]}"
      let "EXECUTED += ${ARR[3]}"

      # add data from time
      if [ -n "$USE_GNU_TIME" ]; then
        tail -n "$NUMLPS" "$TMPFILE" | grep "\[$n\]" | sed "s/\[$n\] //" >> "$f"
      fi

    done
    EFFICIENCY=`echo - | awk "{print $COMMITTED / $EXECUTED}"`
    EVENTRATE=`echo - | awk "{print $COMMITTED / $RUNTIME}"`

    if [ -n "$HAVE_PARAMS" ]; then
      for param in "${PARAMS[@]}"; do
        echo -n "$param," >> "$CSVFILE"
      done
    fi
    echo "$RUNTIME,$ROLLBACKS,$EFFICIENCY,$EVENTRATE" >> "$CSVFILE"

    # reset runtime
    RUNTIME=""

    # build directory structure
    if [ -n "$DATADIR" ]; then
      CSVDIR="$INTERMEDIATEDIR/$RUN"
      if [ ! -d "$CSVDIR" ]; then
        mkdir "$CSVDIR"
      fi

      # copy the CSVs to a dir for their run
      mv lp*.csv "$CSVDIR"
    else
      rm -f lp*.csv
    fi

    if [ -e "$KERN_LOG" ] && grep -iq "temperature above threshold" "$KERN_LOG"
    then
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
