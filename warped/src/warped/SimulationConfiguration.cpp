// See copyright notice in file Copyright in the root directory of this archive.

#include <algorithm> ///std::transform
#include <utils/tokens.h>
#include <utils/ConfigurationParserHandle.h>
#include <utils/ConfigurationScope.h>
#include "SimulationConfiguration.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>

class SimulationConfiguration::Implementation {
public:
	const vector<string> &getArguments() {
		return myArguments;
	}

	bool communicationManagerIs(const string &testValue) const;
	const string getCommunicationManagerType() const;

	bool eventListTypeIs(const string &testValue) const;
	bool eventListOrganizationIs(const string &testValue) const;

	const string getEventListType() const;
	const string getEventListOrganization() const;

	bool objectQueueTypeIs(const string &testValue) const;

	const string getObjectQueueType() const;
	bool getObjectQueueNumberOfBuckets(unsigned int &bucketWidth) const;
	bool getObjectQueueBucketWidth(unsigned int &bucketWidth) const;

	bool gvtManagerTypeIs(const string &testValue) const;
	const string getGVTManagerType() const;
	bool getGVTPeriod(unsigned int &period);

	bool simulationTypeIs(const string &testValue) const;
	const string getSimulationType() const;

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

	bool optFossilCollManagerTypeIs(const string &testValue) const;
	const string getOptFossilCollManagerType() const;
	bool getOptFossilCollPeriod(unsigned int &period);
	bool getOptFossilCollMinSamples(unsigned int &minSamples);
	bool getOptFossilCollMaxSamples(unsigned int &maxSamples);
	bool getOptFossilCollDefaultLength(unsigned int &defaultLength);
	bool getOptFossilCollRiskFactor(double &riskFactor);

	bool getDVFSStringOption(string&, string&) const;
	bool getDVFSIntOption(string&, int&) const;
	bool getDVFSDoubleOption(string&, double&) const;

	const string getBinaryName() const;

	bool getWorkerThreadCount(unsigned int &workerThreadCount) const;
	const string getSyncMechanism() const;

	Implementation() :
		myOuterScope(0) {
	}

	Implementation(const ConfigurationScope *initScope,
			const vector<string> &arguments) :
		myOuterScope(initScope), myArguments(arguments) {
	}

	~Implementation() {
		delete myOuterScope;
	}

private:
	///Convert strings to upper case
	const inline string stringToUpper(string s) const {
		std::transform(s.begin(), s.end(), s.begin(),
				(int(*)(int)) std::toupper);
		return s;
	}
	/**
	 Returns the outer ConfigurationScope - i.e. global level options, and access
	 to all "first level" scopes.
	 */
	const ConfigurationScope &getOuterScope() const;

	/**
	 Finds the global level configuration choice "choiceName".
	 @param choiceName The choice name to look for.
	 @return The choice, if it exists in the outerscope, or NULL if it doesn't.
	 */
	const ConfigurationChoice *findChoice(const string &choiceName) const;

	/**
	 Finds the global level configuration scope "scopeName".
	 @param scopeName The scope name to look for.
	 @return The scope, if it exists in the outerscope, or NULL if it doesn't.
	 */
	const ConfigurationScope *findScope(const string &scopeName) const;

	/**
	 Returns the scope for TimeWarp::CommunicationManager or null if not
	 found.
	 */
	const ConfigurationScope *getTimeWarpScope() const;

	/**
	 Returns the ConfigurationScope for Thread Information
	 */
	const ConfigurationScope *getThreadControlScope() const;

	/**
	 Returns the scope for TimeWarp::CommunicationManager or null if not
	 found.
	 */
	const ConfigurationScope *getCommunicationManagerScope() const;

	const ConfigurationScope *getGVTManagerScope() const;

	/**
	 Returns the ConfigurationScope for TimeWarp::EventList
	 */
	const ConfigurationScope *getEventListScope() const;

