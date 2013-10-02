
#include "SerializedInstance.h"
#include "DeserializerManager.h"
#include <iostream>
using std::cerr;
using std::endl;

SerializedInstance::SerializedInstance(const vector<char>& initData) :
    data(initData),
    extractIteratorInitialized(false) {}

SerializedInstance::SerializedInstance(const string& dataType) :
    extractIteratorInitialized(false) {
    addString(dataType);
}

SerializedInstance::SerializedInstance(const char* initData,
                                       unsigned int initSize):
    extractIteratorInitialized(false) {
    data.insert(data.begin(), initData, initData + initSize);
}

SerializedInstance::~SerializedInstance() {}

const string
SerializedInstance::getDataType() const {
    string retval;

    vector<char>::iterator iter(const_cast<char*>(&data[0]));

    checkType(DATA_TYPE(readInt(iter)), CHAR_BLOB);
    unsigned int numChars = readUnsigned(iter);
    // Stored with a null terminator, have to add one for that.
    retval.insert(retval.begin(),
                  iter,
                  iter + numChars - 1);
    iter += numChars;


    return retval;
}


const vector<char>&
SerializedInstance::getData() const {
    return data;
}

const unsigned int
SerializedInstance::getSize() const {
    return data.size();
}

void
SerializedInstance::setData(const vector<char>& newData) {
    data = newData;
}

void
SerializedInstance::addBytes(const char* toAdd, unsigned int numBytes) {
    writeInt((int)CHAR_BLOB);
    writeInt((int)numBytes);
    writeBytes(toAdd, numBytes);
}

void
SerializedInstance::getBytes(char* buffer, unsigned int numBytes) {
    initExtract();
    checkType(DATA_TYPE(readInt(extractIterator)), CHAR_BLOB);
    ASSERT(readUnsigned(extractIterator) == numBytes);
    readBytes(buffer, numBytes, extractIterator);
}


void
SerializedInstance::addCharVector(const vector<char>& toAdd) {
    addBytes(&(*toAdd.begin()), toAdd.size());
}

vector<char>
SerializedInstance::getVectorChar() {
    vector<char> retval;
    initExtract();
    checkType(DATA_TYPE(readInt(extractIterator)), CHAR_BLOB);
    unsigned int numChars = readUnsigned(extractIterator);
    retval.insert(retval.begin(),
                  extractIterator,
                  extractIterator + numChars);

    extractIterator += numChars;

    return retval;
}


void
SerializedInstance::addString(const string& toAdd) {
    // Have to add a byte for the null termination.
    addBytes(toAdd.c_str(), toAdd.size() + 1);
}

const string
SerializedInstance::getString() {
    initExtract();

    string retval;
    checkType(DATA_TYPE(readInt(extractIterator)), CHAR_BLOB);
    unsigned int numChars = readUnsigned(extractIterator);
    // Stored with a null terminator, have to add one for that.
    if (numChars > 0) {
        retval.insert(retval.begin(),
                      extractIterator,
                      extractIterator + numChars - 1);
        extractIterator += numChars;
    }

    return retval;
}

void
SerializedInstance::addUnsigned(unsigned int toAdd) {
    writeInt(UNSIGNED);
    writeInt(toAdd);
}

unsigned
SerializedInstance::getUnsigned() {
    initExtract();
    checkType(DATA_TYPE(readInt(extractIterator)), UNSIGNED);
    return readUnsigned(extractIterator);
}

void
SerializedInstance::addInt(int toAdd) {
    writeInt(INT);
    writeInt(toAdd);
}


int
SerializedInstance::getInt() {
    initExtract();
    checkType(DATA_TYPE(readInt(extractIterator)), INT);
    return readInt(extractIterator);
}


warped64_t
SerializedInstance::getInt64() {
    initExtract();
    checkType(DATA_TYPE(readInt(extractIterator)), LONGLONG);
    return readLongLong(extractIterator);
}

void
SerializedInstance::addInt64(warped64_t toAdd) {
    writeInt(LONGLONG);
    writeLongLong(toAdd);
}

void
SerializedInstance::addDouble(double toAdd) {
    writeInt(DOUBLE);
    writeDouble(toAdd);
}


double
SerializedInstance::getDouble() {
    initExtract();
    checkType(DATA_TYPE(readInt(extractIterator)), DOUBLE);
    return readDouble(extractIterator);
}

void
SerializedInstance::addSerializable(const Serializable* toAdd) {
    SerializedInstance* serialized = toAdd->serialize();
    addSerializedInstance(serialized);
    delete serialized;
}

Serializable*
SerializedInstance::getSerializable() {
    Serializable* retval = 0;
    SerializedInstance* serialized = getSerializedInstance();
    retval = serialized->deserialize();
    delete serialized;
    return retval;
}


void
SerializedInstance::addSerializedInstance(const SerializedInstance* toAdd) {
    writeInt(SERIALIZED_INSTANCE);
    addCharVector(toAdd->getData());
}

SerializedInstance*
SerializedInstance::getSerializedInstance() {
    initExtract();
    checkType(DATA_TYPE(readInt(extractIterator)), SERIALIZED_INSTANCE);
    return new SerializedInstance(getVectorChar());
}


Serializable*
SerializedInstance::deserialize() {
    deserializeFunc func =  DeserializerManager::instance()->findDeserializer(getDataType());
    if (func == 0) {
        cerr << "Attempting to deserialize type " << getDataType() << " and could not find "
             << "deserialization function.  Please make sure all Serializable types register "
             << "before they attempt to deserialize!" << endl;
        abort();
    }
    return (*func)(this);
}

void
SerializedInstance::initExtract() {
    if (!extractIteratorInitialized) {
        extractIterator = data.begin();
        extractIteratorInitialized = true;
        // Pull off the data type.
        getString();
    }
}

void
SerializedInstance::checkType(DATA_TYPE typeFound, DATA_TYPE expected) const {
    if (typeFound != expected) {
        cerr << "While extracting a(n) " << getDataType() << ", found a " << typeFound
             << " where a " << expected << " was expected" << endl;
        abort();
    }
}
