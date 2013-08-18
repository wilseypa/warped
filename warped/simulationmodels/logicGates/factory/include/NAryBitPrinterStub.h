#ifndef NARY_BIT_PRINTER_STUB_H
#define NARY_BIT_PRINTER_STUB_H

#include "ObjectStub.h"

/** The NAryBitPrinterStub class.

    This class is responsible for instantiating the NAryBitPrinter object
    after it deciphers the arguments to the createSimulationObject
    method.
*/
class NAryBitPrinterStub : public ObjectStub {
public:
   
   /**@name Public Class Methods of NAryBitPrinterStub. */
   //@{

   /// Default Constructor
   NAryBitPrinterStub(FactoryImplementationBase *owner):ObjectStub(owner){}

   /// Default Destructor
   ~NAryBitPrinterStub(){}
  
   /// get the name of this object
   string &getName() const {
      static string name("NAryBitPrinter");
      return name;
   }

   /// get object information
   const string &getInformation() const {
      static string info("A simple NAry bit printer");
      return info;
   }
  
   /// is this object local
   const bool isLocalObject() const {
      return true;
   }
  
   /// create a NAryBitPrinter simulation object with the input arguments
   SimulationObject* createSimulationObject(int numberOfArguments,
                                            ostrstream &argumentStream);
  
   //@} // End of Public Class Methods of NAryBitPrinterStub.
};

#endif
