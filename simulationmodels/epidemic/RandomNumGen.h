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
