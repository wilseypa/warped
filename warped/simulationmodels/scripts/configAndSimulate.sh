#!/bin/bash

# Allows batch runs of simulations. Saves results to log files

cd ..

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

	echo -e "\nStarting Large Raid Simulation: $threads threads, $scheduleQCount scheduleQueues, and $scheduleQScheme\n"
	
	set_config $threads $scheduleQScheme $scheduleQCount
	runCommand="./raidSim -configuration parallel.config -simulate raid/LargeRaid"
	date=`date +"%m-%d-%y_%T"`
	logFile="scripts/logs/LargeRaid-${threads}T-${scheduleQScheme}-${scheduleQCount}SQ_${date}.log"
	$runCommand > $logFile
}

#run_LargeRaid THREADS SCHEME SQCOUNT
run_LargeRaid 48 MULTILTSF 2
run_LargeRaid 60 MULTILTSF 2
