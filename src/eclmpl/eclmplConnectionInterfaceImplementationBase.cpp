
#include <stdio.h>                      // for sprintf, perror
#include <stdlib.h>                     // for atoi, abort, free
#include <string.h>                     // for strlen, memcpy, strcpy, etc
#include <unistd.h>                     // for execv, fork, getcwd
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string, operator>>, etc
#include <vector>                       // for vector

#include "eclmpl/eclmplConfigFileTable.h"  // for eclmplConfigFileTable
#include "eclmpl/eclmplContactInfo.h"   // for eclmplContactInfo
#include "eclmplConnectionInterfaceImplementationBase.h"
#include "warped.h"                     // for ASSERT

const std::string configFile = "procgroup";
const unsigned int maxBuf = 65535;

eclmplConnectionInterfaceImplementationBase::~eclmplConnectionInterfaceImplementationBase() {
} // End of destructor.

eclmplConnectionInterfaceImplementationBase::eclmplConnectionInterfaceImplementationBase(
    const unsigned int& mtuSize) {
    mtu = mtuSize;
} // End of constructor.

bool
eclmplConnectionInterfaceImplementationBase::establishConnections(const int* const argc,
                                                                  const char* const* const* const argv) {
    ASSERT(*argc > 0);

    // We have to check the last argument on the command line to be able
    // to figure out if we are the first communicator started, i.e. the
    // master, or if we have been forked off, i.e. a slave.
    std::string finalParameter((*argv)[*argc - 1]);
    if (finalParameter == "amslave") {
        slaveStartupInfo startupInfo;
        startupInfo = parseCommandLineArguments(argc, argv);
        establishConnections(startupInfo);
    } else {
        eclmplConfigFileTable* connTable = scanConfigFile(configFile);
        establishConnections(argc, argv, *connTable);
        delete connTable;
    }
    connected = true;
    return connected;
} // End of establishConnections(...).

void
eclmplConnectionInterfaceImplementationBase::tearDownConnections() {
    std::cerr << "Error - eclmplConnectionInterfaceImplementationBase::tearDownConnections called.\n"
              << "Whoever implements this class must also implement this virtual method" << std::endl;
    abort();
} // End of tearDownConnections().

void
eclmplConnectionInterfaceImplementationBase::send(const unsigned int&,
                                                  const char* const,
                                                  const unsigned int&) {
    std::cerr << "Error - eclmplConnectionInterfaceImplementationBase::send called.\n"
              << "Whoever implements this class must also implement this virtual method" << std::endl;
    abort();
} // End of send(...)/

bool
eclmplConnectionInterfaceImplementationBase::recv(unsigned int&,
                                                  char* const,
                                                  unsigned int&) {
    std::cerr << "Error - eclmplConnectionInterfaceImplementationBase::recv called.\n"
              << "Whoever implements this class must also implement this virtual method" << std::endl;
    abort();
    return false;
} // End of recv(...).

slaveStartupInfo
eclmplConnectionInterfaceImplementationBase::parseCommandLineArguments(const int* const argc,
                                                                       const char* const* const* const argv) {
    ASSERT(*argc >= 8);
    slaveStartupInfo info;
    int currArg;
    unsigned int tmp;

    // argv on form:
    // hostname fullPathNameOfExecutable <oldArgv> <optional arguments>
    // masterContactInfo numberOfConnections connectionId amslave
    connectionId = atoi((*argv)[*argc-2]);
    numberOfConnections = atoi((*argv)[*argc-3]);
    tmp = atoi((*argv)[*argc-4]); // nrOfMasterContactArgs
    std::vector<std::string> contactArgs;
    currArg = 5;
    for (unsigned int i = 0; i < tmp; i++) {
        contactArgs.push_back(std::string((*argv)[*argc-currArg]));
        currArg++;
    }
    info.masterContactInfo.setContactArguments(contactArgs);

    tmp = (unsigned int)atoi((*argv)[*argc-currArg]); // Intended for...
    ASSERT(tmp == connectionId);
    info.masterContactInfo.setIntendedFor(tmp);
    currArg++;

    tmp = (unsigned int)atoi((*argv)[*argc-currArg]); // Provided by...
    info.masterContactInfo.setProvidedBy(tmp);
    currArg++;

    tmp = (unsigned int)atoi((*argv)[*argc-currArg]); // Number of optional arguments...
    currArg++;
    for (unsigned int i = 0; i < tmp; i++) {
        info.configTableEntry.push_back(std::string((*argv)[*argc-currArg]));
        currArg++;
    }
    return info;
} // End of parseCommandLineArguments(...).

