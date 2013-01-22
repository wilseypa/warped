#ifndef NARY_BIT_GENERATOR_STUB_H
#define NARY_BIT_GENERATOR_STUB_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "ObjectStub.h"

/** The NAryBitGeneratorStub class.

    This class is responsible for instantiating the NAryBitGenerator object
    after it deciphers the arguments to the createSimulationObject
    method.
*/
class NAryBitGeneratorStub : public ObjectStub {
public:
   /**@name Public Class Methods of NAryBitGeneratorStub. */
   //@{

   /// Default Constructor
   NAryBitGeneratorStub(FactoryImplementationBase *owner):ObjectStub(owner){}

   /// Default Destructor
   ~NAryBitGeneratorStub(){}
  
   /// get the name of this object
   string &getName() const {
      static string name("NAryBitGenerator");
      return name;
   }

   /// get object information
   const string &getInformation() const {
      static string info("A simple NAry bit generator");
      return info;
   }
  
   /// is this object local
   const bool isLocalObject() const {
      return true;
   }
  
   /// create a NAryBitGenerator simulation object with the input arguments
   SimulationObject* createSimulationObject(int numberOfArguments,
                                            ostrstream &argumentStream);
  
   //@} // End of Public Class Methods of NAryBitGeneratorStub.
};

#endif
