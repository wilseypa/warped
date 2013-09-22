#ifndef TYPES_HH
#define TYPES_HH


/** This file holds the common stuff that is needed all round the system
    like enumerations, error codes etc... */

enum Kind { UNKNOWN, FACTORY, SUB_FACTORY, STUB, OBJECT, ANY,
            SSL_BASE, SSL_COMPONENT_DEFINITION, SSL_COMPONENT_INSTANTIATION,
            SSL_NET_LIST, SSL_LABEL, SSL_MODULE, SSL_DESIGN_FILE,
            SSL_NET_LIST_ENTRY };

enum MessageKind{ UNKNOWN_MSG, LIST, SETUP_SIMULATION, SIMULATE,
                  FINISH, ABORT_MSG, SEARCH, CREATE_OBJECT, NET_LIST_MSG,
                  EVENT, SPECIFICATION, GENERAL, ERROR_MSG};

#endif
