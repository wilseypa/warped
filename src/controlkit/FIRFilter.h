#ifndef FIR_FILTER_H
#define FIR_FILTER_H

#include <iostream>
#include "Filter.h"

/** The FIRFilter class.

    This template class implements an nth order FIR filter, where n is
    specified by the user.
    Replace "double" with whatever type you'd like it to input and output,
    and "3" with whatever order filter you'd like to use.  If you specify
    a 0 or a 1 for a filter order, an error message is generated.
*/
template <class Type>
class FIRFilter : public Filter<Type> {
public:

    /**@name Public Class Methods of FIRFilter */
    //@{

    /// Default Constructor
    FIRFilter(int n = 1000);

    /// Default Destructor
    ~FIRFilter();

    /// reset the value of the filter
    inline void reset();

    /// add another sampled value to the set being filtered
    inline void update(Type);

    /// obtain the current value of the filter
    inline Type getData();

    //@} // End of Public Class Methods of FIRFilter

protected:

    /**@name Protected Class Methods of FIRFilter */
    //@{

    /// return the number of valid data points
    inline int numDatapoints() { return (num_valid); };

    //@} // End of Protected Class Methods of FIRFilter

private:

    /**@name Private Class Attributed of FIRFilter */
    //@{

    /// size of the filter buffer
    unsigned int size;

    /// number of valid entries
    unsigned int num_valid;

    /// index into the filter array
    unsigned int index;

    /// the current value of the filter
    Type current_sum;

    /// the filter buffer/array
    vector<Type> fir_array;

    //@} // End of Private Class Attributed of FIRFilter
};


// This is the constructor code for the class FIR. The integer passed into
// it is the order of the filter.
template <class Type>
inline FIRFilter<Type>::FIRFilter(int n) {
    if (n <= 1) {
        std::cerr << "A window size of " << n <<" makes NO sense!" << std::endl;
    }

    size = n;
    num_valid = 0;
    current_sum = 0;
    index = 0;

    fir_array.resize(size);
    for (unsigned int i = 0; i < size; i++) {
        fir_array[i] = 0;
    }
}


// This is the destructor method for the class FIR.
template <class Type>
inline FIRFilter<Type>::~FIRFilter() {
}

// zero out all the stored entries.
template <class Type>
inline void
FIRFilter<Type>::reset() {
    num_valid = 0;
    for (unsigned int i = 0; i < size; i++) {
        fir_array[i] = 0;
    }
}


// This is the method which calculates and returns the current
// output of the filter.  If the filter isn't full yet, then the
// average is computed on the valid data only.  The return type is
// whatever the template was initialized with.
template <class Type>
inline
Type FIRFilter<Type>::getData() {
    if (num_valid < size) {
        if (num_valid != 0) {
            return (current_sum / num_valid);
        } else {
            return 0;
        }
    } else {
        return (current_sum / (Type)size);
    }
}


// This is the method that updates the current state of the filter with
// a new data point.  The type of the new data should be the same as
// the type that the filter was initialized with.  The variable
// num_valid will overflow if this method is called more than 2*INT_MAX
// times.  To avoid an extra computation, this check isn't made by the
// method.
template <class Type>
inline void FIRFilter<Type>::update(Type new_element) {
    current_sum = current_sum + new_element;
    current_sum = current_sum - fir_array[index];
    fir_array[index] = new_element;
    if (num_valid < size) {
        num_valid++;
    }
    index++;
    if (index == size) {
        index = 0;
    }
}

#endif
