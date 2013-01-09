#ifndef SIMULATION_CONFIGURATION_H
#define SIMULATION_CONFIGURATION_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <string>
#include <vector>

using std::string;
using std::vector;

class ConfigurationScope;

/** This class represents a simulation configuration. */
class SimulationConfiguration {

	/** Simulation needs to be able to allocate SimulationConfigurations in
	 the default case. */
	friend class Simulation;

public:
	/**
	 Parse a configuration file and return it.

	 @param configFileName The name of the configuration file.  Must be a
	 valid file name relative to where the program was run from.

	 @param argc The number of arguments in the command line.

	 @param argv The arguments from the command line.
	 */
	static SimulationConfiguration *parseConfiguration(
			const string &configFileName, const vector<string> &argumentVector);

	/** This method returns additional command line arguments that were
	 provided at startup.  */
	const vector<string> &getArguments();

	bool communicationManagerIs(const string &testValue) const;
	const string getCommunicationManagerType() const;

	/**
	 If the EventList value is what is passed in, return true, else, false;
	 */
	bool eventListTypeIs(const string &testValue) const;
	bool eventListOrganizationIs(const string &testValue) const;
	/**
	 Return the string of the EventList section of the config file, or
	 "(none)".
	 */
	const string getEventListType() const;
	const string getEventListOrganization() const;

	/**
	 If the ObjectQueue is what is passed in, return true, else, false;
	 */
	bool objectQueueTypeIs(const string &testValue) const;
	/**
	 Return the string of the ObjectQueue section of the config file, or
	 "(none)".
	 */
	const string getObjectQueueType() const;
	/**
	 Return bool if numberOfBuckets and bucketWidth is successfully specified
	 */
	bool getObjectQueueNumberOfBuckets(unsigned int &numberOfBuckets) const;
	bool getObjectQueueBucketWidth(unsigned int &bucketWidth) const;

	/**
	 If the GVTManager value is what is passed in, return true, else, false;
	 */
	bool gvtManagerTypeIs(const string &testValue) const;
	/**
	 Return the string of the GVTManager of the config file, or
	 "(none)".
	 */
	const string getGVTManagerType() const;

	/**
	 If the OptFossilCollManager value is what is passed in, return true, else, false;
	 */
	bool optFossilCollManagerTypeIs(const string &testValue) const;
	/**
	 Return the string of the OptFossilCollManager of the config file, or
	 "(none)".
	 */
	const string getOptFossilCollManagerType() const;

	bool simulationTypeIs(const string &testValue) const;
	const string getSimulationType() const;

	/**
	 Return the gvt period that was specified.
	 @return true if an int was found, false otherwise.
	 @param period The integer to write the period into if a period is
	 found, unchanged otherwise.
	 */
	bool getGVTPeriod(unsigned int &period);

	bool physicalLayerIs(const string &testValue) const;
	const string getPhysicalLayerType() const;

	bool outputManagerIs(const string &testValue) const;
	const string getOutputManagerType() const;

	bool getDynamicFilterDepth(unsigned int &filterDepth) const;
	bool getAggressive2Lazy(double &aggr2lazy) const;
	bool getLazy2Aggressive(double &lazy2aggr) const;
	bool getThirdThreshold(double &thirdThreshold) const;

	bool antiMessagesIs(const string &testValue) const;
	const string getAntiMessagesType() const;

	bool spinKeySet(const string &testValue) const;

	bool schedulerTypeIs(const string &testValue) const;
	const string getSchedulerType() const;
	const string getScheduleQScheme() const;
	bool getScheduleQCount(unsigned int &scheduleQCount) const;

	bool stateManagerTypeIs(const string &testValue) const;
	const string getStateManagerType() const;

	bool getStatePeriod(unsigned int &period) const;

	bool getWorkerThreadCount(unsigned int &workerThreadCount) const;

	const string getSyncMechanism() const;

	bool getOptFossilCollPeriod(unsigned int &period);

	bool getOptFossilCollMinSamples(unsigned int &minSamples);

	bool getOptFossilCollMaxSamples(unsigned int &maxSamples);

	bool getOptFossilCollDefaultLength(unsigned int &defaultLength);

	bool getOptFossilCollRiskFactor(double &riskFactor);

	bool getDVFSStringOption(string, string&) const;

	bool getDVFSIntOption(string, int&) const;

	bool getDVFSDoubleOption(string, double&) const;

	/**
	 Returns the binary name which started this execution.  Presumably we
	 will use this binary on every node.
	 */
	const string getBinaryName() const;

	~SimulationConfiguration();

private:
	SimulationConfiguration(const ConfigurationScope *outerScope,
			const vector<string> &argumentVector);

	class Implementation;
	Implementation *_impl;

	//converts strings to all uppercase, used in comparing strings
	const string stringToUpper(string s) const;
};

#endif
