#ifndef NOT_GATE_STUB_H
#define NOT_GATE_STUB_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "ObjectStub.h"

/** The NotGateStub class.

    This class is responsible for instantiating the NotGate
    object after it deciphers the arguments to the
    createSimulationObject method.
*/
class NotGateStub : public ObjectStub {
public:

   /**@name Public Class Methods of NotGateStub. */
   //@{

   /// Default Constructor
   NotGateStub(FactoryImplementationBase *owner) : ObjectStub(owner){}

   /// Default Destructor
   ~NotGateStub(){};
  
   /// get the name of this object
   string &getName() const {
      static string name("NotGate");
      return name;
   }

   /// get object information
   const string &getInformation() const {
      static string info("A simple not gate");
      return info;
   }
  
   /// is this object local
   const bool isLocalObject() const {
      return true;
   }
  
   /// create the simulation object
   SimulationObject *createSimulationObject(int numberOfArguments,
                                            ostrstream &argumentStream);

   //@} // End of Public Class Methods of NotGateStub.
};

#endif
