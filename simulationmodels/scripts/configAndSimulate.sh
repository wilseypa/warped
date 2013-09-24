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
	loadBalancing=$4
	loadBalancingMetric=$5
	loadBalancingTrigger=$6
	loadBalancingNormalInterval=$7
	loadBalancingNormalThresh=$8
	loadBalancingRelaxedInterval=$9
	loadBalancingRelaxedThresh=${10}

	# Copy config file to tmp folder
	cp parallel.json /tmp/$hostname.parallel.json
	configFile="/tmp/$hostname.parallel.json"

	# Set configuration parameters
	sed -i "s/\"WorkerThreadCount\"\s*:\s*[0-9]+/\"WorkerThreadCount\": $threads/p" $configFile
	sed -i "s/\"ScheduleQScheme\"\s*:\s*[A-Za-z]+/\"ScheduleQScheme\": \"$scheduleQScheme\"/p" $configFile
	sed -i "s/\"ScheduleQCount\"\s*:\s*[0-9]+/\"ScheduleQCount\": $scheduleQCount/p" $configFile
	sed -i "s/\"LoadBalancing\"\s*:\s*[A-Za-z]+/\"LoadBalancing\": $loadBalancing/p" $configFile
	sed -i "s/\"LoadBalancingMetric\"\s*:\s*[A-Za-z]+/\"LoadBalancingMetric\": \"$loadBalancingMetric\"/p" $configFile
	sed -i "s/\"LoadBalancingTrigger\"\s*:\s*[A-Za-z]+/\"LoadBalancingTrigger\": \"$loadBalancingTrigger\"/p" $configFile
	sed -i "s/\"LoadBalancingNormalInterval\"\s*:\s*[0-9]+/\"LoadBalancingNormalInterval\": $loadBalancingInterval/p" $configFile
}

function run {
	binary=$1
	binaryConfig=$2
	threads=$3
	scheduleQScheme=$4
	scheduleQCount=$5
	loadBalancing=$6
	loadBalancingMetric=$7
	loadBalancingTrigger=$8
	loadBalancingNormalInterval=$9
	loadBalancingNormalThresh=${10}
	loadBalancingRelaxedInterval=${11}
	loadBalancingRelaxedThresh=${12}
	simulateUntil=${13}

	echo -e "\nStarting $binary $binaryConfig Simulation: $threads threads, $scheduleQCount scheduleQueues, $scheduleQScheme, and loadBalancing $loadBalancing\n"

	set_config $threads $scheduleQScheme $scheduleQCount $loadBalancing $loadBalancingMetric $loadBalancingInterval $loadBalancingTrigger

	if [ $simulateUntil == "-" ]
	then
		runCommand="./$binary --configuration $configFile --simulate $binaryConfig"
	else
		runCommand="./$binary --configuration $configFile --simulate $binaryConfig --simulateUntil $simulateUntil"
	fi
	date=`date +"%m-%d-%y_%T"`
	grepMe=`$runCommand | grep "Simulation complete"`
	runTime=`echo $grepMe | sed -e 's/.*complete (\(.*\) secs.*/\1/'`
	rollbacks=`echo $grepMe | sed -e 's/.*Rollbacks: (\(.*\)).*/\1/'`
	echo $runTime
	echo $rollbacks

	# Write to log file
	echo "$binary,$binaryConfig,$threads,$scheduleQScheme,$scheduleQCount,$loadBalancing,$loadBalancingMetric,$loadBalancingInterval,$loadBalancingTrigger,$simulateUntil,$runTime,$rollbacks" >> $logFile

	sleep 10
}

hostname=`hostname`
date=`date +"%m-%d-%y_%H-%M-%S"`
logFile="scripts/logs/$hostname---$date.csv"

# Write csv header
## Simulation Threads Scheme ScheduleQCount SimulateUntil Runtime Rollbacks
echo "Simulation,SimulationConfig,Threads,Scheme,ScheduleQCount,LoadBalancing,LoadBalancingMetric,LoadBalancingInterval,LoadBalancingTrigger,SimulateUntil,Runtime,Rollbacks" > $logFile

trap control_c SIGINT

. scripts/$1

# Upload output to dropbox
scripts/dropbox_uploader.sh upload $logFile
