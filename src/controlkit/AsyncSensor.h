#ifndef ASYNC_SENSOR_H
#define ASYNC_SENSOR_H


#include "SensorImplementationBase.h"

/** The AsyncSensor class.

    The AsyncSensor class represents a asynchronous sensor which is
    nothing but a point sample sensor or a on-demand sensor. All
    sensor types must be derived from the SensorImplementationBase
    base class.

    An AsyncSensor does not filter the data updates that are posted to
    the sensor. If a filtered sensor is desired, use a AsyncIIRSensor
    or AsyncFIRSensor.

*/
template <class NormalType>
class AsyncSensor : public SensorImplementationBase <NormalType> {
public:

    /**@name Public Class Methods of AsyncSensor */
    //@{
    /** Default Constructor that accepts parameters

        The default constructor expects a unique name for this sensor
        and an initial value for the sensor.

        @param name the unique name for this sensor
        @param initVal the initial value for this sensor
    */
    AsyncSensor(string name, NormalType initVal)
        : SensorImplementationBase<NormalType> (name, SENSOR_INACTIVE,
                                                SCALAR_SENSOR),
        controlledElement(initVal) {}

    /// Default destructor
    ~AsyncSensor() {};

    /// get the type of this sensor
    sensorType getType() {
        return type;
    };

    NormalType& getSensorData() {
        return controlledElement;
    };

    /// return the name of this sensor
    string& getSensorName() {
        return sensorName;
    }

    /// dump the values of this sensor to the std::ostream
    void dump(std::ostream& out) {
        out << sensorName << ": value: " << controlledElement << endl;
    };

    /// overloaded assignment operator
    inline SyncSensor<NormalType>& operator=(const NormalType& newVal) {
        if (isSensorActive()) {
            controlledElement = newVal;
        }
        return *this;
    }

    /// overloaded assignment operator
    inline SyncSensor<NormalType>& operator+=(const NormalType& newVal) {
        if (isSensorActive()) {
            controlledElement += newVal;
        }
        return *this;
    }

    /// overloaded increment operator
    inline SyncSensor<NormalType>& operator++(int) {
        // check to see if this sensor is active or not
        if (isSensorActive()) {
            controlledElement++;
        }
        return *this;
    }


    /// overloaded decrement operator
    inline AsyncSensor<NormalType>& operator--() {
        // check to see if this sensor is active or not
        if (isSensorActive()) {
            controlledElement--;
        }
        return *this;
    };

    /// overloaded less than comparison operator
    inline bool operator<(const NormalType& right) const {
        if (controlledElement < right.getSensorData()) {
            return true;
        } else {
            return false;
        }
    }

    /// overloaded greater than comparison operator
    inline bool operator>(const NormalType& right) const {
        if (controlledElement > right.getSensorData()) {
            return true;
        } else {
            return false;
        }
    }

    /// overloaded equality operator
    inline bool operator==(const NormalType& right) const {
        if (controlledElement == right.getSensorData()) {
            return true;
        } else {
            return false;
        }
    }

    /// overloaded not equal to operator
    inline bool operator!=(const NormalType& right) const {
        return !operator==(right);
    }

    /// overloaded less than or equal to operator
    inline bool operator<=(const NormalType& right) const {
        if (*this < right || *this == right) {
            return true;
        } else {
            return false;
        }
    }

    /// overloaded greater than or equal to operator
    inline bool operator>=(const NormalType& right) const {
        if (*this > right || *this == right) {
            return true;
        } else {
            return false;
        }
    }

    /// Overloaded operator <<
    friend std::ostream& operator <<(std::ostream& out, AsyncSensor<NormalType>& sensor) {
        out << "sensorName: " << sensor.sensorName << " value: "
            << sensor.controlledElement;

        if (isSensorActive()) {
            out << " sensorStatus = ACTIVE ";
        } else {
            out << " sensorStatus = INACTIVE ";
        }
        return out;
    }

    //@} // End of Public Class Methods of AsyncSensor
};
#endif
