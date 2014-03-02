#ifndef FACTORY_IMPLEMENTATION_BASE_H
#define FACTORY_IMPLEMENTATION_BASE_H


#include <string>                       // for string

#include "Factory.h"                    // for Factory
#include "FactorySymbolTable.h"
using std::string;

class FactorySymbolTable;
class ObjectStub;

/** The FactoryImplementationBase base class.

    This class provides the fundamental definition for the different
    factories in the system. The main factory and sub-factory are
    derived from this class.  The user does not start off from this
    level but rather from the sub-factory level. This class provides a
    more uniform interface in the system. This class cannot be
    instantiated directly.

*/
class FactoryImplementationBase : public Factory {
public:
    /**@name Public Class Methods of FactoryImplementationBase. */
    //@{

    /// Virtual Destructor.
    virtual ~FactoryImplementationBase();

    /** Add an object stub.

        @param stub A pointer to the stub to add.
        @return True/False - Was the stub already added?
    */
    bool add(ObjectStub* stub);

    /** Add a subfactory.

        @param subFactory A pointer to the sub factory to add.
        @return True/False - Was the sub factory already added?
    */
    bool add(FactoryImplementationBase* subFactory);

    /** Get object stub if object is present.

        @param objectName The string representation of the object.
        @return The object stub (if present).
    */
    ObjectStub* isObjectPresent(const string& objectName);

    /** Get object stub if object is present.

        @param objectClass The class of the object.
        @return The object stub (if present).
    */
    ObjectStub* isObjectPresent(const Factory* objectClass);

    /** Get a handle to the main factory.

        @return A handle to the main factory.
    */
    FactoryImplementationBase* getMainFactory() const;

    /** Get a handle to the parent factory.

        @return A handle to the main factory.
    */
    FactoryImplementationBase* getParentFactory() const;

    /** Get a string containing all symbol table information.

        @param prefix List all with this prefix.
        @return A string containing all symbol table information.
    */
    string listAll(const string& prefix);
    //@} // End of Public Class Methods of FactoryImplementationBase.

protected:
    /**@name Protected Class Methods of FactoryImplementationBase. */
    //@{

    /** Constructor.

        @param parent A pointer to the parent factory.
    */
    FactoryImplementationBase(FactoryImplementationBase* parent);

    /** Get the prefix(es) for an object with a particular name.

        @param objectName String rep. of the object name.
        @return Prefix of the object with the given name.
    */
    string* getPrefix(const string& objectName);

    /** Get the suffix(es) for an object with a particular name.

        @param objectName String rep. of the object name.
        @return Suffix of the object with the given name.
    */
    string* getSuffix(const string& objectName);
    //@} End of Protected Class Methods of FactoryImplementationBase.

    /**@name Protected Class Attributes of FactoryImplementationBase. */
    //@{

    /// The factory symbol table.
    FactorySymbolTable* mySymbolTable;

    /// A handle to the parent factory.
    FactoryImplementationBase* parentFactory;
    //@} // End of Protected Class Attributes of FactoryImplementationBase.
};

#endif

