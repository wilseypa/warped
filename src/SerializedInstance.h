#ifndef SERIALIZED_INSTANCE_H
#define SERIALIZED_INSTANCE_H

#include <cstring>                      // for memcpy
#include <ostream>                      // for operator<<, ostream, etc
#include <string>                       // for string
#include <vector>                       // for vector, vector<>::iterator

#include "DeserializerManager.h"        // for string
#include "warped.h"                     // for warped64_t

class Serializable;

/** The class SerializedInstance.

    To achieve full portability and inter-operability of the applications
    on a distributed network of workstations, serialization and
    de-serialization of events is necessary.  It is the responsibility of
    the application writer to implement the methods to serialize and
    deserialize the events.  To assist the developer the
    SerializedInstance, Serializable, and Inflater infrastructure exists.

*/
class SerializedInstance {

public:
    /**@name Public Class Methods of SerializedInstance. */
    //@{

    /** Constructor.

        @param initData Char * pointer to data.  Will be freed on destruction
          with delete []...

        @param newSize Size of the new data.
    */
    SerializedInstance(const std::vector<char>& initData);

    /**
       Use the chunk of data passed in as our serialized instance.  You had
       better know what you're doing to use this constructor.
    */
    SerializedInstance(const char* data, unsigned int size);


    /**
       Starts an empty SerializedInstance that will contain the data type
       passed in.
    */
    SerializedInstance(const std::string& dataType);


    /// Destructor.
    ~SerializedInstance();

    /**
       Returns a string containing the datatype of this serialized instance.
       We can use this to find the Inflater for this object and to
       deserialize it.
    */
    const std::string getDataType() const;

    /** Get a representation of the data.

        This method returns a representation of the event to the kernel (for
        example, this representation could be in the form of a string).

        @return A pointer to the data.
    */
    const std::vector<char>& getData() const;

    /** Return size of the data.

        This method returns the size of the serialized object's data string.
        With this information, {\tt NULL}s and other binary data can be
        safely embedded in the string. Since event class definitions can not
        have pointers or virtual functions, a call to {\tt getSize}() will
        always return the exact size of the event.

        @return Size of the data.  */
    const unsigned int getSize() const;

    /** Set new data information.

        @param newData The new data to be set.
        @param newSize Size of the new data.
    */
    void setData(const std::vector<char>& newData);

    /**
       Deserialize this object into a "Serializable".  This can only happen
       if the SerializedInstance is in proper form and it is registered with
       the DeserializerManager.

       @see DeserializerManager#registerDeserializer
       @see DeserializerManager#findDeserializer
    */
    Serializable* deserialize();


    /**
       Add a fixed number of bytes to this serialized instance.  The
       deserializer had better know how many bytes to pull out!

       The memory is copied into our local memory, and not deleted or freed.
    */
    void addBytes(const char* toAdd, unsigned int);

    /**
       Get a string out of the serialized instance.  Not necessarily null
       terminated (as it may/may not have been when it was put in.)  Caller
       must know the size of the data they are pulling out, and must pass in
       the buffer to copy the data to.
    */
    void getBytes(char* buffer, unsigned int bytes);

    /**
       Adds the referenced vector to our SerializedInstance.
    */
    void addCharVector(const std::vector<char>& toAdd);

    /**
       Returns a vector of characters.
    */
    std::vector<char> getVectorChar();

    /**
       Add a C++ string to this serialized instance.
    */
    void addString(const std::string& toAdd);

    const std::string getString();

    /**
       Add an unsigned int to this serialized instance.
    */
    void addUnsigned(unsigned int toAdd);

    unsigned int getUnsigned();

    /**
       Add an int to this serialized instance.
    */
    void addInt(int toAdd);

    int getInt();

    void addDouble(double toAdd);

    double getDouble();

    /**
       Add an longLong to this serialized instance.
    */
    void addInt64(warped64_t toAdd);

    warped64_t getInt64();


    /**
       Add another serialized instance to this one.  Caller retains ownership
       of the Serializable and is responsible for deleting it.
    */
    void addSerializable(const Serializable* toAdd);

    /**
       Extract a serializable from this SerializedInstance.  Caller assumes
       ownership of the Serializable passed back.
    */
    Serializable* getSerializable();



    //@} // End of Public Class Methods of SerializedInstance.

private:
    /**@name Private Class Attributes of SerializedInstance. */
    //@{

    /**
       Add another serialized instance to this one.
    */
    void addSerializedInstance(const SerializedInstance* toAdd);

