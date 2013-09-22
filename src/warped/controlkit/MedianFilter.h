#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H


#include <iostream>
#include <deque>
#include "Filter.h"
#include <numeric>
#include <algorithm>

using std::deque;
using std::ostream_iterator;
using std::cout;

/** The MedianFilter class.
   
    The MedianFilter is an implementation of a simple filter that
    reports the median of a set of value. The number of elements in
    this set is specified by the user at start-up.

    If X1, X2, ..., Xn represent a random sample of size n, arranged in
    increasing order of magnitude, then the sample median is defined by
    the statistic:

              X' = X((n+1)/2)          if n is odd
                 = X(n/2) + X((n/2)+1) if n is even
*/
template <class Type>
class MedianFilter : public Filter <Type> {
public:
   
   /**@name Public Class Methods of MedianFilter */
   //@{

   /// Default Constructor
   MedianFilter(int numSamples = 20);

   /// Default Destructor
   ~MedianFilter();

   /// reset the value of this filter
   inline void reset();

   /// add another value to the filtered set
   inline void update(Type);

   /// obtain the value of the filter
   inline Type getData();

   //@} // End of Public Class Methods of MedianFilter

private:

   /**@name Private Class Attributes of MedianFilter */
   //@{

   /// current number of samples to calculate median over
   int numberOfSamples;

   /// the number of effective samples
   int count;

   /// variable size buffer of samples
   deque<Type> *buffer;

   /// the actual median that we are maintaining
   Type runningMedian;

   //@} // End of Private Class Attributes of MedianFilter
};


// This is the constructor code for the class MedianFilter.
template <class Type>
MedianFilter<Type>::MedianFilter(int numSamples){
   numberOfSamples = numSamples;
   runningMedian = 0;
   count = 0;
   buffer = new deque<Type>;
}

template <class Type>
MedianFilter<Type>::~MedianFilter(){
   delete buffer;
}


template <class Type>
inline void MedianFilter<Type>::reset(){
   runningMedian = 0;
}

// This is the method to enter new data into the MedianFilter.  The
// argument is the actual data, and should be whatever type the
// template class was declared with.  This function returns nothing.
template <class Type>
inline void MedianFilter<Type>::update(Type new_sample) {

   // Note: we are going to calculate the median of a set of samples
   // and return the result.
   buffer->push_front(new_sample);
   count++;
   copy(buffer->begin(), buffer->end(), std::ostream_iterator<Type>(cout, " "));

   // first sort the entries in the vector
   stable_sort(buffer->begin(), buffer->end());

   if(numberOfSamples % 2 != 0){
      // n is odd
      runningMedian = (*buffer)[((numberOfSamples + 1)/2)-1];
   }
   else {
      // n is even
      runningMedian = ((*buffer)[(numberOfSamples/2)-1] +
                       (*buffer)[(numberOfSamples/2)])/2;
   }

   if (count >= numberOfSamples){
      buffer->pop_front();
   }
}

// This is the method to read the output of the filter.  It returns the
// running median in the type the the template was declared with.
template <class Type>
inline Type MedianFilter<Type>::getData() {
  return runningMedian;
}

#endif

