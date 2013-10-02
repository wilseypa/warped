#ifndef AVERAGE_FILTER_H
#define AVERAGE_FILTER_H


#include "Filter.h"
#include <vector>
#include <numeric>

using std::vector;

/** The AverageFilter class.

    The AverageFilter is an implementation of a simple filter that
    reports the mean of a set of values. The number of elements in this
    set is specified by the user at start-up.

    If X1, X2, ... Xn represent a random sample of size n, then the
    sample mean is defined by the statistic:

              X' = SUM(Xi), 1 <= i <= n
                   -------
                      n
*/
template <class Type>
class AverageFilter : public Filter<Type> {
public:

    /**@name Public Class Methods of AverageFilter */
    //@{

    /// Default Constructor
    AverageFilter(int numSamples = 20);

    /// Default Destructor
    ~AverageFilter();

    /// reset the value of this filter
    inline void reset();

    /// add another value to the averaged set of values
    inline void update(Type);

    /// get the current value of the filter
    inline Type getData();

    //@} // End of Public Class Methods of AverageFilter

private:

    /**@name Private Class Attributes of AverageFilter */
    //@{

    /// current number of samples to average over
    int numberOfSamples;

    /// the number of effective samples
    int count;

    /// variable size buffer of samples
    vector<Type>* buffer;

    /// the actual average that we are maintaining
    Type runningAverage;

    //@} // End of Private Class Attributes of AverageFilter
};


// This is the constructor code for the class AverageFilter.
template <class Type>
AverageFilter<Type>::AverageFilter(int numSamples) {
    numberOfSamples = numSamples;
    runningAverage = 0;
    count = 0;
    buffer = new vector<Type>(numberOfSamples);
}

template <class Type>
AverageFilter<Type>::~AverageFilter() {
    delete buffer;
}


template <class Type>
inline void AverageFilter<Type>::reset() {
    runningAverage = 0;
}

// This is the method to enter new data into the AverageFilter.  The
// argument is the actual data, and should be whatever type the
// template class was declared with.  This function returns nothing.
template <class Type>
inline void AverageFilter<Type>::update(Type new_sample) {

    // Note: we are going to average over a set of samples and return
    // the average.
    (*buffer)[count] = new_sample;
    count++;
    runningAverage =
        (accumulate(buffer->begin(), buffer->end(), (Type)0)/numberOfSamples);
    if (count == numberOfSamples - 1) {
        count = 0;
    }
}

// This is the method to read the output of the filter.  It returns the
// running average in the type the the template was declared with.
template <class Type>
inline Type AverageFilter<Type>::getData() {
    return runningAverage;
}

#endif

