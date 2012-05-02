#ifndef PROCESS_H
#define PROCESS_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "../rnd/MLCG.h"
#include "../rnd/Normal.h"
#include "../rnd/Poisson.h"
#include "../rnd/Binomial.h"
#include "../rnd/Uniform.h"
#include "../rnd/NegExp.h"
#include "../rnd/DiscUnif.h"
#include "SimulationObject.h"
#include "../include/PHOLDEvent.h"
#include <vector>
using std::string;
using std::vector;

enum distribution_t {UNIFORM, POISSON, EXPONENTIAL, NORMAL, BINOMIAL, FIXED,
                     ALTERNATE, ROUNDROBIN, CONDITIONAL, ALL};

class Process : public SimulationObject {

public:
   Process(unsigned int, string &, unsigned int, vector<string>,
           unsigned int, unsigned int, distribution_t, double, double,
           int hotspotProb=1, int hotspotNum=0);
   ~Process();

   void initialize();
   void finalize();

   void executeProcess();

   State* allocateState();
   void deallocateState(const State *state);

   const string &getName() const{
     return myObjectName;
   }

   void reclaimEvent(const Event *event);
   
   void reportError(const string&, SEVERITY);

private:
   // The designated number for this object.
   unsigned int processNumber;

   // The name of this object.
   string myObjectName;
   
   // The number of objects this object can send events to.
   unsigned int numberOfOutputs;

   // The names of the output objects.
   vector<string> outputNames;

   // The actual output object handles.
   vector<vector<SimulationObject *> >outputHandles;

   // The number of bytes for the state size.
   unsigned int sizeOfState;

   // The number of events being sent at once.
   unsigned int numberOfTokens;

   // The type of random distribution to use for all random generation.
   distribution_t sourceDistribution;
   
   // This is the number of double division instructions to perform for
   // the computational grain.
   unsigned int compGrain;

   // Used to prevent computational grain loop from being optimized out.
   double lastX;

   double first;
   double second;

   // Generates a delay between the send and receive times.
   int msgDelay();

   // Contains a loop of double divisions to act as the computational grain.
   void computationGrain();

   int hotspotProb;
   int destLPMin;
   int destLPMax;
   int numLPs;
};

#endif
