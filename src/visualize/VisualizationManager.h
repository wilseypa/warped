#ifndef VISUALIZATION_MANAGER_H
#define VISUALIZATION_MANAGER_H


class TimeWarpSimMgrWithVisualization;

/** The VisualizationManager class.

    This is the class for visualization thingies.  More documentation to
    follow...

        "It Better" -DJ

*/
class VisualizationManager {
    friend class TimeWarpSimMgrWithVisualization;
public:

    /**@name Public Class Methods of VisualizationManager. */
    //@{


    /// destructor
    virtual ~VisualizationManager();

    /** This enumeration list represents the different types of input/output
        visualization tools that a program can create.
    */
    enum DisplayKind {UNKOWN_DISPLAY, DISPLAY_ONLY, DISPLAY_MODIFY, MODIFY_ONLY,
                      OTHERS
                     };

    /** This enumeration list represeents the different types of visualization
        tools that a program can create.
    */
    enum VisualizationTools {UNKOWN_TOOL, ON_OFF_BUTTON, SLIDER, HISTOGRAM};

    enum DataType {UNKOWN_TYPE, CHAR, INT, FLOAT, DOUBLE, STRING};

    virtual int openVisualizationObject(const char* name,
                                        VisualizationManager::DisplayKind kind,
                                        VisualizationManager::VisualizationTools vizTools) = 0;

    virtual int openVisualizationObject(const char* name,
                                        VisualizationManager::DisplayKind kind,
                                        VisualizationManager::VisualizationTools vizTools, VisualizationManager::DataType, void*) = 0;

    virtual void closeVisualizationObject(int)  = 0;

    virtual void addListener(int, void*) = 0;
    virtual void updateVisalizationValue(int)   = 0;

    TimeWarpSimMgrWithVisualization* getTimeWarpSimManager() const;

    virtual void receiveData(char*) = 0;

protected:

    /** Default constructor.

        @param void
     */
    VisualizationManager(TimeWarpSimMgrWithVisualization*);

private:
    TimeWarpSimMgrWithVisualization* timeWarpSimMgr;
};

#endif