    SerializedInstance* getSerializedInstance();



    /**
       This type is used internally to mark the serialized data's structure
       so that we can validate as we extract.
    */
    enum DATA_TYPE {
        CHAR_BLOB,
        INT,
        UNSIGNED,
        LONGLONG,
        DOUBLE,
        SERIALIZED_INSTANCE
    };

    /**
       Writes in this chunk of data, no extra metadata included.
    */
    void writeBytes(const char* toAdd, unsigned int numBytes) {
        data.insert(data.end(), toAdd, toAdd + numBytes);
    }

    /**
       Writes an int to the stream, with no additional metadata.
    */
    void writeInt(int toWrite) {
        writeBytes(reinterpret_cast<char*>(&toWrite), sizeof(int));
    }

    /**
       Writes an int to the stream, with no additional metadata.
    */
    void writeLongLong(warped64_t toWrite) {
        writeBytes(reinterpret_cast<char*>(&toWrite), sizeof(warped64_t));
    }

    /**
       Writes a double to the stream, with no additional metadata.
    */
    void writeDouble(double toWrite) {
        writeBytes(reinterpret_cast<char*>(&toWrite), sizeof(toWrite));
    }

    /**
       Reads the number of bytes into the buffer provided.
    */
    static void readBytes(char* buffer,
                          unsigned int numBytes,
                          std::vector<char>::iterator& iter) {
        memcpy(buffer, &(*iter), numBytes);
        iter += numBytes;
    }

    /**
       "Raw" read of an int - no metadata processed.
    */
    static int readInt(std::vector<char>::iterator& iter) {
        int retval;
        readBytes(reinterpret_cast<char*>(&retval), sizeof(int), iter);
        return retval;
    }

    static unsigned int readUnsigned(std::vector<char>::iterator& iter) {
        unsigned int retval;
        readBytes(reinterpret_cast<char*>(&retval),
                  sizeof(unsigned int),
                  iter);
        return retval;
    }

    static warped64_t readLongLong(std::vector<char>::iterator& iter) {
        warped64_t retval;
        readBytes(reinterpret_cast<char*>(&retval),
                  sizeof(warped64_t),
                  iter);
        return retval;
    }

    static double readDouble(std::vector<char>::iterator& iter) {
        double retval;
        readBytes(reinterpret_cast<char*>(&retval),
                  sizeof(double),
                  iter);
        return retval;
    }

    /**
       All extraction methods start by calling this.  It makes sure that
       things are setup for extraction.
    */
    void initExtract();

    /// A pointer to the data.
    std::vector<char> data;

    /// Size of the data.
    unsigned int size;

    //@} // End of Private Class Attributes of SerializedInstance.

    /**@name Private Class Methods of SerializedInstance. */
    //@{

    // copy constructor and operator= are defined private to prevent copies
    // and assignment of instances as well as to prevent the compiler from
    // generating its own operator=.

    /** Copy Constructor.

        Copy Constructor is defined as private to prevent copies of
        instances.
    */
    SerializedInstance(const SerializedInstance&);

    /** Overloaded operator =.

        Operator = is defined as private to prevent assignment of instances,
        as well as to prevent the compiler from generating its own operator=
    */
    SerializedInstance& operator=(const SerializedInstance&);

    /**
       The iterator that keeps track of where we are in extraction.
    */
    std::vector<char>::iterator extractIterator;
    /**
       Flag telling us if the extractIterator has been initialized.
    */
    bool extractIteratorInitialized;

    void checkType(DATA_TYPE typeFound, DATA_TYPE expected) const;

    friend std::ostream& operator<<(std::ostream&, const SerializedInstance::DATA_TYPE);

    //@} // End of Private Class Methods of SerializedInstance.
};

inline std::ostream&
operator<<(std::ostream& os, const SerializedInstance::DATA_TYPE dt) {
    switch (dt) {
    case SerializedInstance::CHAR_BLOB:
        os << "CHAR_BLOB";
        break;
    case SerializedInstance::INT:
        os << "INT";
        break;
    case SerializedInstance::UNSIGNED:
        os << "UNSIGNED";
        break;
    case SerializedInstance::LONGLONG:
        os << "LONGLONG";
        break;
    case SerializedInstance::DOUBLE:
        os << "DOUBLE";
        break;
    case SerializedInstance::SERIALIZED_INSTANCE:
        os << "SERIALIZED_INSTANCE";
        break;
    default:
        os << "unknown SerializedInstance::DATA_TYPE - value = " << int(dt);
    }
    return os;
}

#endif
