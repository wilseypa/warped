#ifndef TERMINATION_MANAGER_H
#define TERMINATION_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "CommunicatingEntity.h"

/**
   Interface describing a termination manager.
*/

class TerminationManager : public CommunicatingEntity {
public:
  /**
     This method gets called by the kernel to see if it's time to
     terminate.
  */
  virtual bool terminateSimulation() = 0;

  /**
     A type defining our simulation manager's idle status.
  */
  enum SIM_MGR_IDLE_STATUS { IDLE, ACTIVE };

  /**
     Our simulation manager calls this to set our status as active.
  */
  virtual void setStatusActive() = 0;

  /**
     Our simulation manager calls this to set our status as passive.
  */
  virtual void setStatusPassive() = 0;

  /**
     Reset the termination manager. Used for optimistic fossil collection rollbacks.
  */
  virtual void ofcReset() = 0;
};

#endif
