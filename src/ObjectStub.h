#ifndef OBJECT_STUB_H
#define OBJECT_STUB_H


#include "warped.h"
#include "Factory.h"
#include <sstream>

using std::ostringstream;

class SimulationObject;
class FactoryImplementationBase;

/** The ObjectStub abstract base class.

    This class provides the fundamental definition for object
    stubs. An object stub provides the users (when queried) with
    information about the object that is associated with that stub. Of
    course, each object should have a corresponding stub
    present. Otherwise, the object is not visible to the outside
    world. Object stubs can contain information such as what the
    object contains, the security issues associated with the object
    trade marks, licence and property issues etc.. etc..

*/
class ObjectStub : public Factory {
public:
    /**@name Public Class Methods of ObjectStub. */
    //@{

    /// Destructor.
    ~ObjectStub() {};

    /** Get the kind of object (i.e. a stub).

        @return The kind of object this is.
    */
    const Kind getKind() const;

    /** Get a handle to the parent factory.

        @return A handle to the parent factory.
    */
    FactoryImplementationBase* getParentFactory() const;

    /** Get information about this object.

        This is a pure virtual function that has to be overridden.

        @return Information about this object.
    */
    virtual const string& getInformation() const = 0;

    /** Is this object local?

        @return True/False - Is this object local?
    */
    virtual const bool isLocalObject() const = 0;

    /** Should be called when creating a new simulation object.

        @param argc The number of command-line arguments.
        @param stream The output stream.
        @return A handle to the created object.
    */
    virtual SimulationObject* createSimulationObject(int argc,
                                                     ostringstream& stream) = 0;

    //@} // End of Public Class Methods of ObjectStub.

protected:
    /**@name Protected Class Methods of ObjectStub. */
    //@{

    /** Constructor.

        @param myFactory Handle to the parent factory.
    */
    ObjectStub(FactoryImplementationBase* myFactory);
    //@} // End of Protected Class Methods of ObjectStub.

private:
    /**@name Private Class Attributes of ObjectStub. */
    //@{

    /// Handle to the parent factory.
    FactoryImplementationBase* factory;
    //@} // End of Private Class Attributes of ObjectStub.
};

#endif

