#ifndef ACTUATOR_IMPLEMENTATION_BASE_H
#define ACTUATOR_IMPLEMENTATION_BASE_H


#include "warped.h"
#include "Actuator.h"

/** The ActuatorImplementationBase implementation base class.

    The ActuatorImplementationBase class represents an implementation
    of the abstract Actuator abstract class. User instantiates this
    class.
    
*/
template <class ElementType>
class ActuatorImplementationBase : public Actuator {
public:

   /**@name Public Class Methods of ActuatorImplementationBase */
   //@{

   /// Constructor
   ActuatorImplementationBase(){};

   ~ActuatorImplementationBase(){};
   

   /// Actuate or set the new value of the control element
   void setValue( ElementType &newValue ){
      actuatorElement = newValue;
   }

   //@} // End of Public Class Methods of ActuatorImplementationBase

private:
   
   /**@name Private Class Attributes of ActuatorImplementationBase */
   //@{

   string actuatorName;
   ElementType actuatorElement;
   

   //@} // End of Private Class Attributes of ActuatorImplementationBase
   

};

#endif


