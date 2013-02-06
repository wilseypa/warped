#!/bin/bash

# Allows batch runs of simulations. Saves results to log files

cd ..

function control_c()
# run if user hits control-c
{
  echo -en "\n*** Ouch! Exiting ***\n"
  scripts/dropbox_uploader.sh upload $logFile
  exit $?
}

#Modifies the config file to the given settings
function set_config {
	threads=$1
	scheduleQScheme=$2
	scheduleQCount=$3
	
	# Set configuration parameters
	sed -i "s/WorkerThreadCount : [0-9]*$/WorkerThreadCount : "$threads"/g" parallel.config
	sed -i "s/ScheduleQScheme : [A-Z]*$/ScheduleQScheme : "$scheduleQScheme"/g" parallel.config
	sed -i "s/ScheduleQCount : [0-9]*$/ScheduleQCount : "$scheduleQCount"/g" parallel.config
}

function run_LargeRaid {
	threads=$1
	scheduleQScheme=$2
	scheduleQCount=$3
	simulateUntil=$4

	echo -e "\nStarting Large Raid Simulation: $threads threads, $scheduleQCount scheduleQueues, and $scheduleQScheme\n"
	
	set_config $threads $scheduleQScheme $scheduleQCount
	if [ $simulateUntil == "-" ]
	then
		runCommand="./raidSim -configuration parallel.config -simulate raid/LargeRaid"
	else
		runCommand="./raidSim -configuration parallel.config -simulate raid/LargeRaid -simulateUntil $simulateUntil"
	fi
	date=`date +"%m-%d-%y_%T"`
	grepMe=`$runCommand | grep "Simulation complete"`
	runTime=`echo $grepMe | sed -e 's/.*complete (\(.*\) secs.*/\1/'`
	rollbacks=`echo $grepMe | sed -e 's/.*Rollbacks: (\(.*\)).*/\1/'`
	echo $runTime
	echo $rollbacks

	# Write to log file
	echo "LargeRAID,$threads,$scheduleQScheme,$scheduleQCount,$simulateUntil,$runTime,$rollbacks" >> $logFile

	sleep 10
}

hostname=`hostname`
date=`date +"%m-%d-%y_%T"`
logFile="scripts/logs/$hostname---$date.csv"

# Write csv header
## Simulation Threads Scheme ScheduleQCount SimulateUntil Runtime Rollbacks
echo "Simulation,Threads,Scheme,ScheduleQCount,SimulateUntil,Runtime,Rollbacks" > $logFile

trap control_c SIGINT

. scripts/$1

# Upload output to dropbox
scripts/dropbox_uploader.sh upload $logFile
