#ifndef N_INPUT_AND_GATE_STUB_H
#define N_INPUT_AND_GATE_STUB_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "ObjectStub.h"

/** The NInputAndGateStub class.

    This class is responsible for instantiating the NInputAndGate
    object after it deciphers the arguments to the
    createSimulationObject method.
*/
class NInputAndGateStub : public ObjectStub {
public:
   /**@name Public Class Methods of NInputAndGateStub. */
   //@{

   /// Default Constructor
   NInputAndGateStub(FactoryImplementationBase *owner):ObjectStub(owner){}

   /// Default Destructor
   ~NInputAndGateStub(){};
  
   /// get the name of this object
   string &getName() const {
    static string name("NInputAndGate");
    return name;
  }

   /// get object information
   const string &getInformation() const {
      static string info("A simple N input and gate");
      return info;
   }
   
   /// is this object local
   const bool isLocalObject() const {
      return true;
   }

   /// create the simulation object
   SimulationObject* createSimulationObject(int numberOfArguments,
                                            ostrstream &argumentStream);
   
   //@} // End of Public Class Methods of NInputAndGateStub.
};

#endif
