#ifndef SENSOR_H
#define SENSOR_H


#include <warped/warped.h>
#include <string>
using std::string;

enum sensorStatus {SENSOR_INACTIVE, SENSOR_ACTIVE};
enum sensorType {SCALAR_SENSOR, AGGREGATE_SENSOR};
enum filterType {AVERAGE_FILTER, FIR_FILTER, IIR_FILTER, MEDIAN_FILTER};
   
/** The Sensor abstract base class.

    The Sensor abstract base class represents the abstract interface
    to the different sensors that exist in the system. All sensor
    types must be derived from this abstract base class.

*/
class Sensor {
public:

   /**@name Public Class Methods of Sensor */
   //@{

   /// Default constructor
   Sensor(){};

   /// Default Destructor
   virtual ~Sensor(){};

   /// what type of sensor is this
   virtual sensorType getType() = 0;
   
   /// make the sensor active
   virtual void makeSensorActive() = 0;

   /// make the sensor inactive
   virtual void makeSensorInActive() = 0;

   /// is the sensor currently active
   virtual bool isSensorActive() = 0;

   /// get the name of this sensor
   virtual string& getSensorName() = 0;

   /// dump the current value of the sensor to the ostream
   virtual void dump(ostream &out) = 0;

   //@} // End of Public Class Methods of Sensor

};

#endif
