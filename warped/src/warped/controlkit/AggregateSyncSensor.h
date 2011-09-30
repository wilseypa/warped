#ifndef AGGREGATE_SYNC_SENSOR_H
#define AGGREGATE_SYNC_SENSOR_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "SensorImplementationBase.h"

/** The AggregateSyncSensor class.

    The AggregateSyncSensor class implements a synchronous sensor. This type of
    sensor is a continuous/point sample sensor. ie. it does not
    accumulate the sensor information; it just reports what it
    currently sees. Therefore, it is usually always active.

*/
template <class NormalType> 
class AggregateSyncSensor : public SensorImplementationBase <NormalType> {
public:
   
   /**@name Public Class Methods of AggregateSyncSensor */
   //@{

   /** Default Constructor that accepts parameters

       The default constructor expects a unique name for this sensor
       and an initial value for the sensor.

       @param name the unique name for this sensor
       @param numElements the number of sensors to instantiate
       @param initVal the initial value for this sensor
   */
   AggregateSyncSensor( string name, unsigned int numElements,
                        NormalType initVal )
      : SensorImplementationBase<NormalType> (name, SENSOR_ACTIVE,
                                              AGGREGATE_SENSOR){
      numberOfElements = numElements;
      sensorElements =
         new vector<SyncSensor<NormalType> *>(numElements);

      typename vector<SyncSensor<NormalType> *>::iterator iter_begin =
         sensorElements->begin();
      typename vector<SyncSensor<NormalType> *>::iterator iter_end =
         sensorElements->end();

      while(iter_begin != iter_end){
         (*iter_begin) = new SyncSensor<NormalType>(name, initVal);
         ++iter_begin;
      }
   };
   

   /// Default destructor
   ~AggregateSyncSensor(){

     typename vector<SyncSensor<NormalType> *>::iterator iter_begin =
         sensorElements->begin();
     typename vector<SyncSensor<NormalType> *>::iterator iter_end =
         sensorElements->end();

      while(iter_begin != iter_end){
         delete (*iter_begin);
         ++iter_begin;
      }
      
      delete sensorElements;
   };

   /// get the type of this sensor
   sensorType getType(){
      return sensor_type;
   };

   /// return the name of this sensor
   string& getSensorName(){
      return sensorName;
   }
   
   /// dump the values of this sensor to the ostream
   void dump(ostream &out){

      out << sensorName << ": " << endl;

      typename vector<SyncSensor<NormalType> *>::iterator iter_begin =
         sensorElements->begin();
      typename vector<SyncSensor<NormalType> *>::iterator iter_end =
         sensorElements->end();

      int count = 0;
      while(iter_begin != iter_end){
         out << "---- Element[" << count << "] value: "
             <<  (*iter_begin)->getSensorData() << endl;
         ++iter_begin;
         count++;
      }
   };
   
   /// overloaded subscript  operator
   inline SyncSensor<NormalType>& operator[](unsigned int elem){
      ASSERT(elem < numberOfElements);
      return *((*sensorElements)[elem]);
   }
   
   /// Overloaded operator <<
   friend ostream& operator <<(ostream &out,
                               AggregateSyncSensor<NormalType> &sensor){

      out << "sensorName: " << sensor.sensorName << endl;

      typename vector<SyncSensor<NormalType> *>::iterator iter_begin =
         sensor.sensorElements->begin();
      typename vector<SyncSensor<NormalType> *>::iterator iter_end =
	sensor.sensorElements->end();

      while(iter_begin != iter_end){
         out << " value: " <<  (*iter_begin)->getSensorData() << endl;
         ++iter_begin;
      }
      return out;
   };
   
   //@} // End of Public Class Methods of AggregateSyncSensor

private:

   /**@name Private Class Attributes of AggregateSyncSensor */
   //@{

   /// number of elements in this aggregate
   unsigned int numberOfElements;
   
   /// the sensor value element
   vector<SyncSensor<NormalType> *> *sensorElements;

   //@} // End of Private Class Attributes of AggregateSyncSensor
   
};

#endif
