#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include "eclmplCommonInclude.h"

/** The NetworkMessage class.

*/
class NetworkMessage {
public:

  /// Default Constructor.
  NetworkMessage() { userDataSize = 0; userData = NULL; }
  
  /// Constructor.
  NetworkMessage(const unsigned int &size, char *data);

  /// Destructor.
  virtual ~NetworkMessage();

  virtual char *serialize() const;
  virtual void serializedUserDataSize(char *buf, int &nrBytes) const;
  virtual void deserialize(char *msg);

  void setUserDataSize(const unsigned int &dataSize) { userDataSize = dataSize; }
  unsigned int getUserDataSize() const { return userDataSize; }
  void setUserData(char *data) { userData = data; }
  char *getUserData() const { return userData; }

  friend std::ostream& operator<< (std::ostream& os, const NetworkMessage& msg);
protected:
  unsigned int userDataSize;
  char *userData;
};
#endif