	/**
	 Returns the ConfigurationChoice for TimeWarp::EventList::EventOrganization
	 */
	const ConfigurationChoice *getEventListOrganizationChoice() const;
	/**
	 Returns ConfigurationChoice for TimeWarp::EventList::Type
	 */
	const ConfigurationChoice *getEventListTypeChoice() const;

	/**
	 Returns the ConfigurationScope for TimeWarp::ObjectQueue
	 */
	const ConfigurationScope *getObjectQueueScope() const;
	/**
	 Returns the ConfigurationChoice for TimeWarp::ObjectQueue
	 */
	const ConfigurationChoice *getObjectQueueTypeChoice() const;

	const ConfigurationScope *getOutputManagerScope() const;
	const ConfigurationScope *getSchedulerScope() const;

	/**
	 Returns ConfigurationScope for TimeWarp::StateManager
	 */
	const ConfigurationScope *getStateManagerScope() const;
	/**
	 Returns ConfigurationChoice for TimeWarp::StateManager::Type
	 */
	const ConfigurationChoice *getStateManagerTypeChoice() const;
	/**
	 Returns ConfigurationChoice for TimeWarp::StateManager::Period
	 */
	const ConfigurationChoice *getStateManagerPeriodChoice() const;

	/**
	 Returns ConfigurationScope for TimeWarp::OptFossilCollectionManager
	 */
	const ConfigurationScope *getOptFossilCollManagerScope() const;

	/**
	 Returns ConfigurationScope for TimeWarp::OptFossilCollectionManager
	 */
	const ConfigurationScope *getDVFSManagerScope() const;

	static const string &getCommManagerScopeName();
	static const string &getEventListScopeName();
	static const string &getObjectQueueScopeName();
	static const string &getOrganizationName();
	static const string &getPeriodName();
	static const string &getStateManagerScopeName();
	static const string &getTypeName();
	static const string &getTimeWarpScopeName();
	static const string &getOutputManagerScopeName();

	const ConfigurationScope *myOuterScope;
	const vector<string> myArguments;
};

SimulationConfiguration::SimulationConfiguration(
		const ConfigurationScope *outerScope,
		const vector<string> &argumentVector) :
			_impl(
					new SimulationConfiguration::Implementation(outerScope,
							argumentVector)) {
}

SimulationConfiguration::~SimulationConfiguration() {
	delete _impl;
}

SimulationConfiguration *
SimulationConfiguration::parseConfiguration(const string &configFileName,
		const vector<string> &argumentVector) {
	SimulationConfiguration *configuration = 0;
	const ConfigurationScope *outerScope =
			ConfigurationParserHandle::parseFile(configFileName);
	if (outerScope != 0) {
		configuration = new SimulationConfiguration(outerScope, argumentVector);
	}

	return configuration;
}

const vector<string> &
SimulationConfiguration::getArguments() {
	return _impl->getArguments();
}

bool SimulationConfiguration::communicationManagerIs(const string &testValue) const {
	return _impl->communicationManagerIs(testValue);
}

const string SimulationConfiguration::getCommunicationManagerType() const {
	return _impl->getCommunicationManagerType();
}

bool SimulationConfiguration::eventListTypeIs(const string &testValue) const {
	return _impl->eventListTypeIs(testValue);
}

bool SimulationConfiguration::eventListOrganizationIs(const string &testValue) const {
	return _impl->eventListOrganizationIs(testValue);
}

bool SimulationConfiguration::objectQueueTypeIs(const string &testValue) const {
	return _impl->objectQueueTypeIs(testValue);
}

const string SimulationConfiguration::getEventListType() const {
	return _impl->getEventListType();
}

const string SimulationConfiguration::getEventListOrganization() const {
	return _impl->getEventListOrganization();
}

const string SimulationConfiguration::getObjectQueueType() const {
	return _impl->getObjectQueueType();
}

