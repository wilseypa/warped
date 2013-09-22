
#include "VisualizationManager.h"
#include <Socket.h>
#include <iostream>
using std::cout;

VisualizationManager::VisualizationManager(TimeWarpSimMgrWithVisualization *visman) {
  int x;
  Socket mySocketServe(2345), jimmyServe;
  char readBuff[5];

  cout << "Accepting connections...\n";
  x = mySocketServe.getSocketHandle();

  if (mySocketServe._listen() != -1) {
    jimmyServe = *mySocketServe._accept();
    jimmyServe._write( "Ping\n", 5 );
    cout << "Wrote Ping\n";
    cout << "Client Message: ";
    jimmyServe._read( readBuff, 5 );
    cout << readBuff;

  }
}

VisualizationManager::~VisualizationManager(void){
}
