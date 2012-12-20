#ifndef IIR_FILTER_H
#define IIR_FILTER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <iostream>
#include "Filter.h"

/** The IIRFilter class.
    
    The IIR Filter is an implementation of a simple first-order lowpass
    filter. It has one input, one output and one parameter. Taking an
    input sequence of measurements {U(k)}, where k >= 0, and an output
    sequence {Y(k)}, k >= 0, the lowpass filter with a parameter R
    (0 <= R <= 1.0) is defined by the following equation:

    Assume that during initialization, Y(-1) = 0.
    
    Y(k) = R * U(k) + (1.0 - R) * Y(k - 1), where k >= 0. ---(1)

    The output of the lowpass filter is an average of its recent
    inputs. The parameter R is an aging factor that specifies how much
    contribution the old values have to the average. The filter has an
    internal state variable to store the previous output Y(k - 1).

    Note: Equation (1) can be rewritten as follows:
    
    Y(k) = R * U(k) + (1.0 - R) * Y(k - 1)
         = R * U(k) + Y(k - 1) - R * Y(k - 1)
         = Y(k - 1) + R * (U(k) - Y(k - 1))

    Final Form: Y(k) = Y(k - 1) + R * (U(k) - Y(k - 1)) ---(2)
*/
template <class Type>
class IIRFilter : public Filter<Type> {
public:

   /**@name Public Class Methods of IIRFilter */
   //@{

   /// Default Constructor
   IIRFilter(float parameter_R = 0.4);

   /// Default Destructor
   ~IIRFilter(){};

   /// reset the value of the filter
   inline void reset() {running_avg = 0;};

   /// add another sampled value to the filter buffer
   inline void update(Type);

   /// obtain the current value of the filter
   inline Type getData();

   /// set the aging factor for the IIR filter
   inline void setAgingFactor(float);

   // set aging factor and average of the IIR filter
   inline void setAgingFactorAndAvg(float, Type);

   /// obtain the aging factor of this filter
   inline float getAgingFactor() { return agingFactor; };

   //@} // End of Public Class Methods of IIRFilter

private:

   /**@name Private Class Attributes of IIRFilter */
   //@{

   /// the aging factor of this filter
   float agingFactor;

   /// the current running average
   Type running_avg;

   /// the new average
   Type new_avg;

   //@} // End of Private Class Attributes of IIRFilter
};


// This is the constructor code for the class IIR.
template <class Type>
IIRFilter<Type>::IIRFilter(float parameter_R) {
  agingFactor = parameter_R;
  running_avg = 0;
  new_avg = 0;
}


template <class Type>
inline void
IIRFilter<Type>::setAgingFactorAndAvg(float parameter_R, Type avg) {
  agingFactor = parameter_R;
  running_avg = avg;
  new_avg = 0;
}



// This is used to reset the weight to other values
template <class Type>
inline void IIRFilter<Type>::setAgingFactor(float parameter_R) {
  agingFactor = parameter_R;
}


// This is the method to enter new data into the IIR filter.  The
// argument is the actual data, and should be whatever type the
// template class was declared with.  This function returns nothing.
template <class Type>
inline void IIRFilter<Type>::update(Type new_sample) {
   // Note: we are using Equation (2) form here
   running_avg = running_avg + (Type)(agingFactor*(new_sample - running_avg));
}


// This is the method to read the output of the filter.  It returns the
// running average in the type the the template was declared with.
template <class Type>
inline Type IIRFilter<Type>::getData() {
  return running_avg;
}

#endif
  