bool SimulationConfiguration::getObjectQueueNumberOfBuckets(
		unsigned int &numberOfBuckets) const {
	return _impl->getObjectQueueNumberOfBuckets(numberOfBuckets);
}

bool SimulationConfiguration::getObjectQueueBucketWidth(
		unsigned int &bucketWidth) const {
	return _impl->getObjectQueueBucketWidth(bucketWidth);
}

bool SimulationConfiguration::gvtManagerTypeIs(const string &testValue) const {
	return _impl->gvtManagerTypeIs(testValue);
}

const string SimulationConfiguration::getGVTManagerType() const {
	return _impl->getGVTManagerType();
}

bool SimulationConfiguration::optFossilCollManagerTypeIs(
		const string &testValue) const {
	return _impl->optFossilCollManagerTypeIs(testValue);
}

const string SimulationConfiguration::getOptFossilCollManagerType() const {
	return _impl->getOptFossilCollManagerType();
}

bool SimulationConfiguration::simulationTypeIs(const string &testValue) const {
	return _impl->simulationTypeIs(testValue);
}

const string SimulationConfiguration::getSimulationType() const {
	return _impl->getSimulationType();
}

bool SimulationConfiguration::getGVTPeriod(unsigned int &period) {
	return _impl->getGVTPeriod(period);
}

bool SimulationConfiguration::physicalLayerIs(const string &testValue) const {
	return _impl->physicalLayerIs(testValue);
}

const string SimulationConfiguration::getPhysicalLayerType() const {
	return _impl->getPhysicalLayerType();
}

bool SimulationConfiguration::outputManagerIs(const string &testValue) const {
	return _impl->outputManagerIs(testValue);
}

const string SimulationConfiguration::getOutputManagerType() const {
	return _impl->getOutputManagerType();
}

bool SimulationConfiguration::antiMessagesIs(const string &testValue) const {
	return _impl->antiMessagesIs(testValue);
}

const string SimulationConfiguration::getAntiMessagesType() const {
	return _impl->getAntiMessagesType();
}

bool SimulationConfiguration::spinKeySet(const string &testValue) const {
	return _impl->spinKeySet(testValue);
}

bool SimulationConfiguration::schedulerTypeIs(const string &testValue) const {
	return _impl->schedulerTypeIs(testValue);
}

const string SimulationConfiguration::getSchedulerType() const {
	return _impl->getSchedulerType();
}

const string SimulationConfiguration::getScheduleQScheme() const {
	return _impl->getScheduleQScheme();
}

bool SimulationConfiguration::getScheduleQCount(
		unsigned int &scheduleQCount) const {
	return _impl->getScheduleQCount(scheduleQCount);
}

bool SimulationConfiguration::stateManagerTypeIs(const string &testValue) const {
	return _impl->stateManagerTypeIs(testValue);
}

const string SimulationConfiguration::getStateManagerType() const {
	return _impl->getStateManagerType();
}

bool SimulationConfiguration::getStatePeriod(unsigned int &period) const {
	return _impl->getStatePeriod(period);
}

bool SimulationConfiguration::getDynamicFilterDepth(unsigned int &filterDepth) const {
	return _impl->getDynamicFilterDepth(filterDepth);
}

bool SimulationConfiguration::getAggressive2Lazy(double &aggr2lazy) const {
	return _impl->getAggressive2Lazy(aggr2lazy);
}

bool SimulationConfiguration::getLazy2Aggressive(double &lazy2aggr) const {
	return _impl->getLazy2Aggressive(lazy2aggr);
}

bool SimulationConfiguration::getThirdThreshold(double &thirdThreshold) const {
	return _impl->getThirdThreshold(thirdThreshold);
}

bool SimulationConfiguration::getOptFossilCollPeriod(unsigned int &period) {
	return _impl->getOptFossilCollPeriod(period);
}

bool SimulationConfiguration::getOptFossilCollMinSamples(
		unsigned int &minSamples) {
	return _impl->getOptFossilCollMinSamples(minSamples);
}

