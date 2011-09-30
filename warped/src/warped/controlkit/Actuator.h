#ifndef ACTUATOR_H
#define ACTUATOR_H

// See copyright notice in file Copyright in the root directory of this archive.

/** The Actuator base class.

    The Actuator class represents those elements in the application
    code that manipulate a particular variable depending on
    certain pre-conditions.
    
*/
class Actuator {
public:

   /**@name Public Class Methods of Actuator */
   //@{

   /// Constructor
   Actuator(){};

   virtual ~Actuator(){};
   
   //@} // End of Public Class Methods of Actuator

};

#endif


