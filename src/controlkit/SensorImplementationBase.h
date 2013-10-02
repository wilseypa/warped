#ifndef SENSOR_IMPLEMENTATION_BASE_H
#define SENSOR_IMPLEMENTATION_BASE_H


#include "Sensor.h"

/** The SensorImplementationBase base class.

    The SensorImplementationBase base class represents the base class
    from which all sensor implementations are derived from. It also
    contains all data/methods that are common to all sensor
    implementations.

*/
template <class NormalType>
class SensorImplementationBase : public Sensor {
public:

    /**@name Public Class Methods of SensorImplementationBase */
    //@{

    /// Default constructor
    SensorImplementationBase(string name,
                             sensorStatus currentStatus=SENSOR_INACTIVE,
                             sensorType currentType=SCALAR_SENSOR)
        : sensorName(name), status(currentStatus), sensor_type(currentType) {};

    /// Default Destructor
    virtual ~SensorImplementationBase() {};

    /// make the sensor active
    virtual void makeSensorActive() {
        status = SENSOR_ACTIVE;
    };

    /// make the sensor inactive
    virtual void makeSensorInActive() {
        status = SENSOR_INACTIVE;
    };

    /// is the sensor currently active
    virtual bool isSensorActive() {
        return (status == SENSOR_ACTIVE ? true : false);
    };

    //@} // End of Public Class Methods of SensorImplementationBase

protected:

    /**@name Protected Class Attributes of SensorImplementationBase */
    //@{

    /// the name of the sensor
    string sensorName;

    /// is the sensor active or inactive
    sensorStatus status;

    /// what type of sensor is it
    sensorType sensor_type;

    //@} // End of Protected Class Attributes of SensorImplementationBase

};

#endif