bool SimulationConfiguration::getOptFossilCollMaxSamples(
		unsigned int &maxSamples) {
	return _impl->getOptFossilCollMaxSamples(maxSamples);
}

bool SimulationConfiguration::getOptFossilCollDefaultLength(
		unsigned int &defaultLength) {
	return _impl->getOptFossilCollDefaultLength(defaultLength);
}

bool SimulationConfiguration::getOptFossilCollRiskFactor(double &riskFactor) {
	return _impl->getOptFossilCollRiskFactor(riskFactor);
}

bool SimulationConfiguration::getDVFSStringOption(string opt, string& val) const {
	return _impl->getDVFSStringOption(opt, val);
}

bool SimulationConfiguration::getDVFSIntOption(string opt, int& val) const {
	return _impl->getDVFSIntOption(opt, val);
}

bool SimulationConfiguration::getDVFSDoubleOption(string opt, double& val) const {
	return _impl->getDVFSDoubleOption(opt, val);
}

const string SimulationConfiguration::getBinaryName() const {
	return _impl->getBinaryName();
}

bool SimulationConfiguration::getWorkerThreadCount(
		unsigned int &workerThreadCount) const {
	return _impl->getWorkerThreadCount(workerThreadCount);
}

const string SimulationConfiguration::getSyncMechanism() const {
	return _impl->getSyncMechanism();
}

const ConfigurationChoice *
SimulationConfiguration::Implementation::findChoice(const string &choiceName) const {
	return myOuterScope->findChoice(choiceName);
}

const ConfigurationScope *
SimulationConfiguration::Implementation::findScope(const string &scopeName) const {
	return myOuterScope->findScope(scopeName);
}

const ConfigurationScope &
SimulationConfiguration::Implementation::getOuterScope() const {
	return *myOuterScope;
}

const ConfigurationScope *
SimulationConfiguration::Implementation::getTimeWarpScope() const {
	return findScope(getTimeWarpScopeName());
}

const ConfigurationScope *
SimulationConfiguration::Implementation::getCommunicationManagerScope() const {
	const ConfigurationScope *twScope = getTimeWarpScope();
	return twScope->findScope(getCommManagerScopeName());
}

const ConfigurationScope *
SimulationConfiguration::Implementation::getEventListScope() const {
	const ConfigurationScope *retval = 0;
	string simType = stringToUpper(getSimulationType());
	if (simType == "SEQUENTIAL") {
		retval = findScope(getEventListScopeName());
	} else if (simType == "TIMEWARP" || simType
			== "THREADEDTIMEWARP") {
		const ConfigurationScope *twScope = getTimeWarpScope();
		if (twScope != 0) {
			retval = twScope->findScope(getEventListScopeName());
		}
	}

	return retval;
}

const ConfigurationChoice *
SimulationConfiguration::Implementation::getEventListOrganizationChoice() const {
	const ConfigurationChoice *retval = 0;
	if (getEventListScope() != 0) {
		retval = getEventListScope()->findChoice(getOrganizationName());
	}
	return retval;
}

/*const ConfigurationChoice *
 SimulationConfiguration::Implementation::getEventListTypeChoice() const {
 const ConfigurationChoice *retval = 0;
 if( getEventListScope() != 0 ){
 retval = getEventListScope()->findChoice( getTypeName() );
 }
 return retval;
 }*/

const ConfigurationScope *
SimulationConfiguration::Implementation::getObjectQueueScope() const {
	const ConfigurationScope *retval = 0;
	string simType = stringToUpper(getSimulationType());
	const ConfigurationScope *twScope = getTimeWarpScope();
	if (twScope != 0) {
		retval = twScope->findScope(getObjectQueueScopeName());
	}
	return retval;
}

