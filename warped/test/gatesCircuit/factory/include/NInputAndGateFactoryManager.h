#include <warped/warped.h>
#include <warped/FactoryManager.h>

class FactoryImplementationBase;

/** The class RAIDFactoryManager.

    The RAID Factory.
*/
class NInputAndGateFactoryManager : public FactoryManager {
public:
  /**@name Public Class Methods of RAIDFactoryManager */
  //@{

  /** Constructor.

      @param parent Handle to the parent factory.
  */
  NInputAndGateFactoryManager(FactoryImplementationBase *parent);

  /// Destructor.
  ~NInputAndGateFactoryManager() {};

  /** Get the name of the factory manager.

      @return A reference to the factory manager's name.
  */
  const string& getName() const {
     static string name("NInputAndGateFactoryManager");
     return name;
  }
  //@} // End of Public Class Methods of RAIDFactoryManager.
};

#endif

