#ifndef FLAT_STATE_H
#define FLAT_STATE_H


#include "State.h"                      // for State
#include "warped.h"

/** The abstract base class FlatState.

    This class provides an interface for a default implementation of a
    state class. The following is an example of the inteded use of this
    class:

    \begin{verbatim}
    class ApplicationState : public FlatState{
      int someData;

      // don't need to define copyState() as application is satisfied
      // with the one defined in FlatState

      // but we HAVE to define getSize()
      unsigned int getSize() const { return sizeof(ApplicationState); }
    }
    \end{verbatim}
*/
class FlatState : public State {
public:
    /**@name Public Class Methods of FlatState */
    //@{

    /// Destructor.
    virtual ~FlatState();

    /** Return size of the state.

        This is a pure virtual virtual function that has to be overridden.

        @return Return size of the state.
    */
    virtual const int getSize() const = 0;

    /** Make a bitwise copy of a given state.

        See State::\Ref{State::copyState}.

        @param state is a pointer to the state to be copied.
    */
    void copyState(const State* state);

    //@} // End of Public Class Methods of FlatState.

protected:
    /**@name Protected Class Methods of FlatState */
    //@{

    /// Default Constructor.
    FlatState();

    //@} // End of Protected Class Methods of FlatState
};

#endif
