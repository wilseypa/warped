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

	# Copy config file to tmp folder
	cp parallel.config /tmp/$hostname.parallel.config
	configFile="/tmp/$hostname.parallel.config"

	# Set configuration parameters
	sed -i "s/WorkerThreadCount : [0-9]*$/WorkerThreadCount : "$threads"/g" $configFile
	sed -i "s/ScheduleQScheme : [A-Z]*$/ScheduleQScheme : "$scheduleQScheme"/g" $configFile
	sed -i "s/ScheduleQCount : [0-9]*$/ScheduleQCount : "$scheduleQCount"/g" $configFile
}

function run {
	binary=$1
	binaryConfig=$2
	threads=$3
	scheduleQScheme=$4
	scheduleQCount=$5
	simulateUntil=$6

	echo -e "\nStarting $binary $binaryConfig Simulation: $threads threads, $scheduleQCount scheduleQueues, and $scheduleQScheme\n"
	
	set_config $threads $scheduleQScheme $scheduleQCount
	if [ $simulateUntil == "-" ]
	then
		runCommand="./$binary -configuration $configFile -simulate $binaryConfig"
	else
		runCommand="./$binary -configuration $configFile -simulate $binaryConfig -simulateUntil $simulateUntil"
	fi
	date=`date +"%m-%d-%y_%T"`
	grepMe=`$runCommand | grep "Simulation complete"`
	runTime=`echo $grepMe | sed -e 's/.*complete (\(.*\) secs.*/\1/'`
	rollbacks=`echo $grepMe | sed -e 's/.*Rollbacks: (\(.*\)).*/\1/'`
	echo $runTime
	echo $rollbacks

	# Write to log file
	echo "$binary,$binaryConfig,$threads,$scheduleQScheme,$scheduleQCount,$simulateUntil,$runTime,$rollbacks" >> $logFile

	sleep 10
}

hostname=`hostname`
date=`date +"%m-%d-%y_%T"`
logFile="scripts/logs/$hostname---$date.csv"

# Write csv header
## Simulation Threads Scheme ScheduleQCount SimulateUntil Runtime Rollbacks
echo "Simulation,SimulationConfig,Threads,Scheme,ScheduleQCount,SimulateUntil,Runtime,Rollbacks" > $logFile

trap control_c SIGINT

. scripts/$1
#run raidSim raid/LargeRAID 6 MULTISET 1 100000

# Upload output to dropbox
scripts/dropbox_uploader.sh upload $logFile
