#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

const unsigned int numProcs = 16;
const unsigned int numEntries = 360;
const unsigned int numTests = 10;

int main(){

  vector< vector<float> > exeTimes(numProcs, vector<float>(numEntries, 0.0));
  vector< vector<int> > numRollbacks(numProcs, vector<int>(numEntries, 0));

  // Read in all of the data.
  for(int proc = 0; proc < exeTimes.size(); proc++){
    ifstream inFile;
    stringstream inName;
    inName << "/home/kingr8/results/Results.LP." << proc;
    inFile.open(inName.str().c_str());

    for(int n = 0; n < numEntries; n++){
      inFile >> exeTimes[proc][n] >> numRollbacks[proc][n];
    }
    inFile.close();
  }

  // This is the output file.
  ofstream exeFile;
  exeFile.open("/home/kingr8/results/exeResults");
  ofstream rollFile;
  rollFile.open("/home/kingr8/results/rollResults");

  // Initialize the output vectors.
  vector<float> pholdExe(numTests, 0.0);
  vector<float> raidExe(numTests, 0.0);
  vector<float> smmpExe(numTests, 0.0);

  vector<int> pholdRoll(numTests, 0);
  vector<int> raidRoll(numTests, 0);
  vector<int> smmpRoll(numTests, 0);

  int place = 0;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "Aggressive: " << endl;
  rollFile << "Aggressive: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "Aggressive One Anti: " << endl;
  rollFile << "Aggressive One Anti: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "Lazy Cancellation: " << endl;
  rollFile << "Lazy Cancellation: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "Adaptive Cancellation: " << endl;
  rollFile << "Adaptive Cancellation: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "Periodic State 1: " << endl;
  rollFile << "Periodic State 1: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "Periodic State 30: " << endl;
  rollFile << "Periodic State 30: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "Adaptive State: " << endl;
  rollFile << "Adaptive State: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "MultiSet: " << endl;
  rollFile << "MultiSet: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "MPI: " << endl;
  rollFile << "MPI: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "Mattern GVT Period 100: " << endl;
  rollFile << "Mattern GVT Period 100: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "Mattern GVT Period 10000: " << endl;
  rollFile << "Mattern GVT Period 10000" << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile << endl;
  rollFile << endl;

  // Average and total the data.
  for(int sim = 0; sim < 3; sim++){//NOTE: There are 3 sims.
    for(int res = 0; res < numTests; res++){
      float exeSum = 0.0;
      int rollSum = 0;

      for(int proc = 0; proc < numProcs; proc++){
        exeSum = exeSum + exeTimes[proc][res+(sim*numTests+place*3*numTests)];
        rollSum = rollSum + numRollbacks[proc][res+(sim*numTests+place*3*numTests)];
      }

      float exeAvg = exeSum / (float) numProcs;

      if(sim == 0){
        pholdExe[res] = exeAvg;
        pholdRoll[res] = rollSum;
      }
      else if(sim == 1){
        raidExe[res] = exeAvg;
        raidRoll[res] = rollSum;
      }
      else{
        smmpExe[res] = exeAvg;
        smmpRoll[res] = rollSum;
      }
    }
  }

  place++;

  // Output to the file.
  exeFile << "OFC:" << endl;
  rollFile << "OFC: " << endl;
  exeFile << "PHOLD\tRAID\tSMMP" << endl;
  rollFile << "PHOLD\tRAID\tSMMP" << endl;
  for(int i = 0; i < numTests; i++){
    exeFile << pholdExe[i] << "\t" << raidExe[i] << "\t" << smmpExe[i] << endl;
    rollFile << pholdRoll[i] << "\t" << raidRoll[i] << "\t" << smmpRoll[i] << endl;
  }

  exeFile.close();
  rollFile.close();

  return 0;
}
