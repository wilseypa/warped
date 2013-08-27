// Copyright (c) The University of Cincinnati.  
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

#ifndef LOCATION_OBJECT_H
#define LOCATION_OBJECT_H

#include "SimulationObject.h"
#include "LocationState.h"
#include "Person.h"
#include "RandomNumGen.h"
#include "DiseaseModel.h"
#include "DiffusionNetwork.h"
#include "EpidemicEvent.h"

#include <vector>
#include <map>

using namespace std;

/* LocationObject Class */

class LocationObject : public SimulationObject {

public:

    /* Default Constructor */
    LocationObject( string locationName,
                    float transmissibility,
                    unsigned int latentDwellTime,
                    unsigned int incubatingDwellTime,
                    unsigned int infectiousDwellTime,
                    unsigned int asymptDwellTime,
                    float latentInfectivity,
                    float incubatingInfectivity,
                    float infectiousInfectivity,
                    float asymptInfectivity,
                    float probULU,
                    float probULV,
                    float probURV,
                    float probUIV,
                    float probUIU,
                    unsigned int locStateRefreshInterval,
                    unsigned int locDiffusionTrigInterval,
                    vector <Person *> *personVec,
                    unsigned int travelTimeToHub );

    /* Destructor */
    ~LocationObject();

    /* Initialize the simulation object */
    void initialize();

    /* Finalize the simulation object before termination */
    void finalize();

    /* Execute the scheduled event */
    void executeProcess();

    /* Allocate a new state */
    State* allocateState();

    /* Deallocate a state */
    void deallocateState(const State *state);

    /* Reclaim an event */
    void reclaimEvent(const Event *event);

    /* Accessor for objectName */
    const string &getName() const {return locationName;}

    void populateTravelMap( map <string, unsigned int> *travelMap ) {
        diffusionNetwork->populateTravelCost(travelMap, locationName);
    }

private:

    /* Create and send the refresh location state event */
    void refreshLocStateEvent( IntVTime currentTime );

    /* Create and send the diffusion trigger event */
    void triggerDiffusionEvent( IntVTime currentTime );

    /* Migrate person(s) to different location event */
    void migrateLocationEvent(  IntVTime currentTime, 
                                LocationState *locationState  );

    /* Location name */
    string locationName;

    /* Random Number Generator */
    RandomNumGen *randNumGen;

    /* Disease model */
    DiseaseModel *diseaseModel;

    /* Diffusion Network */
    DiffusionNetwork *diffusionNetwork;

    /* Initial population */
    vector <Person *> *personVec;

    /* Location state refresh interval */
    unsigned int locStateRefreshInterval;

    /* Location diffusion trigger interval */
    unsigned int locDiffusionTrigInterval;
};

#endif
