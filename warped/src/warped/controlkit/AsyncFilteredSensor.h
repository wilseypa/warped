#ifndef ASYNC_FILTERED_SENSOR_H
#define ASYNC_FILTERED_SENSOR_H


#include <warped/warped.h>
#include "AverageFilter.h"
#include "FIRFilter.h"
#include "IIRFilter.h"
#include "MedianFilter.h"
#include "SensorImplementationBase.h"

#include <sstream>
using std::cerr;
using std::endl;
using std::istringstream;

/** The AsyncFilteredSensor class.

    The AsyncFilteredSensor class represents a asynchronous cumulative
    sensor. ie all data values posted to the sensor are
    filtered. Filter types that can be dynamically selected include:
    AverageFilter, FIRFilter, IIRFilter, and MedianFilter. All sensor
    types must be derived from the SensorImplementationBase base
    class.

    An AsyncFilteredSensor filters the data updates that are posted to
    the sensor. If an un-filtered sensor is desired, use a AsyncSensor.

*/
template <class NormalType>
class AsyncFilteredSensor : public SensorImplementationBase <NormalType> {
public:
   
   /**@name Public Class Methods of AsyncFilteredSensor */
   //@{

   /** Default Constructor that accepts parameters

       The default constructor expects a unique name for this sensor,
       the type of filter, parameters for the filter, and an initial
       value for the sensor.

       @param name the unique name for this sensor
       @param type the filter type
       @param parameters the filter parameters
       @param initVal the initial value for this sensor

   */
   AsyncFilteredSensor( string name, filterType filType, string parameters,
                        NormalType initVal )
      : SensorImplementationBase<NormalType> (name, SENSOR_INACTIVE,
                                              SCALAR_SENSOR){
      filter_type = filType;
      istringstream inputStream(parameters);
      int numSamples;
      float agingFactor;
      
      switch(filter_type){
      case AVERAGE_FILTER:
         inputStream >> numSamples;
         filter =
            (Filter<NormalType> *) new  AverageFilter<NormalType>(numSamples);
         break;
      case FIR_FILTER:
         inputStream >> numSamples;
         filter =
            (Filter<NormalType> *) new  FIRFilter<NormalType>(numSamples);
         break;
      case IIR_FILTER:
         inputStream >> agingFactor;
         filter =
            (Filter<NormalType> *) new  IIRFilter<NormalType>(agingFactor);
         break;
      case MEDIAN_FILTER:
         inputStream >> numSamples;
         filter =
            (Filter<NormalType> *) new  MedianFilter<NormalType>(numSamples);
         break;
      default:
         cerr << "An unknown filter type encountered in AsyncFilteredSensor"
              << endl;
         break;
         
      };
   };

   /// Default destructor
   ~AsyncFilteredSensor(){
      delete filter;
   };

   /// get the type of this sensor
   sensorType getType(){
      return sensor_type;
   };

   /// get a handle to the sensor's filter
   Filter<NormalType>* getFilter(){
      return filter;
   };

   /// get the current value of this sensor
   NormalType& getSensorData(){
      return filter->getData();
   };
   
   /// get the type of this sensor's filter
   filterType getFilterType(){
      return filter_type;
   };

   /// return the name of this sensor
   string& getSensorName(){
      return sensorName;
   }
   
   /// dump the values of this sensor to the std::ostream
   void dump(std::ostream &out){
      out << sensorName << ": value: " << filter->getData() << endl;
   };

   /// overloaded assignment operator
   inline AsyncFilteredSensor<NormalType>& operator=(const NormalType &newVal){
      if(isSensorActive()){
         ASSERT(filter != NULL);
         filter->update(newVal);
      }
      return *this;
   }

   /// overloaded increment operator
   inline AsyncFilteredSensor<NormalType>& operator++(int){
      // check to see if this sensor is active or not
      if(isSensorActive()){
         ASSERT(filter != NULL);
         filter->update(filter->getData() + 1);
      }
      return *this;
   }
   

   /// overloaded decrement operator
   inline AsyncFilteredSensor<NormalType>& operator--(){
      // check to see if this sensor is active or not 
      if(isSensorActive()){
         ASSERT(filter != NULL);
         filter->update(filter->getData() - 1);
      }
      return *this;
   };

   /// Overloaded operator <<
   friend std::ostream& operator <<(std::ostream &out, AsyncFilteredSensor<NormalType> &sensor){
      out << "sensorName: " << sensor.sensorName;

      switch(sensor.getFilterType()){

      case AVERAGE_FILTER:
         out << " filterType: AverageFilter ";
         break;
      case FIR_FILTER:
         out << " filterType: FIRFilter ";
         break;
      case IIR_FILTER:
         out << " filterType: IIRFilter ";
         break;
      case MEDIAN_FILTER:
         out << " filterType: MedianFilter ";
         break;
      default:
         cerr << " Unknown filter type encountered " << endl;
         break;
      };
      
      ASSERT(sensor.getFilter() != NULL);
      out << " value: " << sensor.getFilter()->getData();

      if(sensor.isSensorActive()){
         out << " sensorStatus = ACTIVE ";
      }
      else {
         out << " sensorStatus = INACTIVE ";
      }
      return out;
   }
   
   //@} // End of Public Class Methods of AsyncFilteredSensor

private:

   filterType filter_type;
   Filter<NormalType> *filter;
};
#endif
