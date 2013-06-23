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
 
#include "LocationObject.h"
#include "IntVTime.h"

using namespace std;

LocationObject::LocationObject(string locationName, vector <PersonState *> *occSet) :
	locationName(locationName), occSet(occSet) {

	depSet = new vector <PersonState *>;
	occMap = new map <unsigned int, PersonState *>;
	depMap = new map <unsigned int, PersonState *>;

	for(int index = 0; index < occSet->size(); index++) {
		occMap->insert( std::pair<unsigned int, PersonState *> ( ((*occSet)[index])->getPID(), ((*occSet)[index]) ) );
	}
}

LocationObject::~LocationObject() {}

void LocationObject::initialize() {}

void LocationObject::finalize() {}

void LocationObject::executeProcess() {}

State* LocationObject::allocateState() {
	return NULL;
}

void LocationObject::deallocateState( const State *state ) {
	delete state;
}

void LocationObject::reclaimEvent(const Event *event) {
}


#if 0
SMMPForkObject::~SMMPForkObject(){
   deallocateState(getState());
}

void
SMMPForkObject::initialize() {
   SMMPForkState *myState = dynamic_cast<SMMPForkState*>(getState());
   myState->gen = new MLCG(seed , seed + 1);
   
   fanOutHandles.resize(fanOutNames.size(),NULL);
   for(int i = 0; i < fanOutHandles.size(); i++){
     fanOutHandles[i] = getObjectHandle(fanOutNames[i]);
   }
}

#endif
