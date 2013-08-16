#ifndef EPIDEMIC_APPLICATION
#define EPIDEMIC_APPLICATION

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

#include <warped/Application.h>
#include <warped/IntVTime.h>

class EpidemicApplication : public Application {

public:

	/* Constructor */
	EpidemicApplication(string inputFileName, int numObjects);

	int initialize( vector<string> &arguments );

	int getNumberOfSimulationObjects(int mgrId) const;

	const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable );
  
	int finalize();

	void registerDeserializers();
  
	string getCommandLineParameters() const { return "foo"; }

	const VTime &getPositiveInfinity(){ return IntVTime::getIntVTimePositiveInfinity(); }

	const VTime &getZero(){ return IntVTime::getIntVTimeZero(); }

	const VTime &getTime(string &time ){ 
		IntVTime *vtime = new IntVTime (atoi(time.c_str())); 
		return *vtime;
	}

private:
	int numObjects;

	string inputFileName;
};

#endif
