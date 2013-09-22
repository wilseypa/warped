#ifndef TIMEWARP_SIM_MGR_WITH_VISUALIZATION_H
#define TIMEWARP_SIM_MGR_WITH_VISUALIZATION_H


#include <TimeWarpSimulationManager.h>

using std::string;

// the new visualization component that sits on top of the
// simulation manager and acts as an interface for the visualization
// component. 


class VisualizationManager;

/** The TimeWarpSimMgrWithVisualization class.

*/
class TimeWarpSimMgrWithVisualization : public TimeWarpSimulationManager {
public:

   /**@type friend class declarations */
   //@{

   /** Builder class */
   friend class TimeWarpConfigurationManager;
   
   //@} // End of friend class declarations  

   /**@name Public Class Methods of TimeWarpSimMgrWithVisualization. */
   //@{

   /** Constructor.

       @param numProcessors Number of processors used.
   */
   TimeWarpSimMgrWithVisualization( unsigned int numProcessors, 
				    Application *initApplication );
   
   /// Destructor.
   ~TimeWarpSimMgrWithVisualization();
   
   /** Return a handle to the visualization manager.

       @return A handle to the visualization manager.
   */
   virtual VisualizationManager *getVisualizationManager(){
      return myVisualizationManager;
   }

   /// Registers a set of simulation objects with this simulation manager.
   void registerSimulationObjects();

   /** Run the simulation.
       
       @param simulateUntil Time upto which to simulate.
   */
   void simulate(const VTime &simulateUntil);

  /** Remove the next event in the event set and return it.
      
  @param object The simulation object whose event set we get event from.
  @return A reference to the removed event.
  */
  const Event *getEvent(SimulationObject *object);
  
  void receiveKernelMessage(KernelMessage *msg);
   
   //@} // End of Public Class Methods of TimeWarpSimMgrWithVisualization.

private:

   /**@name Private Class Attributes of TimeWarpSimMgrWithVisualization. */
   //@{
   
   /// Handle to the visualization manager.
   VisualizationManager *myVisualizationManager;

   //@} // End of Private Class Attributes of TimeWarpSimMgrWithVisualization.
};
#endif







