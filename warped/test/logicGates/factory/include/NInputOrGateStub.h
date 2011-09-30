#ifndef N_INPUT_OR_GATE_STUB_H
#define N_INPUT_OR_GATE_STUB_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "ObjectStub.h"

/** The NInputOrGateStub class.

    This class is responsible for instantiating the NInputOrGate
    object after it deciphers the arguments to the
    createSimulationObject method.
*/
class NInputOrGateStub : public ObjectStub {
public:
   /**@name Public Class Methods of NInputOrGateStub. */
   //@{
   
   /// Default Constructor
   NInputOrGateStub(FactoryImplementationBase *owner):ObjectStub(owner){}

   /// Default Destructor
   ~NInputOrGateStub(){};
  
   /// get the name of this object
   string &getName() const {
      static string name("NInputOrGate");
      return name;
   }
  
   /// get object information
   const string &getInformation() const {
      static string info("A N input or gate");
      return info;
  }

   /// is this object local
   const bool isLocalObject() const {
      return true;
   }
  
   /// create the simulation object
   SimulationObject* createSimulationObject(int numberOfArguments,
                                            ostrstream &argumentStream);
  
   //@} // End of Public Class Methods of NInputOrGateStub.
};

#endif
