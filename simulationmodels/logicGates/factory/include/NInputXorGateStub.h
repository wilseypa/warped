#ifndef N_INPUT_XOR_GATE_STUB_H
#define N_INPUT_XOR_GATE_STUB_H

#include "ObjectStub.h"

/** The NInputXorGateStub class.

    This class is responsible for instantiating the NInputXorGate
    object after it deciphers the arguments to the
    createSimulationObject method.
*/
class NInputXorGateStub : public ObjectStub {
public:
   /**@name Public Class Methods of NInputXorGateStub. */
   //@{

   /// Default Constructor
   NInputXorGateStub(FactoryImplementationBase *owner):ObjectStub(owner){}

   /// Default Destructor
   ~NInputXorGateStub(){};
  
   /// get the name of this object
   string &getName() const {
    static string name("NInputXorGate");
    return name;
   }

   /// get object information
   const string &getInformation() const {
      static string info("A simple N input exclusive-or gate");
      return info;
   }
  
   /// is this object local
   const bool isLocalObject() const {
      return true;
   }

   /// create the simulation object
   SimulationObject* createSimulationObject(int numberOfArguments,
                                            ostrstream &argumentStream);
  
   //@} // End of Public Class Methods of NInputXorGateStub.
};

#endif
