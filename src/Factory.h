#ifndef FACTORY_H
#define FACTORY_H


#include <string>
#include "Types.h"

using std::string;

/** The Factory abstract base class.  

    All classes related to the object factory subsystem must be
    derived from this base class.  The base class provides a common
    point for may disjoint classes such as the main-factory,
    sub-factory, object stubs etc. The base class is useful to provide
    an uniform interface for all these classes. This class cannot be
    directly instantiated.

**/
class Factory {
public:
   /**@name Public Class Methods of Factory */
   //@{
   
   /// Destructor.
   virtual ~Factory() {};
   
   /** Return the name of this factory object.

       This is a pure virtual function that has to be overridden.

       @return The name of this factory object.
   */
   virtual const string& getName() const = 0;

   /** Return the kind of this factory object.

       This is a pure virtual function that has to be overridden.

       @return The kind of this factory object.
   */
   virtual const Kind  getKind() const = 0;

   //@} // End of Factory Public Class Methods.

protected:
   
   /**@name Protected Class Methods of Factory */
   //@{

   /// Constructor
   Factory() {};

   //@} // End of Protected Class Methods of Factory.
};

#endif
