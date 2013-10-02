#ifndef STATE_H
#define STATE_H


/** The abstract base class State.

    Any simulation object of significance will have some state that needs
    to be defined.  The process will modify its state in response to
    various events.  This behavior is completely application specific and
    so the application must define certain methods related to state for
    the simulation kernel to call.  These methods include the creation,
    duplication, and reclamation of state.  State creation and reclamation
    is a function of the simulation object (see \Ref{SimulationObject}).

    This class provides a consice interface for states.
*/
class State {
public:
    /**@name Public Class Methods of State */
    //@{

    /// Destructor.
    virtual ~State();

    /** Make a copy of a given state.

        The kernel calls this method to copy the data from the {\tt
        State} instance passed into this newly created one.  If the
        application contains pointers in its state, or objects that contain
        pointers, this method will need to take appropriate actions to copy
        the pointers correctly, as defined by the needs of the
        application. Whether {\tt copyState} gets called zero times, once,
        or many times during simulation is a function of the simulation
        kernel.  If the application's definition contains no pointers, a
        bitwise copy is adequate. In either case, this method must be
        over-ridden by the user application state definition (since this
        method is pure virtual in {\tt StateBase}).

        However, if the user's application state only requires the default
        bitwise copy for its {\tt copyState} method, an implementation of
        {\tt StateBase} called {\tt \Ref{FlatState}} with a default implementation
        of {\tt copyState} is provided.  In this case, the user
        application's state can just be derived from {\tt FlatState} instead
        of deriving from {\tt State}.

        Note that for the class {\tt FlatState}, the method {\tt
        \Ref{FlatState::copyState}} is private and the method {\tt getSize}
        is pure virtual requiring that the user must override the {\tt
        getSize} method (in order to return the size of the derived class).

        This is a pure virtual function and must be overridden.

        @param State A pointer to the state which should be copied.
    */
    virtual void copyState(const State*) = 0;

    /** Return the size of the state.

        This is a pure virtual function and must be overridden.

    */
    virtual unsigned int getStateSize() const = 0;

    //@} // End of Public Class Methods of State.
protected:
    /**@name Protected Class Methods of State */
    //@{

    /// Default Constructor.
    State();

    //@} // End of Protected Class Methods of State
};

#endif
