// See copyright notice in file Copyright in the root directory of this archive.

#include "InFileQueue.h"
#include <cstdio>
using std::ios;
using std::cout;
using std::cerr;
using std::endl;

InFileQueue::InFileQueue(const string& fileName )
  : inFileName( fileName ),
    lastFossilCollPos(0){
  open( fileName );
}

InFileQueue::~InFileQueue() {
  inFile.close();
}

void 
InFileQueue::open(const string &fileName) {

  inFileName = fileName;
  
  inFile.open(inFileName.c_str(), ios::in);
  if (!inFile.good()) {
    perror( ("InFileQueue: Error Opening File: " + inFileName).c_str() );
  }
}

void
InFileQueue::close() {
  inFile.close();
}

void 
InFileQueue::fossilCollect(const VTime &fossilCollectTime){

   multiset< InFileData >::iterator iter_begin = begin();
   multiset< InFileData >::iterator iter_end = end();
   
   while(iter_begin != iter_end){
      if (iter_begin->getTime() < fossilCollectTime){
         // this is a little trick using iterators.
         // the old value of iter_begin is saved and passed to erase
         // AFTER the iterator has been incremented.
         delete &(iter_begin->getTime());
         erase(iter_begin++);
      }
      else {
         break;
      }
   }

   // Save the current file position.
   lastFossilCollPos = inFile.tellg();

   if(begin() == end()){
     lastFossilCollPos = inFile.tellg();
   }
   else{
     lastFossilCollPos = begin()->getPosition();
   }
}

void
InFileQueue::fossilCollect(int fossilCollectTime){

   multiset< InFileData >::iterator iter_begin = begin();
   multiset< InFileData >::iterator iter_end = end();

   while(iter_begin != iter_end){
      if (iter_begin->getTime().getApproximateIntTime() < fossilCollectTime){
         // this is a little trick using iterators.
         // the old value of iter_begin is saved and passed to erase
         // AFTER the iterator has been incremented.
         delete &(iter_begin->getTime());
         erase(iter_begin++);
      }
      else {
         break;
      }
   }

   // Save the current file position.
   lastFossilCollPos = inFile.tellg();

   if(begin() == end()){
     lastFossilCollPos = inFile.tellg();
   }
   else{
     lastFossilCollPos = begin()->getPosition();
   }
}

void
InFileQueue::rollbackTo(const VTime &rollbackToTime){

   InFileData findElement(rollbackToTime);
   multiset< InFileData >::iterator iter_find = upper_bound(findElement);
   
   // delete every thing after the found element
   while(iter_find != end()){
     delete &(iter_find->getTime());
     erase(iter_find++);
   }

   multiset< InFileData >::reverse_iterator last = rbegin();

   // If there are no elements left, then restore to the beginning of the file.
   if(last != rend()){
     inFile.seekg(last->getPosition());
   }
   else{
     inFile.seekg(lastFossilCollPos);
   }
}

void 
InFileQueue::storePos(const VTime &time) {
   // Only insert if this is the only data at this time.
   InFileData inPos(time, inFile.tellg());
   if(find(inPos) == end()){
     insert(inPos);
   }
   else{
     delete &(inPos.getTime());
   }
}

void
InFileQueue::restoreFilePosition(streampos newPos) {
   inFile.seekg(newPos);
   lastFossilCollPos = newPos;
}
