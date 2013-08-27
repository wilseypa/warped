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

#ifndef RANDOM_NUM_GEN_H
#define RANDOM_NUM_GEN_H

#include <stdlib.h>
#include <time.h>

class RandomNumGen {

public:

    /* Default constructor */
    RandomNumGen() {}

    /* Destructor */
    ~RandomNumGen() {}

    /* Seed the random number generator */
    void seedRandNumGen() {
        srand( time(NULL) );
    }

    /* Generate the random number */
    unsigned int genRandNum( unsigned int upperLimit ) {

        unsigned int randNum = 0;
        if(upperLimit > 0) {
            randNum = (unsigned int) rand()%upperLimit;
        } else {
            randNum = (unsigned int) rand();
        }
        return randNum;
    }
};

#endif
