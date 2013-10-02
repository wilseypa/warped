#ifndef VISUALIZATION_CLIENT_H
#define VISUALIZATION_CLIENT_H


#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <string>
#include <Socket.h>
#include <string.h>
using std::cout;
using std::cerr;
using std::endl;

/** The VisualizationClient class.

    This class connects to the Visualization Server which runs a
    a specified host and a specified port.

*/
class VisualizationClient {
public:

    /**@name Public Class Methods of VisualizationClient. */
    //@{

    /** Default constructor.

        @param hostname
        @param port
     */
    VisualizationClient(const char* hostname, int port);

    /**
     * Destructor
     */
    ~VisualizationClient();

    /** Connects this client to a server
    */
    int clientConnect(void);

    /** Close connection
     */
    int clientClose(void);


protected:


private:
    const char* hostname;
    int port;
    //  Socket visSocket;
    //void* readIn(void*);

};

void* readIn(void*);
void* writeOut(void*);

#endif






