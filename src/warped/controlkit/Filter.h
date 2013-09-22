#ifndef FILTER_H
#define FILTER_H


/** The Filter abstract base class.

    The Filter abstract base class represents the abstract interface
    for all the different implementations of filters. The base class
    is templatized so that any variable type can be filtered.

*/
template <class Type>
class Filter {
public:

   /**@name Public Class Methods of Filter */
   //@{

   Filter(){};

   virtual ~Filter(){};

   virtual void reset() = 0;
   virtual void update(Type) = 0;
   virtual Type getData() = 0;

   //@} // End of Public Class Methods of Filter 
};


#endif
