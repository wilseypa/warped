#ifndef AGGREGATE_ASYNC_FILTERED_SENSOR_H
#define AGGREGATE_ASYNC_FILTERED_SENSOR_H


#include <warped/warped.h>
#include "AsyncFilteredSensor.h"
#include "SensorImplementationBase.h"

using std::vector;

/** The AggregateAsyncFilteredSensor class.

    The AggregateAsyncFilteredSensor class represents a collection of
    asynchronous cumulative sensors.

*/
template <class NormalType>
class AggregateAsyncFilteredSensor : public SensorImplementationBase <NormalType>{
public:
   
   /**@name Public Class Methods of AggregateAsyncFilteredSensor */
   //@{

   /** Default Constructor that accepts parameters

       The default constructor expects a unique name for this sensor,
       the type of filter, parameters for the filter, and an initial
       value for the sensor.

       @param name the unique name for this sensor
       @param type the filter type
       @param parameters the filter parameters
       @param numElements number of sensors to instantiate
       @param initVal the initial value for this sensor

   */
   AggregateAsyncFilteredSensor( string name, filterType filType,
                                 string parameters, unsigned int numElements,
                                 NormalType initVal )
      : SensorImplementationBase<NormalType> (name, SENSOR_INACTIVE,
                                              AGGREGATE_SENSOR){
      numberOfElements = numElements;
      sensorElements =
         new vector<AsyncFilteredSensor<NormalType> *>(numElements);

      typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_begin =
         sensorElements->begin();
      typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_end =
         sensorElements->end();

      while(iter_begin != iter_end){
         (*iter_begin) = new AsyncFilteredSensor<NormalType>(name, filType,
                                                             parameters,
                                                             initVal);
         ++iter_begin;
      }
   };

   /// Default destructor
   ~AggregateAsyncFilteredSensor(){
      
     typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_begin =
         sensorElements->begin();
     typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_end =
         sensorElements->end();

      while(iter_begin != iter_end){
         delete (*iter_begin);
         ++iter_begin;
      }
      
      delete sensorElements;
   };

   void makeSensorInActive(){
     typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_begin =
         sensorElements->begin();
     typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_end =
         sensorElements->end();
      while(iter_begin != iter_end){
         (*iter_begin)->makeSensorInActive();
         ++iter_begin;
      }
   }

   void makeSensorActive(){
     typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_begin =
         sensorElements->begin();
     typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_end =
         sensorElements->end();
      while(iter_begin != iter_end){
         (*iter_begin)->makeSensorActive();
         ++iter_begin;
      }
   }
   
   /// get the type of this sensor
   sensorType getType(){
      return sensor_type;
   };

   /// return the name of this sensor
   string& getSensorName(){
      return sensorName;
   }

   /// dump the values of this sensor to the std::ostream
   void dump(std::ostream &out){
      out << sensorName << ": " << endl;

      typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_begin =
         sensorElements->begin();
      typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_end =
         sensorElements->end();

      int count = 0;
      while(iter_begin != iter_end){
         out << "---- Element[" << count << "] value: "
             << (*iter_begin)->getFilter()->getData()
             << endl;
         ++iter_begin;
         count++;
       }
   };

   /// overloaded subscript  operator
   inline AsyncFilteredSensor<NormalType>& operator[](unsigned int elem){
      ASSERT(elem < numberOfElements);
      return *((*sensorElements)[elem]);
   }
   /// Overloaded operator <<
   friend std::ostream& operator <<(std::ostream &out,
                               AggregateAsyncFilteredSensor<NormalType>
                               &sensor){

      out << "sensorName: " << sensor.sensorName;

      
      typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_begin =
         sensor.sensorElements->begin();
      typename vector<AsyncFilteredSensor<NormalType> *>::iterator iter_end =
         sensor.sensorElements->end();

      switch((*iter_begin)->getFilterType()){

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
         
      out << endl;

      while(iter_begin != iter_end){

         ASSERT((*iter_begin)->getFilter() != NULL);
         out << " value: " << (*iter_begin)->getFilter()->getData();

         if((*iter_begin)->isSensorActive()){
            out << " sensorStatus = ACTIVE ";
         }
         else {
            out << " sensorStatus = INACTIVE ";
         }
         out << endl;
         ++iter_begin;
      }
      
      return out;
   };
   
   //@} // End of Public Class Methods of AggregateAsyncFilteredSensor

private:

   /**@name Private Class Attributes of AggregateASyncFilteredSensor */
   //@{

   /// number of elements in this aggregate
   unsigned int numberOfElements;

   /// the sensor value element
   vector<AsyncFilteredSensor<NormalType> *> *sensorElements;

   //@} // End of Private Class Attributes of AggregateASyncFilteredSensor
};
#endif




