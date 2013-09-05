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
	cp parallel.config /tmp/$hostname.parallel.config
	configFile="/tmp/$hostname.parallel.config"

	# Set configuration parameters
	sed -i "s/WorkerThreadCount : [0-9]*$/WorkerThreadCount : "$threads"/g" $configFile
	sed -i "s/ScheduleQScheme : [A-Z]*$/ScheduleQScheme : "$scheduleQScheme"/g" $configFile
	sed -i "s/ScheduleQCount : [0-9]*$/ScheduleQCount : "$scheduleQCount"/g" $configFile
	sed -i "s/LoadBalancing : [A-Z]*$/LoadBalancing : "$loadBalancing"/g" $configFile
	sed -i "s/LoadBalancingMetric : [A-Za-z]*$/LoadBalancingMetric : "$loadBalancingMetric"/g" $configFile
	sed -i "s/LoadBalancingTrigger : [A-Za-z]*$/LoadBalancingTrigger : "$loadBalancingTrigger"/g" $configFile
	sed -i "s/LoadBalancingNormalInterval : [0-9]*$/LoadBalancingInterval : "$loadBalancingInterval"/g" $configFile
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
	echo "$binary,$binaryConfig,$threads,$scheduleQScheme,$scheduleQCount,$loadBalancing,$loadBalancingMetric,$loadBalancingInterval,$loadBalancingTrigger,$simulateUntil,$runTime,$rollbacks" >> $logFile

	sleep 10
}

hostname=`hostname`
date=`date +"%m-%d-%y_%T"`
logFile="scripts/logs/$hostname---$date.csv"

# Write csv header
## Simulation Threads Scheme ScheduleQCount SimulateUntil Runtime Rollbacks
echo "Simulation,SimulationConfig,Threads,Scheme,ScheduleQCount,LoadBalancing,LoadBalancingMetric,LoadBalancingInterval,LoadBalancingTrigger,SimulateUntil,Runtime,Rollbacks" > $logFile

trap control_c SIGINT

. scripts/$1
#run raidSim raid/LargeRAID 6 MULTISET 1 100000

# Upload output to dropbox
scripts/dropbox_uploader.sh upload $logFile
