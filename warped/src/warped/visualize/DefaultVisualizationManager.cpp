
#include "DefaultVisualizationManager.h"

DefaultVisualizationManager::DefaultVisualizationManager(TimeWarpSimMgrWithVisualization *mySimMgr, map<const string, string>& configurationMap) : VisualizationManager(mySimMgr) {
  string fullAddress = configurationMap["VISUALIZATIONSERVER"];
  int colonPos       = fullAddress.find_first_of(':', 0);

  if (colonPos < 1) {
    cerr << "Invalid/Non-existent Visualization Server address.\n"
	 << "Check configuration file for valid address.\n"
	 << "Aborting.\n";
    abort();
  }

  string serverName  = fullAddress.substr(0, colonPos);
  string sPortNumber = fullAddress.substr(colonPos + 1, string::npos);
  int    portNumber  = atoi(sPortNumber.c_str());


  if ((server = new Socket(serverName.c_str(), portNumber)) == NULL) {
  //if ((server = new VisualizationClient(serverName.c_str(), portNumber)) == NULL) {
    cerr << "Unable to connect to Visualization Server (at "
	 << serverName << ":" << portNumber << ").\n"
	 << "Aborting.\n";
    abort();
  }

    if (server->getError() != 0) {
  cerr << "Unable to connect to Visualization Server (at "
   << serverName << ":" << portNumber << ").\n"
   << "Aborting.\n";
  abort();
  }

  server->_write("Connection From Cluster #\n", 25 );
}

DefaultVisualizationManager::~DefaultVisualizationManager() {
  server->_write("Simulation Complete From Cluster #", 34);
  server->_write("quit", 4);
  delete server;
}

int 
DefaultVisualizationManager::openVisualizationObject(const char *, VisualizationManager::DisplayKind, DefaultVisualizationManager::VisualizationTools) {
  return 0; // Quiet the compiler down.
}
 

int 
DefaultVisualizationManager::openVisualizationObject(const char *, VisualizationManager::DisplayKind, VisualizationManager::VisualizationTools,  VisualizationManager::DataType, void *) {
  return 0;  // Quiet the compiler down.
}


void 
DefaultVisualizationManager::closeVisualizationObject(int) {}

void 
DefaultVisualizationManager::addListener(int, void *) {
  server->_write("New Listener", 12); 
}

void 
DefaultVisualizationManager::updateVisalizationValue(int) {}


void
DefaultVisualizationManager::receiveData(char *data) {
  server->_write(data, strlen(data) + 1);
}