/*const ConfigurationChoice *
 SimulationConfiguration::Implementation::getObjectQueueTypeChoice() const {
 const ConfigurationChoice *retval = 0;
 if( getEventListScope() != 0 ){
 retval = getObjectQueueScope()->findChoice( getTypeName() );
 }
 return retval;
 }*/

const ConfigurationScope *
SimulationConfiguration::Implementation::getStateManagerScope() const {
	const ConfigurationScope *retval = 0;
	if (getTimeWarpScope() != 0) {
		retval = getTimeWarpScope()->findScope(getStateManagerScopeName());
	}
	return retval;
}

const ConfigurationChoice *
SimulationConfiguration::Implementation::getStateManagerTypeChoice() const {
	const ConfigurationChoice *retval = 0;
	if (getStateManagerScope() != 0) {
		retval = getStateManagerScope()->findChoice(getTypeName());
	}
	return retval;
}

const ConfigurationChoice *
SimulationConfiguration::Implementation::getStateManagerPeriodChoice() const {
	const ConfigurationChoice *retval = 0;
	if (getStateManagerScope() != 0) {
		retval = getStateManagerScope()->findChoice(getPeriodName());
	}
	return retval;
}

const ConfigurationScope *
SimulationConfiguration::Implementation::getGVTManagerScope() const {
	const ConfigurationScope *retval = 0;
	if (getTimeWarpScope() != 0) {
		retval = getTimeWarpScope()->findScope("GVTManager");
	}
	return retval;
}

const ConfigurationScope *
SimulationConfiguration::Implementation::getOutputManagerScope() const {
	const ConfigurationScope *retval = 0;
	if (getTimeWarpScope() != 0) {
		retval = getTimeWarpScope()->findScope("OutputManager");
	}
	return retval;
}

const ConfigurationScope *
SimulationConfiguration::Implementation::getSchedulerScope() const {
	const ConfigurationScope *retval = 0;
	if (getTimeWarpScope() != 0) {
		retval = getTimeWarpScope()->findScope("Scheduler");
	}
	return retval;
}

const ConfigurationScope *
SimulationConfiguration::Implementation::getOptFossilCollManagerScope() const {
	const ConfigurationScope *retval = 0;
	if (getTimeWarpScope() != 0) {
		retval = getTimeWarpScope()->findScope("OptFossilCollManager");
	}
	return retval;
}

const ConfigurationScope*
SimulationConfiguration::Implementation::getDVFSManagerScope() const {
	if (const ConfigurationScope* twScope = getTimeWarpScope())
		return twScope->findScope("DVFSManager");
	return NULL;
}

const string &
SimulationConfiguration::Implementation::getCommManagerScopeName() {
	static const string commMgrScope = "CommunicationManager";
	return commMgrScope;
}

const string &
SimulationConfiguration::Implementation::getEventListScopeName() {
	static const string eventListScopeName = "EventList";
	return eventListScopeName;
}

const string &
SimulationConfiguration::Implementation::getOrganizationName() {
	static const string orgName = "Organization";
	return orgName;
}

const ConfigurationScope *
SimulationConfiguration::Implementation::getThreadControlScope() const {
	const ConfigurationScope *retval = 0;
	if (getTimeWarpScope() != 0) {
		retval = getTimeWarpScope()->findScope("ThreadControl");
	}
	return retval;
}

const string &
SimulationConfiguration::Implementation::getObjectQueueScopeName() {
	static const string objQueueName = "ObjectQueue";
	return objQueueName;
}

const string &
SimulationConfiguration::Implementation::getPeriodName() {
	static const string periodName = "Period";
	return periodName;
}

const string &
SimulationConfiguration::Implementation::getStateManagerScopeName() {
	static const string stateManagerScopeName = "StateManager";
	return stateManagerScopeName;
}

const string &
SimulationConfiguration::Implementation::getOutputManagerScopeName() {
	static const string outputManagerScopeName = "OutputManager";
	return outputManagerScopeName;
}

const string &
SimulationConfiguration::Implementation::getTypeName() {
	static const string typeName = "Type";
	return typeName;
}

