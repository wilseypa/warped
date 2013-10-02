#ifndef FACTORY_MANAGER_H
#define FACTORY_MANAGER_H


#include "FactoryImplementationBase.h"

/** The FactoryManager base class.

    The FactoryManager base class is the class from where the user
    starts derives his/her own factory definitions. Each
    factory/sub-factory needs to define a name for itself. When the
    factory is instantiated the corresponding stubs and sub-factories
    for the various objects associated with this factory can be
    instantiated.  This class cannot be directly instantiated.


*/
class FactoryManager : public FactoryImplementationBase {
public:

    /**@name Public Class Methods of FactoryManager */
    //@{

    /// Virtual Destructor.
    virtual ~FactoryManager() {};

    /** This method will be defined by the user.

        @return A handle to the created user factory manager.
    */
    static FactoryManager* createUserFactory();

    /** Get the kind of factory.

        @return The kind of factory.
    */
    const Kind getKind() const {
        return SUB_FACTORY;
    }

    //@} // End of Public Class Methods of FactoryManager

protected:

    /**@name Protected Class Methods of FactoryManager */
    //@{

    /** Constructor.

        @param parent Handle the to oarect factory.
    */
    FactoryManager(FactoryImplementationBase* parent);

    //@} // End of Protected Class Methods of FactoryManager.
};

#endif