// We assume the default number of arguments for each table entry is
// two (which should be enough for most comm-layers). These two
// arguments are, however, required for every comm-layer, and they
// are:
// 1. nodename where process will execute
// 2. full pathname of executable to execute
eclmplConfigFileTable*
eclmplConnectionInterfaceImplementationBase::scanConfigFile(const std::string& fileName,
                                                            int argsPerEntry) {
    std::ifstream infile;
    std::string arg;
    eclmplConfigFileTable* connTable = new eclmplConfigFileTable;
    std::vector<std::string> tableEntry;

    char* pwd = getcwd(NULL, 65535);
    ASSERT(pwd != NULL);
    std::string fullFilePathName(pwd);
    // using free instead of delete cause pwd was allocated with malloc
    // and not new ...
    free(pwd);

    fullFilePathName += "/" + fileName;
    infile.open(fullFilePathName.c_str(), std::ios::in);
    ASSERT(infile != NULL);

    connectionId = 0;
    numberOfConnections = 0;
    while (infile >> arg) {
        tableEntry.clear();
        tableEntry.push_back(arg);
        for (int i = 1; i < argsPerEntry; i++) {
            infile >> arg;
            tableEntry.push_back(arg);
        }
        connTable->addEntry(tableEntry);
        numberOfConnections++;
    }
#if 0
    std::cerr << "0: numberOfConnections=" << numberOfConnections << std::endl;
    std::cerr << "SCANNED CONFIG FILE" << std::endl << *connTable << std::endl;
#endif
    return connTable;
} // End of scanConfigFile(...).

// Assumptions:
// configTableEntry should contain at a minimum the following information -
// 1. hostname (nodename) of the computer where the slave will execute
// 2. full pathname of the executable that the slave will execute
// It should be contained in the following format -
// hostname fullPathNameOfExecutable <optional arguments>
// The new argv will look like:
// hostname fullPathNameOfExecutable <oldArgv> <optional arguments>
// masterContactInfo numberOfConnections connectionId amslave NULL
void
eclmplConnectionInterfaceImplementationBase::createSlaveCommandLineArguments(const int* const argc,
                                                                             const char* const* const* const argv,
                                                                             int& newArgc, char**& newArgv,
                                                                             const std::vector<std::string>& configTableEntry,
                                                                             const eclmplContactInfo& masterContactInfo,
                                                                             const unsigned int& id) {
    ASSERT(configTableEntry.size() >= 2);
    ASSERT(masterContactInfo.nrOfContactArguments() >= 1);
    char tmp[maxBuf];
    newArgc = 2 + *argc + configTableEntry.size() + 2 +
              masterContactInfo.nrOfContactArguments() + 1 + 4 + 1;
    int currArg;

    newArgv = new char *[newArgc];

    // newArgv[0] = hostname of slave
    newArgv[0] = new char [configTableEntry[0].length()+1];
    strncpy(newArgv[0], configTableEntry[0].c_str(), configTableEntry[0].length());
    newArgv[0][configTableEntry[0].length()] = '\0';

    // newArgv[1] = full-path filename of executable for slave
    newArgv[1] = new char [configTableEntry[0].length()+1];
    strncpy(newArgv[1], configTableEntry[0].c_str(), configTableEntry[0].length());
    newArgv[1][configTableEntry[0].length()] = '\0';

    // newArgv[2] = \"argv[0]\"
    // . . .
    // newArgv[2+argc-1] = \"argv[argc-1]\"
    for (int i = 0; i < *argc; i++) {
        newArgv[2+i] = new char [strlen((*argv)[i])+3]; // \" \" \0 == 3 characters
        sprintf(tmp, "%s%c", (*argv)[i], '\0');
        strcpy(newArgv[2+i], tmp);
    }

    currArg = 2+*argc;
    // <optional arguments>
    //for (unsigned int i = 0; i < configTableEntry.size(); i++) {
    for (int i = configTableEntry.size()-1; i >= 0; i--) {
        //currArg = 2+*argc+i;
        newArgv[currArg] = new char [configTableEntry[i].length()+1];
        strncpy(newArgv[currArg], configTableEntry[i].c_str(), configTableEntry[i].length());
        newArgv[currArg][configTableEntry[i].length()] = '\0';
        currArg++;
    }
    //currArg = 2+*argc+configTableEntry.size();

    // newArgv[currArg] =  \"numberOfOptionalArguments\"
    sprintf(tmp, "%d%c", configTableEntry.size(), '\0');
    newArgv[currArg] = new char [strlen(tmp)+1];
    memcpy(newArgv[currArg], tmp, strlen(tmp)+1);
    currArg++;

    // newArgv[currArg] =  \"masterContactInfo.providedBy\"
    sprintf(tmp, "%d%c", masterContactInfo.providedBy(), '\0');
    newArgv[currArg] = new char [strlen(tmp)+1];
    memcpy(newArgv[currArg], tmp, strlen(tmp)+1);
    currArg++;

    // newArgv[currArg] =  \"masterContactInfo.intendedFor\"
    sprintf(tmp, "%d%c", masterContactInfo.intendedFor(), '\0');
    newArgv[currArg] = new char [strlen(tmp)+1];
    memcpy(newArgv[currArg], tmp, strlen(tmp)+1);
    currArg++;

    std::vector<std::string> contactArgs = masterContactInfo.contactArguments();
    for (int i = contactArgs.size()-1; i >= 0; i--) {
        // newArgv[currArg] =  \"masterContactInfo.contactArgs[i]\"
        sprintf(tmp, "%s%c", contactArgs[i].c_str(), '\0');
        newArgv[currArg] = new char [strlen(tmp)+1];
        memcpy(newArgv[currArg], tmp, strlen(tmp)+1);
        currArg++;
    }

    // newArgv[currArg] =  \"masterContactInfo.nrOfContactArguments\"
    sprintf(tmp, "%d%c", contactArgs.size(), '\0');
    newArgv[currArg] = new char [strlen(tmp)+1];
    memcpy(newArgv[currArg], tmp, strlen(tmp)+1);
    currArg++;

    // newArgv[currArg] = \"numberOfConnections\"
    sprintf(tmp, "%d%c", numberOfConnections, '\0');
    newArgv[currArg] = new char [strlen(tmp)+1];
    strcpy(newArgv[currArg], tmp);
    currArg++;

    // newArgv[currArg] = \"slaveConnectionId\"
    sprintf(tmp, "%d%c", id, '\0');
    newArgv[currArg] = new char [strlen(tmp)+1];
    strcpy(newArgv[currArg], tmp);
    currArg++;

    // newArgv[currArg] = "amslave"
    sprintf(tmp, "amslave%c", '\0');
    newArgv[currArg] = new char [strlen(tmp)+1];
    strcpy(newArgv[currArg], tmp);
    currArg++;

    // newArgv[currArg] = NULL            --- execl wants argument list to end with NULL
    newArgv[currArg] = NULL;

#if 0
    std::cerr
            << std::endl
            << "hostname fullPathNameOfExecutable <oldArgv> <optional arguments> numberOfOptionalArguments masterContactInfo<providedBy intentedFor <contact args> nrOfContArgs>  numberOfConnections connectionId amslave NULL"
            << std::endl << endl;
#endif
} // End of createSlaveCommandLineArguments(...).

