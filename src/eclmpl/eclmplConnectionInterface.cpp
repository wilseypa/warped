#include "eclmplConnectionInterface.h"

using std::cerr;
using std::endl;

eclmplConnectionInterface::eclmplConnectionInterface() : connectionId(0),
    numberOfConnections(0), mtu(0), connected(false) {

    char* cmd = getenv("SSH_COMMAND");
    if (cmd != NULL) {
        rshCmd = string(cmd);
    } else {
#ifdef SSH_COMMAND
        rshCmd = string(__local__add_quotes(SSH_COMMAND));
#else
        cerr << "Could not set command for starting remote shells.\n"
             << "Since you did not specify a location of a remote shell command\n"
             << "at configure-time (e.g. ./configure --with-ssh=/usr/bin/ssh),\n"
             << "you must set the environment variable SSH_COMMAND to a correct\n"
             << "location of rsh or ssh (e.g. setenv SSH_COMMAND /usr/bin/ssh)" << endl;
        abort();
#endif
    }
} // End of default constructor.
