#ifndef FILE_READERWRITER_H
#define FILE_READERWRITER_H

//See copyright notice in file Copyright in the root directory of this archive.

#include <warped/SimulationObject.h>
#include <warped/SimulationStream.h>
#include<vector>
#include<string>


using std::string;
using std::vector;

/**
		FileReaderWriter class definition.
*/

class FileReaderWriter : public SimulationObject {
	public:
	/**@name Public Class Methods of FileReaderWriter. */
   //@{
	/// Default Constructor
	FileReaderWriter(string &filename,int numgates,string io,vector<int> *desportid,
	                 vector<string> *desgatename,int maxnumlines);
	/// Default Destructor
  virtual ~FileReaderWriter();
  ///
	virtual const string &getName() const {return fileName; };
	/// initialize this object
  virtual void initialize();
							 
  /// finish up
	virtual void finalize();
										 
  /// execute an event
	virtual void executeProcess();
													 
	/// allocate state for the kernel
	virtual State* allocateState();
																 
  /// deallocate state for the kernel
  virtual void deallocateState(const State* state);
																			 
  /// delete this event
  virtual void reclaimEvent(const Event *event);
																						    
  /// report any errors in the simulation
	virtual void reportError(const string& msg, SEVERITY level);
																											
	/// get the number of outputs of this component
  //	virtual int getNumberOfOutputs() const { return numberOfOutputs; }
																														
	void sendEvent(const int outputBitValue); 
	
	/// get a handle to a simulation input stream
	SimulationStream* openInputFile(string& filename);
	 
  /// determine whether there are more lines for reading or not
  bool haveMoreLines(SimulationStream* simPt);
	
	/// read one line from the file 
  string getLine(SimulationStream* simPt, ostringstream& ost);
	
  /// clear the ostringstream object
  void clearOstringstream(ostringstream& ost);
			
  /// change the string value to the bit value
	int getBitValue(string logicBit);
	 
	/// read the file line by line 
//  void fileReader(SimulationStream* simPt_1,SimulationStream* simPt_2,ostringstream& ost);
	
	/// open the output file
	SimulationStream* openOutputFile(string& filename, ios::openmode mode);
  //@}//End of Public Class Methods of FileReaderWriter.

	/**@name Private Class Attributes of FileReaderWriter*/
	//@{
	private:

  /// input or output file name
	string fileName;

	/// indicate this file is input file or output file
  string IO;

  /// number of gates the file drive 
	int numOfGates;

  /// destination port id 
	vector<int> *desPortId;

	/// names of object which the input file connects to
	vector<string> *desGatesNames;

	/// simulation of the inputfile
	SimulationStream* fileIoStream;  

  /// number of lines in the file will be prcessed 
  int maxNumLines;

  /// set of destination object handles
	SimulationObject **outputHandles;

  //@} // End of Private Class Attributes of FileReaderWriter
};

#endif	
