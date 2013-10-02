#ifndef SYNC_SENSOR_H
#define SYNC_SENSOR_H


#include "SensorImplementationBase.h"

/** The SyncSensor class.

    The SyncSensor class implements a synchronous sensor. This type of sensor is
    a continuous/point sample sensor. ie. it does not accumulate the sensor
    information; it just reports what it currently sees. Therefore, it is
    usually always active.

*/
template <class NormalType>
class SyncSensor : public SensorImplementationBase <NormalType> {
public:

    /**@name Public Class Methods of SyncSensor */
    //@{

    /** Default Constructor that accepts parameters

        The default constructor expects a unique name for this sensor
        and an initial value for the sensor.

        @param name the unique name for this sensor
        @param initVal the initial value for this sensor
    */
    SyncSensor(string name, NormalType initVal)
        : SensorImplementationBase<NormalType> (name, SENSOR_ACTIVE,
                                                SCALAR_SENSOR),
        controlledElement(initVal) {};

    /// Default destructor
    ~SyncSensor() {};

    /// get the type of this sensor
    sensorType getType() {
        return sensor_type;
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
        controlledElement = newVal;
        return *this;
    }

    /// overloaded assignment operator
    inline SyncSensor<NormalType>& operator+=(const NormalType& newVal) {
        controlledElement += newVal;
        return *this;
    }

    /// overloaded increment operator
    inline SyncSensor<NormalType>& operator++(int) {
        // the check to see if this sensor is active or not is
        // not done to avoid extra cycles of computation. It is assumed
        // that a SyncSensor is usually active.
        controlledElement++;
        return *this;
    }


    /// overloaded decrement operator
    inline SyncSensor<NormalType>& operator--() {
        // the check to see if this sensor is active or not is
        // not done to avoid extra cycles of computation. It is assumed
        // that a SyncSensor is usually active.
        controlledElement--;
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
    friend std::ostream& operator <<(std::ostream& out, SyncSensor<NormalType>& sensor) {
        out << "sensorName: " << sensor.sensorName << " value: "
            << sensor.controlledElement;
        return out;
    }

    //@} // End of Public Class Methods of SyncSensor

private:

    /**@name Private Class Attributes of SyncSensor */
    //@{

    /// the sensor value element
    NormalType controlledElement;

    //@} // End of Private Class Attributes of SyncSensor

};

#endif