const string &
SimulationConfiguration::Implementation::getTimeWarpScopeName() {
	static const string timeWarpScope = "TimeWarp";
	return timeWarpScope;
}

const string SimulationConfiguration::Implementation::getCommunicationManagerType() const {
	string retval = "(none)";
	if (getCommunicationManagerScope() != 0) {
		const ConfigurationChoice *type =
				getCommunicationManagerScope()->findChoice("Type");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::communicationManagerIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(
			getCommunicationManagerType()));
}

const string SimulationConfiguration::Implementation::getEventListType() const {
	string retval = "(none)";
	if (getEventListScope() != 0) {
		const ConfigurationChoice *type = getEventListScope()->findChoice(
				getTypeName());
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::eventListTypeIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(getEventListType()));
}

const string SimulationConfiguration::Implementation::getEventListOrganization() const {
	string retval = "(none)";
	if (getEventListScope() != 0) {
		const ConfigurationChoice *Organization =
				getEventListScope()->findChoice(getOrganizationName());
		if (Organization != 0) {
			retval = stringToUpper(Organization->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::eventListOrganizationIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(
			getEventListOrganization()));
}

const string SimulationConfiguration::Implementation::getObjectQueueType() const {
	string retval = "(none)";
	if (getObjectQueueScope() != 0) {
		const ConfigurationChoice *type = getObjectQueueScope()->findChoice(
				getTypeName());
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::objectQueueTypeIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(getObjectQueueType()));
}
bool SimulationConfiguration::Implementation::getObjectQueueNumberOfBuckets(
		unsigned int &numberOfBuckets) const {
	bool retval = false;
	const ConfigurationScope *scope = getObjectQueueScope();
	if (scope != 0) {
		if (scope->getIntValue("NumberOfBuckets") != -1) {
			numberOfBuckets = scope->getIntValue("NumberOfBuckets");
			retval = true;
		}
	}
	return retval;
}
bool SimulationConfiguration::Implementation::getObjectQueueBucketWidth(
		unsigned int &bucketWidth) const {
	bool retval = false;
	const ConfigurationScope *scope = getObjectQueueScope();
	if (scope != 0) {
		if (scope->getIntValue("BucketWidth") != -1) {
			bucketWidth = scope->getIntValue("BucketWidth");
			retval = true;
		}
	}
	return retval;
}

const string SimulationConfiguration::Implementation::getGVTManagerType() const {
	string retval = "(none)";
	if (getGVTManagerScope() != 0) {
		const ConfigurationChoice *type = getGVTManagerScope()->findChoice(
				"Type");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::gvtManagerTypeIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(getGVTManagerType()));
}

const string SimulationConfiguration::Implementation::getOptFossilCollManagerType() const {
	string retval = "(none)";
	if (getOptFossilCollManagerScope() != 0) {
		const ConfigurationChoice *type =
				getOptFossilCollManagerScope()->findChoice("Type");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::optFossilCollManagerTypeIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(
			getOptFossilCollManagerType()));
}

const string SimulationConfiguration::Implementation::getSimulationType() const {
	string retval = "(none)";
	if (findChoice("Simulation") != 0) {
		const ConfigurationChoice *type = findChoice("Simulation");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::simulationTypeIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(getSimulationType()));
}

bool SimulationConfiguration::Implementation::getGVTPeriod(unsigned int &period) {
	bool retval = false;
	if (getGVTManagerScope() != 0) {
		if (getGVTManagerScope()->findChoice("Period") != 0) {
			period = getGVTManagerScope()->findChoice("Period")->getIntValue();
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::physicalLayerIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(getPhysicalLayerType()));
}

const string SimulationConfiguration::Implementation::getPhysicalLayerType() const {
	string retval = "(none)";
	if (getCommunicationManagerScope() != 0) {
		const ConfigurationChoice *type =
				getCommunicationManagerScope()->findChoice("PhysicalLayer");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

const string SimulationConfiguration::Implementation::getOutputManagerType() const {
	string retval = "(none)";
	if (getOutputManagerScope() != 0) {
		const ConfigurationChoice *type = getOutputManagerScope()->findChoice(
				"Type");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::outputManagerIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(getOutputManagerType()));
}

const string SimulationConfiguration::Implementation::getAntiMessagesType() const {
	string retval = "(none)";
	if (getOutputManagerScope() != 0) {
		const ConfigurationChoice *type = getOutputManagerScope()->findChoice(
				"AntiMessages");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::antiMessagesIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(getAntiMessagesType()));
}

const string SimulationConfiguration::Implementation::getSchedulerType() const {
	string retval = "(none)";
	if (getSchedulerScope() != 0) {
		const ConfigurationChoice *type = getSchedulerScope()->findChoice(
				"Type");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

const string SimulationConfiguration::Implementation::getScheduleQScheme() const {
	string retval = "(none)";
	if (getSchedulerScope() != 0) {
		const ConfigurationChoice *type = getSchedulerScope()->findChoice(
				"ScheduleQScheme");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getScheduleQCount(
		unsigned int &scheduleQCount) const {
	bool retval = false;
	const ConfigurationScope *scope = getSchedulerScope();
	if (scope != 0) {
		if (scope->getIntValue("ScheduleQCount") != -1) {
			scheduleQCount = scope->getIntValue("ScheduleQCount");
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::schedulerTypeIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(getSchedulerType()));
}

bool SimulationConfiguration::Implementation::spinKeySet(
		const string &testValue) const {
	bool retval = false;
	const ConfigurationScope *debugScope = findScope("ParallelDebug");
	if (debugScope != 0) {
		const ConfigurationChoice *choice = debugScope->findChoice(testValue);
		if (choice != 0) {
			retval = stringToUpper(choice->getStringValue()) == "TRUE";
		}
	}
	return retval;
}

const string SimulationConfiguration::Implementation::getStateManagerType() const {
	string retval = "(none)";
	if (getStateManagerScope() != 0) {
		const ConfigurationChoice *type = getStateManagerScope()->findChoice(
				"Type");
		if (type != 0) {
			retval = stringToUpper(type->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::stateManagerTypeIs(
		const string &testValue) const {
	return (stringToUpper(testValue) == stringToUpper(getStateManagerType()));
}

bool SimulationConfiguration::Implementation::getStatePeriod(
		unsigned int &period) const {
	bool retval = false;
	const ConfigurationScope *scope = getStateManagerScope();
	if (scope != 0) {
		if (scope->getIntValue("Period") != -1) {
			period = scope->getIntValue("Period");
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getDynamicFilterDepth(
		unsigned int &filterDepth) const {
	bool retval = false;
	const ConfigurationScope *scope = getOutputManagerScope();
	if (scope != 0) {
		if (scope->findChoice("FilterDepth") != 0) {
			filterDepth = scope->findChoice("FilterDepth")->getIntValue();
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getAggressive2Lazy(
		double &aggr2lazy) const {
	bool retval = false;
	const ConfigurationScope *scope = getOutputManagerScope();
	if (scope != 0) {
		if (scope->findChoice("AggrToLazyRatio") != 0) {
			aggr2lazy = scope->getDoubleValue("AggrToLazyRatio");
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getLazy2Aggressive(
		double &lazy2aggr) const {
	bool retval = false;
	const ConfigurationScope *scope = getOutputManagerScope();
	if (scope != 0) {
		if (scope->findChoice("LazyToAggrRatio") != 0) {
			lazy2aggr = scope->getDoubleValue("LazyToAggrRatio");
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getThirdThreshold(
		double &thirdThreshold) const {
	bool retval = false;
	const ConfigurationScope *scope = getOutputManagerScope();
	if (scope != 0) {
		if (scope->findChoice("ThirdThreshold") != 0) {
			thirdThreshold = scope->getDoubleValue("ThirdThreshold");
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getWorkerThreadCount(
		unsigned int &workerThreadCount) const {
	bool retval = false;
	const ConfigurationScope *scope = getThreadControlScope();
	if (scope != 0) {
		if (scope->getIntValue("WorkerThreadCount") != -1) {
			workerThreadCount = scope->getIntValue("WorkerThreadCount");
			retval = true;
		}
	}
	return retval;
}

const string SimulationConfiguration::Implementation::getSyncMechanism() const {
	string retval = "(none)";
	if (getThreadControlScope() != 0) {
		const ConfigurationChoice *scope = getThreadControlScope()->findChoice(
				"SyncMechanism");
		if (scope != 0) {
			retval = stringToUpper(scope->getStringValue());
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getOptFossilCollPeriod(
		unsigned int &period) {
	bool retval = false;
	if (getOptFossilCollManagerScope() != 0) {
		if (getOptFossilCollManagerScope()->findChoice("CheckpointTime") != 0) {
			period = getOptFossilCollManagerScope()->findChoice(
					"CheckpointTime")->getIntValue();
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getOptFossilCollMinSamples(
		unsigned int &minSamples) {
	bool retval = false;
	if (getOptFossilCollManagerScope() != 0) {
		if (getOptFossilCollManagerScope()->findChoice("MinimumSamples") != 0) {
			minSamples = getOptFossilCollManagerScope()->findChoice(
					"MinimumSamples")->getIntValue();
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getOptFossilCollMaxSamples(
		unsigned int &maxSamples) {
	bool retval = false;
	if (getOptFossilCollManagerScope() != 0) {
		if (getOptFossilCollManagerScope()->findChoice("MaximumSamples") != 0) {
			maxSamples = getOptFossilCollManagerScope()->findChoice(
					"MaximumSamples")->getIntValue();
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getOptFossilCollDefaultLength(
		unsigned int &defaultLength) {
	bool retval = false;
	if (getOptFossilCollManagerScope() != 0) {
		if (getOptFossilCollManagerScope()->findChoice("DefaultLength") != 0) {
			defaultLength = getOptFossilCollManagerScope()->findChoice(
					"DefaultLength")->getIntValue();
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getOptFossilCollRiskFactor(
		double &riskFactor) {
	bool retval = false;
	if (getOptFossilCollManagerScope() != 0) {
		if (getOptFossilCollManagerScope()->findChoice("AcceptableRisk") != 0) {
			riskFactor = getOptFossilCollManagerScope()->getDoubleValue(
					"AcceptableRisk");
			retval = true;
		}
	}
	return retval;
}

bool SimulationConfiguration::Implementation::getDVFSStringOption(string& opt,
		string& val) const {
	if (const ConfigurationScope* cfmScope = getDVFSManagerScope()) {
		if (cfmScope->findChoice(opt)) {
			val = stringToUpper(cfmScope->getStringValue(opt));
			return true;
		}
	}
	return false;
}

bool SimulationConfiguration::Implementation::getDVFSIntOption(string& opt,
		int& val) const {
	if (const ConfigurationScope* cfmScope = getDVFSManagerScope()) {
		if (cfmScope->findChoice(opt)) {
			val = cfmScope->getIntValue(opt);
			return true;
		}
	}
	return false;
}

bool SimulationConfiguration::Implementation::getDVFSDoubleOption(string& opt,
		double& val) const {
	if (const ConfigurationScope* cfmScope = getDVFSManagerScope()) {
		if (cfmScope->findChoice(opt)) {
			val = cfmScope->getDoubleValue(opt);
			return true;
		}
	}
	return false;
}

const string SimulationConfiguration::Implementation::getBinaryName() const {
	string retval = "";
	if (!myArguments.empty()) {
		retval = myArguments[0];
	}
	return retval;
}