void
eclmplConnectionInterfaceImplementationBase::forkOffSlave(const int* const argc,
                                                          const char* const* const* const argv,
                                                          const eclmplConfigFileTable& connTable,
                                                          const eclmplContactInfo& masterContactInfo,
                                                          const unsigned int& id) {
    ASSERT(id >= 1 && id < numberOfConnections);

    std::vector<std::string> slaveEntry = connTable.getEntry(id);
    eclmplContactInfo masterInfo = masterContactInfo;
    masterInfo.setIntendedFor(id);
    if (fork() == 0) {
        int newArgc;
        char** newArgv = NULL;
        createSlaveCommandLineArguments(argc, argv, newArgc, newArgv, slaveEntry, masterInfo, id);

#if 0
        std::cerr << id << ": " << rshCmd << " ";
        for (int i = 0; i < newArgc; i++) {
            std::cerr << newArgv[i] << " ";
        }
        std::cerr << std::endl;
#endif
        // Slave.
        // execv will subsitute the running child program with a new program.
        // The new program is SSH_COMMAND followed by a set of arguments that will be
        // the new argv for the executing program. The set of new arguments must
        // end with a NULL pointer.
        if (execv(rshCmd.c_str(), newArgv) == -1) {
            perror("ConnectionInterface::forkOffSlaves - execv");
            std::cerr << "Could not start slave (" << id << ")" << std::endl;
            abort();
        }
    } else {
        // Master.
        // We need to deallocate newArgv so it's free to use for next slave.
        //for (int j = 0; j < newArgc; j++) {
        //  delete newArgv[j];
        //}
        //delete[] newArgv;
    }
} // End of forkOffSlave(...).

void
eclmplConnectionInterfaceImplementationBase::forkOffSlaves(const int* const argc,
                                                           const char* const* const* const argv,
                                                           const eclmplConfigFileTable& connTable,
                                                           const eclmplContactInfo& masterContactInfo) {
    for (unsigned int i = 1; i < (unsigned int)connTable.getNumberOfEntries(); i++) {
        forkOffSlave(argc, argv, connTable, masterContactInfo, i);
    } // End of for (unsigned int i = 1; i < (unsigned int)connTable.getNumberOfEntries(); i++).
} // End of forkOffSlaves(...).

void
eclmplConnectionInterfaceImplementationBase::distributeConfigFileTable(
    const eclmplConfigFileTable& connTable) {
    unsigned int serializedConnTableSize = maxBuf; // Value return parameter.
    char serializedConnTable[maxBuf];
    connTable.serialize(serializedConnTable, serializedConnTableSize);
    ASSERT(serializedConnTableSize <= mtu);

    for (unsigned int i = 1; i < numberOfConnections; i++) {
        send(serializedConnTableSize, serializedConnTable, i);
    }
} // End of distributeConfigFileTable(...).

