#ifndef SMMPPARTITIONER_H
#define SMMPPARTITIONER_H

// Copyright (c) Clifton Labs, Inc.
// All rights reserved.

// CLIFTON LABS MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
// SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  CLIFTON LABS SHALL NOT BE
// LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, RESULT
// OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

#include <warped/Partitioner.h>

/* 
   This class is designed specifically to partition the objects
   in the SMMP simulation.
*/
class SMMPPartitioner : public Partitioner {
public:
  SMMPPartitioner();

  const PartitionInfo *partition( const vector<SimulationObject *> *objects,
                                  const unsigned int numLPs ) const;
				  
  void addObjectGroup( const vector<SimulationObject *> *group );
  
private:
  vector< const vector<SimulationObject *>* > groups;
};

#endif
