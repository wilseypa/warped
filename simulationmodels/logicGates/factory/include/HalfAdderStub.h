#ifndef HALF_ADDER_STUB_H
#define HALF_ADDER_STUB_H

#include "ObjectStub.h"

/** The HalfAdderStub class.

    This class is responsible for instantiating the HalfAdder object
    after it deciphers the arguments to the createSimulationObject
    method.
*/
class HalfAdderStub : public ObjectStub {
public:
   /**@name Public Class Methods of HalfAdderStub. */
   //@{

   /// Default Constructor
   HalfAdderStub(FactoryImplementationBase *owner):ObjectStub(owner){}


   /// Default Destructor
   ~HalfAdderStub(){};
  
   /// get the name of this object
   string &getName() const {
      static string name("HalfAdder");
      return name;
   }

   /// get object information
   const string &getInformation() const {
      static string info("A simple two bit half adder");
      return info;
   }
   
   /// is this object local
   const bool isLocalObject() const {
      return true;
   }
  
   /// create a HalfAdder simulation object with the input arguments
   SimulationObject* createSimulationObject(int numberOfArguments,
                                            ostrstream &argumentStream);
  
   //@} // End of Public Class Methods of HalfAdderStub.
};

#endif
